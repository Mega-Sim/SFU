/**
\page pg_ecmaster EtherCAT Master 

\ref main_toc "back"


\section ec_general General theory

EtherCAT(Ethernet for Control Automation Technology) 기술은 Beckhoff사에서 
제안한 Ethernet 기반의 Field Bus 규약이다. 
필드 버스란 제어기와 센서나 액츄에이터 같은 슬레이브 장치를 
제어하기 위한 실시간 분산제어 네트워크를 통칭하는 개념이다. 

EtherCAT은 기존 필드 버스 규약에 비하여 다음과 같은 장점을 가지고 있으며, 
현 시점에서 자동화 시스템을 위해 가장 널리 쓰이는 필드 버스 중의 하나이다.
- Ethernet 기반으로 높은 대역폭 제공 : Full-duplex 200Mbps (참고, CAN 1Mbps 미만)
- ESC(EtherCAT Slave Controller)에 의한 Slave Node 고속 패캣 처리 (노드당 약 150ns)
- Open Protocol : Royalty 없이 누구나 프로토콜 이용 가능 
- Cable Redundancy 
- Distributed Clock 
- Up to 65535 slaves 

EtherCAT 네트워크는 하나의 Master와 하나 이상의 Slave들로 구성된다.
Slave들은 Master에 데이지체인 형태로 연결되는데, 
이더캣을 지원하는 스위칭 허브를 이용하면 Tree형태로도 연결 가능하다. 

일반적으로 Slave Node들은 패킷을 송수신하는 Ethernet PHY chip과, 
EtherCAT Protocol을 처리하는 ESC logic, ESC 설정을 담은 EEPROM(register),
Slave Appliction을 처리하는 MCU로 구성된다.
ESC 내부에는 MCU와 ESC의 데이터 동기화를 위한 SyncManager와 
Process 데이터 매핑을 위한 FMMU(Fieldbus Memory Management Unit)이 존재한다.


Slave에 있는 정보에 접근하거나 변경하는 것을 쉽게 하기 위해서 
외부로 접근 가능한 모든 정보들은 Object Dictionary 형태로 관리한다.
각 Object는 Index값과 Subindex값 및 Type 정보를 가진다. 
Master Appliction은 Slave Id, Index, Subindex 조합을 통해 네트워크상에서
Unique한 Object 접근이 가능하다.


Master는 매 주기마다 Ethernet Frame안에 EtherCAT Datagram을 담아 발송하며,
Slave들은 Frame을 수신받아 자기 Node에 해당하는 데이터만 처리하고 다음 Slave로 넘긴다.  Master와 Slave 장치들은 두가지 방식으로 통신 가능하다. 

첫번째 방식은 매주기마다 전송이 이뤄지는 Process Data를 통하는 방법이다. 
EtherCAT은 Process Data를 처리하기 위해서 
4GB의 주소 공간을 정의하고 Slave마다 독립적인 주소공간을 할당한다.
Master는 네트워크 초기화 단계에서 각 Slave들이 어떤 주소 공간을 사용할지 
결정하게 되는데, Process Data는 이 주소공간에 매핑된 데이터다.
다시말해서 Process Data로 교환하고 싶은 Slave Object들이 Field Memory로 매핑되도록 
Slave FMMU를 설정하는 것이다. 
Process Data는 네트워크 셋업 단계에서 Datagram이 정의되므로 Overhead가 없다.
따라서 이론적으로 Process Data만 활용시 EtherCAT은 90% 이상의 Band Width 활용이 가능하다. 

두번째 방식은 Mail Box(혹은 Service Data)를 통하는 방식이다. 
EtherCAT의 Process 주소 공간은 제한이 있고, 
Process Data는 실제 사용 여부와 관계없이 매 주기마다 주고 받게 되므로 
모든 데이터를 Process Data로 다루는 것은 비효율적이다.
EtherCAT은 이러한 불합리를 해결하기 위해서 Mail Box 통신이라는 이름의 
비주기적인 통신을 지원한다. 
Mail Box 통신에서는 매 Datagram마다 해당 Object 정보를 기입해야 하므로 Overhead가 
생기고, Slave Application에 의해 요청이 처리되어야 하므로 PDO보다 느리다. 
반면 전송 패킷 길이에 사실상 제약이 없어 다양한 프로토콜을 터널링하는 것이 가능하다.
다음은 Mail Box를 통한 서비스 예이다.
- EoE : Ethernet over EtherCAT 
- CoE : CANopen over EtherCAT
- FoE : File Access over EtherCAT 
- SoE : Servo Drive(SERCOS) over EtherCAT

Field bus에 연결되는 Slave들은 그 목적상 전형적인 구성 모델을 가진다.
가령 IO 노드, 서보 제어 노드 등이 존재할 수 있다.
EtherCAT에서는 이러한 Device들을 모듈화 하여 통제 가능하도록 
Device Profile이라는 개념을 도입하고,  
Object Dictionary의 Index값의 용도를 미리 규정하고 있다.
- 0x0000 – 0x0FFF Data Type Area
- 0x1000 – 0x1FFF: Communication Area
- 0x2000 – 0x5FFF: Manufacturer specific Area
- 0x6000 – 0x6FFF: Input Area
- 0x7000 – 0x7FFF: Output Area
- 0x8000 – 0x8FFF: Configuration Area
- 0x9000 – 0x9FFF: Information Area
- 0xA000 – 0xAFFF: Diagnosis Area
- 0xB000 – 0xBFFF: Service Transfer Area
- 0xC000 – 0xEFFF: Reserved Area
- 0xF000h – 0xFFFF: Device Area
서보제어를 위해서는 다양한 Device Profile들 중에서 
CiA402 Drive Profile은 살펴볼 필요가 있다.


EtherCAT 기술에 대한 자세한 설명은 다음 자료를 참고 하기 바란다.
- https://www.ethercat.org/en/technology.html
- <a href="https://www.yaskawa.com/delegate/getAttachment?documentId=SIEPS80000155&cmd=documents&openNewTab=true&documentName=Sigma-7S%20EtherCAT%20Manual_SIEP_S800001_55G_6_0.pdf">Yaskawa EtherCAT기반 서보팩 매뉴얼</a> "13장 CiA402 드라이브 프로파일" 

\section ec_master_stack EtherCAT Master Stack  
EtherCAT은 Master에 의해 제어되는 구조이므로 
EtherCAT network을 제어하는 Master Stack이 필요하다. 
Sephirot Platform은 Acontis사에서 개발한 상용 solution과,
Etherlab에서 개발한 Open source solution(2018년 5월 현재는 일부 기능만 지원)을 지원한다.

EtherCAT Master는 Network을 설정하기 위한 인터페이스와  Mailbox 통신 기반 서비스 
인터페이스를 제공해야한다.
- Network 설정 
- Network Lifecycle 제어 
- 메일박스 통신 기반 서비스 

\subsection ec_acontis Acontis stack

Acontis Stack은 ENI(EtherCAT Network Information)파일을 이용하여 네트워크를 설정할 수 있다.
ENI 파일은 같은 회사 제품인 EcEngineer나 Beckhoff사의 TwinCAT등을 이용하여 제작 가능하다. 
ENI 파일 제작을 위해서는 ESI(EtherCAT Slave Information) 파일이 필요하다. 
ESI 파일에는 Slave의 Object Dictionary 목록 및 가용한 서비스 정보가 포함되어 있다.
ENI 파일은 네트워크 구성에 관한 정보, 가령 Slave 연결 순서, 초기화 명령 및 PDO Map에 관한 정보가 포함된다.
ENI 작성 방법은 EcEngineer 설명서 혹은 별도 문서를 참고 바랍니다.


Sephirot EtherCAT master 모듈은 Acontis Demo Application과 거의 동일한 
Application 인터페이스를 가지고 있다. 
따라서 다음과 같은 데모 프로그램 인자를 사용 할 수 있다.

\code 
EcMasterDemoMotion [-f ENI-FileName] [-dcmmode] [link_layer_param]    
   -f                Use given ENI file
     FileName        file name .xml   
   -dcmmode          Set DCM mode
     off             Off
     busshift        BusShift mode (default)
     mastershift     MasterShift mode
     masterrefclock  MasterRefClock mode
     dcx             External synchronization mode   
   -mb               Enable mailbox communication only.
   -emul             Run ecmaster as emulation mode.
 <link_layer_param>
    -cpsw Instance Mode PortPriority MasterFlag [RefBoard CpswType PhyAddress PhyConnectionMode NotUseDmaBuffers]
    -i8254x Instance Mode
    -SockRaw device 
\endcode 

Acontis Stack의 경우 잘 정리된 매뉴얼이 존재하므로 자세한 설명은 생략한다.
Master Stack 담당자는 다음 문서들을 통독하길 바란다.
- EC-Master_QuickStart_Guide.pdf
- EC-Master_ClassB.pdf
- EC-Master_ClassA.pdf 

아래 그림은 Demo Application의 Life Cycle인데, Acontis stack을 사용할 경우
이와 유사한 Application 구조를 가지므로 참고 바란다.
\image html img/acontis.png "Typical Acontis Master Application Lifecycle"


\subsection ec_etherlab Etherlab stack

Etherlab EtherCAT Master solution은  
https://www.etherlab.org/en/ethercat/ 에서 다운 가능하다.
프로젝트 자체는 GPLv2로 라이센스 되어 있지만, 사용자들이 
User library를 이용하여 상용 제품에도 활용할 수 있도록 User library는 
LGPL로 라이센스 되고 있다. 

안타깝게도 버전 1.5.2에서 개발이 중단된후 더 이상 공식적인 관리가 이뤄지지 않고 있다. 
하지만 다양한 커뮤너티에서 이후 개발된 Network adapter나 커널을 지원하기 위한 활동을 하고 있다.

igb 디바이스 및 커널 4.13까지 패치된 버전을 아래 사이트에서 얻을 수 있다.
- https://sourceforge.net/p/etherlabmaster/code/ci/default/tree/


\subsubsection ec_etherlab_install Etherlab EtherCAT Master 설치 

-  빌드에 필요한 툴 및 linux kernel source 파일이 있어야 함
-  첨부된 소스 파일을 다운 받아 적당한 폴더에 푼다.
\code{.sh}
tar xvfJ etherlabsrc.patched.tar.xz
\endcode
- 새로 생성된 etherlab 폴더로 이동하여 다음 실행
\code{.sh}
./bootstrap
./configure --enable-igb --disable-8139too --enable-hrtimer 
make -j4 all modules
\endcode
- root 계정으로 설치
\code{.sh}
make install
make modules_install
depmod
\endcode
- script 파일 및 심볼링 링크 생성
\code{.sh}
ln -s /opt/etherlab/etc/init.d/ethercat /etc/init.d/ethercat
ln -s /opt/etherlab/bin/ethercat /usr/bin/ethercat
mkdir /etc/sysconfig
cp /opt/etherlab/etc/sysconfig/ethercat /etc/sysconfig/ethercat
\endcode
- Start up 설정  
\code{.sh}
vi /etc/sysconfig/ethercat
\endcode
- EtherCAT master 시작 
\code{.sh}
/etc/init.d/ethercat start
\endcode
* 참고 : 커널 로그 메시지 확인
\code{.sh}
tail -f /var/log/kern.log
\endcode

\par 참고: sepmaster에서 master stack 변경하기 
- sepmaster root folder의 CMakeLists.txt 파일을 다음과 같이 수정
\code{.sh}
...전략...
set(ECMASTER etherlab) # possible value is acontis, etherlab
#set(ECMASTER acontis)
...후략...
\endcode
- CLion에서  "Reload CMake Project" 를 수행하거나 빌드 폴더에서 cmake를 수행하여 build system update

\par Issues
 - Yaskawa 서보팩은 LRW 명령을 지원 안함. Domain 생성시 LRD 명령만 사용하는 도메인과 RWR 명령만 사용하는 도메인으로 구분하여 생성해야 함.
 
\subsubsection ec_etherlab_manage EtherCAT network 관리 

Etherlab은 리눅스의 전통적인 command line 인터페이스를 통해 network 관리 가능하다.
Command line 관리툴인 /usr/bin/ethercat의 사용법은 매뉴얼을 참고하기 바란다.

Etherlab은 Acontis와 달리 ENI 파일을 사용하지 않는다. 
따라서 PDO맵 등의 Network 정보 설정과 DC구현을 스택에서 알아서 처리하는 대신 사용자가 설정해야 한다. 
기본적인 설정 방법은 Etherlab EtherCAT Master 소스의 예제 폴더를 참고하면 된다.

Sephirot의 경우 
EcMasterSvc::initMaster() 및 EcMasterSvc::configureMaster()에서 관련 설정이 이뤄진다.

\par 참고 자료
- 매뉴얼 : ethercat-1.5.2.pdf

\section ec_service EtherCAT master services
\par 목적 
- EtherCAT Network 설정 및 상태 관리 
    - command line parsing 
    - slave information management 
    - network state change : INIT - PREOP - SAFEOP - OP
    - DC management 
- Process cycle 관리  
    - send/receive data frames 
    - update PDO data 
- SDO data handling 
    - upload/download CoE SDO data 

    
\par class architecture
- EcMasterSvc
    - slave information :
        - deque<ECatSlaveInfo> slaves_
        - deque<CiA402Servo> servos_
        - deque<ECatSerialSlave> serials_
        - ECatSlaveInfo* stocker_io_
    - client communication channel
        - unique_ptr<SepShm> shm_
        - unique_ptr<ECatMsgQ> msgq_
        - unique_ptr<IpcCondition> pdo_arrival_event_
    - process tasks 
        - unique_ptr<EmTask> em_task_
        - unique_ptr<std::thread> sdo_msg_handle_task_
    - assistant tasks 
        - RFC2217ServiceTask rfc2217task_
        - GrpcServer<EcMasterRpcImpl> ecmaster_rpc_svr

\par 주요 구현 
- command line parsing : EmCommandLineOption::parse()
- service 초기화 : EcMasterSvc::initService()
- process cycle 수행 : EmTask::svc(), EcMasterSvc::doPdoJobs()
- client SDO request handling : EcMasterSvc::handleMessages()

\remark Slave 보드 타입 추가
EcMaster는 Yaskawa서보팩과 Fine의 StokerIO, OHTFrontIO, OHTRearIO보드만 처리 가능하다. 
Slave타입을 추가하려면 acontis stack의 경우 EcMasterSvc::getSlaveInfos()내에서 하기와 같이 처리가능한 board 타입을 추가해야 한다. 
아래 예에서는 CIA402 compliant servopack을 추가하는 예이기 때문에 추가 작업 없이 보드 타입만 추가해도 서보 제어가 가능하다.
그렇지만 device특화된 기능을 사용하려면 user library를 해당 device에 맞게 만들어야 한다.
\code{.cpp}
else if (slaveinfo.vendor_id == 0x0000033D && // RSA
                       slaveinfo.product_code == 0x02010004 ) { // Servo pack
                slaveinfo.type = SlaveType::CIA402_COMPLIANT_SERVO;
            }
\endcode 
etherlab의 경우 slave_pdos.cpp에 아래와 같이 보드 정보를 추가해 주면 된다.
\code{.cpp}
   std::map<BoardType, BoardInfo> slave_config = {
            {{ecvendor_yaskawa, 0x2200301},                  {"yaskawa_servo", SlaveType::CIA402_COMPLIANT_SERVO,   cia402_servo_syncs, &cia402_servo_sdo}},
            {{ecvendor_fine,    ecprodcode_fine_stocker_io}, {"stk_io",        SlaveType::FINE_STK_IO,              stk_io_syncs,       &stk_io_sdo}},
            {{ecvendor_fine,    ecprodcode_fine_oht_front},  {"oht_front",     SlaveType::FINE_OHT_FRONT_IO,        oht_front_syncs,    &oht_front_sdo}},
            {{ecvendor_fine,    ecprodcode_fine_oht_rear},   {"oht_rear",      SlaveType::FINE_OHT_REAR_IO,         oht_rear_syncs,    &oht_rear_sdo}},
            // add here
    };
\endcode

\par EtherCAT Master Client-side Usage

Sephirot 플랫폼은 safety 측면 및 관리 측면에서 EtherCAT master와 
user application  process를 분리하여 구현하고 있다. 
사용하는 slave가 겹치지 않는 이상 EtherCAT master는 복수개의 user application과 
연결될 수 있다. 

EtherCAT master는 EtherCAT network 리소스를 요청하고 
해제할 수 있도록 GRPC 인터페이스 제공하고 있다.
- EcMasterRpcCli, EcMasterRpcImpl
- Poco::SingletonHolder<EcMasterRpcCli> the_em_rpc_client

User application이 EtherCAT master를 사용하는 순서는 다음과 같다.
- master에 slave를 요청
\code{.cpp}
the_em_rpc_client.get()->requestSlave(axis, res)
\endcode
- SDO 요청을 위한 메시지 큐 설정 : 
\code{.cpp}
msg_recvq_ = std::make_unique<ECatSlaveMsgQ>(EM_MSG_Q_KEY_SLAVE(axis).c_str());
\endcode 
- PDO 데이터 교환을 위한 공유메모리 매핑 
\code{.cpp}
shm_ = std::make_unique<SepShm>(EM_SHM_KEY());
out_pdo_ = (Cia402PdoOut *) (shm_->address + EM_SHM_SECTION_SIZE + res.out_pd_offset);
in_pdo_ = (Cia402PdoIn *) (shm_->address + res.in_pd_offset);
\endcode
- 작업 수행 
- slave release 
\code{.cpp}
the_em_rpc_client.get()->releaseSlave(axis_no_);
\endcode
        
\section ec_management EtherCAT master management
Sephirot EtherCAT master는 master를 통해 slave node를 제어할 수 있도록
GRPC 인터페이스를 제공하고 있다. 

다음은 master가 제공하고 있는 RPC 서비스 목록이다.
\code{.proto} 
package sephi;
service EcMasterRpc {
    rpc requestSlave(Request) returns (Response) { }
    rpc releaseSlave(Request) returns (Response) { }
    // stocker I/O
    rpc stockerRead(Result) returns (StockerDataIn) { }
    rpc stockerOutRead(Result) returns (StockerDataOut) { }
    rpc stockerWrite(StockerDataOut) returns (Result) { }

    // for admin tools
    rpc changeECatStateToOP(AdminCommand ) returns (Result) { }
    rpc makeSwitchOnDisabled(AdminCommand) returns (Result) { }
    rpc enableUserParameter(AdminCommand) returns (Result) { }
    rpc resetAbsEncoder(AdminCommand) returns (Result) { }
    rpc storeParameter(AdminCommand) returns (Result) { }
    rpc restoreDefaultParameter(AdminCommand) returns (Result) { }
    rpc readCoeValue(CoeInfo) returns (CoeData) { }
    rpc writeCoeValue(CoeData) returns (Result) { }

    // for simulator
    rpc getSystemName(Empty) returns (ModelName) { }
    rpc getStockerStatus(Empty) returns (StockerStatus) {}
    rpc getFoupLocations(Empty) returns (FoupList){}
    rpc getOHTStatus(Empty) returns (OHTStatus) {}
}
\endcode

GRPC 인터페이스를 사용한 admin tool은 python으로 제공된다. 
- tools/ecmaster
- tools/ec_admin.py 

GRPC는 simulator GUI 인터페이스로도 사용되는데, 다음 폴더를 참고 바란다.
- tools/sim 
- tools/sim/stocker_view.py : stocker simulator
- tools/sim/oht_state_server.py, oht_trans_view.py, oht_view.py : OHT simulator 


\section ec_serial Serial-over-EtherCAT 
OHT IO Slave 장치들은 Digital IO, Analog IO 및 Serial Port 인터페이스 기능을 제공한다.
DIO와 AIO의 경우 PDO mapping을 통한 단순한 인터페이스가 가능하지만 Serial 
Port의 경우 port 설정 및 stream data 전송 기능이 필요하므로 application layer logic이 필요하다. 

\subsection ec_serial_1 Master-Slave Transaction

Master-Slave간의 Serial Data Tunneling 프로토콜의 개요는 다음과 같다.

\par Serial Port 설정 

현재 IO Slave는 최대 4개의 Serial Port를 제공한다. 
각 Port는 최대 115.2kbps까지 지원된다.
각 포트 설정은 다음 주소값과 매핑되어 있다.
\code 
0x8010	RECORD			UART_CONF						
		0x01	UINT32	UART0_BAUDRATE	00960000			rw
		0x02	BYTE	UART0_DATABIT	8					rw
		0x03	BYTE	UART0_STOPBIT	1					rw
		0x04	BYTE	UART0_PARITY	1					rw
		0x05	Pad_8							
		0x06	UINT32	UART1_BAUDRATE	00960000			rw
		0x07	BYTE	UART1_DATABIT	8					rw
		0x08	BYTE	UART1_STOPBIT	1					rw
		0x09	BYTE	UART1_PARITY	1					rw
		0x0A	Pad_8							
		0x0B	UINT32	UART2_BAUDRATE	00960000			rw
		0x0C	BYTE	UART2_DATABIT	8					rw
		0x0D	BYTE	UART2_STOPBIT	1					rw
		0x0E	BYTE	UART2_PARITY	1					rw
		0x0F	Pad_8							
		0x10	UINT32	UART3_BAUDRATE	00960000			rw
		0x11	BYTE	UART3_DATABIT	8					rw
		0x12	BYTE	UART3_STOPBIT	1					rw
		0x13	BYTE	UART3_PARITY	1					rw
		0x14	Pad_8							
\endcode
Slave 설정은 ECatSerialSlave 클래스의 멤버 함수로 구현되어 있다.
- ECatSerialSlave::setBaudRate()
- ECatSerialSlave::setDataBit()
- ECatSerialSlave::setStopBit()
- ECatSerialSlave::setParity()


\par 데이터 Transaction 

Master와 Slave는 예약된 PDO 공간을 이용하여 Serial Data를 전송한다.

- 패킷 구조 
\verbatim
M -> S(RX)					S -> M(TX)
    0	length				0	length
    1	p				1	p
    2	a				2	a
    3	y				3	y
    4	l				4	l
    5	o				5	o
    6	a				6	a
    7	d				7	d
            ...
    15					15	

    a	rx_pkt_cnt		a	tx_pkt_cnt
    b	tx_pkt_cnt_echo	b	rx_pkt_cnt_echo
\endverbatim

- Master -> Slaver 전송 시나리오; slave polling
\verbatim
    M : rx_pkt_cnt_echo == { rx_pkt_cnt, rx_pkt_cnt -1}  상태인지 확인
        length, payload,rx_pkt_cnt((last pkt cnt + 1) % 256)에 데이터를 실어 보냄
    S : rx_pkt_cnt가 old 값과 다른 지 확인				
        copy palyload by length				
        rx_pkt_cnt_echo := rx_pkt_cnt 				
\endverbatim

- Slave -> Master 전송도 유사하게 진행; master polling

- error case(length==0xff)			payload := error code		

- 실제 PDO 매핑 
\code
0x6010	RECORD			IN_UART_STAT				
		0x01	BYTE	UART0_TX_PKT_CNT	0			ro
		0x02	BYTE	UART1_TX_PKT_CNT	0			ro
		0x03	BYTE	UART2_TX_PKT_CNT	0			ro
		0x04	BYTE	UART3_TX_PKT_CNT	0			ro
		0x05	BYTE	UART0_RX_PKT_CNT_ECHO	0		ro
		0x06	BYTE	UART1_RX_PKT_CNT_ECHO	0		ro
		0x07	BYTE	UART2_RX_PKT_CNT_ECHO	0		ro
		0x08	BYTE	UART3_RX_PKT_CNT_ECHO	0		ro
										
0x6020	RECORD			IN_UART						
		0x01	BYTE	UART0_TX_LEN	0				ro
		0x02	BYTE	UART0_TX1	0					ro
        ...
		0x10	BYTE	UART0_TX15	0					ro
		0x11	BYTE	UART1_TX_LEN	0				ro
        ...
		0x21	BYTE	UART2_TX_LEN	0				ro
        ...
		0x31	BYTE	UART3_TX_LEN	0				ro
        ...
		0x40	BYTE	UART3_TX15	0					ro


0x7010	RECORD			OUT_UART_STAT			
		0x01	BYTE	UART0_RX_PKT_CNT	0			rw
		0x02	BYTE	UART1_RX_PKT_CNT	0			rw
		0x03	BYTE	UART2_RX_PKT_CNT	0			rw
		0x04	BYTE	UART3_RX_PKT_CNT	0			rw
		0x05	BYTE	UART0_TX_PKT_CNT_ECHO	0		rw
		0x06	BYTE	UART1_TX_PKT_CNT_ECHO	0		rw
		0x07	BYTE	UART2_TX_PKT_CNT_ECHO	0		rw
		0x08	BYTE	UART3_TX_PKT_CNT_ECHO	0		rw
										
0x7020	RECORD			OUT_UART						
		0x01	BYTE	UART0_RX_LEN	0				rw
		0x02	BYTE	UART0_RX1	0					rw
        ...
		0x10	BYTE	UART0_RX15	0					rw
		0x11	BYTE	UART1_RX_LEN	0				rw
        ...
		0x21	BYTE	UART2_RX_LEN	0				rw
        ...
		0x31	BYTE	UART3_RX_LEN	0				rw
        ...
		0x40	BYTE	UART3_RX15	0					rw
\endcode
- 구현부 
    ECatSerialSlave::transferRxTx()

\subsection ec_serial_2 Master-IPC 인터페이스 
IPC와 Master는 Ethernet으로 연결되어 있기 때문에 IPC Application이 
Slave의 Serial Port를 사용하기 위해서는 한차례 더 Port Forwading을 해야한다. 

IPC 쪽에는 com0com virtual com port driver를 등록하고,
hub4com을 이용하여 com port와 tcp port간 link를 만들어 준다. (com2tcp-rfc2217.bat)

rfc-2217 규약은 전송데이터 외에 terminal 제어를 위한 protocol 데이터를 포함하고 있다.
(https://tools.ietf.org/html/rfc2217)
따라서 이 프로토콜을 처리해 줄 필요가 있는데, 우리의 경우 Port 설정에 관한 명령을 위에 설명한 Slave와의 통신규약에 맞게 처리해야한다.

관련된 구현부는 다음과 같다.
- TelnetComFilter class : rfc-2217 프로토콜 처리 
- RFC2217ServiceTask class : rfc-2217 client를 위한 tcp server 
- RFC2217ServiceConnection class : rfc-2217 client handler 
- ECatSerialSlave class : EtherCAT-Serial Port interface class 

\subsection ec_serial_2 Master-tty 인터페이스 
Master 제어기 내부 application에서 EtherCAT Slave의 Serial Port를 사용하고자 할 경우는 
Master 보드의 serial port로 인식해야한다. 

Linux에서는 socat이라는 utility를 통하여 tcp port와 pty device를 link할 수 있으며, 
각각 다음과 같이 link 되어 있다. 
\code{.sh}
socat  pty,link=/dev/ttyOhtR0  tcp:localhost:40000
socat  pty,link=/dev/ttyOhtR1  tcp:localhost:40001
socat  pty,link=/dev/ttyOhtR2  tcp:localhost:40002
socat  pty,link=/dev/ttyOhtR3  tcp:localhost:40003
socat  pty,link=/dev/ttyOhtF0  tcp:localhost:40004
socat  pty,link=/dev/ttyOhtF1  tcp:localhost:40005
socat  pty,link=/dev/ttyOhtF2  tcp:localhost:40006
socat  pty,link=/dev/ttyOhtF3  tcp:localhost:40007
\endcode


\section ec_emulator Emulation mode

Emulation모드는 Slave 장치의 직접 구동 대신 Emulation 기능수행 한다.
EtherCAT master를 실행할 때 "-emul" 옵션을 추가함으로써 활성화 할 수 있으며,
Emulator에서 다루는 타겟 시스템 모델을 "-model=모델파일명"의 형식으로 지정할 수 있다.

Emulation mode에서는 PDO 데이터 및 SDO Request가 EtherCAT master stack으로 전송 되는 대신
ECatEmulator instance로 전달 된다.

Emulator는 EtherCAT master 및 ESC 검증이 아닌 application 검증이 목적이므로
EtherCAT state는 OP모드로 둔 채, slave application layer의 packet핸들링에만 초점을 두고 있다.
엄밀하게 말해서 Emulation보다는 Simulation이 적합하지만 EtherCAT binary packet을 그대로 사용하고 있으므로
Emulation이라 통칭한다.

\par yaml eni file 생성

인자 들 중에서 eni_file_name은 yaml 포맷 형태의 ENI 파일이며 EcEnigeer등의 도구로 생성된 ENI 파일을
tools/xml2yaml.py 스크립트를 이용하여 만들어 낼 수 있다.
\code{.sh}
# oht_eni.xml로부터 oht_eni.yml을 만든는 명령
python tools/xml2yaml.py oht_eni
\endcode
위 스크립트가 성공적으로 수행되면 해당 폴더에 oht_eni.yml이 생긴 것을 확인할 수 있다.
\note 만약 위 스크립트 수행이 실패했다면 PyYaml 및 xmltodict 모듈이 설치되어 있는 확인하기 바란다.
\note Sephirot에서는 가독성 및 확장성을 위해서 대부분의 Configuration파일은 Yaml 형태로 작성되어 있다. Yaml 문법에 대해서는 다음 사이트들을 참고 바란다.
	- http://anitoy.pe.kr/yaml-format/ 및 reference
	- http://yaml.org/spec/1.2/spec.html


\par 모델 파일
모델 파일은 Emulator에서 사용할 Slave 장치 및 Target System Configuration을 담은 파일이다.
Yaml 파일 포맷을 따르며, 구조는 다음과 같다.
\verbatim
Slave(*): # \note (*)는 0개 이상 존재 가능을 의미
	Config(*):
	Instance(*):

TargetSystem: 타겟명 # Systems 중에 존재 해야함

Systems:
	Target(*):
		Config(*)
\endverbatim


아래는 Stocker 및 OHT simulation을 지원하는 모델 파일의 예이다.
\code{.yml}
YaskawaServo:
  CoE:
  - {Index: 0x2700, SubIndex: 0, DataType: UDINT, Name: UserParamConfig, Value: 0}
  - {Index: 0x6060, SubIndex: 0, DataType: UDINT, Name: ModeOfOperation, Value: 0}
  - {Index: 0x6061, SubIndex: 0, DataType: UDINT, Name: ModeOfOperationDisplay, Value: 0}
  # 이하 CoE 생략
  Instance:
    1001: {Position: 0, PositionLimitMin: 0, PositionLimitMax: 0, HomeOffset: 0}
    1002: {Position: 0, PositionLimitMin: 0, PositionLimitMax: 0, HomeOffset: 0}
    1003: {Position: 0, PositionLimitMin: 0, PositionLimitMax: 0, HomeOffset: 0}
    1004: {Position: 0, PositionLimitMin: 0, PositionLimitMax: 0, HomeOffset: 0}

FINE_STK_IO:
  AI: 4 # number of analog input
  AO: 4
  DI: 47
  DO: 38

FINE_OHT_FRONT_IO:
  AI: 2
  AO: 2
  DI: 47
  DO: 51
  SERIAL: 4
  CoE:
  - {Index: 0x8010, SubIndex: 1, DataType: UDINT, Name: UART0_BAUDRATE, Value: 960000 }
  - {Index: 0x8010, SubIndex: 2, DataType: USINT, Name: UART0_DATABIT, Value: 8 }
  - {Index: 0x8010, SubIndex: 3, DataType: USINT, Name: UART0_STOPBIT, Value: 1 }
  - {Index: 0x8010, SubIndex: 4, DataType: USINT, Name: UART0_PARITY, Value: 1 }
  # 이하 CoE 생략

FINE_OHT_REAR_IO:
  AI: 4
  AO: 4
  DI: 37
  DO: 41
  SERIAL: 4
  CoE:
  - {Index: 0x8010, SubIndex: 1, DataType: UDINT, Name: UART0_BAUDRATE, Value: 960000 }
  # 이하 CoE 생략

TargetSystem: OHT # simulation target

Systems:
  Stocker:
    Name: Stocker
    FrameSize: {x: 4200, z: 5070, y: 1530 }
    ShelfSize: {x: 340, z: 382, y: 376.5, thickness: 44, blank_x: 215, blank_y: 324, port_y: 376 }
    Shelves:
    - { id: L002001, z: 0.00, x: 0.00, r: 0.00, y: 371.60}
    - { id: L002005, z: 1539.80, x: -4.66, r: 0.12, y: 367.01}
    - { id: L002010, z: 3690.00, x: -4.77, r: 0.00, y: 369.96}
    # 이하 Shelves 생략
    Robot:
      PPMM: {z: -292.577, x: 262.070, r: 2063.170, y: -409.613 } # encoder pulse per mm or deg; using IPC's scale; 2^13 pls / rot
      Offset: {z: 482, x: 615, robot_shelf_distance: 50 } # robot origin offset with respect to frame origin
      Size: {fork_width: 200, fork_offset: 300 } # fork_offset: fork y-axis offset
    Foups: [R002005, Robot]

  OHT:
    Name: OHT
    MapFilePath: "test/map/data"
    InitialLocation: {from: 110, to: 111, offset: 100 }
    FoupList: [10001, 10005, OHT] # list of station id or OHT
    MarkSize:
      node: 26
      station: 180
      station_front_part: 130
      station_stop: 5
    Mark:
      premark_start: 400  # mark starts before target; mm
      node_premark_speed: 200 # in mm/s
      station_premark_speed: 170 # in mm/s
      premark_acc: 1500 # in mm/s^2
      node_mark_speed: 100 # in mm/s
      station_mark_speed: 50 # in mm/s
      mark_acc: 500 # in mm/s^
    OnboardIO:
      shmkey: "sephi_oht_onbard_io"
      AI: 4
      AO: 4
      DI: 38 # bit count
      DO: 22  # bit count
      SERIAL: 10

    Robot:
      PPMM: # encoder pulse per mm or deg; using IPC's scale; 2^13 pls / rot
        wheel: 114.7341619
        hoist:
          pulse: [-500, 33000, 210000, 500000, 750000, 1040000, 1170000]
          mm: [-2, 103, 643, 1491, 2184, 2943, 3268]
        slide: 682.666667
      Offset: {hoist: 0, slide: 0 } # orobot origin offset with respect to frame origin
      Size:
        precheck2stop: 350  # distance from precheck sensor to driving stop
      limits:
        pos: # min=0 && max=0 means no limit
          min: [0, -5, -500] # [0, 0, 0] #wheel, hoist, slide each (in mm),
          max: [0, 3500, 500] # [0, 0, 0] #wheel, hoist, slide each (in mm)
          inpos: [4, 4, 4] # wheel, hoist, slide each (in mm)
          p_margin: [0, 35, 20] # positive soft limit margin (in mm)
          n_margin: [0, 3.5, 20] # negative soft limit margin (in mm)
        vel: [5000, 1834.622, 475] # wheel, hoist, slide each (in mm/s)
        acc:
          min: [3000, 1000, 1000] # wheel, hoist, slide each (in mm/s^2)
          max: [2000, 1000, 1000] # wheel, hoist, slide each (in mm/s^2)
\endcode



*/
