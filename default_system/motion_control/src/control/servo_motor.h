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
// Created by master on 17. 4. 13.
//

#ifndef SEPMASTER_CIA402_SERVO_H
#define SEPMASTER_CIA402_SERVO_H

#include "sephi_ipc.h"
#include "event_host.h"
#include "ecat_cia402.h"

#include <array>
#define SEP_REQUIRE_IMPL_RET(ret) do{ SEP_TRACE("%s is not implemented yet!\n", __PRETTY_FUNCTION__); return (ret); } while(0);
namespace sephi
{
    typedef std::array<double,3> Vector3;
    typedef std::array<double,2> Vector2;
    typedef std::pair<double, double> Range;
    typedef std::pair<int32_t , int32_t > RangeI32;
    using ServoState = Cia402StatusWord::STATE;


    class EventCounter {
    public:
        EventCounter(int window_size = 0)
                : window_size_(window_size), index_(0), counter_(0), window_(window_size, false) { }
        void reset(int window_size) {
            window_size_ = window_size;
            counter_ = 0;
            index_ = 0;
            window_.resize(window_size, false);
        }
        void write(bool event) {
            if (window_[index_++ % window_size_]) --counter_;
            if (event) ++counter_;
        }
        int counter() { return counter_;}
        int size() { return window_size_;}

    private:
        int window_size_;
        int64_t index_;
        int counter_;
        std::vector<bool> window_;
    };

    /**\ingroup grp_robotic_module
     * \defgroup grp_servo_motor Servo motor
       \brief 서버모터 인터페이스 모듈
       \details
        - single axis 제어에 필요한 파라미터를 가짐
        - 모든 단위는 user단위임
        - 위치 : degree, 속도 : degree/s, 가속도 : degree/s^2
      \todo 토크 유닛은 현재는 서보 유닛을 그대로 사용하고, 필요시 추후에 정의할 예정터
      \{
     */
    /** 서보 모터 인터페이스 추상 클래스
     *
     */
    class ServoMotor : public IEventHost {
    public:
        enum DefaultValues {
            SPEED_SCALE_INV     = 10, // 최고 속도 제한
            MAX_VELOCITY_RPM    = 5000, // 최고 속도
            RATED_VELOCITY_RPM = 3000, 
            MAX_VELOCITY_DPS    = MAX_VELOCITY_RPM * 6,
            MAX_ACCEL_RPMS      = RATED_VELOCITY_RPM/2, // 2초 만에 최고 속도에 도달
            MAX_ACCEL_DPSS      =  MAX_ACCEL_RPMS*6,
            MAX_TORQUE_RATE_x1000 = 2000,
            MIN_VELOCITY_DPS    = 1,
            MIN_ACCEL_DPSS      = 1,
            FOLLOW_ERROR_TOL_x1000  = 1024*4, // follow error tollerance 무시
            NEAR_ZERO_VELOCITY_x1000  = 1, // neer-zero velocity (deg/s)
            OVER_TRQ_WINDOW_SIZE    = 100,
            E_FOLLOW_ERROR = -1,
            E_SERVO_OVERLOAD = -2,
        };
    public :
        ServoMotor();
        virtual ~ServoMotor();

        // 초기화
        virtual bool initialize(int axis, ServoOpMode mode, const char* model = nullptr, bool enable_on_init = true){return false;};
        virtual bool finalize() {return true;}
        // setting
        int getAxisNo(){ return axis_no_; }
        virtual ServoOpMode getServoMode() { return op_mode_; }
        virtual bool setServoMode(ServoOpMode new_mode) { op_mode_ = new_mode;  return true;}
        const Range& getPosLimit() { return pos_range_; }
        const Range& getVelLimit() { return vel_range_; }
        const Range& getAccLimit() { return acc_range_; }
        virtual MotorDirection getMotorDirection() { return dir_;}
        virtual bool setPosLimit(double min, double max) { pos_range_.first = min; pos_range_.second = max; return true; }
        virtual bool setVelLimit(double min, double max) { vel_range_.first = min; vel_range_.second = max; return true; }
        virtual bool setAccLimit(double min, double max) { acc_range_.first = min; acc_range_.second = max; return true; }
        virtual bool setTrqLimit(double min, double max) { trq_range_.first = min; trq_range_.second = max; return true; }
        virtual bool setMaxTrq(uint16_t max) { return true; }// max trq in 0.1%
        virtual bool getMaxTrq(uint16_t trq) { return true; }
        virtual bool setOverloadCondition(double over_trq_duration, double following_error_tolerance, double speed_to_following_error_ratio) { return false;}
        virtual int checkOverload() { return 0;}
        virtual bool setMasterAxis(int slave_id) { return false; }
        virtual bool resetMasterAxis(){ return true;}

