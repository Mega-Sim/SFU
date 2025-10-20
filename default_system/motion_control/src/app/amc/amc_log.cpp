//
// Created by yj.choe on 2021-10-18.
//
#include <boost/filesystem.hpp>
#include "amc_oht.h"

namespace amc
{
    void Amc4OHT::logUpdate_thread()
    {
        auto wake_up_at = std::chrono::system_clock::now();
        auto duration = std::chrono::minutes(60);
        //setCpuAffinity(2);
        addIntoRealtimeSchedule(LOG_THREAD_PRIO);
        static uint32_t log_count = 0;
        log_delete();
        while (true) {
            wake_up_at += duration;
            auto now = std::chrono::system_clock::now();
            if (stop_) break;
            if (wake_up_at > now ) {
                std::this_thread::sleep_until(wake_up_at);
            } else {
                wake_up_at = now;
            }

            log_delete();

            auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - wake_up_at);
            if (dur.count() > 1) {
                SEP_LOGD("Amc4OHT::logUpdate_thread time : %dms", dur.count());
            }
            //OHT_LOGT1("Test Log for oht ISR\n");

        }
        SEP_LOGT("Exit LOG_ISR");
    }

    struct path_leaf_string
    {
        std::string operator()(const boost::filesystem::directory_entry& entry) const
        {
            return entry.path().leaf().string();
        }
    };

    void Amc4OHT::log_delete()
    {
        std::vector<std::string> log_list;
        boost::filesystem::path log_path("/home/semes/work/log/");
        boost::filesystem::directory_iterator start(log_path);
        boost::filesystem::directory_iterator end;

        std::transform(start, end, std::back_inserter(log_list), path_leaf_string());

        std::vector<std::string> main_log_list;
        for(auto i : log_list) {
            if (i.substr(1, i.find("]") - 1) == "master")
                main_log_list.push_back(i);
        }
        remove_file_in_list_day(main_log_list, 14);
        /*
        std::vector<std::string> motion_log_list;
        for(auto i : log_list) {
            if (i.substr(1, i.find("]") - 1) == "front_wheel")
                motion_log_list.push_back(i);
        }
        for(auto i : log_list) {
            if (i.substr(1, i.find("]") - 1) == "rear_wheel")
                motion_log_list.push_back(i);
        }
        for(auto i : log_list) {
            if (i.substr(1, i.find("]") - 1) == "hoist")
                motion_log_list.push_back(i);
        }
        for(auto i : log_list) {
            if (i.substr(1, i.find("]") - 1) == "slide")
                motion_log_list.push_back(i);
        }
        remove_file_in_list_hour(motion_log_list, 6);
        */
    }

    void Amc4OHT::remove_file_in_list_day(std::vector<std::string> list, uint32_t duration)
    {
        std::vector<std::string> delete_list;

        std::time_t curr_time = time(nullptr);
        std::tm log_tm{};

        try {
            for(auto i : list) {
                auto log_data = i.substr(i.find("]")+2, 10);
                log_tm.tm_year = std::stoi(log_data.substr(0,4)) - 1900;
                log_tm.tm_mon = std::stoi(log_data.substr(4,2)) - 1;
                log_tm.tm_mday = std::stoi(log_data.substr(6,2));
                log_tm.tm_hour = std::stoi(log_data.substr(8,2));

                std::time_t log_time = std::mktime(&log_tm);
                auto diff = std::difftime(curr_time, log_time);

                if(diff < 0.0 || diff > (60*60*24*duration)) // duration * 1day
                    delete_list.emplace_back(i);
            }

            for(auto i : delete_list) {
                std::string delete_file = "";
                delete_file = "/home/semes/work/log/" + i;
                auto exist = boost::filesystem::remove(delete_file);
                if (!exist)
                    return;
            }
        } catch(std::exception &e) {
            SEP_LOGD("Delete log Error : %s\n", e.what());
            return;
        }
    }

    void Amc4OHT::remove_file_in_list_hour(std::vector<std::string> list, uint32_t duration)
    {
        std::vector<std::string> delete_list;

        std::time_t curr_time = time(nullptr);
        std::tm log_tm{};

        try {
            for(auto i : list) {
                auto log_data = i.substr(i.find("]")+2, 10);
                log_tm.tm_year = std::stoi(log_data.substr(0,4)) - 1900;
                log_tm.tm_mon = std::stoi(log_data.substr(4,2)) - 1;
                log_tm.tm_mday = std::stoi(log_data.substr(6,2));
                log_tm.tm_hour = std::stoi(log_data.substr(8,2));

                std::time_t log_time = std::mktime(&log_tm);
                auto diff = std::difftime(curr_time, log_time);

                if(diff < 0.0 || diff > (60*60 *duration)) // duration * 1hour
                    delete_list.emplace_back(i);
            }

            for(auto i : delete_list) {
                std::string delete_file = "";
                delete_file = "/home/semes/work/log/" + i;
                auto exist = boost::filesystem::remove(delete_file);
                if (!exist)
                    return;
            }
        } catch(std::exception &e) {
                SEP_LOGD("Delete log Error : %s\n", e.what());
            return;
        }
    }
}