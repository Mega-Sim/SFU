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
// Created by sem1 on 17. 5. 1.
//

#ifndef SEPMASTER_AXES_MODULE_H_H
#define SEPMASTER_AXES_MODULE_H_H
#include "robotic_module.h"

namespace sephi
{
    /// \addtogroup grp_robotic_module
    /// \{
    class AxesModule : public RoboticModule {
    public:
        struct Command {
            enum Code : int {
                NONE,
                MOVE_D,     // move distance (relative movement)
                MOVE_I,     // move distance iterative ... (final velocity is not zero)
                MOVE_A,     // move absolute
                ROTATE,     // rotate
                PAUSE,      // pausing motion (position mode only)
                RESUME,     // resume motion when paused, ignored otherwise
                STOP,       // normal stop
                ESTOP,      // emergent stop
                FSTOP,      // stop due to the cpu freezing
                CLEAR,      // previous command will be cleared.
            };
            Code code;
            int id;    // unique id for command identification
        };
    public:
        using parent = RoboticModule;
        AxesModule(const char* name, ServoOpMode cmode);
        virtual bool stop(double dec, int emergent = 0){ throw  "Not implemented"; };
        virtual bool emergentStop() { return stop(-1, true); } // default 구현
        virtual Motion::MotionState getMotionState(int axis) { throw "Not implemented";}
        bool isRunning(int axis=0) { return getMotionState(axis)&0x00F;}
        bool isStopped(int axis=0) { return !isRunning(axis);}
    protected:
        ServoOpMode control_mode_;
        int id_base_;
        int generateCommandId() {
            id_base_ = (++id_base_ < 0) ? 0 : id_base_;
            return id_base_;
        };
    };

    // 단축에 대한 모션을 처리
    class SingleAxis : public AxesModule {
        // 모션 스레드 지연에 영향을 주어 삭제, 220727 yjchoe
/*        struct LogItem { // t, ref_p, ref_v, real_p, real_v, real_torq, pos_e, system time, detect
            int mc_id;
            std::array<double,9> data;
            void fwrite(FILE *fp) {
                //fprintf(fp, "%d %lf %.0f %.0f %.0f %.0f %.0f %.0f %lf %.0f\n", mc_id, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8]);
                fprintf(fp, "%d %lf %.0f %.0f %.0f %.0f %.0f %.0f %lf\n", mc_id, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
            }
            double getTs() {
                struct timespec now;
                clock_gettime(CLOCK_REALTIME, &now);
                return now.tv_sec % 10000 + now.tv_nsec * 1e-9;
            }
            LogItem(){}
            LogItem(std::initializer_list<double> l) {
                int i = 0;
                for(auto& item : l) {
                    data[i++] = item;
                    if (i >= data.size()) break;
                }
                data[7] = getTs();
            }
        };
        using TrajLogger = TrajectoryLogger<LogItem>;*/
    public:
        struct Command : public AxesModule::Command {
            double target; // distance or absolute target, target velocity
            double max_v;  // max velocity
            double max_a;  // max acceleration
            double max_d;  // max deceleration
            union {
                double duration; // duration or end_vlocity
                double v_end;
            };
            Command():target(0), max_v(0), max_a(0), max_d(0), duration(-1){ code = NONE; }
            Command(int _id, Code _code, double _target, double _max_v, double _max_a, double _max_d, double dur)
                    :target(_target), max_v(_max_v), max_a(_max_a), max_d(_max_d), duration(dur) {  code = _code; id = _id;}
        };

    public:
        SingleAxis(const char* name, ServoOpMode cmode, bool use_external_pdo_task = false);
        ~SingleAxis(){ finalize(); }
        bool initialize(int axis_no, const char* motor_model = nullptr, double gear_ratio = 1,  bool enable_on_init = true);
        bool changeControlMode(ServoOpMode new_cmode);
        bool finalize();
        bool reenable() override ;
        bool enableServo(int axis = 0);
        bool clearFaults() override;
        void disableMotion() override;