        bool setUserUnit(const char* unitname, double pulse_per_unit); // jump if changed in operation
        const char* getUserUnitName();
        double getPulsePerUnit();
        double getGearRatio() { return gear_ratio_; } // gear비가 50이라는 것은 x50된다는 뜻임.

        double getToleranceMargin(ControlTarget mode) { return tolerance_[static_cast<int>(mode)]; }
        virtual bool setToleranceMargin(ControlTarget mode, double new_val) { tolerance_[static_cast<int>(mode)] = new_val; return true; }

        // control
        virtual bool enable(){return false;}
        virtual bool disable(){return false;}
        virtual ServoState getServoState() { return ServoState::UNKNOWN_STATE;}
        virtual bool stop(){ return false;} //emergent stop
        virtual bool brakeOn() { return false;}
        virtual bool brakeOff() { return false; }
        virtual bool isBrakeOn() { return false;}
        virtual bool setHome() { return false; }
        virtual bool isHome() { return false; }
        virtual bool resetEncoder(bool soft = 1, uint8_t AxisType = 0) { return false;}
        virtual bool clearFaults(){ return false;}

        virtual bool setReferencePP(double p, double* v=nullptr, double* a=nullptr, bool immediate = true); // PP{p, vmax, accmax},
        virtual bool setReferenceCSP(double p, double* voffs=nullptr, double* toffs=nullptr); //  CSP{p, voff, toff}
        virtual bool setReferenceCSV(double v, double* voffs=nullptr, double* toffs=nullptr); // CSV
        virtual const Vector3& getActual(){return actual_;} // pos, vel, acc
        virtual bool inPosition(); // target reached
        virtual double getPosError() { return 0; }
        virtual double getVelError() { return 0; }
        virtual double getRefPos() { return 0; }
        virtual double getRefVel() { return 0; }
        virtual uint16_t getErrcode() { return 0; }
        virtual double getActPos() { return 0; }
        virtual double getActVel() { return 0; }
        virtual double getActErr() { return 0; }
        virtual double getRawActErr() { return 0; }
        virtual double getRefPulse() { return 0; }
        virtual int32_t getLoopbackPos() { return 0; }
        // info
        const char* getName() const { return name_.c_str(); }

    public:
        // unit conversion
        double pulse2unit(pos_unit_t pulse) { return ((double)pulse)/(pulse_per_unit_*gear_ratio_);}
        pos_unit_t unit2pulse(double unit) { return unit*(pulse_per_unit_*gear_ratio_); }
        double pos2unit(PosUnit pulse) { return ((double)pulse.val)/(pulse_per_unit_*gear_ratio_);}
        PosUnit unit2pos(double deg) {  return PosUnit(deg*(pulse_per_unit_*gear_ratio_)); }

    protected:
        ServoOpMode op_mode_; // servo operation mode
        int axis_no_;
        double gear_ratio_;
        EncoderType enc_type_; // encoder type
        double pulse_per_unit_; // 1 unit당 encoder pulse @ master
        Range pos_range_;   // 위치 범위.
        Range acc_range_;   // 가속도 범위 최저~최고 가속도
        Range vel_range_;   // 속도 범위. 최저~최고 속도
        Range trq_range_;   // torque range, 정격 토크의 1/1000 단위. +-1000이면 정격 토크
        Vector3 tolerance_; // 위치, 속도, 가속도 tolerance
        Vector3 gain_;      // gain설정은 추후에
        MotorDirection dir_; // 회전 방향
        double home_pos_;   // home position
        PosUnit ref_pos_;   // reference position of CSP
        Vector3 actual_;    // actual value
        EventCounter overtrq_counter_; // over torque counter of given duration
        double follow_error_tolerance_;
        double near_zero_velocity_;
        std::mutex *pdo_mutex_;
        std::string name_;
        std::string unit_name_;
    protected:
        virtual bool setGearRatio(double new_val) { gear_ratio_ = new_val; return true; } // operation 도중에 기어비를 바꾸면 ref. jump 현상이 있을 수 있음
        friend class AxesModule;
        friend class SingleAxis;
        friend class PairedAxes;
        friend class MultiAxes;
    };

    /// \}
}

#endif //SEPMASTER_CIA402_SERVO_H
