/**
\page pg_backgrounds Backgrounds

\ref main_toc "back"


\section bg_sec_overview 개요
 Sephirot Master 제어기 SW는 EtherCAT master stack(acontis 혹은 etherlab)이 구동가능한 환경(Linux(kerenl 4.4 이상 realtime patched)이 필요하다.

 마스터 제어기 SW는 크게 EtherCAT master 모듈과 Motion 및 IO control 모듈, Application 모듈로 나뉜다.
 master 모듈은 독립된 실행파일로 구동되며, motion 및 io 모듈은 library, 제어 application은 별개의 실행파일을 구성한다.

 제어기 성능을 최대한 끌어내기 위해 Master SW의 많은 부분은 Modern C++로 작성되어 있다.
 그렇지만 성능 영향을 적게 받는 상태 모니터링 및 제어기 관리는 확장성 및 생산성을 위해서 python과
 javascript를 활용하고 있다.
 Portability를 위해 CMake를 빌드 시스템으로 채용하고 있다.

 또한 Linux 개발환경의 특성상 shell 인터페이스를 통해 실행 및 디버깅이 이뤄지며, 다양한 자동화 툴이 사용된다.

 Master제어기 개발자들은 Linux 기반 개발환경에 익숙할 수록 개발 생산성이 높아진다.



\section os_shell  Linux, Shell
\subsection bg_ssec_os Linux
- OS 설치

  일반적으로 배포판 이미지를 이용하여 OS를 설치할 수 있다.
  배포판 이미지는 [Ubuntu Download](https://www.ubuntu.com/download)페이지에서 다운로드 가능하다.
  제어기 SW 개발환경 구성을 위해서는 Desktop용을 추천한다.
  배포판 버전은 LTS(Long Term Support) 버전을 사용하는 것이 장기적으로 볼 때 유리하다.
  현재는 18.04TLS 버전을 사용할수 있다.

  이미 리눅스가 설치되어 있다면 다음 명령으로 배포판 버전을 확인할 수 있다.
  \code{.sh}
  cat /etc/issue
  \endcode

  Ubuntu외에 다른 배포판도 사용가능할 것으로 예상되지만 테스트 되지 않았다.
  이후 설명은 Ubuntu 배포판을 기준으로 설명 될 것이다.


- Kernel 버전 확인
  EtherCAT master의 원활한 구동을 위해서는 Realtime Linux 환경이 필요하다.
  현재 개발환경의 Linux kernel 버전은 다음 명령을 통해 확인 가능하다.
  \code{.sh}
  uname -r
  \endcode
  실시간 커널이 아니라면 커널 변경이 필요하다.

- Kernel 확보

  Linux Kernel Project는 가장 큰 규모의 Opensource Project 중의 하나이며 매우 활발하게 개발이 진행된다.
  제어기용 커널은 최신 커널버전 보다는 안정적인 커널을 활용하는 것이 좋다.
  [Release Notes](https://www.kernel.org/category/releases.html) 페이지에서 Longterm release kernel
  버전 확인 가능하다. 현시점(2018/5/8)에서는 4.4 버전이 가장 오랫동안 유지보수 예정인 longterm kernel이다.

  커널 소스는 https://mirrors.edge.kernel.org/pub/linux/kernel/v4.x/ 에서 다운로드 가능하다.
  [linux.4.4.131.tar.xz](https://mirrors.edge.kernel.org/pub/linux/kernel/v4.x/linux-4.4.131.tar.xz) 파일을 다운 받는 경우를
  가정하고 이후 과정을 진행한다.
  root권한을 가진 상태에서 다운 받은 파일을 /usr/src/로 옮기고 다음을 수행한다.
  \code{.sh}
  tar xvfJ linux.4.4.131.tar.xz
  \endcode
  linux-4.4.131 폴더가 생성되어야 한다.

- Kernel Patch

  실시간 리눅스 사용을 위해서는  vanilla kernel(아무 패치가 이뤄지지 않은 커널)에 rt-patch를 수행해야 한다.
  https://mirrors.edge.kernel.org/pub/linux/kernel/projects/rt/ 하위 폴더에서
  다운받은 커널 버전에 해당하는 패치 파일을 얻을 수 있다.
  [patch-4.4.131-rt147.patch.xz](https://mirrors.edge.kernel.org/pub/linux/kernel/projects/rt/4.4/patch-4.4.131-rt147.patch.xz) 파일을 다운 받는다.
  패치 파일을 /usr/src로 옮기고 다음을 실행하여 패치를 적용한다.
  \code{.sh}
  cd linux-4.4.131
  xzcat ../patch-4.4.131-rt147.patch.xz | patch -p1
  \endcode

- Kernel 빌드 환경 설정

  배포 가능한 커널 빌드를 위해서는 다음 패키지를 설치해야 한다.
  \code{.sh}
  sudo apt install build-essential libncurses5 libncurses5-dev bin86 kernel-package -y
  \endcode

- Kernel configuratoin

  커널 컴파일 전에 커널 configuration이 필요하다.
  일반적으로 현재 시스템에 사용중인 configuration 파일을 복사하여 필요한 부분만 수정하는 것이 안전하다.
  \code{.sh}
  sudo cp /boot/config-현재커널명 ./.config
  \endcode
  menuconfig를 실행하여 RT Patch 등 필요한 설정을 변경한다.
  RT patch를 위해서는 [이 페이지](https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/preemptrt_setup)를 참고해도 좋다.
  \code{.sh}
  make menuconfig
  \endcode

- Kernel Compile

  다음 명령을 실행하여 커널 패키지를 빌드한다.
  \code{.sh}
  sudo make-kpkg --initrd --revision=1.0 kernel_image
  \endcode
  빌드가 성공하면 linux-image-4.4.131-rt147_1.0_amd64.deb 파일이 생성될 것이다.

- Kernel 설치

  deb 파일 설치 후 재부팅하면 된다.
  \code{.sh}
  sudo dpkg -i  linux-image-4.4.131-rt147_1.0_amd64.deb
  \endcode
  재부팅 후 'uname -r' 명령으로 버전 확인했을 때 4.4.131-rt147 버전이 아니면
  /boot/grub/grub/config 파일을 수정하여 4.4.131-rt147 버전이 선택되도록 한다.

  (grub 설정은 인터넷 참고)

- 시스템 정보
  Linux는 거의 모든 것이 파일로 관리되므로  시스템 정보도 특수 파일 시스템인 sysfs 및 proc을 통해 관리 및 모니터링 가능하다.
    + /proc
        /proc 하위에는 시스템 정보를 담고 있는 다양한 파일들이 있다.
        가령 cpu 정보를 알고 싶으면 다음을 실행하면 된다.
        \code{.sh}
        cat /proc/cpuinfo
        \endcode
        proc 하위의 숫자로 된 폴더는 각각의 pid(process id)에 해당하는 프로세스 정보가 반영되어 있다.
        가령 cmaline 은 커맨드라인 인자가, environ에는 환경변수 값들이 들어 있다.
    + /sys
        sysfs는 파일시스템 기반으로 시스템을 설정할 수 있는 인터페이스를 제공한다.
        내용이 많으므로 자세한 사항은 인터넷을 참고 바란다.

\subsection bg_ssec_shell Shell

- bash shell

  ubuntu의 기본 쉘이 bash이므로 기본적인 사항은 알아 두면 유용하다. \n
  하지만 우리는 bash shell의 설정 파일인 ~/.bashrc 파일을 이해하고 수정할 수 있는 수준이면 된다. \n
  가령 사용자 실행 파일 탐색 경로를 추가하고 싶으면 .bashrc 파일 끝에 다음을 추가하면 된다는 정도.
  \code{.sh}
  export PATH="추가경로:$PATH"
  \endcode
  자세한 bash 문법은 https://blog.gaerae.com/2015/01/bash-hello-world.html 참고


- tool 설치 : APT 활용

  Windows의 경우 Visual Studio만 설치하면 개발환경 셋업이 사실상 끝난다.
  하지만 Linux 개발환경은 수많은 작은 툴들이 모여 개발환경 생태계를 이룬다.

  Ubuntu 배포판의 경우 Debian 패키지 툴인 apt(advaned package tool)를 사용하여 프로그램을 설치할 수 있다.\n
  apt 사용법은 https://www.lesstif.com/pages/viewpage.action?pageId=24445574 를 참고 바란다.

  Ubuntu 14.04 이후에서 기본적인 사용법은 다음과 같다.
  \code{.sh}
  sudo apt install <패키지명>
  \endcode

  다음은 개발을 위해 일반적으로 설치하는 툴  중 몇가지다. Disk space절약을 위해서 필요할 때마다 설치하는 것이 좋다.
  \code{.sh}
  sudo apt install  build-essential autoconf libtool screen expect
  \endcode

- tool 설치 : 다른 방법

  apt로 설치 안되는 것들은 다음의 두가지 방법으로 설치 가능하다.
    + 설치 프로그램 다운로드 및 설치 : Anaconda3, CLion, PyCharm 등
    + 소스 다운로드 및 설치 : boost(1.66이상 버전), GRPC 등

- tool 내부 library 설치
  Python이나 Javascrip의 경우 내부적인 package manager를 이용하여 그 언어용 패키지를 설치할 수 있다.
    + python : pip 혹은 conda (anaconda 배포판의 경우)
    + javascript : npm

- 자주 쓰는 명령어
    + 일반 명령
        - man : 명령에 대한 reference manual을 보여줌        
            \code{.sh}
            man ls
            \endcode
        - whatis : 명령에 대한 간략한 설명을 보여줌        
            \code{.sh}
            whatis ls
            \endcode
        - which : 명령 파일 위치를 보여 줌
            \code{.sh}
            which ls
            \endcode
        - whereis : 명령의 binary, source, manual 위치를 보여줌
            \code{.sh}
            whereis ls
            \endcode
        - who : log on 되어 있는 사용자 목록을 보여 줌
        - whoami : effective userid를 출력함 
        - history : shell에서 사용한 명령어 기록을 보여줌 
        - clear : 화면을 지움. (windows에서의 cls)
        - sudo : super user권한으로 실행함, sudo user group에 속해 있어야 가능 
            \code{.sh}
            sudo su  # super user로 switch user 수행 
            \endcode
        - su : switch user, 특별한 인자가 없으면 super user(root)로 변경 
            \code{.sh}
            su sem  # sem으로 effective userid switch. sem의 password 필요 
            \endcode        
        - passwd : 현재 사용자의 password 변경 
        - echo : text 출력. 환경 변수 확인 등에 사용 
            \code{.sh}
            echo $PATH # 환경변수 PATH 내용 출력
            \endcode
    + 파일/폴더 관리
        - ls : 파일 list 출력 
            \code{.sh}
            ls
            ls /home
            ls -al
            \endcode
        - cd : change directory 
        - pwd : present working directory
        - mv : MoVe, 파일 이름 변경 or 파일 위치 변경 
            \code{.sh}
            mv a.out exe1
            \endcode
        - cp : CoPy, 파일 복사
            \code{.sh}
            cp -rp adir bdir  # adir을 bdir로 복사, -r 디렉토리, -p 모드 preserve
            \endcode
        - mkdir : MaKeDIRectory        
        - touch : 파일의 timestamp 변경. 파일이 없을 경우 빈 파일 생성 
            \code{.sh}
            touch file 
            \endcode
        - rm : ReMove, 파일제거 delete 
            \code{.sh}
            rm -rf adir # adir의 하위 폴더까지 삭제, -r 디렉토리 -f 강제수행
            \endcode
        - ln : LiNk, 링크 생성 
            \code{.sh}
            ln -s srcfile linkfile  # srcfile에 대한 symbolic link 생성 
            \endcode
        - chown : 파일에 대한 ownership 변경 
            \code{.sh}
            sudo chown -R sem .  # 현재 폴더 하위의 모든 파일과 폴더의 owner를 
            \endcode
        - chgrp : 파일의 group 변경 
        - chmod : 파일의 mode (r,w,x) 변경 
            \code{.sh}
            chmod +x a.sh  # a.sh를 실행 가능하게 변경 
            chmod 0557 a.sh # a.sh를 타 사용자도 읽기 및 실행 가능하게 변경 
            \endcode
        - find : 특정 패턴의 파일을 찾음 (파일 시스템을 직접 lookup)
            \code{.sh}
            find / -name '*.pl' # pl 확장자를 가진 모든 파일을 찾음 
            find / -name 'et*' -type d # et로 시작하는 모든 directory를 찾음 
            \endcode
        - locate : 특정 패턴의 파일을 찾음. (내부적으로 db활용, 매우 빠름)
            \code{.sh}
            locate *.pl # pl 확장자를 가진 모든 파일을 찾음 
            locate -i *.pl # ignore case옵션으로 pl 확장자를 가진 모든 파일을 찾음 
            locate -c *.pl # pl 확장자를 가진 파일 개수를 셈 
            \endcode
        - rename : 여러 파일의 이름 변경 (특정 패턴으로 변경시 mv보다 편리)
            \code{.sh}
            rename img img00 img?.jpg  # img?.jpg 패턴에 해당하는 파일들의 이름에서 img를 img00으로 변경 
            \endcode
    + Package 설치/관리
        - apt, apt-get, apt-cache, aptitude : ubuntu의 Advanced Package Tool 명령어
            \code{.sh}
            sudo apt install geany  # geany program 설치 
            sudo apt remove geany  # geany program 제거 
            apt-cache search geany  # apt db에서 geany 항목을 찾아 출력             
            \endcode
        - dpkg : Debian package manager 
            \code{.sh}
            sudo dpkg -i a.deb  # 패키지 설치 
            dpkg -L gcc  # gcc package 정보 출력 
            \endcode
        - pip, pip2, pip3, conda : python package manager, pip2는 python2, pip3는 python3용, ~/.pip/pip.conf에 default 옵션을 저장할 수 있음 
            \code{.sh}
            pip install --trusted-host pypi.org xmltodict # xml2dict package 설치 
            \endcode
        - npm : javascript package manager 
            \code{.sh}
            npm install bower -g # 패키지 설치. -g는 global 
            \endcode
        - bower : javascript front-end package 관리자
            \code{.sh}
            bower install jquery bootstrap -save # jquery 및 bootstrap 설치  
            \endcode
    + 파일 보기, 편집
        - cat, zcat, bzcat, xzcat : 파일 내용을 stdout으로 출력, *cat은 압축파일 출력 
            \code{.sh}
            cat a.txt 
            zcat a.tar.gz 
            xzcat a.xz 
            \endcode
        - more : 페이지 단위로 파일 내용을 볼 수 있음 (편집 불가)
            \code{.sh}
            more a.txt
            \endcode
        - less : more와 유사한 프로그램, 좀더 편리         
        - head : 파일의 앞 부분을 보여 줌 
            \code{.sh}
            head -n 30 a.txt # 앞의 30라인만 보여줌 
            \endcode
        - tail : 파일의 뒷 부분을 보여줌
            \code{.sh}
            tail -n 30 a.txt # 파일의 마지막 30라인만 보여 줌 
            tail -f /var/log/kern.log # kern.log 파일을 계속 watch하면서 보여줌 
            \endcode
        - tee : 내용을 파일 및 stdout으로 출력함 
            \code{.sh}
            ls -al | tee files.txt  # ls -al의 실행 결과가 stdout 및 files.txt에 저장됨
            \endcode
        - grep, egrep, fgrep : 패턴 매칭된 라인만 출력해 줌 
            \code{.sh}
            grep -n -i -v -e 'ab' *.log # 현재 폴더의 *.log파일들 중에서 ab/Ab/aB/AB(-i)가 포함되지 않은(-v) 파일들을 줄번호와 함께(-n) 출력함
            \endcode        
        - sed : stream editor
            \code{.sh}
             echo "1234hello5678" | sed "s/hello/goodbye/" # hello를 goodby로 변경 
            \endcode
        - sort : 라인단위로 정렬 
        - unique : 중복된 부분 제거 
            \code{.sh}
            cat a.txt | sort -r | uniq # a.txt 내용을 역순으로(-r) sorting후 중복된 부분을 제거하고 출력 
            \endcode
        - wc : Word Count, 단어, 라인, 문자수를 계수한다
            \code{.sh}
            wc -l *.h *.cpp # 현재 폴더의 .cpp 파일 및  .h 파일의 라인 수를 계수한다
            \endcode
        - nano : shell 기반의 simple editor
        - vi : nano보다 기능이 다양한 에디터 
        - vim : vi improvement 버전, http://gyuha.tistory.com/157 참고             
        - gedit, geany : windows의 notepad++과 유사한 gui 기반 에디터 
        - meld : gui 기반 diff 툴. araxis merge 툴과 유사 
    + 파일 압축/해제
        - tar : tar archive utility 
            \code{.sh}
            tar cvf a.tar a_folder  # a_folder를 a.tar 파일로 archiving
            tar cvfz a.tar.gz a_folder  # a_folder를 tar한 후 gzip으로 압축 
            tar xvfj a.tar.bz2  # bzip2로 압축된 tar ball의 압축을 품 
            tar xvfJ a.tar.xz   # xz로 압축된 tar ball의 압축을 품 
            tar tvfz a.tar.gz   # 압축을 풀지 않고 내용만 확인 
            \endcode
        - gzip, ungzip : gzip 엔진 사용 압축 및 해제 
        - zip, unzip : ZIP 엔진 사용 압축 및 해제 
        - bzip2, bunzip2 : bzip2 엔진 사용 압축 및 해제 (gzip대비 압축률 높고 느림)
        - xz, unxz : xz엔진 사용 압축 및 해제 (압축률 가장 높으나 가장 느림)
        - 7z : 7-zip 엔진 사용 압축 및 해제 
        
    + 프로세스 관리
        - ps : 현재 process 목록을 출력 
            \code{.sh}
            ps      # 현재 쉘에서 실행한 프로세스 목록
            ps -Al   # 모든 프로세스 목록을 long 포맷으로 출력             
            \endcode
        - pstree : 모든 process들의 부모 관계를 알수 있도록 tree형식으로 출력 
        - top : process 목록을 지속적으로 보여줌 ()
        - htop : top의 impoved version 
        - kill, killall, pkill : 프로세스에 시그널 전달 
            \code{.sh}
            kill -9 1234 # pid가 1234인 프로세스에 9번 신호(강제 종료) 전달 
            killall -9 hello # process 이름이 hello인 모든 프로세스에 9번 신호 전달 
            \endcode
        - pgrep : 특정 pattern의 프로세스 id 출력 
        - nice, renice : 프로세스의 스케쥴링 우선순위를 변경 
        
    + shell, 원격접속
        - bash : GNU Boune-Again SHell, ubuntu의 기본 shell
        - zsh : bash보다 더 다양한 기능을 제공하는 shell중의 하나 
        - telnet : telnet protocol shell interface, telnet port(3389)는 일반적으로 막혀서 안씀 
        - ssh : ssh client interface 
            \code{.sh}
            ssh sem@master1     # master1에 sem 계정으로 로그인
            ssh sem@master1 "ls -al > files.txt" #master1에 sem 계정으로 로그인 후 ls -al 수행 결과를 files.txt에 저장 
            \endcode
        - exit : shell interface를 종료하는 명령         
        - scp : ssh 프로토콜을 이용한 원격 복사 
            \code{.sh}
            scp -r sem@master1:/home/sem/app . # master1의 /home/sem/app를 폴더째로 현재 디렉토리에 복사 
            \endcode
            
    + 커널
        - uname : 시스템 이름 출력
        - arch : cpu architecture 출력
        - lsmod : 커널 모듈 목록 출력 
        - insmod : 커널 모듈 load  
            \code{.sh}
            sudo insmod a.ko 
            \endcode
        - rmmod : 커널 모듈 unload
            \code{.sh}
            sudo rmmod a 
            \endcode
        - depmod : modules.dep 및 map 파일 생성             
        - modprobe : insmod/rmmod보다 다양한 방식으로 모듈 관리 하는 툴 
        - modinfo : 모듈 정보 제공 
        - dmesg : 커널 메시지 확인, tail /var/log/kern.log 수행 결과 동일 
        - sysctl : 커널 파라미터를 runtime에 변경 
        - reboot : 재부팅 명령 
            \code{.sh}
            sudo reboot 
            \endcode
        - shutdown : 시스템 종료 명령 
    + 디바이스, 파일시스템
        - lsblk : block device 목록 출력 
        - fdisk : disk partition table 편집 툴
        - resize2fs : 파티션 크기         
        - mkfs : 파일시스템 생성 
            \code{.sh}
            sudo mkfs -t ext4 /dev/sda1 # sudo mkfs.ext4 /dev/sda1 과 동일
            \endcode
        - mknod, mktemp : node파일, 임시파일 등 특수 파일 생성 
        - fsck : FileSystem ChecK 
        - mount, umount : 파일 시스템 마운트 및 마운트 해제 
            \code{.sh}
            sudo mount /dev/sda1 /mnt/disk1 # /dev/sda1를 /mnt/disk1로 마운트 
            sudo umount /mnt/disk1 # /mnt/disk1을 un mount 
            \endcode
        - df : disk별 space 사용량을 보여 줌        
        - du : 파일의 disk 사용량을 보여 줌 
        - dd : copy 파일 (low level), 특수 파일도 복사 가능 
            \code{.sh}
            dd if=/dev/sda1 of=dump.img bs=1024 count=10000 # /dev/sda1의 raw data를 10000kB 읽어 dump.img로 씀
            \endcode
        - sync : cache된 데이터를 filesystem에 씀. removable storage가 있을 경우 유용.
        
    + 네트워크
        - ifconfig, ifup, ifdown, ifquery : network interface  정보 조작 
            \code{.sh}
            ifconfig                        # interface 목록 조회
            ifconfig enp5s0 192.168.0.1 up  # enp5s0 interface를 192.168.0.1 ip로 활성화
            ifconfig enp5s0 down            : enp5s0 interface 비활성화 
            \endcode
        - ping :  ICMP ECHO_REQUEST 메세지를 host로 보냄. 호스트와 연결 여부 확인.
        - tcpdump : network traffic을 dump함 
            \code{.sh}
            tcpdump -i eth0     #eth0으로 송수신 되는 데이터 패킷 덤프하여 확인
            tcpdump -i eth0 -w TCPDUMP  #특정 ethernet으로 송수신 되는 패킷들 파일에 저장 및 확인
            \endcode
        - wireshark : 네트워크 패킷 캡쳐 및 분석을 위한 gui 툴. shell만 사용 가능한 환경에서는 tcpdump로 패킷을 캡쳐 후 wireshark로 분석 가능  
        - nstat, netstat, lnstat : 네트워크 상태를 보여 줌 
            \code{.sh}
            netstat -tnlp   # 대기중인 모든 tcp port를 보여 줌
            \endcode
        - nmap : network mapper, 포트 스캐너 
            \code{.sh}
            netmap localhost  # 열려있는 모든 port를 보여 줌 
            \endcode
        - ip, iptables, iptables-save, iptables-restore, route : ip 설정관련 명령        
        - nslookup, host : dns 조회 
            \code{.sh}
            nslookup www.naver.com
            \endcode
        - nc : net-cat, network stream을 cat함 
            \code{.sh}
            nc -l 1024 | sudo tar -xvpzf - -C / #  1024포트로 오는 데이터를 tar로 /폴더에 저장 
            cat rfs_backup.tar.gz | nc -q 0 host1 1024 # rfs_backup.tar.gz파일을 host1에 1024포트로 streaming
            \endcode
        - socat : SOcket CAT, 다목적 stream relay 툴 
            \code{.sh}
            socat  pty,link=/dev/ttyPort0,  tcp:localhost:40000 #/dev/ttyPort0와 tcp:40000 포트를 relay함 
            \endcode        
        - ser2net : serial 포트를 network으로 redirection함, /etc/ser2net.conf 파일로 설정 
        - rsync :  원격 백업 툴
            \code{.sh}
            rsync -avz /home/sem/ sem1@master1:/home/sem1/backup # /home/sem/폴더 내용을 master1의 /home/sem1/backup에 동기화 한다
            \endcode
        
    + 개발 툴
        - gcc, g++, cc, c++ :  c/c++ 컴파일러 
            \code{.sh}
            g++ -o myprg  myprg.cpp mylib.cpp -Imyincpath1 -Lmylibpath1  -D_MYDEF1_ -lmylib  # 간단 예제 
            \endcode
        - ld : link 및 loader 
        - ldd : library 의존성 체크 
            \code{.sh}
            ldd a.out 
            \endcode
        - ldconfig : dynamci link 경로를 설정함. /etc/ld.so.conf 파일 사용 
            \endcode
            ldconfig -v
            \endcode
        - ar : static library 파일 생성
            \code{.sh}
            ar r libbs.a mylib.o yourlib.o
            \endcode
        - cmake : cmake 인터페이스 
            \code{.sh}
            cmake -DDefine1 .. 
            \endcode
        - make : make 인터페이스 
            \code{.sh}
            make -j4 ecmaster    
            \endcode
        - python, python2, python3 : python interpreter 인터페이스
        - nodejs : javascript commandline 인터페이스 
        - java, javac, jar : java compiler 및 유틸리티 
        - gdb, ddd :  gnu debugger 및 gui 버전 
        - diff : 파일 diff 툴, 패치 생성에 사용 
            \code{.sh}
            diff -uNr hello.c.orig hello.c > hello-1.patch
            \endcode
        - patch : patch 툴 
            \code{.sh}
            patch -p1 < ../xchat-1.2.0-serverlist.patch
            \endcode
        - nm : 심볼 정보 추출 
            \code{.sh}
            nm a.out
            \endcode
        - readelf : elf 파일 정보 추출 
            \code{.sh}
            readelf -a a.out
            \endcode
        - objdump : 바이너리 파일 정보 추출 
            \code{.sh}
            objdump -d a.out    #disassemble
            \endcode
        - c++filt : c++ symbol을 demangle함            
        - svn : subversion command line client tool
        - git : git command line client tool
        - doxygen, doxywizard : doxygen command line tool 및 gui 버전        
        - dot : dot 스크립트로 그래프 생성, https://www.graphviz.org/pdf/dotguide.pdf
            \code{.sh}
            dot -Tpng hello.dot
            \endcode
        - mscgen : message sequence chart 생성, http://www.mcternan.me.uk/mscgen/
            \code{.sh}
            mscgen -i test.msc -Tpng 
            \endcode
        - sysbench : 시스템 성능 벤치마크 
            \code{.sh}
            sysbench --test=cpu --cpu-max-prime=20000 run
            sysbench --test=memory --memory-block-size=1K --memory-scope=global --memory-total-size=100G --memory-oper=read run
            \endcode
        - cyclictest : 실시간 성능 확인 프로그램 
            \code{.sh}
            cyclictest -p80 -t5     # 실시간 우선순위 80을 가진 5개의 thread를 생성하여  jitter 통계
            \endcode
        - minicom : serial terminal 인터페이스 
            \code{.sh}
            sudo minicom -s 
            \endcode
    + 기타
        - date : 날짜 확인 및 설정 
            \code{.sh}
            date
            sudo date -s "2018/5/8 13:50:10"
            \endcode
        - hwclock : HW clock(RTC)을 읽고 씀 
            \code{.sh}
            sudo hwlock -w  # 현재 시간을 RTC에 저장 
            \endcode
        - service : 서비스(데몬) 관리 툴 
            \code{.sh}
            service --status-all
            service myservice restart 
            \endcode
        - screen : terminal emulation을 통해 terminal(ssh, telnet등)연결이 종료되어도 프로그램이 종료되지 않게 한다.
            \code{.sh}
            screen -list    # screen 목록을 보여줌
            screen -S ses1  # ses1을 생성, 스크린에서 빠져나올 때는 Ctrl-a,d 
            screen -r ses1  # ses1에 재접속 
            \endcode            
        - expect : 자동화를 위한 interactive script 
        - sleep : 주어진 시간 만큼 대기 
        - cron : 주기적인 예약 작업 수행



\section ide_build 빌드 시스템, IDE 
\subsection bg_ssec_cmake cmake
- 개요

  cmake는 Cross-Platform Build를 지원하는 일종의 meta-make 툴이다.\n
  meta-make는 결과물이 최종 빌드산출물이 아니라 산출물을 생성하는 스크립트(Linux의 경우 Makefile)를
  생성하는 tool을 말한다.

  cmake는 단순 컴파일 뿐 아니라  조건부 컴파일, 라이브러리 빌드, pre/post build 이벤트 처리, 배포 등의 기능을 수행할 수 있다.


- 설치

   cmake는 3.10 이상의 최신 버전 설치를 추천한다.\n
   apt로 설치하면 낮은 버전이 설치되므로 https://cmake.org/download/ 소스를 다운 받아 설치한다. \n
   설치 방법은 https://cmake.org/install/ 참고

- 사용법 : 아래 사이트 참고
    + 소개 : https://tuwlab.com/ece/27234
    + 주요 명령 및 예약 변수 : https://tuwlab.com/ece/27260
    + CMakeLists.txt 기본 패턴 : https://tuwlab.com/ece/27270


\subsection bg_ssec_make make (gnumake)
make는 unix 계열에서는 매우 오래된 빌드 툴이지만 여전히 많이 사용되고 있는 개발 툴이다.\n
Master SW는 Makefile을 cmake를 통해 생성해 내므로 Makefile 문법을 알아야 할 필요가 없으므로 단순한 사용법만 숙지하면 된다.\n
CLion과 같은 IDE를 사용할 수 없는 환경에서 숙지해야하는 명령이다.

- 전체 빌드
  \code{.sh}
  make -j4
  \endcode
  참고로 -j4 옵션은 동시 수행가는 job수를 4로 하라는 옵션으로 빌드 속도를 높일 수 있다.
  job수는 CPU 코어수*2 - 1이 적정하다고 알려져 있다.

- 전체 clean
  \code{.sh}
  make clean
  \endcode

- 특정 모듈들만 빌드 (module1과 module2를 빌드 하는 경우)
  \code{.sh}
  make -j4 module1 module2
  \endcode

\subsection bg_ssec_clion CLion
JetBrain사의 Modern C++ IDE(Integrated Development Environment) 툴이다.\n
CMake+GnuMake 조합을 기본 빌드 시스템으로 사용한다.\n
최신 CMake 및 C++17 문법을 완벽하게 
Linux상에서 Visual Studio 수준의 개발 및 디버깅의 편리함을 제공하며, 
Plugin 설치를 통해 다양한 추가 기능을 사용할 수 있다.
https://www.jetbrains.com/clion/ 에서 상세한 정보를 얻을 수 있다.


\subsection bg_ssec_pycharm PyCharm
JetBrain사의 Python2 및 Python3 IDE(Integrated Development Environment) 툴로,
Python application개발에 가장 많이 사용되는 IDE이다.\n
Python엔진을 내장하고 있지 않으므로 Python은 Anaconda3 등을 별도로 설치해야 한다.\n
Community 버전의 경우 회사에서도 무료로 사용 가능하다.\n
https://www.jetbrains.com/pycharm/ 에서 상세한 정보를 얻을 수 있다.


\section bg_sec_languages Languages

\subsection bg_ssec_ Modern C++
C++은 C++11버전 이후 부터 매우 큰 변화가 있었다.
Modern C++은 기존 C/C++을 완전하게 포용하면서 Java나 C#, Python 등의 진화된 언어의 컨셉을 도입하였다.
첫째, 기존 smart pointer 클래스가 메모리 누수 문제를 완전히 해결가능한 smart pointer로 진화했다.
둘째, 메모리 재할당의 비용을 줄일 수 있도록 move semantic(r-reference)가 도입되었다.
셋째, auto 변수 타입 및 lambda, user-defied literal등 의 도입으로 생산성이 향상되었다.
그밖에 많은 신규 feature들이 포함되었으며, 다른 언어들 처럼 광범위한 Standard Library를 통해 이를 구현하고 있다.

- Moder C++ 소개 : https://www.slideshare.net/jacking/modern-c-cpp11-14
- 신규 feature 목록 :  https://github.com/AnthonyCalandra/modern-cpp-features
- reference : http://en.cppreference.com/w/ or http://ko.cppreference.com/w/ (불완전)

\subsection bg_ssec_python Python

파이썬 초보자에게 유용한 교육 사이트
* 생활코딩 : https://opentutorials.org/course/1750
* 코드아카데미 : https://www.codecademy.com/ko/tracks/python-ko
* 예제로 배우는 파이썬 : http://pythonstudy.xyz/python/basics

조금 익숙한 사람들은 reference 사이트
* https://docs.python.org/2/reference/
* https://matplotlib.org/ 
* https://pandas.pydata.org/ 
* https://www.scipy.org/
* http://scikit-learn.org/stable/


\subsection gb_ssec_web Web Programming 
\ref spg_web_program "link to Web Programming"    


*/
