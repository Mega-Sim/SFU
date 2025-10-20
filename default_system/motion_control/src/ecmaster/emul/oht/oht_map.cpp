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
#include <limits>
#include "csv.h"
#include "oht_map.h"
#include "oht_info.h"
namespace sephi
{
    using namespace oht_sim;

    bool OHTMap::NodeData::addLink(LinkData* ln)
    {
        SEP_REQUIRE_RET(link_count < MAX_LINKS_PER_NODE, false);
        link[link_count++] = ln;
        return true;
    }

    OHTMap::LinkData* OHTMap::NodeData::linkTo(uint32_t nextid)
    {
        for(auto& lk : link) {
            if (lk != nullptr && lk->to == nextid) return lk;
        }
        SEP_LOGE("Unreachable next node (%u) from this node(%u)!", nextid, id);
        return nullptr;
    }

    OHTMap::StationData* OHTMap::LinkData::getStation(double offset, double front_margin, double rear_margin)
    {
        // if oht.location(offset) is in [st.offset - front_margin, st.offset + rear_margin]
        for(auto st : stations) {
            if (offset >= st->offset - front_margin && offset <= st->offset + rear_margin) {
                return st;
            }
        }
        return nullptr;
    }

    void OHTMap::Travel::initialize(OHTMap &map, Path &path)
    {
        map_ = &map;
        path_ = &path;
        path_length_ = 0;
        // set node iterator
        next_node_ = path_->begin();
        ++next_node_;
        // set location
        if (path.size()>0){
            location_.from = path[0];
            location_.to = path[1];
            path_length_ = map.calculatePathLength(path);
        } else {
            location_.from = 0xFFFFFFFF;
            location_.to = 0xFFFFFFFF;
        }

        location_.displacement = 0;
    }

    OHTMap::LinkData* OHTMap::Travel::getLink()
    {
        if (isEnd()) return nullptr;
        return map_->node_map[location_.from].linkTo(*next_node_);
    }

    OHTMap::LinkData* OHTMap::Travel::getNextLink()
    {
        if (onTheLastLink()) return nullptr;
        auto next_next_node = next_node_;
        ++next_next_node;
        return map_->node_map[*next_node_].linkTo(*next_next_node);
    }

    double OHTMap::Travel::maxSpeed()
    {
        return getLink()->speed;
    }

    bool OHTMap::Travel::moveDistance(double distance)
    {
        while(distance > 0) {
            if (isEnd()) return false;
            auto link = getLink();
            // inter link movement
            if (link->distance - location_.displacement > distance ) {
                location_.displacement += distance;
                return true;
            }
            // extra link movement
            distance -= link->distance - location_.displacement;
            location_.from = *next_node_++;
            location_.to = (next_node_!= path_->end()) ? *next_node_ : 0xFFFFFFFF;
            location_.displacement = 0;
        }
        return true;
    }

    bool OHTMap::Travel::onTheLastLink()
    {
        auto link = getLink();
        if (link != nullptr) {
            return (link->to == path_->back());
        }
        return true; // ... hmm
    }

    bool OHTMap::Travel::isEnd() const
    {
        return (location_.from == path_->back());
    }

    /**
     * (from, disp) ~ (to, disp)의 거리 계산.
     * 거리가 OHT 센서 감지 거리 보다 길어지면 계산 종료
     *
     *
     */
    double OHTMap::calcDistanceTo(const Location& from, const Location& to)
    {
        // 두 location이 동일 link 위에 위치할 때
        if (from.onTheSameLink(to)) {
            return to.displacement - from.displacement - OHTInfo::OHT_LENGTH_MM;
        }

        auto from_link = getLink(from.from, from.to);
        double distance = from_link->distance - from.displacement + to.displacement - OHTInfo::OHT_LENGTH_MM;
        return calcRecursively(from.to, to.from, distance);
    }

    double OHTMap::calcRecursively(int from_node, int to_node, double distance)
    {
        auto& next_node = node_map[from_node];
        if (next_node.id == to_node || distance > OHTInfo::OHT_SENS_RANGE_7) {
            return distance;
        }
        double min_dist = std::numeric_limits<double>::max();
        for(int i = 0; i < next_node.link_count; ++i) {
            auto new_dist = calcRecursively(next_node.link[i]->to, to_node, distance + next_node.link[i]->distance);
            if (min_dist > new_dist) min_dist = new_dist;
        }
        return min_dist;
    }

    double OHTMap::calculatePathLength(Path& path)
    {
        double length = 0;
        auto nodeid = path.begin();
        while(*nodeid != path.back()) {
            auto& node = node_map[*nodeid]; // from node
            ++nodeid; // to node
            auto link = node.linkTo(*nodeid); // get link
            length += link->distance;
        }
        return length;
    }

