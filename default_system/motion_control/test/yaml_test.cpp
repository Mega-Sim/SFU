//
// Created by sem1 on 17. 8. 20.
//

#include <string>
#include <iostream>
#include <exception>
#include <yaml-cpp/yaml.h>
#include <list>
#include <map>
#include <array>
using namespace std;

namespace YAML {
    template<typename T, unsigned long N>
    inline Emitter &operator<<(Emitter &emitter, const std::array<T,N> &v) {
        return EmitSeq(emitter, v);
    }
}

int main()
{
    try {
        cout << "# Read Test\n";
        auto config_file = YAML::LoadFile("../../config/stocker_config.yml");
        auto config = config_file["config"];
        auto axes = config["axes"];
        cout << "typename:" << config["type"].as<string>() << endl;
        cout << "axis count: " << config["axis_count"].as<int>() << endl;
        for(auto key : {"Z", "X", "R", "Y"}) {
            auto axis = axes[key];
            cout << key << ".id:" << axis["id"].as<int>() << endl;
            cout << key << ".p_max:" << axis["p_max"].as<double>() << endl;
        }
    } catch (std::exception &e) {
        cout << "Error! " << e.what() << endl;
    }

    cout << "# Write Test\n";
    YAML::Emitter out;
    map<string,int> testmap = {{"a",1}, {"b",2}};
    list<int> testlist = {1,2,3,4,5};
    array<int,5> testarray = {1,2,3,4,5};

    out << YAML::BeginMap;
    out << YAML::Key << "str" << YAML::Value << "Hello world!";
    out << YAML::Key << "map" << YAML::Value << testmap;
    out << YAML::Key << "list" << YAML::Value << testlist;
    out << YAML::Key << "array" << YAML::Value << testarray;
    out << YAML::EndMap;
    cout << out.c_str() << endl;

    return 0;
}