        bool setMax(double max_vel_deg, double max_acc_deg = -1, double max_dec_deg = -1, bool set_motor_limit = true);
        bool setMaxRPM(double max_vel_rpm, double max_acc_rpm = -1, double max_dec_rpm = -1, bool set_motor_limit = true);
        bool setPosLimit(double pos_min, double pos_max);
        bool setMaxTrq(uint16_t max_trq);
        bool getMaxTrq(uint16_t trq);
        // robot api interface
        int moveDistance(double dist, double max_v = -1, double max_a = -1, double max_d = -1, double duration = -1);
        int moveDistanceIterative(double dist, double v_end, double max_v = -1, double max_a = -1, double max_d = -1);
        int moveTo(double pos, double max_v = -1, double max_a = -1, double max_d = -1, double duration = -1);
        int rotateAt(double vel, double max_v = -1, double max_a = -1, double max_d = -1, double duration = -1);
        bool stop(double dec = -1, int emergent = 0);
        bool pause();
        bool resume();
        bool clearCommand();
        bool resetEncoder(bool soft=false, uint8_t AxisType = 0);
        void setMotionChangeMode(bool immediate) { immediate_motion_change_ = immediate; }
        bool isImmediateChangeMode() {return immediate_motion_change_; }

        int lastMotionCommandId(){ return last_mc_id_; }
        int getCurrentMotionId();
        const Command getCurrentCommand() const { return current_motion_command_; }
        Motion::MotionState getMotionState(int axis = 0);
        virtual bool waitForMotionDone(int command_id);
        void OhtDetectLv(double lv);

    protected:
        void checkCommandParam(double& max_v, double &max_a, double &max_d);
        double validateTargetPosition(double p, const char* msg = nullptr);
        bool createMotions();

        // pdo context functions
        virtual void pdoUpdateTask();
        bool processCommand(); // process motion command in pdoUpdateTask context
        virtual bool updateReference();
        void stayReference();
        //virtual void logTrajectory(); // 모션 스레드 지연에 영향을 주어 삭제, 220727 yjchoe
        virtual bool onMoveD(Command& cmd);
        virtual bool onMoveA(Command& cmd);
        virtual bool onRotate(const Command& cmd);
        virtual bool onStop(const Command& cmd);
        virtual bool onPause(const Command& cmd);
        virtual bool onResume(const Command& cmd);

        static int onServoFault(IEventHost*ins, int evid, void *arg);
    protected:
        ServoMotion *motion_; // current motion
        std::deque<Command> commandq_;
        // pre made motions;
        std::unique_ptr<CSPMotion> csp_motion_;
        std::unique_ptr<CSVMotion> csv_motion_;
        std::unique_ptr<PPMotion>  pp_motion_;
        // sw limit
        Range pos_limit_;
        // staus
        double t_; // motion time
        double t_offset_, t_save_; // 모션의 연속 display를 위해 도입된 변수
        double ref_pos_, ref_vel_;
        double pos_, vel_, trq_;
        double detect_; // oht,obs detect level
        bool paused_;
        bool immediate_motion_change_; // not queuing.
        // traj logger
        //std::unique_ptr<TrajLogger> traj_logger_; // 모션 스레드 지연에 영향을 주어 삭제, 220727 yjchoe
        // current motion command for resume
        bool iterative_motion_;
        Command current_motion_command_;
        // last mc id, used for waiting
        atomic_int last_mc_id_;

        friend class MultiAxes;
        int log_count;
    };

    // 추종 모션만 인터페이스
    class ISlaveAxis {
    public:
        ISlaveAxis(ServoOpMode slave_cmode = ServoOpMode::CST);
        virtual bool follow(int master_id) = 0;
    protected:
        ServoOpMode slave_ctrl_mode_;
        int master_id_;
    };

    // 단축과 연결된 축을 처리
    class PairedAxes : public SingleAxis, ISlaveAxis {
        // 모션 스레드 지연에 영향을 주어 삭제, 220727 yjchoe
        /*struct LogItem { // t, ref_p, real_p, real_v, real_torq, slave_real_p, v, t
            int mc_id;
            std::array<double,9> data;
            void fwrite(FILE *fp) {
                fprintf(fp, "%d %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", mc_id, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],  data[8]);
            }
            LogItem(){}
            LogItem(std::initializer_list<double> l) {
                int i = 0;
                for(auto& item : l) {
                    data[i++] = item;
                    if (i >= data.size()) break;
                }
            }
        };
        using TrajLogger = TrajectoryLogger<LogItem>;*/

    public:
        PairedAxes(const char* name, ServoOpMode cmode, ServoOpMode slave_cmode = ServoOpMode::CST);
        ~PairedAxes(){ finalize(); }
        bool initialize(int master_axis, int slave_axis, const char* master_model = nullptr, const char* slave_model = nullptr, double master_gear_ratio = 1);
        bool finalize();
        bool enableServo(int axis = 0) override ;
        bool disableServo(int axis = 0) override ;
        bool clearFaults() override;

