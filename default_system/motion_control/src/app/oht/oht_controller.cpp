/*
 * SEMES CONFIDENTIAL
 *
 * Copyright (c) 2017 SEMES Co.,Ltd.
 * All Rights Reserved.
 *
 * NOTICE: All information contained herein is, and remains the property
 * of SEMES and its suppliers, if any. The intellectual and technical concepts
 * contained herein are proprietary to SEMES and its suppliers and may be
 * covered by S.Korea and Foreign Patents, patents in process, and are
 * protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material is
 * strictly forbidden unless prior written permission is obtained from SEMES.
 */


//
// Created by root on 17. 12. 11.
//
#include <yaml-cpp/yaml.h>
#include "oht_controller.h"

using namespace sephi;
using namespace sephi::oht;

OHTController::OHTController()
    : initialized_(false), thread_exit_(false), thread_(nullptr)
{
    loadIOStatus = false;
}

OHTController::~OHTController()
{
    finalize();
}

void OHTController::setToInitialPosition()
{
    std::lock_guard<std::mutex> guard(lock_);
    localizer_->setLocation(config_.initial_location.from, config_.initial_location.to, config_.initial_location.offset);
}

double OHTController::getPosmm(int axis)
{
    if (axis == OHT_HOIST) {
        return config_.hoist_unit_conv.pulse2mm( hoist_->getPos());
    } else {
        return motors_[axis]->getActual()[0];
    }
}

double OHTController::getPos(int axis)
{
    return motors_[axis]->getActPos();
}

double OHTController::getRemainingDistance()
{
    //return drive_->getRemainingDistance();
}

/*
OHTLocation OHTController::getLocation()
{
    return localizer_->getLocation();
}
*/
int OHTController::getMarkState()
{
    return localizer_->getMarkingState();
}

int OHTController::getRunState()
{
    //return drive_->getRunState();
}

bool OHTController::localize()
{
    //std::lock_guard<std::mutex> guard(lock_);
    //return drive_->runForLocalization();
}

bool OHTController::runToNode(uint32_t nodeid)
{
    //std::lock_guard<std::mutex> guard(lock_);
    //SEP_REQUIRE_RET(map_.node_map.find(nodeid) != map_.node_map.end(), false);
    //return drive_->runToNode(nodeid);
}

bool OHTController::runToEqStation(uint32_t stid)
{
    //std::lock_guard<std::mutex> guard(lock_);
    //SEP_REQUIRE_RET(map_.station_map.find(stid) != map_.station_map.end(), false);
    //return drive_->runToStation(stid);
}

bool OHTController::runDisplacement(int32_t disp_mm)
{
    //std::lock_guard<std::mutex> guard(lock_);
    //return drive_->moveDistance(disp_mm);
}

bool OHTController::stopRun(bool quick)
{
    //return drive_->stop(quick);
}

bool OHTController::load(bool step)
{
	/*
    std::lock_guard<std::mutex> guard(lock_);
    auto loc = localizer_->getLocation();
    auto sta = map_.getLink(loc.from, loc.to)->getStation(loc.offset);
    SEP_REQUIRE_RET(sta != nullptr, false);
    SEP_REQUIRE_RET(sta->teach != nullptr, false);
    return trans_->load(sta, step);
	*/
}

bool OHTController::unload(bool step)
{
	/*
    std::lock_guard<std::mutex> guard(lock_);
    auto loc = localizer_->getLocation();
    auto sta = map_.getLink(loc.from, loc.to)->getStation(loc.offset);
    SEP_REQUIRE_RET(sta != nullptr, false);
    SEP_REQUIRE_RET(sta->teach != nullptr, false);
    return trans_->unload(sta, step);
	*/
}

bool OHTController::transStop()
{
    //std::lock_guard<std::mutex> guard(lock_);
    //return trans_->stop();
}

bool OHTController::transOrigin()
{
    //std::lock_guard<std::mutex> guard(lock_);
    //return trans_->origin();
}

