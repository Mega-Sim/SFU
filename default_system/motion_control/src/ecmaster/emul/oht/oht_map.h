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
// Created by root on 17. 11. 22.
//

#ifndef SEPMASTER_OHT_MAP_H
#define SEPMASTER_OHT_MAP_H

#include <array>
#include <map>
#include <list>
#include <climits>
#include <set>
#include "sephi_ipc.h"
namespace sephi
{
    namespace oht_sim {
        struct OHTLocation {
            uint32_t from, to; // node id
            double offset; // displacement
            void set(uint32_t _fr, uint32_t _to, double _offs) {
                from = _fr; to = _to; offset = _offs;
            }
        };

        class OHTMap {
        public:
            struct LinkData;
            using Path = std::deque<uint32_t>; // list of node id

            struct NodeData {
                enum {
                    MAX_LINKS_PER_NODE = 2
                };
                uint32_t id;
                uint32_t x, y, z;
                int link_count;
                std::array<LinkData*, MAX_LINKS_PER_NODE> link;

                bool addLink(LinkData *ln);
                LinkData* linkTo(uint32_t nextid);
            };

            struct TeachData;
            struct StationData {
                enum class Type:int { Unknown = -1, DualAccess };
                enum class PortType:int { Unknown = -1, EQ, LSTB, RSTB };
                enum class Material{  Unknown = -1, FOUP, RETICLE };

                uint32_t id;
                uint32_t parent, next;
                float offset;
                Type type;
                PortType port_type;
                string port_id;
                Material material;
                TeachData *teach;
                bool isStb() const { return port_type == PortType::LSTB || port_type == PortType::RSTB; }
                bool isLStb() const { return port_type == PortType::LSTB; }
                bool isRStb() const { return port_type == PortType::RSTB; }
                bool isEqStation() const { return  port_type == PortType::EQ; }
            };
            struct LinkData {
                string id;
                uint32_t from, to;
                bool is_left;
                float speed;
                float distance;
                std::list<StationData*> stations; // station list on the link
                StationData* getStation(double offset, double front_margin = 135, double rear_margin = 180-135);
            };
            struct TeachData {
                enum PortType {
                    Unknown = -1, EQ
                };
                enum PIOType {
                    RF, IR
                };
                enum PIODir {
                    Left, Right
                };

                uint32_t station_id;
                PortType type;
                float hoist; // in mm
                float shift; // in mm
                float rotate; // in deg
                PIOType pio_type;
                PIODir pio_dir;
                uint32_t pio_id;
                // add ...
                bool isLeft() const { return pio_dir == PIODir::Left; }
                bool isRight() const { return !isLeft(); }
                bool isIrType() const { return pio_type == PIOType ::IR; }
                bool isRfType() const { return pio_type == PIOType ::RF; }
            };
            struct CIDData {
                enum LayoutType {
                    Unknown = -1, NormalCID, SlopeCID
                };
                enum ResetNodeType {
                    NormalNode, CommonNode
                };
                string id;
                LayoutType type;
                uint32_t reset_node;
                uint32_t stop_node1, stop_node2;
                ResetNodeType reset_type;
                // add ...
            };

            struct Location {
                uint32_t from;
                uint32_t to;
                double displacement;
            public:
                Location() = default;
                Location(const Location&) = default;
                Location(uint32_t f, uint32_t t, double d = 0.0) : from(f), to(t), displacement(d){}
                bool onTheSameLink(const Location& o) const { return (from == o.from && to == o.to);}
            };

            class Travel {
            public:
                Travel(): map_(nullptr), path_(nullptr){}
                void initialize(OHTMap& map, Path& path);
                double getLength() const {return path_length_;};
                Location getLocation() const {return location_;}
                LinkData* getLink();
                LinkData* getNextLink();
                double maxSpeed(); // max allowable speed of current edge
                bool moveDistance(double dist);
                bool onTheLastLink();
                bool isEnd() const;
            private:
                OHTMap* map_;
                Path* path_;
                Path::iterator next_node_; // path iteration
                Location location_;     // current location : nodeid, displacement of current edge
                double path_length_;    // total path length
            };


        public:
            OHTMap() = default;

            bool loadMapData(const string data_dir);

            bool findPath(uint32_t start_node, uint32_t target_node, Path &path, double known_min_cost = std::numeric_limits<double>::infinity());
            double calculatePathLength(Path& path);
            double calcDistanceTo(const Location& from, const Location& to);
            LinkData* getLink(uint32_t from); // any link that starts from from
            LinkData* getLinkTo(uint32_t to); // any link that goes to to
            LinkData* getLink(uint32_t from, uint32_t to);
        private:
            bool loadNode(string path);
            bool loadLink(string path);
            bool loadStation(string path);
            bool loadTeaching(string path);
            bool loadCID(string path);
            void mapStationToLink();
            void assignStationTeachData();
            double evaluatePathCost(Path &path);
            double calcRecursively(int from_node, int to_node, double distance);

        public:
            std::map<uint32_t, NodeData> node_map;
            std::map<string, LinkData> link_map;
            std::map<uint32_t, StationData> station_map;
            std::map<uint32_t, TeachData> teach_map;
            std::map<string, CIDData> cid_map;
        };
        using OhtLocation = std::pair<oht_sim::OHTMap::LinkData*,double>; // link ptr, displacement from parent

    }
}
#endif //SEPMASTER_OHT_MAP_H
