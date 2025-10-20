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
// Created by root on 18. 1. 12.
//

#ifndef SEPMASTER_OHT_AMC_AXIS_H
#define SEPMASTER_OHT_AMC_AXIS_H

#include "axes_module.h"
#include "oht_mark_detector.h"

namespace sephi {
    class SingleAxisAmc : public SingleAxis {
        using super = SingleAxis;
    public:
        SingleAxisAmc(const char* name, ServoOpMode cmode, bool use_external_pdo_task = false)
                : super(name, cmode, use_external_pdo_task) {}

        int moveDistancePulse(double dist, double max_v = -1, double max_a = -1, double max_d = -1, double duration = -1);
        int moveDistanceIterativePulse(double dist, double v_end, double max_v = -1, double max_a = -1, double max_d = -1);
        int moveToPulse(double pos, double max_v = -1, double max_a = -1, double max_d = -1, double duration = -1);
        int rotateAtPulse(double vel, double max_v = -1, double max_a = -1, double max_d = -1, double duration = -1);
        bool stopPulse(double dec = -1, bool emergent = false);

        double getPosPulse(int ax = 0) { return mm2pulse(motors_[ax]->getActual()[0]); }
        double getPos(int ax = 0) { return motors_[ax]->getActual()[0]; }
        RangeI32 getPosLimitPulse(int ax = 0);
        double getVelPulse(int ax = 0) { return mm2pulse(motors_[ax]->getActual()[1]); }
        double getRefPosPulse(int ax = 0) { return mm2pulse(motors_[ax]->getRefPos()); }
        double getRefVelPulse(int ax = 0) { return mm2pulse(motors_[ax]->getRefVel()); }
	double getRefVel(){return ref_vel_;}
        double getPosErrorPulse(int ax = 0) { return mm2pulse(motors_[ax]->getPosError()); }
        double getVelErrorPulse(int ax = 0) { return mm2pulse(motors_[ax]->getVelError()); }
        double getPulse2Mm(double pulse) { return pulse2mm(pulse);}

        bool setPosLimitPulse(int32_t pos_min, int32_t pos_max);

        // SmallAdd motion capabilities
        virtual void setDetector(const oht::MarkDetector* detector){}
        virtual int32_t getMarkPosition() {return 0;}
        virtual int moveDistanceSmallAdd(int32_t target_limit, int32_t dist, int32_t max_v = -1, int32_t max_a = -1, int32_t max_d = -1) {return -1;}
        virtual int completeSmallAdd(int32_t destination){ return -1;}

        // For Hoist unit conversion
//        virtual double mm2pulse(double mm) { return motors_[0]->unit2pulse(mm) / (1<<7);}
//        virtual double pulse2mm(double pulse) { return motors_[0]->pulse2unit(pulse*(1<<7)); }
        virtual double mm2pulse(double mm) { return motors_[0]->unit2pulse(mm);}
        virtual double pulse2mm(double pulse) { return motors_[0]->pulse2unit(pulse); }
    };


    class SingleAxisAmcDrive : public SingleAxisAmc {
        using super = SingleAxisAmc;
    public:
        SingleAxisAmcDrive(const char* name, ServoOpMode cmode, bool use_external_pdo_task = false)
                : super(name, cmode, use_external_pdo_task), detector_(nullptr) { resetSmallAdd(); }

        void setDetector(const oht::MarkDetector* detector) override ;
        int32_t getMarkPosition() override ;
        int moveDistanceSmallAdd(int32_t target_limit, int32_t dist, int32_t max_v, int32_t max_a, int32_t max_d) override ;
        int completeSmallAdd(int32_t destination) override;  //stop at destination & disable small add

    private:
        bool small_add_enabled_;
        int32_t target_limit_;
        int32_t small_add_distance_actual_;
        int32_t small_add_distance_;
        double small_add_v_, small_add_a_, small_add_d_;
        const oht::MarkDetector* detector_;

        void resetSmallAdd();
        bool isSameSmallAddMotion(int32_t target_limit);
    };

    class SingleAxisAmcDriveRear : public SingleAxisAmcDrive, ISlaveAxis {
        using super = SingleAxisAmcDrive;
    public:
        SingleAxisAmcDriveRear(const char* name, ServoOpMode following_cmode)
                : SingleAxisAmcDrive(name, following_cmode, false), ISlaveAxis(following_cmode) {}

        bool follow(int master_id) override;
        bool enableServo(int axis = 0) override {
            SEP_REQUIRE_RET(super::enableServo(), false);
            follow(master_id_);
            return true;
        }
    };

    class SingleAxisAmcHoist:public SingleAxisAmcDrive {
        using super = SingleAxisAmcDrive;
    public:
        SingleAxisAmcHoist(const char* name, ServoOpMode cmode, bool use_external_pdo_task = false)
                : super(name, cmode, use_external_pdo_task) {}
    protected:
        double mm2pulse(double mm) override { return mm;}
        double pulse2mm(double pulse) override  { return pulse; }
    };


}

#endif //SEPMASTER_OHT_AMC_AXIS_H