bool OHTController::transPause()
{
    //std::lock_guard<std::mutex> guard(lock_);
    //return trans_->pause();

}

bool OHTController::transResume()
{
    //std::lock_guard<std::mutex> guard(lock_);
    //return trans_->resume();

}

bool OHTController::transStepRun()
{
    //std::lock_guard<std::mutex> guard(lock_);
    //return trans_->step();

}


bool OHTController::initialize()
{
    SEP_REQUIRE_RET(loadConfig(), false);
    SEP_REQUIRE_RET(initializeIO(), false);
    SEP_REQUIRE_RET(loadIOConfig(), false);
    SEP_REQUIRE_RET(initializeServo(), false);
    SEP_REQUIRE_RET(makeBehaviors(), false);
    SEP_REQUIRE_RET(startService(), false);
    return true;
}

void OHTController::finalize()
{
    rear_wheel_->disableServo();
    writeDo(ioConfig_.getportid("hoist_ext_brake_release"), false);  //writeDo(hoist_ext_brake_release, false);    // DO - 63
    writeDo(ioConfig_.getportid("driving_brake_release"), false);     //writeDo(driving_brake_release, false);
    stopService();
}

bool OHTController::initializeIO()
{
    front_io_ = make_unique<EcOHTFrontIo>();
    rear_io_ = make_unique<EcOHTRearIo>();

    SEP_REQUIRE_RET(front_io_->initialize(5), false);
    SEP_REQUIRE_RET(rear_io_->initialize(4), false);
    SEP_REQUIRE_RET(onboard_io_.initialize(), false);
    if (AMC_CLIENT_MODE) { // to disable

//        driving_bcr_ = make_unique<BcrBL185>("BCR1", RF2217_ECAT_SLAVE_PORT + 15, "/dev/tty5", 38400);
//       trans_bcr_ = make_unique<BcrBL185>("BCR2", RF2217_ECAT_SLAVE_PORT + 16, "/dev/tty7", 38400);
    } else {
//        driving_bcr_ = make_unique<BcrBL185>("BCR1", RF2217_ECAT_SLAVE_PORT + 15, "/dev/ttyS5", 38400);
//        trans_bcr_ = make_unique<BcrBL185>("BCR2", RF2217_ECAT_SLAVE_PORT + 16, "/dev/ttyS7", 38400);
    }
    return true;
}

