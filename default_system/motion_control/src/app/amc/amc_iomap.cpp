//
// Created by yj.choe on 2022-04-20.
//
#include "amc_oht.h"

namespace amc {
    void Amc4OHT::InitializeIO() {
        printf("Initialize IO!\n");
        // enable barcoder readers
        writeDo(ioconfig_.getportid("bcr1_in2"), true);  //writeDo(bcr1_in2, true); //false);
        writeDo(ioconfig_.getportid("bcr2_in4"), true);  //writeDo(bcr2_in4, true); //false);
        writeDo(ioconfig_.getportid("qrr1_in1"), true);  //writeDo(qrr1_in1, true); //false);
        writeDo(ioconfig_.getportid("qrr2_in3"), true);  //writeDo(qrr2_in3, true); //false);

        writeDo(ioconfig_.getportid("_oht_en"), false);  //writeDo(_oht_en, false);
        writeDo(ioconfig_.getportid("_obs_en"), false);  //writeDo(_obs_en, false);
        writeDo(ioconfig_.getportid("oht_en_slave"), true);    // slave b/d oht power, for p4 mask
        writeDo(ioconfig_.getportid("obs_en_slave"), true);    // slave b/d obs power, for p4 mask
        writeDo(ioconfig_.getportid("driving_stop_in1"), true);  //writeDo(driving_stop_in1, true); //false);
        writeDo(ioconfig_.getportid("trans_stop_in2"), true);  //writeDo(trans_stop_in2, true); //false);
        writeDo(ioconfig_.getportid("LD_Power_EN"), false);  //writeDo(LD_Power_EN, false);
        writeDo(ioconfig_.getportid("CID_Power_EN"), false);  //writeDo(CID_Power_EN, false);

        writeDo(ioconfig_.getportid("_eq_pio_left_select"), true);  //writeDo(_eq_pio_left_select, true);
        writeDo(ioconfig_.getportid("_eq_pio_right_select"), false);  //writeDo(_eq_pio_right_select, false);

        //writeDo(LAN_PWR_EN, true); // insert 20190523 lsj
        writeDo(ioconfig_.getportid("Driving_Stop_EN"), true);  //writeDo(Driving_Stop_EN, true); //210603 yjchoe, STB L/R Power
        writeDo(ioconfig_.getportid("hand_pio_select"), true);  //writeDo(hand_pio_select, true); // 210813 yjchoe

        //release HW BB
        writeDo(ioconfig_.getportid("safety_control"), true);  //writeDo(safety_clear, true);
        sleep_for(50ms);
        writeDo(ioconfig_.getportid("safety_control"), false);  //writeDo(safety_clear, false);
        sleep_for(50ms);

        writeDo(ioconfig_.getportid("safety_clear"), true);  //writeDo(safety_clear, true);
        sleep_for(50ms);
        writeDo(ioconfig_.getportid("safety_clear"), true);  //writeDo(safety_clear, false);
        sleep_for(50ms);

        writeDo(ioconfig_.getportid("LAMP_RIGHT_M"), true);   // LAMP_RIGHT_M
        writeDo(ioconfig_.getportid("LAMP_LEFT_M"), true);   // LAMP_LEFT_M
    }
}
