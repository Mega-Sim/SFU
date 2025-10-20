/**
\page pg_simulator Simulators 
\ref main_toc "back"


\section sim_general  General description

Simulation이란 모델을 이용하여 실험을 수행하는 것을 말한다.
Sephirot 제어기용 시뮬레이터는 
Sephirot 제어기가 서보팩과 IO 인터페이스 Slave들을 이용하여 
타겟 시스템을 제어하므로 제어 대상이 되는 Slave 장치 및 환경 모델이 필요하다.

Sephirot에는 두 단계의 장치 및 환경 모델을 가지고 있다. 

첫째 단계는 EtherCAT slave 장치 모델이며, EtherCAT frame data를 해석하고 처리한다.
가령 Servo Slave의 경우 매 주기 PDO 데이터를 수신받아서 제어 로직을 수행하여
서보모터의 위치 및 속도를 갱신한다.
또한 SDO 데이터가 있을 경우 이에 해당하는 Slave 상태값을 변경해 준다.

둘째 단계는 타겟 시스템 및 환경 모델이다. 타겟 시스템 및 환경이란 Slave 장치들이 interaction하는 모든 부분이다.
가령 Stocker의 경우 Stocker Frame과 로봇 기구, Foup 등이 모델링 대상이며,
서보 Slave에 의해 갱신된 모터 엔코더 값들로 부터 로봇의 기구학을 업데이트 하고,
간섭이나 센서의 센싱 여부를 체크하고 해당하는 센서 값을 업데이트 한다.


\section sim_ecat  EtherCAT Slaves

Slave 장치 모델은 ECatSlaveModel을 조상 클래스로하여 생성된다.
EtherCAT slave이므로 EtherCAT packet을 처리하기 위한 공통 데이터 구조를 포함하고 있다.
    - slave 정보 : id_, name_  addr_
    - PDO 데이터접근: rxpdo_ 및 txpdo_ 포인터
    - SDO 데이터에 접근: sdo_map_
    - Configuration 파일로부터 Object map을 초기화:  loadModelData()
    - PDO 데이터 처리 가상 인터페이스 : processPDO()
    - SDO 데이터 처리 가상 인터페이스 : processSDO(), handleMailBox()

ECatSlaveModel의 하위 클래스는 서보팩을 모사한 YaskawaServoModel와
Stocker용 IO 보드를 모사한 StockerIOModel,
StockerIOModel의 DIO 및 AIO기능에 Serial Port기능을 추가한 SerialIOSlaveModel 및 이를
상속한 concrete class인 OHTFrontIOModel 및 OHTRearIOModel이 있다.


\section sim_stk  Stocker & Environments

타겟 시스템 모델은 인터페이스 클래스인 SimulatorBase를 상속 받아 구현한다.
SimulatorBase는 리소스 초기화 및 정리를 위한 initialization()와 finalize() 인터페이스 및
매 사이클의 time update를 위한 update() 인터페이스를 가지고 있다.

Concrete class에서는 SimulatorBase 클래스 인터페이스들을 구현하고,
타겟 시스템 동작 모사 및 테스트를 위한 인터페이스를 export 해 준다.
현재 구현된 concrete class는 StockerSystem과 OHTSystem이 있다.

이해를 돕기위해 StockerSystem 클래스의 구현을 살펴보자.
StockerSystem class는 Stocker를 모사하기 위해서 다음의 데이터 멤버를 가지고 있다.
\code{.cpp}
        // Axes
        std::array<YaskawaServoModel *, 4> axes_; // z, x, r, fork axis each
        // Io
        StockerIOModel *stk_io_;
        // io map; viewpoint of simulator
        stk_sim::StockerDI *di_;
        stk_sim::StockerDO *do_;
        // external event
        std::map<string, bool> ext_event_map_; // external events: estop, interlock_key, safety_door_close

        // geometry
        Vector3 frame_size_; // x,y,z
        Vector3 shelf_size_; // x,y,z
        Vector2 shelf_blank_; // x,y; recessed area of shelf
        Vector2 port_blank_; // x,y
        std::map<string, stk_sim::Shelf> shelf_map_;
        std::list<string> foups_;
        stk_sim::Robot robot_;
        double shelf_plate_thickness_;
        double distance_to_shelf_;
        double x_inposition_margin_;

        // status
        int crash_status_;
        stk_sim::Shelf *curr_shelf_;
\endcode
StockerSystem::initialize()함수에서는 configuration file을 바탕으로 위의 데이터를 초기화 한다. \n
StockerSystem::update()함수는 두 단계로 구현 되어 있는데,
modelUpdate()에서 제어기에 주어진 명령(IO 출력 및 servo 지령)에 따른 기구 모델을 업데이트 하고 기구 간섭이나 오류 등을 체크하고,
outputUpdate()에서 modelUpdate() 결과를 반영하도록 제어기로의 IO 입력을 만들어 낸다. \n
Stocker 동작 모사를 위해서 Stocker IO map, Stocker Robot 및 Shelf 상태 모델 클래스들이 만들어져 있는데, 간단한 구현이므로 설명은 생략한다.


\section sim_oht  OHT & Environments 

OHTSystem의 경우 StockerSystem와 개념적으로 동일하지만
Line Map을 포함하고 있어서 위치에 따른 인지와 동작이 달라야 하고,
EQ Port 등 state machine을 가지는 외부 요소들과  steer나 gripper와 같이 동작에 시간 지연을 필요로 하는 하위요소들을
포함하고 있어서 다소 복잡한 구조를 가진다.\n
OHTSystem의 time update 함수를 보면 다음과 같다.
\code{.cpp}
    void OHTSystem::update()
    {
        doDelayedJobs();
        updateOhtLocation();
        updateDrivngSensors();
        updateEQportSensors();
        updateSTBportSensors();
        updateTransSensors();
        updateAuxSensors();
    }
\endcode
doDelayedJobs()은 EQ PIO처럼 state machine을 가지거나 steer처럼 delay가 있는 동작을 업데이트 하는 부분이다.
updateOhtLocation()은 OHT의 서보 모터 동작에 따른 map내에서의 위치를 업데이트 하여 센서 인지 범위를 결정하고,
다른 함수들은 그에 따른 각종 센서들의 출력값을 계산하여 업데이트 한다.



\par Emulator 초기화 과정

"-emul" 옵션이 활성화되면 Sephirot EtherCAT master 초기화 함수인 EcMasterSvc::initService()에서
opt_->emulation_mode값이 true인 흐름을 타게 된다.
EmTask대신 EmEmulatorTask가 cyclic task handler가 되며
초기화 과정은 EcMasterSvc::initEmulator()로 대체 된다.

EcMasterSvc::initEmulator() 함수는 ECatEmulator instance를 초기화 한다.
ECatEmulator class 초기화 함수의 prototype은 다음과 같다.
\code{.cpp}
ECatEmulator::initialize(EcMasterSvc *ecmaster, const string& eni_file_name, const string& model_file_name, uint32_t bus_cycle_us, bool as_observer)
\endcode
위 함수는 Slave 장치모델과 타겟 시스템 모델을 초기화 하는 것이 목적이다.
\code{.cpp}
// --- 상략 ----
for(auto& slave: slaves_) {
	slave->setId(slave_id++);
	slave->loadModelData(model_file_name, slave->getSlaveTypeName());
	slave->initialize();
}
// -- 중략 ---
// init simulator
SEP_REQUIRE_RET(initTargetSystem(model_file_name), false);
\endcode


\par Target 시스템 모델 실시간 조작

Emulation모드가 활성화 된 경우 ecmaster 실행창에는 ECatEmulator::runMenu()가 기본 CLI 화면이 된다.

\verbatim
   >> ECat Emulator Test <<
-----------------------------------------------------------
    d.dump slave object dict
    s.dump slave status
    t.target system status
    S.STK manipulation
    O.OHT manipulation
-- X: eXit, U: to Upper, R: to Root -----------------------
 choice?
\endverbatim

이 화면에 원하는 기능을 추가 하고 싶으면 EcEmulatorMonitor class에 필요한 기능을 구현하고,
TestMenu 클래스의 menu_list에 기능 구현 함수를 등록하면 된다.
참고로 현재 Stocker 및 OHT의 경우 다음과 같은 테스트 메뉴를 가지고 있다.

\verbatim
   >> STK manipulation <<
-----------------------------------------------------------
    i.io manipulation
    e.e-stop
    l.lock
    u.unlock
    c.close door
    o.open door
    f.change fork enc
    s.status
-- X: eXit, U: to Upper, R: to Root -----------------------
 choice?
\endverbatim


\verbatim
   >> OHT manipulation <<
-----------------------------------------------------------
    1.serial read
    2.serial write
    3.read onboard di
    4.write onboard do
    5.relocate oht
    6.press EMO
    7.release EMO
-- X: eXit, U: to Upper, R: to Root -----------------------
 choice?
\endverbatim





\section sim_visualization Simulator Visualization

타겟 시스템(Stocker 및 OHT)의 상태 및 동작은 눈으로 확인할 때 제어 로직 개발의 효율성이 높아진다.
Sephirot에서는 Python의 vpython 모듈을 활용하여 간략한 web-based UI를 제공하고 있다.
각 시스템에 대한 구현은 tools/sim/ 하위 폴더에서 확인 가능하다.
\verbatim
base_simulator.py       : simulator 베이스 클래스
load_model_config.py
oht_map.py
oht_model.py
oht_state_to_flags.py
oht_trans_view.py       : oht transport gui; python3 oht_trans_view.py
oht_view.py             : oht map gui; python3 oht_view.py
stocker_model.py
stocker_view.py         : stocker gui; python3 stocker_view.py
oht_state_server.py     : oht status server; python3 oht_state_server.py 실행 후 web browser에서 http://localhost:9001
uidata                  : oht_state_server.py용 static data
rpc                     : grpc 구현 폴더,  ../../src/ecmaster/rpc/
\endverbatim
GUI 모듈과 Emulator간에는 grpc를 통해 데이터를 교환한다.
grpc 인터페이스 정의는 ecmaster.proto에 있으며 다음과 같다.
\code{.proto}
service EcMasterRpc {
    // 중략
    // for simulator
    rpc getSystemName(Empty) returns (ModelName) { }
    rpc getStockerStatus(Empty) returns (StockerStatus) {}
    rpc getFoupLocations(Empty) returns (FoupList){}
    rpc getOHTStatus(Empty) returns (OHTStatus) {}
}
\endcode
상기 인터페이스에 대한 구현은 ecmaster_admin.cpp에 있으니 참고 바란다.

Stocker와 OHT는 Emulator와의 데이터 포맷에 약간의 차이가 있다.
Stocker GUI를 구현하면서는 상태 정보가 그리 많지 않아서 필요한 정보를 얻기 위한 RPC 함수를 개별로 구현했었는데,
이후 OHT GUI를 구현하면서는 상태 정보가 너무 많아서 yaml 형태에 필요 정보를 모두 담은 단일 인터페이스를 구현했다.
OHTSystem::getStatusAsYaml() 함수가 OHT GUI를 위한 상태 정보를 생성하는 함수이다.


\par GUI screenshots
\image html img/stocker.png "Stocker GUI"

\image html img/oht_map.png "OHT Map View"

\image html img/oht_state.png "OHT Status"

*/