bool OHTController::initializeServo()
{
    front_wheel_ = make_unique<SingleAxisAmcDrive>("front_wheel", ServoOpMode::CSP);
    //rear_wheel_ = make_unique<SingleAxisAmcDriveRear>("rear_wheel", ServoOpMode::CST);
    rear_wheel_ = make_unique<SingleAxisAmcDriveRear>("rear_wheel", ServoOpMode::CSV);  // 210119 yjchoe
    hoist_ = make_unique<SingleAxisAmcDrive>("hoist", ServoOpMode::CSP);
    slide_ = make_unique<SingleAxisAmcDrive>("slide", ServoOpMode::CSP);

    // set units
    hoist_->setUserUnit("pulse", 1); //1<<7
    front_wheel_->setUserUnit("pulse", 1);
    rear_wheel_->setUserUnit("pulse", 1);
    slide_->setUserUnit("pulse", 1);

    // initialization -
    sleep_for(30ms);    // 210730 yjchoe, for clear servo alarm(min:20ms)
    SEP_REQUIRE_RET(front_wheel_->initialize(0, "SGM7P-04AFA21", 1, true), false); // \note enable-on-init is disabled!
    SEP_REQUIRE_RET(rear_wheel_->initialize(1, "SGM7P-04AFA2C", 1, true), false);
    SEP_REQUIRE_RET(hoist_->initialize(2, "SGM7J-02A7A2C", 1, true), false);
    SEP_REQUIRE_RET(slide_->initialize(3, "SGM7J-01A7A21", 1, true), false);
    axes_ = {front_wheel_.get(), rear_wheel_.get(), hoist_.get(), slide_.get()};
    motors_ = {axes_[0]->getMotor(0), axes_[1]->getMotor(0), axes_[2]->getMotor(0),axes_[3]->getMotor(0)};

//    front_wheel_->resetEncoder(true);
//    rear_wheel_->resetEncoder(true);

//    front_wheel_->setMaxTrq(300);
//    rear_wheel_->setMaxTrq(300);
    //hoist_->setMaxTrq(300);
    //slide_->setMaxTrq(300);

    // set limits
    for(int i = 0; i < 4; i++) {
        double max_v = config_.v_max[i]*1.05;   // amc limit < motion limit
        double min_a = config_.a_max[i].first;
        double max_a = config_.a_max[i].second;
        double min_p = config_.pos_limit[i].first;
        double max_p = config_.pos_limit[i].second;
        axes_[i]->setMax(max_v, max_a, min_a);      // vel, max, min
        if (config_.pos_limit[i].first != config_.pos_limit[i].second) {
            axes_[i]->setPosLimit( min_p, max_p);
        }
        else {
            axes_[i]->setPosLimitPulse(axes_[i]->getPosLimitPulse().first, axes_[i]->getPosLimitPulse().second);
        }
    }

    // set immediate motion change mode
    for(auto axis : axes_) {
        axis->setMotionChangeMode(true);
    }

    // release hoist tolerance
    motors_[OHT_HOIST]->setOverloadCondition(100, 128*320*2, 128*10);

    // set rear wheel as slave of front wheel
    rear_wheel_->follow(front_wheel_->getMotor(0)->getSlaveId());
    return true;
}

bool OHTController::makeBehaviors()
{
    if (AMC_CLIENT_MODE) {
        perceptions_.emplace_back(std::make_unique<MarkUpdater>(this));
        localizer_ = (Localization *) perceptions_.front().get();
    } else {
        perceptions_.emplace_back(std::make_unique<Localization>(this));
        localizer_ = (Localization *) perceptions_.front().get();
        //perceptions_.emplace_back(std::make_unique<Monitoring>(this));
        //perceptions_.emplace_back(std::make_unique<ObstacleDectection>(this));
        //drive_ = std::make_unique<Driving>(this, *localizer_);
        //trans_ = std::make_unique<Transfer>(this);
    }

    return true;
}

bool OHTController::startService()
{
    if (AMC_CLIENT_MODE) {
        //return localizer_->runAsThread(); --> Amc4OHT::statusUpdate_thread()에서 호출
        return true;
    }
    else {
        //trans_->runAsThread(1000); 
        if (thread_ == nullptr) {
            thread_ = std::make_unique<std::thread>([this]() { this->svc(); });
        }
        return thread_ != nullptr;
    }
}

void OHTController::stopService()
{
    if (AMC_CLIENT_MODE) {
        localizer_->stopThread();
    }
    if (thread_ != nullptr) {
        thread_exit_ = true;
        thread_->join();
        thread_.reset(nullptr);
        //trans_->stopThread();
    }
}

void OHTController::svc()
{
    auto wake_up_at = std::chrono::system_clock::now();
    auto duration = std::chrono::nanoseconds(ECAT_EMULATION_CYCLETIME_NS*1);
    setCpuAffinity(2);
    addIntoRealtimeSchedule(USER_RT_APP_PRIO);
    while(!thread_exit_) {
        //SEP_LOGE("start");
        wake_up_at += duration;
        lock_.lock();
        for(auto& bh: perceptions_) {
            bh->update();
        }
        //drive_->update();

        if (wake_up_at < std::chrono::system_clock::now()) {
            SEP_LOGE("Time Quantum exhaust!... %.2fms\n", (std::chrono::system_clock::now() - wake_up_at).count()/1e6);
            wake_up_at = std::chrono::system_clock::now();
        }
        lock_.unlock();
        std::this_thread::sleep_until(wake_up_at);
    }
}


