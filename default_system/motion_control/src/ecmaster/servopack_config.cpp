//
// Created by yj.choe on 2021-10-21.
//

#include <yaml-cpp/yaml.h>
#include "ecmaster_service.h"
#include <fstream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

using namespace std;
using namespace boost::filesystem;

namespace sephi
{
    void EcMasterSvc::ParamDownload()
    {
        char line[100];
        time_t now = time(NULL);
        struct tm *now_tm = localtime(&now);
        string filename;
        string year_ = to_string(now_tm->tm_year+1900);
        string mon_ = to_string(now_tm->tm_mon+1);
        string day_ = to_string(now_tm->tm_mday);
        string hour_ = to_string(now_tm->tm_hour);
        string min_ = to_string(now_tm->tm_min);
        filename = "backup/servoparam_" + year_ + "_" + mon_ + "_" + day_ + "_" + hour_ + "_" + min_ + ".txt";

        if(!exists("backup"))   create_directory( "backup" );
        sleep(1);
        ofstream fout(filename);

        const char* pwd1 = "qlalfqjsgh";
        const char* pwd2 = "setup123";

        usleep(10000);
        system("clear");
        cout << "Please enter your password" << endl << "> ";

        fflush(stdout);
        fgets(line, sizeof(line), stdin);
        const char* user_pwd(line);

        if(strncmp(pwd1, user_pwd, strlen(pwd1)) != 0 && strncmp(pwd2, user_pwd, strlen(pwd2)) != 0) {
            cout << "Password is wrong.. bye.." << endl;
            return;
        }
        fout << "[" << now_tm->tm_year+1900 << "/" << now_tm->tm_mon+1 << "/" << now_tm->tm_mday << " "
        << now_tm->tm_hour << ":" << now_tm->tm_min << ":" << now_tm->tm_sec << "]" << endl;
        fout << "Password : " << user_pwd << endl;

        system("clear");
        cout << "============================================" << endl;
        cout << " Start Parameter Download!" << endl;
        cout << "============================================" << endl << endl;
        sleep(1);
        try {
            string model_file = "RSA_Parameter.yml";
            auto config_file = YAML::LoadFile(model_file)["axes"];
            for (auto axis_name : {"Driving_front", "Driving_rear", "Hoist", "Slide"}) {
                auto slave_id = config_file[axis_name]["ID"].as<int>();
                auto &slave = slaves_[slave_id];
                CiA402Servo& servo = servos_[slave.context];
                fout << "< " << axis_name << " >" << endl;

                auto Data = config_file[axis_name]["Data"];
                if (Data.size() == 0) {
                    system("clear");
                    cout << "============================================" << endl;
                    cout << " " << axis_name << " Parameter is empty" << endl;
                    cout << "============================================" << endl;
                    sleep(1);
                }
                for (auto i = 0; i < Data.size(); i++) {
                    auto Index = Data[i]["Index"].as<uint16_t>();
                    auto SubIndex = Data[i]["SubIndex"].as<uint16_t>();
                    auto DataType = Data[i]["DataType"].as<uint16_t>();
                    fout << i+1 << ". " << hex << Index << "/" << dec << SubIndex << " : ";

                    // 다축 서보팩 sdo index 처리
                    if (axis_name == "Driving_front") {
                        Index += 0x1000;
                    }

                    bool retsdo = -1;
                    // 데이터 타입에 따라 Value 타입 변경
                    if (DataType == 8) {
                        auto Value = Data[i]["Value"].as<uint8_t>();
                        for (int j = 0; j < 5; ++j) {
                            retsdo = servo.setCoeSdoValue(Index, SubIndex, Value);
                            usleep(100000);
                            auto rtnVal = servo.getCoeSdoValue8(Index, SubIndex);
                            if (Value == rtnVal) {
                                fout << rtnVal << endl;
                                break;
                            }
                            cout << endl << "Retry..." << j << ", Index: " << Index << ", SubIndex: " << ", Value: " << rtnVal << endl;
                        }
                    } else if (DataType == 16) {
                        auto Value = Data[i]["Value"].as<uint16_t>();
                        for (int j = 0; j < 5; ++j) {
                            retsdo = servo.setCoeSdoValue(Index, SubIndex, Value);
                            usleep(100000);
                            auto rtnVal = servo.getCoeSdoValue16(Index, SubIndex);
                            if (Value == rtnVal) {
                                fout << rtnVal << endl;
                                break;
                            }
                            cout << endl << "Retry..." << j << ", Index: " << Index << ", SubIndex: " << ", Value: " << rtnVal << endl;
                        }
                    } else if (DataType == 32) {
                        auto Value = Data[i]["Value"].as<uint32_t>();
                        for (int j = 0; j < 5; ++j) {
                            retsdo = servo.setCoeSdoValue(Index, SubIndex, Value);
                            usleep(100000);
                            auto rtnVal = servo.getCoeSdoValue32(Index, SubIndex);
                            if (Value == rtnVal) {
                                fout << rtnVal << endl;
                                break;
                            }
                            cout << endl << "Retry..." << j << ", Index: " << Index << ", SubIndex: " << ", Value: " << rtnVal << endl;
                        }
                    }

                    if (retsdo != 1) {
                        printf("Fail to download sdo to slave %d: index = 0x%x, sub=%d\n", slave_id, Index, SubIndex);
                        return;
                    }
                    usleep(16);
                    system("clear");
                    cout << "============================================" << endl;
                    cout << " " << axis_name << " Parameter Downloading...(" << i+1 << "/" << Data.size() << ")" << endl;
                    cout << "============================================" << endl;
                    usleep(100000);
					
					if (Data.size() != 0 && i == Data.size()-1) {
						system("clear");
						cout << "============================================" << endl;
						cout << " " << axis_name << " Parameter Save..." << endl;
						cout << "============================================" << endl;
						servo.saveParams(COE_SAVE_SERVO_PARAM);
						sleep(1);
					}
                }
            }
            system("clear");
            cout << "============================================" << endl;
            cout << " Download and Save Complete!" << endl;
            cout << "============================================" << endl << endl;
            sleep(1);
        } catch(std::exception &e) {
            SEP_TRE("Config Loading Error : %s\n", e.what());
            return;
        }
    }
}
