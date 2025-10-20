#pragma once

namespace sephi {
    static std::map<unsigned, unsigned> num_map;
    static std::map<string, unsigned> name_map;
    static std::map<unsigned, string> portid_map;
    static std::set<unsigned> inversion_ports_;
    static std::set<unsigned> low_ports_;
    static std::unordered_map<unsigned, bool> io_val_map;
    static std::map<string, int> size_map;

    static string bit_inversion_str[] = {
        "_stop_driving_front",
        "_stop_driving_precheck",
        "_stop_trans_front",
        "_stop_trans_rear",

        "bcr_driving_ok",
        "bcr_driving_ng",
        "bcr_trans_ok",
        "bcr_trans_ng",

        "_obs_en",
        "_oht_en",
    };

    static string active_low_str[] = {
        // OhtOnboard DI
        "_stop_trans_front",  //active low
        "_stop_trans_rear",  //active low
        "_stop_driving_front",  //active low
        "_stop_driving_precheck",  //active low
    };

    class OhtIoConfig {
    public:
        unsigned getportid(string name);
        unsigned getportid(unsigned num);
        string getportstring(unsigned portid);
        void setportid(string name, unsigned num, unsigned portid);
        void modifyportid(string name, unsigned num, unsigned portid);
        void clear();
        int getportsize(string key);
        void setportsize(string io_name, int size);
        std::set<unsigned> get_inversion_port();
        std::set<unsigned> get_active_low_port();
    private :

    };
}