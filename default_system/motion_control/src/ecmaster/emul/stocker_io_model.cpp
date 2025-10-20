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
#include <yaml-cpp/yaml.h>
#include "stocker_io_model.h"
namespace sephi
{

    void StockerIOModel::initialize()
    {
        ao_ = (int16_t*)rxpdo_;
        ai_ = (int16_t*)txpdo_;
    }

    bool StockerIOModel::loadModelData(const string& model_file, const char* model_name)
    {
        SEP_REQUIRE_RET(super::loadModelData(model_file, model_name), false);

        auto config = YAML::LoadFile(model_file);
        // load model data
        di_count_ = config[model_name]["DI"].as<int>();
        do_count_ = config[model_name]["DO"].as<int>();
        ai_count_ = config[model_name]["AI"].as<int>();
        ao_count_ = config[model_name]["AO"].as<int>();

        do_ = (uint64_t*)(rxpdo_ + ao_count_*2); // each analog port is 16bit-wide
        di_ = (uint64_t*)(txpdo_ + ai_count_*2);

        for(int i = 0; i < di_count_; ++i) di_mask_ |= 1ULL<<i;
        for(int i = 0; i < do_count_; ++i) do_mask_ |= 1ULL<<i;

        return true;
    }

/*
    bool StockerIOModel::getDO(int bit)
    {
        return (*do_ & (1ULL<<bit));
    }

    bool StockerIOModel::getDI(int bit)
    {
        return (*di_ & (1ULL<<bit));
    }

    void StockerIOModel::setDI(int bit, bool val)
    {
        SEP_REQUIRE(bit >= 0 && bit < di_count_);
        if (val){
            *di_ |= 1ULL << bit;
        } else {
            *di_ &= ~(1ULL << bit);
        }
    }
*/

    void StockerIOModel::setDO(int bit, bool val)
    {
        SEP_REQUIRE(bit >= 0 && bit < do_count_);
        if (val){
            *do_ |= 1ULL << bit;
        } else {
            *do_ &= ~(1ULL << bit);
        }
    }
}