    OHTMap::LinkData* OHTMap::getLinkTo(uint32_t to)
    {
        int found = 0;
        OHTMap::LinkData* ret = nullptr;
        for(auto& kv: link_map) {
            if (kv.second.to == to){
                //return &kv.second;
                ret = &kv.second;
                ++found;
            }
        }
        // for safety
        if (found > 1) {
            // throw std::runtime_error("More than 1 incomming link!\n");
        }
        return ret;
    }

    OHTMap::LinkData* OHTMap::getLink(uint32_t from)
    {
        return node_map[from].link[0];
    }

    OHTMap::LinkData* OHTMap::getLink(uint32_t from, uint32_t to)
    {
        return node_map[from].linkTo(to);
    }

    bool OHTMap::loadNode(string path)
    {
        io::CSVReader<4, io::trim_chars<' ', '\t'>, io::no_quote_escape<'\t'>> datafile(path);
        datafile.read_header(io::ignore_extra_column, "Node ID", "X", "Y", "Z");
        uint32_t id, x, y, z;
        while (datafile.read_row(id, x, y, z)) {
            node_map.emplace(std::make_pair(id, NodeData{id, x, y, z, 0, {0,0}}));
        }
        return true;
    }

    bool OHTMap::loadLink(string path)
    {
        io::CSVReader<6, io::trim_chars<' ', '\t'>, io::no_quote_escape<'\t'>> datafile(path);
        datafile.read_header(io::ignore_extra_column, "Link ID", "From Node", "To Node", "IsLeft", "Speed", "Distance");
        string id;
        uint32_t from, to, is_left;
        float speed, distance;
        while (datafile.read_row(id, from, to, is_left, speed, distance)) {
            link_map.emplace(std::make_pair(id, LinkData{id, from, to, bool(is_left), speed, distance}));
            SEP_REQUIRE_RET(node_map[from].addLink(&link_map[id]), false);
        }
        return true;
    }

    bool OHTMap::loadStation(string path)
    {
        io::CSVReader<8, io::trim_chars<' ', '\t'>, io::no_quote_escape<'\t'>> datafile(path);
        datafile.read_header(io::ignore_extra_column, "StationID", "Type", "ParentNode", "NextNode", "Distance", "Port ID", "PortType", "Material");
        string sttype, portid, porttype, material;
        uint32_t stid, parent, next;
        float offset;
        auto getStationType = [](string key) {
            if (key=="DUAL_ACCESS") return StationData::Type::DualAccess;
            return StationData::Type::Unknown;
        };
        auto getPortType = [](string key) {
            if (key=="EQ") return StationData::PortType::EQ;
            else if  (key == "LSTB") return StationData::PortType::LSTB;
            else if  (key == "RSTB") return StationData::PortType::RSTB;
            return StationData::PortType::Unknown;
        };
        auto getMaterialType = [](string key) {
            if (key=="FOUP") return StationData::Material::FOUP;
            else if  (key == "RETICLE") return StationData::Material::RETICLE;
            return StationData::Material::Unknown;
        };

        while (datafile.read_row(stid, sttype, parent, next, offset, portid, porttype, material)) {
            station_map.emplace(std::make_pair(stid, StationData{
                    stid, parent, next, offset, getStationType(sttype), getPortType(porttype),portid,getMaterialType(material),nullptr}));
        }
        return true;
    }

    bool OHTMap::loadTeaching(string path)
    {
        io::CSVReader<8, io::trim_chars<' ', '\t'>, io::no_quote_escape<'\t'>> datafile(path);
        datafile.read_header(io::ignore_extra_column, "StationID", "PortType", "Hoist", "Shift", "Rotate", "PIOType", "PIODirection", "PIOID");
        string port_type, pio_type, pio_dir;
        uint32_t stid, pio_id;
        float hoist, shift, rotate;
        auto getPortType = [](string key) {
            if (key=="EQ") return TeachData::EQ;
            return TeachData::Unknown;
        };
        auto getPIOType = [] (string key) {
            if (key=="RF") return TeachData::RF;
            else if (key=="IR") return TeachData::IR;
            return static_cast<TeachData::PIOType>(TeachData::Unknown);
        };
        auto getPIODir = [] (string key) {
            if (key=="Right") return TeachData::Right;
            else if (key=="Left") return TeachData::Left;
            return static_cast<TeachData::PIODir>(TeachData::Unknown);
        };
        while (datafile.read_row(stid, port_type, hoist, shift, rotate, pio_type, pio_dir, pio_id)) {
            teach_map.emplace(std::make_pair(stid, TeachData{stid, getPortType(port_type),
                                                              hoist*0.1f, shift*0.1f, rotate*0.1f, getPIOType(pio_type),
                                                              getPIODir(pio_dir), pio_id }));
        }
        return true;
    }

