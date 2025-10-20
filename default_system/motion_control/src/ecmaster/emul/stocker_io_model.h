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
// Created by sem1 on 17. 11. 6.
//

#ifndef SEPMASTER_STOCKER_IO_MODE_H_H
#define SEPMASTER_STOCKER_IO_MODE_H_H
#include "ecat_slave.h"
namespace sephi
{
    /// @addtogroup grp_ecmaster_emul
    /// @{

    /** \brief Stocker I/O board 동작을 emulation 한다.
    */
    class StockerIOModel : public ECatSlaveModel
    {
        using super = ECatSlaveModel;
    public:
        void initialize();
        bool loadModelData(const string& model_file, const char* model_name);
        int16_t& AO(int port) { return ao_[port]; }
        int16_t& AI(int port) { return ai_[port]; }

        //bool getDO(int bit);
        //bool getDI(int bit);
        //void setDI(int bit, bool val);
        void setDO(int bit, bool val);
        uint64_t &DO(){ return *do_;}
        uint64_t &DI(){ return *di_;}
        uint64_t DO_val() const { return *do_ & do_mask_;}
        uint64_t DI_val() const { return *di_ & do_mask_;}

    protected:
        // input/output : user's view. i.e a input is user's input
        int ai_count_;
        int ao_count_;
        int di_count_;
        int do_count_;
        int16_t *ai_;
        int16_t *ao_;
        uint64_t *di_;
        uint64_t *do_;
        uint64_t di_mask_;
        uint64_t do_mask_;
    };
    /// @}
}
#endif //SEPMASTER_STOCKER_IO_MODE_H_H
