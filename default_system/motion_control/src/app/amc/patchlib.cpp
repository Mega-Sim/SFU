//
// Created by SB.Hong on 2022.03.03.
//

#include "patchlib.h"
#include "log.h"
using namespace amc;

    patchlib::patchlib(){
        mode_t old_mask = umask(0);
        char strFolderPath[] = { "/home/semes/BackupPatch" };
        int nResult = mkdir( strFolderPath ,0777);
        umask(old_mask);
    }

    patchlib::~patchlib(){
    }
    // c++ execute linux command
    std::string patchlib::exec(const char* cmd) {
        std::array<char, 128> buffer;
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return result;
    }

    void patchlib::MakeTempFolder(){
        mode_t old_mask = umask(0);
        char strFolderPath[] = { "/home/semes/PatchTemp" };
        int nResult = mkdir( strFolderPath ,0777);
        umask(old_mask);
    }

    int patchlib::setautopatch(){
        if(autopatch_flag==false){
            autopatch_flag=true;
            return PASS_OK;
        } else {
            return ERROR_CODE;
        }
    }

    void patchlib::clearautopatch(){
        autopatch_flag=false;
    }

    void patchlib::versionsave(){
    //미사용
    }

    void patchlib::backuppatch(){
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        char cmd[200];
        //sprintf(cmd,"cp /home/semes/work/bin/ecmaster /home/semes/BackupPatch/%d_%d_%d_%d_%d_%d_ecmaster",tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,tm.tm_hour, tm.tm_min, tm.tm_sec);
        sprintf(cmd,"cp -f /home/semes/work/bin/ecmaster /home/semes/BackupPatch/ecmaster");
        exec(cmd);
        //sprintf(cmd,"cp /home/semes/work/bin/amc_oht /home/semes/BackupPatch/%d_%d_%d_%d_%d_%d_amc_oht",tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,tm.tm_hour, tm.tm_min, tm.tm_sec);
        sprintf(cmd,"cp -f /home/semes/work/bin/amc_oht /home/semes/BackupPatch/amc_oht");
        exec(cmd);
    }

    int patchlib::listcheck(){
        DIR *dir;
        struct dirent *ent;
        std::vector<std::string> filelist;

        //clear patch file list
        amc_file.clear();
        ecmaster_file.clear();
        targetpatch_file.clear();
        slavefw_file.clear();
        slavepara_file.clear();
        servofw_file.clear();
        servopara_file.clear();

        //find shared folder file list
        if ((dir = opendir ("/home/semes/Sharepatch")) != NULL) {
          /* print all the files and directories within directory */
          while ((ent = readdir (dir)) != NULL) {
            printf ("%s\n", ent->d_name);
            std::string temp_str(ent->d_name);
            if(temp_str==".." | temp_str=="."){
            }else{
                filelist.push_back(temp_str);
            }
          }
          closedir (dir);
        } else {
          return false;
        }

        //find patch file
        for(auto iter = filelist.begin(); iter!=filelist.end();iter++){
            std::string cmd;
            //압축파일 내 파일명 확인
            cmd= "sudo tar tvf /home/semes/Sharepatch/"+*iter+" | grep amc_oht | awk '{print $6}'";
            if(exec(cmd.c_str()).find("amc_oht")!=std::string::npos){
                amc_file.push_back(*iter);
            }

            cmd = "sudo tar tvf /home/semes/Sharepatch/"+*iter+" | grep ecmaster | awk '{print $6}'";
            if(exec(cmd.c_str()).find("ecmaster")!=std::string::npos){
                ecmaster_file.push_back(*iter);
            }
            //amc, ecmaster 이외의 파일은 복사 명령어가 든 targetpatch.txt파일과 묶어서 .tar.gz로 압축하여 sharefoldder에 받는다 (yml파일 등)
            cmd = "sudo tar tvf /home/semes/Sharepatch/"+*iter+" | grep targetpatch | awk '{print $6}'";
            if(exec(cmd.c_str()).find("targetpatch")!=std::string::npos){
                targetpatch_file.push_back(*iter);
            }

            /*
            cmd = "sudo tar tvf /home/semes/Sharepatch/"+*iter+" | grep slave_fw | awk '{print $6}'";
            if(exec(cmd.c_str()).find("slave_fw")!=std::string::npos){
                slavefw_file.push_back(*iter);
            }

            cmd = "sudo tar tvf /home/semes/Sharepatch/"+*iter+" | grep slave_para | awk '{print $6}'";
            if(exec(cmd.c_str()).find("slave_para_")!=std::string::npos){
                slavepara_file.push_back(*iter);
            }

            cmd = "sudo tar tvf /home/semes/Sharepatch/"+*iter+" | grep servo_fw | awk '{print $6}'";
            if((exec(cmd.c_str()).find("servo_fw")!=std::string::npos)){
                servofw_file.push_back(*iter);
            }

            cmd = "sudo tar tvf /home/semes/Sharepatch/"+*iter+" | grep servo_para_ | awk '{print $6}'";
            if(exec(cmd.c_str()).find("servo_para")!=std::string::npos){
                servopara_file.push_back(*iter);
            }
            */
        }
        return true;
    }

    void patchlib::dopatch(){
        std::string cmd;
        cmd = "sudo chmod 775 /home/semes/Sharepatch/tool/Autopatch.sh";
        exec(cmd.c_str());

        cmd = "sudo -S gnome-terminal --working-directory=/home/semes/Sharepatch/tool/ -e ./Autopatch.sh";
        exec(cmd.c_str());
        /*
        if(ecmaster_file.size() == 1){//파일이 하나일때만 패치하고 지운다 2개이상이면 모호해서 처리하지 않는다.
            cmd = "sudo tar xvf /home/semes/Sharepatch/"+ecmaster_file.at(0)+" -C /home/semes/work/bin/";
            exec(cmd.c_str());
            cmd = "rm /home/semes/Sharepatch/"+ecmaster_file.at(0); //patch tar.gz file delete
            exec(cmd.c_str());
            SEP_LOGT("Patch ECMaster");
        } else if(ecmaster_file.size() == 0){
            SEP_LOGE("No ecmaster patch file");
        }

        if(amc_file.size() == 1){//파일이 하나일때만 패치하고 지운다 2개 이상이면 모호해서 처리하지 않는다.
            cmd = "sudo tar xvf /home/semes/Sharepatch/"+amc_file.at(0)+" -C /home/semes/work/bin/";
            exec(cmd.c_str());
            cmd = "rm /home/semes/Sharepatch/"+amc_file.at(0);//patch tar.gz file delete
            exec(cmd.c_str());
            SEP_LOGT("Patch AMC");
        }else if(amc_file.size() == 0){
            SEP_LOGE("No AMC patch file");
        }

        if(targetpatch_file.size() == 1){//파일이 하나일때만 패치하고 임시 폴더 삭제한다
            std::ifstream ifile;
            char line[255];

            //임시 폴더에 압축 해제 후 targetpatch.txt 파일에 적힌 명령을 수행한다
            //patchlib::MakeTempFolder();
            MakeTempFolder();
            cmd = "sudo tar xzvf /home/semes/Sharepatch/targetpatch.tar.gz -C /home/semes/PatchTemp/";
            exec(cmd.c_str());
            SEP_LOGT("Target patch");
            // 텍스트 파일 내용은 다음과 같이 /home/semes/PatchTemp에 있는 파일을 타겟 폴더 복사하는 명령어 형식으로 전체 파일 덮어쓰기 명령어를 적는다.
            // 예: sudo cp -f /home/semes/PatchTemp/oht_eni_model.yml /home/semes/work/bin/

            usleep(1000000);
            ifile.open("/home/semes/PatchTemp/targetpatch.txt");
            if(ifile.is_open()){
                while (ifile.getline(line,sizeof(line))){
                    cmd = line;
                    exec(cmd.c_str());
                }
            }
            ifile.close();

            cmd = "rm -rf /home/semes/PatchTemp"; //delete temp folder
            exec(cmd.c_str());
            cmd = "rm /home/semes/Sharepatch/"+targetpatch_file.at(0); //patch tar.gz file delete
            exec(cmd.c_str());
        }else if(targetpatch_file.size() == 0){
            SEP_LOGE("No targetpatch file");
        }*/
    }

    void patchlib::doslavepatch(){
        std::string cmd;
/*
        SEP_LOGT("Close AMC/EC-Master and run Servo/SlaveBD Patcher");
        // Servo firmware - D8
        cmd = "sudo mv /home/semes/Sharepatch/D8N_*.bin /home/semes/Slave_Autopatcher/";
        exec(cmd.c_str());
        // Servo firmware - CSD7N
        cmd = "sudo mv /home/semes/Sharepatch/CSD7N_*.bin /home/semes/Slave_Autopatcher/";
        exec(cmd.c_str());
        // Servo Parameter
        cmd = "sudo mv /home/semes/Sharepatch/servopara_*.txt /home/semes/Slave_Autopatcher/";
        exec(cmd.c_str());
        // Slave firmware
        cmd = "sudo mv /home/semes/Sharepatch/firmware_*.bin /home/semes/Slave_Autopatcher/";
        exec(cmd.c_str());
        // Slave Parameter
        cmd = "sudo mv /home/semes/Sharepatch/slavepara_*.yaml /home/semes/Slave_Autopatcher/";
        exec(cmd.c_str());

        cmd = "sudo -S gnome-terminal --working-directory=/home/semes/Slave_Autopatcher/ -e ./Slave_autopatch.sh";
        exec(cmd.c_str());
        */
    }

    void patchlib::reboot(){
        std::string cmd;
        cmd = "sudo shutdown -r";
        exec(cmd.c_str());
    }

    void patchlib::restart(){
        std::string cmd;
        SEP_LOGT("Restart!");
        cmd = "gnome-terminal -- sudo /home/semes/restart.sh";
        exec(cmd.c_str());
    }
