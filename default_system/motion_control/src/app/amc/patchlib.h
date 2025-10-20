#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <dirent.h>
#include <vector>
#include <fstream>

#define PASS_OK 0
#define ERROR_CODE 1
#define MAX_LENGTH 512
namespace amc{
    class patchlib
            {
            private:
                bool autopatch_flag=false;
                std::vector<std::string> amc_file;
                std::vector<std::string> ecmaster_file;
                std::vector<std::string> targetpatch_file;
                std::vector<std::string> slavefw_file;
                std::vector<std::string> slavepara_file;
                std::vector<std::string> servofw_file;
                std::vector<std::string> servopara_file;
                std::string exec(const char* cmd);
            public:
                patchlib();
                ~patchlib();

                int setautopatch();
                void clearautopatch();
                void versionsave();
                void backuppatch();
                int listcheck();
                void dopatch();
                void reboot();
                void restart();
                void MakeTempFolder();
                void doslavepatch();
            };
}