bool OHTController::loadConfig(const char* _dir_prefix)
{
    if (initialized_) return true;

    string dir_prefix = (_dir_prefix == nullptr) ? "../../../" : _dir_prefix;
    //string model_file = dir_prefix + "oht_eni_model.yml"; // 210720 yjchoe
    string model_file = "oht_eni_model.yml";

    // load map
    auto config = YAML::LoadFile(model_file)["Systems"]["OHT"];

    // load map data path
    string map_data_path = dir_prefix + "../" + config["MapFilePath"].as<string>();

    // map loading
    //map_.loadMapData(map_data_path);  // 210720 yjchoe

    // load robot parameters
    //config_.ppmm[OHT_WHEEL_F] = config["Robot"]["PPMM"]["wheel"].as<double>() * (1<<7); // 13bit을 20bit에 맞게 곱함
//    config_.ppmm[OHT_WHEEL_F] = V82_PULSETOMM; //config["Robot"]["PPMM"]["wheel"].as<double>(); // 13bit을 20bit에 맞게 곱함
//    config_.ppmm[OHT_WHEEL_R] = config_.ppmm[OHT_WHEEL_F];
//    config_.ppmm[OHT_SLIDE] = config["Robot"]["PPMM"]["slide"].as<double>();// * (1<<7); // 13bit을 20bit에 맞게 곱함
//    config_.offset[OHT_HOIST] = config["Robot"]["Offset"]["hoist"].as<double>();
//    config_.offset[OHT_SLIDE] = config["Robot"]["Offset"]["slide"].as<double>();

    std::array<double,7> mm_list, pulse_list;
    for(auto i = 0; i < 7; ++i) {
        mm_list[i] = config["Robot"]["PPMM"]["hoist"]["mm"][i].as<double>();
        pulse_list[i] = config["Robot"]["PPMM"]["hoist"]["pulse"][i].as<double>();
    }
    config_.hoist_unit_conv.initialize(pulse_list, mm_list);

    // limits
    // load limit data
    for(int i : {OHT_WHEEL_F, OHT_HOIST, OHT_SLIDE}) {
        int cfg_index = (i==OHT_WHEEL_F)? i : i-1;
        config_.pos_limit[i].first = config["Robot"]["limits"]["pos"]["min"][cfg_index].as<double>();
        config_.pos_limit[i].second = config["Robot"]["limits"]["pos"]["max"][cfg_index].as<double>();
        config_.pos_margin[i].first = config["Robot"]["limits"]["pos"]["n_margin"][cfg_index].as<double>();
        config_.pos_margin[i].second = config["Robot"]["limits"]["pos"]["p_margin"][cfg_index].as<double>();
        config_.v_max[i] = config["Robot"]["limits"]["vel"][cfg_index].as<double>();
        config_.a_max[i].first = config["Robot"]["limits"]["acc"]["min"][cfg_index].as<double>();
        config_.a_max[i].second = config["Robot"]["limits"]["acc"]["max"][cfg_index].as<double>();
        config_.inpos[i] = config["Robot"]["limits"]["pos"]["inpos"][cfg_index].as<double>();
        config_.f_err[i] = config["Robot"]["limits"]["Err"][cfg_index].as<double>();
        config_.f_rawerr[i] = config["Robot"]["limits"]["RawErr"][cfg_index].as<double>();
    }
    printf("%f_____%f\n", config_.pos_limit[OHT_HOIST].first, config_.pos_limit[OHT_HOIST].second);
    config_.pos_limit[OHT_WHEEL_R] = config_.pos_limit[OHT_WHEEL_F];
    config_.pos_margin[OHT_WHEEL_R] = config_.pos_margin[OHT_WHEEL_F];
    config_.inpos[OHT_WHEEL_R] = config_.inpos[OHT_WHEEL_F];
    config_.v_max[OHT_WHEEL_R] = config_.v_max[OHT_WHEEL_F];
    config_.a_max[OHT_WHEEL_R] = config_.a_max[OHT_WHEEL_F];
    config_.f_err[OHT_WHEEL_R] = config_.f_err[OHT_WHEEL_F];
    config_.f_rawerr[OHT_WHEEL_R] = config_.f_rawerr[OHT_WHEEL_F];

    // load size parameter
    config_.precheck2stop = config["Robot"]["Size"]["precheck2stop"].as<double>();
    config_.node_mark_size = config["MarkSize"]["node"].as<double>();
    config_.station_mark_size = config["MarkSize"]["station"].as<double>();
    config_.station_mark_front_part = config["MarkSize"]["station_front_part"].as<double>();
    config_.station_stop_mark_size = config["MarkSize"]["station_stop"].as<double>();
    config_.premark_start = config["Mark"]["premark_start"].as<double>();
    config_.node_premark_speed = config["Mark"]["node_premark_speed"].as<double>();
    config_.station_premark_speed = config["Mark"]["station_premark_speed"].as<double>();
    config_.premark_acc = config["Mark"]["premark_acc"].as<double>();
    config_.node_mark_speed = config["Mark"]["node_mark_speed"].as<double>();
    config_.station_mark_speed = config["Mark"]["station_mark_speed"].as<double>();
    config_.mark_acc = config["Mark"]["mark_acc"].as<double>();
    config_.initial_location.set( config["InitialLocation"]["from"].as<uint32_t>(),
                                  config["InitialLocation"]["to"].as<uint32_t>(),
                                  config["InitialLocation"]["offset"].as<double>() );
    config_.localization_drive_limit = config_.precheck2stop * 2; /// \todo optimize this value

    initialized_ = true;
    return true;
}