        //void logTrajectory() override ;   // 모션 스레드 지연에 영향을 주어 삭제, 220727 yjchoe

        bool follow(int master_id) override;
    protected:
        double slave_pos_, slave_vel_, slave_trq_;
        // traj logger
        //std::unique_ptr<TrajLogger> traj_logger_; // 모션 스레드 지연에 영향을 주어 삭제, 220727 yjchoe
    };

    // 다축 모듈...
    struct MxMcRet{
        int axis, val;
        MxMcRet(int ax, int v): axis(ax), val(v){}
    };

    typedef std::vector<MxMcRet> MultiAxesResult;
    struct MxMcParam {
        int axis;
        double pos;
        double max_v;
        double max_a;
        double max_d;
        double duration;
        MxMcParam(int ax, double p, double v=-1, double a=-1, double dec = -1, double d=-1)
                : axis(ax), pos(p), max_v(v), max_a(a), max_d(dec), duration(d) { }
    };

    class MultiAxes : public AxesModule {
    public:
        using InitParam = std::tuple<int,const char*, double>;
    public:
        MultiAxes(const char* name, int axis_count, ServoOpMode opmode = ServoOpMode::CSP);
        ~MultiAxes();

        bool addAxis(int index, int axis_no, const char* motor_model = nullptr, double gear_ratio = 1);
        bool initialize(bool enable_on_init = true);
        bool finalize();
        bool reenable() override;
        bool enableServo(int axis);
        bool disableServo(int axis);
        bool clearFaults() override;
        void enableMotion() override;
        void enableMotion(int axis);
        void disableMotion() override;
        //bool isMotionEnabled() const override;

        bool setMax(int axis, double max_vel_deg, double max_acc_deg = -1, double max_dec_deg = -1, bool set_motor_limit = true);
        bool setMaxRPM(int axis, double max_vel_rpm, double max_acc_rpm = -1, double max_dec_rpm = -1, bool set_motor_limit = true);
        bool setPosLimit(int axis, double pos_min, double pos_max);
	    double getPos(int axis) override;
	    double getVel(int axis) override;
        virtual EcServoMotor*getMotor(int axis);

        // robot api interface by axis
        // multi axes api
        double calcLongestDuration(Command::Code cmd, std::initializer_list<MxMcParam>& params);
        // duration param을 지정하지 않으면 모든 모션은 동시에 시작하여 동시에 끝난다.
        // duration param을 일부만 지정하면 지정된 모션은 해당 시간에 종료하고, 나머지는 동시에 종료한다.
        MultiAxesResult moveDistance(std::initializer_list<MxMcParam> params);
        MultiAxesResult moveTo(std::initializer_list<MxMcParam> params);
        MultiAxesResult stop(std::initializer_list<int> axis_list, int emergent = 0);
        MultiAxesResult clearCommand(std::initializer_list<int> axis_list);
        bool pause();
        bool resume();


        // all axes api
        bool stopAll(double dec = -1, int emergent = 0);
        bool clearCommand();
        void setMotionChangeMode(bool immediate);

        // per axis api
        bool changeControlMode(int axis, ServoOpMode new_cmode);
        int moveDistance(int axis, const double dist, double max_v = -1, double max_a = -1, double max_d = -1, double duration = -1);
        int moveTo(int axis, double pos, double max_v = -1, double max_a = -1, double max_d = -1, double duration = -1);
        int rotateAt(int axis, double vel, double max_v = -1, double max_a = -1, double max_d = -1, double duration = -1);
        bool stop(int axis, double dec = -1, int emergent = 0);
        bool clearCommand(int axis);
        int lastMotionCommandId(int axis);
        int getCurrentMotionId(int axis);
        Motion::MotionState getMotionState(int axis );
        bool waitForMotionDone(int axis, int command_id);
        bool waitForMotionDone(const MxMcRet r){ return waitForMotionDone(r.axis, r.val);}

    protected:
        virtual void pdoUpdateTask();
        virtual bool updateReference(){ return false;}

        std::vector<std::unique_ptr<SingleAxis>> axes_;
        std::vector<InitParam> init_param_;
    };
    /// \}
}

#endif //SEPMASTER_AXES_MODULE_H_H
