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
// Created by sem on 17. 12. 4.
//

#ifndef SEPMASTER_OHT_CONFIG_H
#define SEPMASTER_OHT_CONFIG_H
#include <string>
#include <yaml-cpp/yaml.h>
#include <array>
#include <map>
#include "log.h"

namespace sephi
{
    using std::unique_ptr;
    using std::make_unique;
    using std::array;
    using std::pair;
    using std::make_pair;
    using std::array;
    using std::map;

    class OhtConfig
    {
    public:
        enum {WHEEL, HOIST, SLIDE};
        std::array<double,3> ppmm; // pulse per millimeter
        std::array<double,3> offset; // offset of hoist & slide
        std::array<pair<double,double >,3> pos_limit; // hoist & slide limit
        std::array<double,3> v_max;
        std::array<Range,3> a_max; // Range.first = min, second = max

    public:
        bool loadConfig(const char* config_file_path) {
            auto config = YAML::LoadFile(config_file_path)["Systems"]["OHT"];
            /*
            // load map data path
            map_data_path = "../" + config["MapFilePath"].as<string>();

            // map loading
            map_.loadMapData(map_data_path);
            */

            // laod robot parameters
            ppmm[WHEEL] = config["Robot"]["PPMM"]["wheel"].as<double>();
            ppmm[SLIDE] = config["Robot"]["PPMM"]["slide"].as<double>();
            offset[HOIST] = config["Robot"]["Offset"]["hoist"].as<double>();
            offset[SLIDE] = config["Robot"]["Offset"]["slide"].as<double>();

            // load limit data
            for(int i : {WHEEL, HOIST, SLIDE}) {
                pos_limit[i].first = config["Robot"]["limits"]["pos"]["min"][i].as<double>();
                pos_limit[i].second = config["Robot"]["limits"]["pos"]["max"][i].as<double>();
                v_max[i] = config["Robot"]["limits"]["vel"][i].as<double>();
                a_max[i].first = config["Robot"]["limits"]["acc"]["min"][i].as<double>();
                a_max[i].second = config["Robot"]["limits"]["acc"]["max"][i].as<double>();
            }
            return true;
        }
    };

}
#endif //SEPMASTER_OHT_CONFIG_H
