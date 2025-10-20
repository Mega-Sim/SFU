//#include "sephi/log/logger.h"
#include "ALSADevice.h"
#include "amc_sound.h"

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define COMPOSE_ID(a,b,c,d)	((a) | ((b)<<8) | ((c)<<16) | ((d)<<24))
#define LE_SHORT(v)		(v)
#define LE_INT(v)		(v)
#define BE_SHORT(v)		bswap_16(v)
#define BE_INT(v)		bswap_32(v)
#else /* __BIG_ENDIAN */
#define COMPOSE_ID(a,b,c,d)	((d) | ((c)<<8) | ((b)<<16) | ((a)<<24))
#define LE_SHORT(v)		bswap_16(v)
#define LE_INT(v)		bswap_32(v)
#define BE_SHORT(v)		(v)
#define BE_INT(v)		(v)
#endif

#define WAV_RIFF		COMPOSE_ID('R','I','F','F')
#define WAV_WAVE		COMPOSE_ID('W','A','V','E')
#define WAV_FMT			COMPOSE_ID('f','m','t',' ')
#define WAV_DATA		COMPOSE_ID('d','a','t','a')
#define WAV_PCM_CODE		1
#define MAX_CHANNELS        7

using namespace amc;

namespace amc {
    bool ALSADevice::initialize() {
        int err;
        snd_pcm_hw_params_t *params;

        if ((err = check_wav_file(path_.c_str())) < 0) {
            return false;
        }

        if ((err = snd_pcm_open(&handle_, device_name_.c_str(), type_, 0)) < 0) {
            printf("Playback open error: %d,%s\n", err, snd_strerror(err));
            return false;
        }

        snd_pcm_hw_params_malloc(&params);

        if ((err = set_hwparams(params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
            printf("Setting of hwparams failed: %s\n", snd_strerror(err));
            snd_pcm_hw_params_free(params); //20220221
            snd_pcm_close(handle_);
            return false;
        }

        file_ = fopen(path_.c_str(), "r");
        snd_pcm_hw_params_free(params); //20220221
        return true;
    }

    int ALSADevice::set_hwparams(snd_pcm_hw_params_t *params, snd_pcm_access_t access) {
        int err, dir;
        unsigned int pcm, val;
        snd_pcm_uframes_t frames;

        /* choose all parameters */
        err = snd_pcm_hw_params_any(handle_, params);
        if (err < 0) {
            fprintf(stderr, ("Broken configuration for playback: no configurations available: %s\n"),
                    snd_strerror(err));
            return err;
        }

        /* set the interleaved read/write format */
        err = snd_pcm_hw_params_set_access(handle_, params, access);
        if (err < 0) {
            fprintf(stderr, ("Access type not available for playback: %s\n"), snd_strerror(err));
            return err;
        }

        /* set the sample format */
        err = snd_pcm_hw_params_set_format(handle_, params, SND_PCM_FORMAT_S16_LE);
        if (err < 0) {
            fprintf(stderr, ("Sample format not available for playback: %s\n"), snd_strerror(err));
            return err;
        }

        err = snd_pcm_hw_params_set_channels(handle_, params, channels_);
        if (err < 0) {
            fprintf(stderr, ("Channels count (%i) not available for playbacks: %s\n"), channels_, snd_strerror(err));
            return err;
        }

        /* set the stream rate */
        err = snd_pcm_hw_params_set_rate_near(handle_, params, &rate_, 0);
        if (err < 0) {
            fprintf(stderr, "ERROR: Can't set rate. %s\n", snd_strerror(pcm));
            return err;
        }

        /* Set period size to 32 frames */
        frames = 32;
        err = snd_pcm_hw_params_set_period_size_near(handle_, params, &frames, &dir);
        if(err < 0) {
            fprintf(stderr, ("Unable to set period size for playback: %s\n"), snd_strerror(err));
            return err;
        }

        /* write the parameters to device */
        err = snd_pcm_hw_params(handle_, params);
        if (err < 0) {
            fprintf(stderr, ("Unable to set hw params for playback: %s\n"), snd_strerror(err));
            return err;
        }

        /* Use a buffer large enough to hold one period */
        err = snd_pcm_hw_params_get_period_size(params, &frames, &dir);
        if (err < 0) {
            fprintf(stderr, ("Unable to get period size for playback: %s\n"), snd_strerror(err));
            return err;
        }
        bytes_per_sample_ = sample_size_ / 8;
        BUFFER_SIZE =   frames * bytes_per_sample_ * channels_
                        * buffer_setting_ * sleep_time_ms_;      //factor for oht play list

        buffer_ = new int8_t[BUFFER_SIZE];

        snd_pcm_uframes_t buffer_size_ = BUFFER_SIZE;
        err = snd_pcm_hw_params_set_buffer_size_near(handle_, params, &buffer_size_);
        if (err < 0) {
            fprintf(stderr, ("Unable to set buffer for playback: %s\n"), snd_strerror(err));
            return err;
        }

        return 0;
    }

    void ALSADevice::play_file() {
        auto start_time = std::chrono::system_clock::now();
        auto end_time = std::chrono::system_clock::now();
        auto execution_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        int8_t get_status = sound_->get_oht_status();
        int err;

        fseek(file_, 0, SEEK_SET);
        snd_pcm_prepare(handle_);

        while (execution_time.count() <= play_time_ms_){
            //Status change
            if(get_status != sound_->get_oht_status()) {
                snd_pcm_prepare(handle_);
                break;
            }

            //File read complete
            if((bytesRead_ = fread(buffer_, sizeof buffer_[0], BUFFER_SIZE / (sizeof buffer_[0]), file_)) <= 0){
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                end_time = std::chrono::system_clock::now();
                execution_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
                continue;
            }

            // Number of frames in the current buffer
            snd_pcm_uframes_t frames = bytesRead_ / (channels_ * bytes_per_sample_);

            // Writes buffer of frames
            err = snd_pcm_writei(handle_, buffer_, frames);

            if (err == -EPIPE) {
                // an underrun occurred
                snd_pcm_prepare(handle_);
            } else if (err < 0) {
                //printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(pcm));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            end_time = std::chrono::system_clock::now();
            execution_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        }
        sound_->set_oht_status(0);
    }

    int ALSADevice::check_wav_file(const char *file) {
        int fd;
        struct wave_header header;

        file_size = LE_INT(header.chunk.length);

        if ((fd = open(file, O_RDONLY)) < 0) {
            fprintf(stderr, ("Cannot open WAV file %s\n"), file);
            return -EINVAL;
        }
        if (read(fd, &header, sizeof(header)) < (int) sizeof(header)) {
            fprintf(stderr, ("Invalid WAV file %s\n"), file);
            goto error;
        }

        if (header.hdr.magic != WAV_RIFF || header.hdr.type != WAV_WAVE) {
            fprintf(stderr, ("Not a WAV file: %s\n"), file);
            goto error;
        }
        if (header.body.format != LE_SHORT(WAV_PCM_CODE)) {
            fprintf(stderr, ("Unsupported WAV format %d for %s\n"),
                    LE_SHORT(header.body.format), file);
            goto error;
        }
        if (header.body.channels != LE_SHORT(1) && header.body.channels != LE_SHORT(2)) {
            fprintf(stderr, ("%s is not a mono stream (%d channels)\n"),
                    file, LE_SHORT(header.body.channels));
            goto error;
        }
        if (header.body.rate != LE_INT(44100) && header.body.rate != LE_INT(22050) &&
            header.body.rate != LE_INT(11025)) {
            fprintf(stderr, ("Sample rate doesn't match (%d) for %s\n"),
                    LE_INT(header.body.rate), file);
            goto error;
        }
        if (header.body.sample_bits != LE_SHORT(16) && header.body.sample_bits != LE_SHORT(8)) {
            fprintf(stderr, ("Unsupported sample format bits %d for %s\n"),
                    LE_SHORT(header.body.sample_bits), file);
            goto error;
        }
        if (header.chunk.type != WAV_DATA) {
            fprintf(stderr, ("Invalid WAV file %s\n"), file);
            goto error;
        }

        sample_size_ = header.body.sample_bits;
        channels_ = header.body.channels;
        rate_ = header.body.rate;

        close(fd);
        return 0;

        error:
        close(fd);
        return -EINVAL;
    }
}