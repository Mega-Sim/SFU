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
// Created by sem1 on 17. 4. 26.
//

#ifndef SEPMASTER_ROBOTIC_MODULE_H
#define SEPMASTER_ROBOTIC_MODULE_H

#include "sephi_ipc.h"
#include "event_host.h"
#include "servo_motion.h"
#include "ec_servo_motor.h"

using namespace KDL;
using std::unique_ptr;
using std::make_unique;

namespace sephi
{
    /** \defgroup grp_robotic_module Robotic Module
     *  \brief 로봇 모듈에 대한 추상화 객체
     *
     *  \details
     *  대상 모터들의 모션 제어에 대한 책임을 지며, 다음과 같은 기능 수행
     *   - motor 설정 : 제어모드, 각종 파라미터
     *   - motor의 예외 동작 처리
     *   - motion의 제어 명령 처리 : start/stop/pause/resume
     *   - motion 수행을 위한 reference update
     *   - motion 수행 상태 update
     *
     *
     *  Robotic module 계층 구조
     *  RoboticModule
     *   - AxesModule ; ServoMotion 활용
     *      - SingleAxis
     *      - MultiAxes
     *      - PairedAxes : master-slave
     *   - CartesianModule ; RobotMotion 활용
     *      - XY_Module
     *      - XYZ_Module
     *      - XYZW_Module (추후)
     *      - Scara_Module (추후)
     *      - ArmModule (추후)
     */
    /// \{
    class RoboticModule : public IEventHost {
    public:
        enum {
            MAX_MOTOR_AXIS_COUNT = 32,
            EV_SERVO_FAULT = 1,
            EV_DEC_END = 2,
            EV_ACC_BEGIN = 3,
        };
    public:
        RoboticModule(const char*name);
        virtual ~RoboticModule();
        const char* name() const {return name_.c_str();}
        bool initialize();
        bool finalize();
        virtual bool reenable();
        virtual bool enableServo(int axis = 0);
        virtual bool disableServo(int axis = 0);
        virtual bool clearFaults() = 0;
        virtual void enableMotion(){ motion_enable_ = true; }
        virtual void disableMotion(){ motion_enable_ = false;}
        virtual bool waitForMotionDone(int cmd_id){motion_event_.wait();}
        virtual EcServoMotor*getMotor(int axis); // 모터 설정을 직접 하고 싶을 때 사용. 모션 동작 중에 motor 파라미터를 바꾸지 말 것.
        virtual double getPos(int ax = 0) { return motors_[ax]->getActual()[0]; }
        virtual double getVel(int ax = 0) { return motors_[ax]->getActual()[1]; }
        bool setUserUnit(int axis, const char* unitname, double  pulse_per_unit); // 동작중에 unit 변경하지 말 것!
        bool setUserUnit(const char* unitname, double  pulse_per_unit){return setUserUnit(0, unitname, pulse_per_unit);}
        const char* getUserUnitName(int axis = 0);
        double getPulsePerUnit(int axis = 0);

    protected:
        // motor profile thread
        virtual void pdoUpdateTask() = 0;
        virtual bool beginPdoUpdateTask();
        virtual void endPdoUpdateTask();
        virtual bool updateReference() = 0; // update servo reference. this will be called in real time context.
        //virtual void logTrajectory() {}   // 모션 스레드 지연에 영향을 주어 삭제, 220727 yjchoe

        std::string name_;
        std::vector<unique_ptr<EcServoMotor>> motors_;
        std::vector<std::pair<string,double>> user_units_;
        atomic_bool motion_enable_;
        bool exit_;
        bool initialized_;
        std::unique_ptr<std::thread> svc_;
        std::unique_ptr<IpcCondition> pdo_arrival_event_;
        std::mutex mutex_;
        Condition motion_event_;
        // pdo task option 동기화된 처리를 하려고.
        bool use_external_pdo_task_;
    };
/*
// 모션 스레드 지연에 영향을 주어 삭제, 220727 yjchoe
    // LogItem은 fwrite()멤버 함수를 가져야 함
    template<typename LogItem, int default_queue_size = 100>
    class TrajectoryLogger {
    public:
        TrajectoryLogger(const char* filename) : exit_(false), logq_(default_queue_size)
        {
            FileName = filename;
            time_t now = time(NULL);
            struct tm *now_tm = localtime(&now);
            pre_min = now_tm->tm_min;
            pre_hr = now_tm->tm_hour;
            pre_day = now_tm->tm_mday;

            char filenamebuf[512];
            sprintf(filenamebuf, "../log/[%s]_%04d%02d%02d%02d.txt", FileName,
                    now_tm->tm_year + 1900, now_tm->tm_mon + 1, now_tm->tm_mday, now_tm->tm_hour);
            traj_file_ = fopen(filenamebuf, "a+");
            logfile_list_.clear();
            logfile_list_.push_back((std::string)filenamebuf);

            logq_.clear();

            if (traj_file_) task_ = std::make_unique<std::thread>([this](){logLoop();});
        }
        ~TrajectoryLogger(){
            exit_ = true;
            task_->join();
            if (traj_file_!= nullptr){
                fclose(traj_file_);
            }
        }

        void write(const LogItem& item) {
            std::lock_guard<std::mutex> guard(mutex_);
            logq_.push_back(item);
        }


    private:
        void logLoop() {
            SEP_REQUIRE(traj_file_ != nullptr);
            addIntoRealtimeSchedule(LOG_THREAD_PRIO);   // 로그 우선순위 설정
            while(!exit_) {
                while(logq_.size()>0) {
                    time_t now = time(NULL);
                    struct tm *now_tm = localtime(&now);

                    if(pre_hr != now_tm->tm_hour){
                        char filenamebuf[512];
                        fclose(traj_file_);
                        sprintf(filenamebuf, "../log/[%s]_%04d%02d%02d%02d.txt", FileName,
                                now_tm->tm_year + 1900, now_tm->tm_mon + 1, now_tm->tm_mday, now_tm->tm_hour);
                        traj_file_ = fopen(filenamebuf, "a+");
                        pre_hr = now_tm->tm_hour;
                    }

                    auto& item = logq_.front();
                    item.fwrite(traj_file_);
                    mutex_.lock();
                    logq_.pop_front();
                    mutex_.unlock();
                }
                usleep(5000); // just for yield
            }
        }

        FILE* traj_file_;
        bool exit_;
        std::unique_ptr<std::thread> task_;
        std::deque<LogItem> logq_;
        std::deque<std::string> logfile_list_;
        std::mutex mutex_;
        const char* FileName;
        int pre_day,pre_hr,pre_min;
    };
*/

    /// \}
}
#endif //SEPMASTER_ROBOTIC_MODULE_H