bool OHTController::loadIOConfig(int OHT_ID) {
    //printf("Load IO Config!\n");
    SEP_LOGT("[I/O Config] Load I/O Config!");
    loadIOStatus = true;

    const std::map<int,string> oht_id_map = {
            {30,    "OHT_STD_01"},
            {31,    "OHT_NAME_V81"},
            {36,    "OHT_NAME_V82"},
            {37,    "OHT_NAME_V86"},
            {9,    "OHT_NAME_P4MASK"},
            {38,    "OHT_NAME_V82MASK"},
            {39,    "OHT_NAME_V90"}
    };
    auto res = oht_id_map.find(OHT_ID);

    // Map 초기화
    ioConfig_.clear();
    bit_inversion_ports_.clear();
    active_low_ports_.clear();

    try {
        string model_file = "IO_MAP.yml";
        auto config_file = YAML::LoadFile(model_file)["Model"];

        for (auto IO_name : {"Input", "Output", "Etc"}) {
            auto IOMap = config_file[res->second][IO_name];
            if (IOMap.size() != 0) {
                //printf("[%s I/O]\n", IO_name);
                SEP_LOGT("[I/O Config] %s I/O", IO_name);
                int IO_size = 0;
                for (int i = 0; i < IOMap.size(); ++i) {
                    auto Num = IOMap[i]["Num"].as<int>();
                    auto Name = IOMap[i]["Name"].as<string>();
                    auto Board = IOMap[i]["Board"].as<int>();
                    auto Port = IOMap[i]["Port"].as<int>();
                    auto Bit = IOMap[i]["Bit"].as<int>();
                    auto Portid = makePortId(Board, Port, Bit);

                    IO_size = Num;
                    if (IO_name == "Input")           Num = Num;
                    else if (IO_name == "Output")     Num += OutOffset;
                    else if (IO_name == "Etc")        Num += EtcOffset;
                    ioConfig_.setportid(Name, Num, Portid);
                    //printf("IO[%d] Name[%s] Board[%d] Port[%d] Bit[%d] MakrportID[%d]\n", Num, Name.c_str(), Board, Port, Bit, ioConfig_.getportid(Num));

                    // Map에 Portid가 제대로 들어갔는지 확인하여 다시 입력
                    for (int j = 0; j < 3; j++) {
                        if (Portid != ioConfig_.getportid(Num)
                        || Portid != ioConfig_.getportid(Name)
                        || Name != ioConfig_.getportstring(Portid)) {
                            Portid = makePortId(Board, Port, Bit);
                            ioConfig_.modifyportid(Name, Num, Portid);
                            SEP_LOGT("Retry %d, I/O[%d] Name[%s] MakeportID[%d]", j+1, Num, Name.c_str(), Portid);
                        }
                    }
                    SEP_LOGT("I/O[%d] Name[%s] Board[%d] Port[%d] Bit[%d] MakeportID[%d]", Num, Name.c_str(), Board, Port, Bit, ioConfig_.getportid(Num));
                }
                ioConfig_.setportsize(IO_name, IO_size);
                //printf("%s %d\n", IO_name, ioConfig_.getportsize(IO_name));
            }
        }
        bit_inversion_ports_ = ioConfig_.get_inversion_port();
        active_low_ports_ = ioConfig_.get_active_low_port();
    } catch (std::exception &e) {
        SEP_TRE("IO Config Loading Error : %s\n", e.what());
        return false;
    }
    loadIOStatus = false;
    return true;
}