    bool OHTMap::loadCID(string path)
    {
        io::CSVReader<6, io::trim_chars<' ', '\t'>, io::no_quote_escape<'\t'>> datafile(path);
        datafile.read_header(io::ignore_extra_column, "LayoutID", "LayoutType", "ResetNode", "StopNode1", "StopNode2", "ResetNodeType");
        string cid, layout_type, reset_node_type;
        uint32_t reset_node, stop_node1, stop_node2;
        auto getLayoutType = [](string key) {
            if (key=="NormalCID") return CIDData::NormalCID;
            else if (key=="SlopeCID") return CIDData::SlopeCID;
            return CIDData::Unknown;
        };
        auto getResetNodeType = [] (string key) {
            if (key=="NormalNode") return CIDData::NormalNode;
            else if (key=="CommonNode") return CIDData::CommonNode;
            return static_cast<CIDData::ResetNodeType>(CIDData::Unknown);
        };

        while (datafile.read_row(cid, layout_type, reset_node, stop_node1, stop_node2, reset_node_type)) {
            cid_map.emplace(std::make_pair(cid, CIDData{cid, getLayoutType(layout_type),
                                                         reset_node, stop_node1, stop_node2,
                                                         getResetNodeType(reset_node_type) }));
        }
        return true;
    }

    void OHTMap::mapStationToLink()
    {
        auto insertInOrder = [](std::list<StationData*>& l, StationData*e) {
            bool found = false;
            for(auto it = l.begin(); it != l.end(); ++it) {
                if (e->offset < (*it)->offset) {
                    l.insert(it, e);
                    found = true;
                    break;
                }
            }
            if (!found) l.push_back(e);
        };

        char linkid[128];
        for(auto& st_kv : station_map) {
            if (!st_kv.second.isEqStation()) continue;
            sprintf(linkid, "%d_%d", st_kv.second.parent, st_kv.second.next);
            insertInOrder(link_map[string(linkid)].stations, &st_kv.second);
        }
    }

    void OHTMap::assignStationTeachData()
    {
        for(auto& st_kv : station_map) {
            auto teach_data = teach_map.find(st_kv.first);
            if (teach_data != teach_map.end()) {
                st_kv.second.teach = &teach_map[st_kv.first];
            }
        }
    }

    bool OHTMap::loadMapData(const string data_dir)
    {
        SEP_REQUIRE_RET(loadNode(data_dir + "/node.tsv"), false);
        SEP_REQUIRE_RET(loadLink(data_dir + "/link.tsv"), false);
        SEP_REQUIRE_RET(loadStation(data_dir + "/station.tsv"), false);
        SEP_REQUIRE_RET(loadTeaching(data_dir + "/teaching.tsv"), false);
        SEP_REQUIRE_RET(loadCID(data_dir + "/cid.tsv"), false);
        mapStationToLink();
        assignStationTeachData();
#if 0 // for debug
        for(auto& kv : link_map) {
            if (kv.second.stations.size() > 0) {
                printf("Link %s :", kv.first.c_str());
                for(auto &sta : kv.second.stations) {
                    printf("(%d, %d, %.1f) ", sta->parent, sta->next, sta->offset);
                }
                printf("\n");
            }
        }
#endif
        return true;
    }

    double OHTMap::evaluatePathCost(Path& path)
    {
        return calculatePathLength(path);
    }

    bool OHTMap::findPath(uint32_t start_node, uint32_t target_node, Path& path, double known_min_cost)
    {
        // check if start node is valid
        SEP_REQUIRE_RET(node_map.find(start_node) != node_map.end(), false);
        // add node until facing a branch
        while (true){
            path.push_back(start_node);
            if (evaluatePathCost(path) >= known_min_cost) return false;
            if (start_node == target_node) return  true;
            if (node_map[start_node].link_count > 1) break;
            start_node = node_map[start_node].link[0]->to;
        }
        // evaluate branch case
        double low_cost = std::numeric_limits<double>::infinity();
        Path shortest_path;
        for(int i = 0; i < node_map[start_node].link_count; ++i) {
            auto node = node_map[start_node].link[i]->to;
            // skip loop path
            if (std::find(path.begin(), path.end(), node) != path.end())  continue;
            // check path recursively and update shortest path if required
            Path newpath(path);
            if (findPath(node, target_node, newpath)) {
                double cost = evaluatePathCost(newpath);
                if (cost < low_cost) {
                    low_cost = cost;
                    shortest_path = std::move(newpath);
                }
            }
        }
        if (shortest_path.size() > 1) {
            path = std::move(shortest_path);
            return true;
        }
        return false;
    }
}