unsigned OhtIoConfig::getportid(string name)
{
    // name에 해당하는 key가 없는 경우 0을 반환
    if (name_map.find(name) != name_map.end())
        return num_map[name_map[name]];
    else
        return 0;
}

unsigned OhtIoConfig::getportid(unsigned num)
{
    // num에 해당하는 key가 없는 경우 0을 반환
    if (num_map.find(num) != num_map.end())
        return num_map[num];
    else
        return 0;
}

string OhtIoConfig::getportstring(unsigned portid)
{
    if (portid_map.find(portid) != portid_map.end())
        return portid_map[portid];
    else
        return "";
}

void OhtIoConfig::setportid(string name, unsigned num, unsigned portid)
{
    // port map
    name_map.insert(std::make_pair(name, num));
    num_map.insert(std::make_pair(num, portid));
    portid_map.insert(std::make_pair(portid, name));

    // inversion port map
    for (int i = 0; i < sizeof(bit_inversion_str)/sizeof(string); ++i) {
        if(name == bit_inversion_str[i]) {
            inversion_ports_.insert(portid);
            break;
        }
    }
    for (int i = 0; i < sizeof(active_low_str)/sizeof(string); ++i) {
        if(name == active_low_str[i]) {
            low_ports_.insert(portid);
            break;
        }
    }

    // io val map
    io_val_map.insert(std::make_pair(portid, false));
}

void OhtIoConfig::modifyportid(string name, unsigned num, unsigned portid)
{
    // 기존 요소 삭제
    name_map.erase(name);
    num_map.erase(num);
    portid_map.erase(portid);
    inversion_ports_.erase(portid);
    low_ports_.erase(portid);
    io_val_map.erase(portid);

    // 요소 추가
    setportid(name, num, portid);
}

void OhtIoConfig::clear()
{
    name_map.clear();
    num_map.clear();
    portid_map.clear();
    inversion_ports_.clear();
    low_ports_.clear();
    io_val_map.clear();
    size_map.clear();
}

std::set<unsigned int> OhtIoConfig::get_inversion_port()
{
    return inversion_ports_;
}

std::set<unsigned int> OhtIoConfig::get_active_low_port()
{
    return low_ports_;
}

int OhtIoConfig::getportsize(string key)
{
    if (size_map.find(key) != size_map.end())
        return size_map[key];
    else
        return 0;
}

void OhtIoConfig::setportsize(string io_name, int size)
{
    size_map.insert(std::make_pair(io_name, size));
}
