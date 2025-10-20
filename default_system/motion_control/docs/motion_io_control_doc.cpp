/**
\page pg_motion_io Motion/IO Library 
\ref main_toc "back"



\section mi_cia402 CiA402-compliant servo interface

\par 개요

CiA402(CAN in Automation 402) Profile은 CANopen 기반의 자동화 시스템에서 사용하던 서보모터 제어용 Device Profile이다.
명칭의 유래에서 보듯 CANopen은 EtherCAT 이전 세대 필드 버스인 CAN(Control Area Network)상에서 구현 되었지만
현재는 EtherCAT과 Ethernet Powerlink 등에서도 CANopen을 Application layer 프로토콜로 채용하고 있다.

CANopen 프로토콜은 필드 버스에서 각 노드들이 data를 어떻게 주고 받을 지 communication model을 정의하고 있다.
Object dictionary 방식으로 각 노드의 상태를 읽고 제어하는 것이 대표적인 특징이다.

CiA에서 Device Profile은 communication model을 활용한 표준  인터페이스를 규정한다.
즉, 어떤 Object를 어떤 용도로 사용하며, Slave Machine이 어떤 상태 머신을 따라야 하는 지를 규정한다.

402외에도 다양한 Device Profile들이 존재 하며 대표적인 예는 다음과 같다.
- CiA 401 : Generic I/O modules
- CiA 404 : Measuring devices and closed-loop controllers
- ...


\par CiA 402 Motion Control Drive State Machine

CiA 402 compliant drive는 다음의 State machine을 따라야 한다.
모션 제어는 Operation Enable 상태에서만 가능하다.

\image html img/cia402_states.png "CiA402 State Machine"

State machine 제어를 위해서는 status word와 control word를 제어해야하며,
각 비트의 의미는 다음과 같다.
\image html img/cia402_status_control.png "CiA402 Status & Control bits"



\par Operation Modes

CiA402에서는 다양한 동작 모드를 정의하고 있다.
\image html img/cia402_opmode.png "CiA402 Operation Mode"
모든 서보팩이 이를 다 지원하는 것은 아니며 보통은 필요한 기능만 제공한다.
현장에서 많이 사용하는  모드는 CSP(Cyclic Synchronous Position), CSV(Cyclic Synchronous Velocity), CST(Cyclic Synchronous Torque) 모드 등이다.
CSP등을 많이 사용하는 까닭은 velocity offset이나 torque offset을 추가적으로 부여할 수 있기 때문에 
부하나 모션이 변하는 상황을 제어에 추가적으로 반영할 수 있기 때문이다.

주요 제어 모드의 동작 구조는 다음과 같다.
\image html img/cia402_csp.png "CiA402 CSP Architecture"

\image html img/cia402_csv.png "CiA402 CSV Architecture"


\par 구현

Sephirot Platform내에서 CiA402 서보 드라이브 인터페이스는 EcServoMotor 클래스에 구현되어 있다. 
EcServoMotor는 EcMaster와의 통신을 통해 서보 모터에 접근하므로 
초기화 단계에서 PDO 데이터 통신를 위한 공유메모리 포인터인  out_pdo_ 및 in_pdo_를 얻어야 한다.
\code{.cpp}
class EcServoMotor : public ServoMotor {
    std::unique_ptr<ECatSlaveMsgQ> msg_recvq_;
    std::mutex msgq_mtx_; // synchronous한 message queue 통신이 되도록
    std::unique_ptr<SepShm> shm_;
    Cia402PdoOut* out_pdo_;
    Cia402PdoIn* in_pdo_;
    // --- 하략 ----
    
bool EcServoMotor::initialize(int axis, ServoOpMode mode, const char* model,  bool enable_on_init)
    //---- 상략 -----    
    ECatSdoMsg::RscResponseMsg res;

    // axis id와 slave id가 동일하다고 가정 했음. 필요시에 수정할 것
    SEP_REQUIRE_RET(the_em_rpc_client.get()->requestSlave(axis, res), false);
    SEP_REQUIRE_RET(res.in_pd_offset != 0xFFFFFFFF, false);
    axis_no_ = axis;
    bus_cycle_us_ = res.bus_cycle_us;

    // channel creation
    msg_recvq_ = std::make_unique<ECatSlaveMsgQ>(EM_MSG_Q_KEY_SLAVE(axis).c_str());
    shm_ = std::make_unique<SepShm>(EM_SHM_KEY());

    SEP_REQUIRE_RET(shm_ != nullptr && msg_recvq_ != nullptr, false);
    out_pdo_ = (Cia402PdoOut *) (shm_->address + EM_SHM_SECTION_SIZE + res.out_pd_offset);
    in_pdo_ = (Cia402PdoIn *) (shm_->address + res.in_pd_offset);
    //----- 하략 ----
\endcode
모션 제어를 위한 PDO 구조는 미리 정의 되어 있으며 다음과 같다.
\code{.cpp}
    struct Cia402PdoIn {
        uint16_t status;
        trq_unit_t torque;
        vel_unit_t velocity;
        pos_unit_t position;
        pos_unit_t following_error;
        uint16_t error_code;
        uint8_t  op_mode;
    } __attribute__((packed));

    struct Cia402PdoOut {
        uint16_t control;
        trq_unit_t torque;
        vel_unit_t velocity;
        pos_unit_t position;
        uint32_t prof_velocity;
        vel_unit_t vel_offset;
        trq_unit_t trq_offset;
    } __attribute__((packed));
\endcode
CiA402 상태 관리를 위해서 status와 control word는 별도의 접근 인터페이스 및 해석 클래스를 구현했다.
\code{.cpp}
    // 접근 메소드 
    Cia402ControlWord& EcServoMotor::controlWord() { return *(Cia402ControlWord*)out_pdo_;}
    Cia402StatusWord& EcServoMotor::statusWord() const { return *(Cia402StatusWord*)in_pdo_;}
    // 해석 클래스 
    struct Cia402ControlWord {
        Cia402ControlWord() = default;
        Cia402ControlWord(uint16_t cmd): command(cmd){}
        uint16_t command;
        enum COMMAND : uint16_t {
            SHUTDOWN = 0x06,
            SWITCH_ON = 0x07,
            FAULT_RESET = 0x80,
            ENABLE_OP = 0x0f,
            DISABLE_OP = 0x0e,
            QUICK_STOP = 0x02,
        };
        // ---- 하략 -----
    };
    
    struct Cia402StatusWord {
        Cia402StatusWord() = default;
        Cia402StatusWord(uint16_t st){ *(uint16_t*)this = st;}

        uint16_t ready_to_switch_on:1;     // # 0
        uint16_t switched_on:1;            // # 1
        uint16_t operation_enabled:1;      // # 2
        uint16_t fault:1;                  // # 3
        uint16_t voltage_enabled:1;        // # 4
        uint16_t quick_stop:1;             // # 5
        uint16_t switch_on_disabled:1;     // # 6
        uint16_t warning:1;                // # 7
        uint16_t manufacturer_specific_1:1;// # 8
        uint16_t remote:1;                 // # 9
        uint16_t target_reached:1;         // # 10 CSP Target Position Ignored
        uint16_t internal_limit_active:1;  // # 11
        uint16_t fault_over_temperature:1; // # 12 PP set point acknowledged
        uint16_t operation_mode_specific:1;// # 13 CSP Following Error
        uint16_t fault_over_current:1;     // # 14
        uint16_t fault_under_voltage:1;    // # 15

        enum STATE :int {
            NOT_READY_TO_SWITCH_ON,
            SWITCH_ON_DISABLED,
            READY_TO_SWITCH_ON,
            SWITCHED_ON,
            OPERATION_ENABLED,
            QUICK_STOP,
            FAULT_REACTION_ACTIVE,
            FAULT,
            FAULT_OVER_VOLTAGE,
            FAULT_OVER_TEMPERATURE,
            FAULT_OVER_CURRENT,
            FAULT_UNDER_VOLTAGE,
            UNKNOWN_STATE
        };
        //---- 하략 -----    
    }
\endcode
CiA402 상태 제어와 관련하여 주요한 함수는 다음과 같다.
- EcServoMotor::enable() : OPERATION ENABLED 상태로 바꿈
- EcServoMotor::disable() : SWITCH_ON_DISABLED 상태로 바꿈
- EcServoMotor::clearFaults() : Fault 상태를 clear함 

\par Servo Motor 제어 

서보 모터 인터페이스 추상 클래스의 주요 메소드를 살펴 보자.
\code{.cpp}
class ServoMotor : public IEventHost {
        // 초기화
        virtual bool initialize(int axis, ServoOpMode mode, const char* model = nullptr, bool enable_on_init = true){return false;};
        virtual bool finalize() {return true;}
    
        // setting
        int getAxisNo(){ return axis_no_; }
        virtual ServoOpMode getServoMode() { return op_mode_; }
        virtual bool setServoMode(ServoOpMode new_mode) { op_mode_ = new_mode;  return true;}
        const Range& getPosLimit() { return pos_range_; }
        const Range& getVelLimit() { return vel_range_; }
        const Range& getAccLimit() { return acc_range_; }
        virtual MotorDirection getMotorDirection() { return dir_;}
        virtual bool setPosLimit(double min, double max) { pos_range_.first = min; pos_range_.second = max; return true; }
        virtual bool setVelLimit(double min, double max) { vel_range_.first = min; vel_range_.second = max; return true; }
        virtual bool setAccLimit(double min, double max) { acc_range_.first = min; acc_range_.second = max; return true; }
        virtual bool setTrqLimit(double min, double max) { trq_range_.first = min; trq_range_.second = max; return true; }
        virtual bool setOverloadCondition(double over_trq_duration, double following_error_tolerance, double speed_to_following_error_ratio) { return false;}
        virtual int checkOverload() { return 0;}
        virtual bool setMasterAxis(int slave_id) { return false; }
        virtual bool resetMasterAxis(){ return true;}

        bool setUserUnit(const char* unitname, double pulse_per_unit); // jump if changed in operation
        const char* getUserUnitName();
        double getPulsePerUnit();
        double getGearRatio() { return gear_ratio_; } // gear비가 50이라는 것은 x50된다는 뜻임.

        double getToleranceMargin(ControlTarget mode) { return tolerance_[static_cast<int>(mode)]; }
        virtual bool setToleranceMargin(ControlTarget mode, double new_val) { tolerance_[static_cast<int>(mode)] = new_val; return true; }

        // control
        virtual bool enable(){return false;}
        virtual bool disable(){return false;}
        virtual ServoState getServoState() { return ServoState::UNKNOWN_STATE;}
        virtual bool stop(){ return false;} //emergent stop
        virtual bool brakeOn() { return false;}
        virtual bool brakeOff() { return false; }
        virtual bool isBrakeOn() { return false;}
        virtual bool setHome() { return false; }
        virtual bool isHome() { return false; }
        virtual bool resetEncoder(bool soft) { return false;}
        virtual bool clearFaults(){ return false;}

        virtual bool setReferencePP(double p, double* v=nullptr, double* a=nullptr, bool immediate = true); // PP{p, vmax, accmax},
        virtual bool setReferenceCSP(double p, double* voffs=nullptr, double* toffs=nullptr); //  CSP{p, voff, toff}
        virtual bool setReferenceCSV(double v, double* voffs=nullptr, double* toffs=nullptr); // CSV
        virtual const Vector3& getActual(){return actual_;} // pos, vel, acc
        virtual bool inPosition(); // target reached
        virtual double getPosError() { return 0; }
        virtual double getVelError() { return 0; }
        virtual double getRefPos() { return 0; }
        virtual double getRefVel() { return 0; }
        // info
        const char* getName() const { return name_.c_str(); }

    public:
        // unit conversion
        double pulse2unit(pos_unit_t pulse) { return pulse/(pulse_per_unit_*gear_ratio_);}
        pos_unit_t unit2pulse(double unit) {  return unit*(pulse_per_unit_*gear_ratio_); }
        double pos2unit(PosUnit pulse) { return pulse.val/(pulse_per_unit_*gear_ratio_);}
        PosUnit unit2pos(double deg) {  return PosUnit(deg*(pulse_per_unit_*gear_ratio_)); }   
\endcode
대부분의 API들은 이름이 설명하는 대로 동작하지만, 약간 더 이해가 필요한 부분이 있으므로 언급한다.
- bool initialize(int axis, ServoOpMode mode, const char* model = nullptr, bool enable_on_init = true)
    - 초기화 하는 함수이다. 
    - model: model db에 있는 이름을 인자로 부여하면 db값대로 초기화 되므로 유용하다.
    - enable_on_init: 초기화 하면서 모터를 enable상태로 만든다. 
- bool setUserUnit(const char* unitname, double pulse_per_unit) 
    - 서보 모터는 기어나 기구부에 연결되어 동작하므로, 기구부 움직임과 모터 엔코더 간에 unit 변환이 필요하다.
    - 이 메소드는 사용자 유닛당 몇 pulse가 필요한 지 설정하고, 설정 이후는 setReferenceXX()류의 함수의 입출력 단위가 된다.
    - 설정하지 않으면 기본값으로 사용자 유닛은 degree이며, 야스카와 모터 기본값인 degree당 360/2^20 pulse.
    - setUserUnit()은 operation 중에 사용하면 reference jump가 발생하여 로봇이 폭주할 수 있으므로 반드시 초기화때 사용.
- bool brakeOn/Off(), isBrakeOn()
    - brake는 서보팩외에 별도의 외부 로직이 필요하다. 
    - 따라서 이 함수들은 overriding하여 brake On/Off시 DO를 인가하고, DI를 읽어 brake상태를 파악하도록 사용자가 작성해야한다.
- bool resetEncoder(bool soft)
    - Absolute encoder인 경우 서보 power reset이 필요하거나 reset 동작이 예상과 다를 수 있으므로 서보 매뉴얼을 읽어 보기 바란다.

    
\section mi_motionlib Motion libraries
\subsection mi_eigen eigen3
다관절 로봇 제어를 위해서는 Forward 및 Inverse Kinematics Problem을 풀어야 하고, 
Rigid body robot의 kinematics는 DH(Denavit-Hartenberg) 표현과 같은 매트릭스 연산 형태로 표현된다.
어떤 매트릭스 솔루션을 사용하느냐는 제어기의 성능 뿐만 아니라 안정성에도 영향을 미칠 수 있다.
대표적인 매트릭스 라이브러리는 다음과 같다.

- eigen3 : http://eigen.tuxfamily.org/
- newmat11 : http://www.robertnz.net/nm_intro.htm
- armadillo : http://arma.sourceforge.net/

성능에 관해서는 여러가지 이견이 많지만(armadillo가 가장 빠르다고 알려져 있음) 
모두 안정성에 대해서는 어느 정도 검증을 받았기 때문에,
Sephirot에서 채용하고있는 Kinematic solution인 Orocos KDL(Kinematics and Dynamics Library, http://www.orocos.org/kdl)에서 Eigen3을 
매트릭스 연산 솔루션으로 사용하고 있으므로, eigen3외 다른 solution porting은 검토하지 않았다.

다음은 eigen3을 이용한 간단한 역행열 연산이다.
\code{.cpp}
#include <iostream>
#include <Eigen/Dense>
using namespace std;
using namespace Eigen;
int main()
{
   Matrix3f A;
   A << 1, 2, 1,
        2, 1, 0,
        -1, 1, 2;
   cout << "Here is the matrix A:\n" << A << endl;
   cout << "The determinant of A is " << A.determinant() << endl;
   cout << "The inverse of A is:\n" << A.inverse() << endl;
}
\endcode


\subsection mi_kdl kdl 
KDL은 오픈소스 프로젝트인 Orocos(Open Robot Control Software)의 로봇 기구학 및 동력학 처리 라이브러리이다.
가장 널리 쓰이는 오픈소스 로봇 SW Platform인 ROS(Robot Operating System, http://www.ros.org/)의 로봇 기구학 모듈로도 사용되고 있다.
다관절 로봇에 대한 Forward 및 Inverse Kinematics Problem의 예는 다음과 같다. 
\code{.cpp}
#include <kdl/chain.hpp>
#include <kdl/chainfksolver.hpp>
#include <kdl/chainfksolverpos_recursive.hpp>
#include <kdl/frames_io.hpp>
#include <stdio.h>
#include <iostream>
 
using namespace KDL;
 
 
int main( int argc, char** argv )
{
    //Definition of a kinematic chain & add segments to the chain
    KDL::Chain chain;
    chain.addSegment(Segment(Joint(Joint::RotZ),Frame(Vector(0.0,0.0,1.020))));
    chain.addSegment(Segment(Joint(Joint::RotX),Frame(Vector(0.0,0.0,0.480))));
    chain.addSegment(Segment(Joint(Joint::RotX),Frame(Vector(0.0,0.0,0.645))));
    chain.addSegment(Segment(Joint(Joint::RotZ)));
    chain.addSegment(Segment(Joint(Joint::RotX),Frame(Vector(0.0,0.0,0.120))));
    chain.addSegment(Segment(Joint(Joint::RotZ)));
 
    // Create solver based on kinematic chain
    ChainFkSolverPos_recursive fksolver = ChainFkSolverPos_recursive(chain);
 
    // Create joint array
    unsigned int nj = chain.getNrOfJoints();
    KDL::JntArray jointpositions = JntArray(nj);
 
    // Assign some values to the joint positions
    for(unsigned int i=0;i<nj;i++){
        float myinput;
        printf ("Enter the position of joint %i: ",i);
        scanf ("%e",&myinput);
        jointpositions(i)=(double)myinput;
    }
 
    // Create the frame that will contain the results
    KDL::Frame cartpos;    
 
    // Calculate forward position kinematics
    bool kinematics_status;
    kinematics_status = fksolver.JntToCart(jointpositions,cartpos);
    if(kinematics_status>=0){
        std::cout << cartpos <<std::endl;
        printf("%s \n","Succes, thanks KDL!");
    }else{
        printf("%s \n","Error: could not calculate forward kinematics :(");
    }
}
\endcode

\code{.cpp}
//Creation of the chain:
KDL::Chain chain;
chain.addSegment(Segment(Joint(Joint::RotZ),Frame(Vector(0.0,0.0,1.020))));
chain.addSegment(Segment(Joint(Joint::RotX),Frame(Vector(0.0,0.0,0.480))));
chain.addSegment(Segment(Joint(Joint::RotX),Frame(Vector(0.0,0.0,0.645))));
chain.addSegment(Segment(Joint(Joint::RotZ)));
chain.addSegment(Segment(Joint(Joint::RotX),Frame(Vector(0.0,0.0,0.120))));
chain.addSegment(Segment(Joint(Joint::RotZ)));
 
//Creation of the solvers:
ChainFkSolverPos_recursive fksolver1(chain1);//Forward position solver
ChainIkSolverVel_pinv iksolver1v(chain1);//Inverse velocity solver
ChainIkSolverPos_NR iksolver1(chain1,fksolver1,iksolver1v,100,1e-6);//Maximum 100 iterations, stop at accuracy 1e-6
 
//Creation of jntarrays:
JntArray q(chain.getNrOfJoints());
JntArray q_init(chain.getNrOfJoints());
 
//Set destination frame
Frame F_dest=...;
 
int ret = iksolverpos.CartToJnt(q_init,F_dest,q);
\endcode


\subsection mi_profile velocity profile
서보 제어는 크게 Feedforward part와 Feedback part로 나눌 수 있다.
Feedforward part는 서보 모터가 움직여야할 trajectory를 생성하는 것이며,
Feedback part는 주어진 trajectory를 잘 추종하도록 서보모터의 전류/속도/위치를 제어하는 것이다.
대부분의 경우 Sephirot Platform의 모션 Library는 Feedforward part를 담당하고 있으며,
서보팩내부의 제어 루프가 Feedback part를 담당하다.

Feedforward 제어기는 뒷단의 Feedback 제어기가 잘 추종할 수 있는 reference를 생성하는 것이 중요하다.
즉, 가능한 smooth한 trajectory를 생성할 수록 뒤의 feedback제어기 reference의 instant change가 적어지므로 
feedback 제어가 쉬워진다. 

Velocity profile은 추종 가능한 trajectory생성에 사용되는 기본적인 라이브러리다.
Sephirot에 구현된 TrVelocityProfile은 boundary 조건을 충족하는 time optimal 및 time fixed trapezoidal profile을 생성한다.
각각은 bool TrVelocityProfile::ComputeTimeOptimalSolution() 및 bool TrVelocityProfile::ComputeTimeFixedSolution(double fixedT)
에 구현 되어 있으며, 간략한 이론적인 설명은 다음과 같다.


\par 1. Time-optimal solution

time-optimal 조건하에서 파라미터 변경 구간 시간을 구한다.(t1,t2,t3)
초기 조건에 따라서 등속구간이 존재하지 않는 톱니 모양의 속도프로파일이 생성될 수도 있다.
구간별 위치, 속도, 가속도식과 경계치 조건을 이용하여 t1,t2,t3에 관한 방정식의 해를 구한다.

\par 문제 정의
최대 가/감속도 및 최대속도가 \f$A,D,V\f$인 제한조건 하에서, 초기 위치 및 속도가 \f$p_0, v_0\f$이고
목표 위치 및 속도가 \f$p_f, v_f\f$가 되는 최소시간 위치 프로파일을 생성.

\par 프로파일 형상
Time-optimality 특성으로 인해 결과 프로파일은 뱅뱅제어 결과물의 형태를 띠게 된다.
즉, 가속->등속->감속 or 감속->등속->가속. <br>
참고) duality : 감속->등속->가속 프로파일은 듀얼 problem인 \f$ D, A, V, -p_0, -v_0, -p_f, -v_f \f$의 제한 조건을 가지는 문제의
 가속->등속->감속 프로파일과 sign 대칭이 된다. 즉, 모든 문제는 가속구간이우선 프로파일로 변환 가능.<br>
참고) 가속구간이 먼저 시작되어야 하는지 여부 : 단위 sampling시간 후에 남은 거리가 속도변화에 필요한 거리보다 멀다면 가속이 먼저 필요하고,
짧다면 감속이 먼저되어야 한다.


\par 미지수 결정
\li 결정해야할 미지수 : \f$ x, \hat x, \bar x \f$
\li 조건 1 : 이동 거리 조건 (\f$ v_p \f$는 peak \f$ v \f$, i.e. \f$ v_1 \f$)
\f[
    L = p_f - p_0 = \frac{v_0 + v_f}{2}x + v_p \hat x + \frac{v_p + v_f}{2} \bar x
\f]
\li 조건 2 : peak velocity : \f$ v_p = v_0 + Ax = v_f + D \bar x \f$ <br>
 - 등속 구간이 존재할 경우 : \f$ v_p = V \f$ <br>
     연립하면
     \f[
           x = \frac{V - v_0}{A}
      \f]
      \f[
           \bar x = \frac{V-v_f}{D}
      \f]
      \f[
           \hat x = \frac{2L - (v_0 + V)x - (V+v_f)\bar x}{2V}
     \f]
     만약, \f$ \hat x \geq 0 \f$ 이면 등속구간이 존재하지 않는 경우이므로, 아래 조건으로부터 해를 구해야한다.
 - 등속 구간이 부재할 경우 : \f$ \hat x = 0, \bar x = \frac{1}{D} (v_0 - v_f + Ax) \f$
    연립하면,
    \f[
        (A + \frac{A^2}{D})x^2 + 2v_0( 1 + \frac{A}{D}) x + \frac{v_0^2 - v_f^2}{D} - 2L = 0
    \f]
    단, \f$ x \f$는 두 근 중에서 \f$ x \geq 0 , \bar x \geq 0 \f$ 만족하는 근.


\par 2. Time-contrained solution

quantization time delay 조건에서 파라미터 변경 구간 시간을 구한다.(t1,t2,t3)
가감속도를 scaling하는 경우는 제외하고, 최대 속도를 scaling 하는 경우만 고려한다.(maxV'=beta*maxV, 0<beta<=1)
beta에 관한 이차방정식을 풀고 t1,t2,t3를 계산한다.

참고) fixedT	:	quantization 을 고려한 전체 모션 시간

\par 문제 정의
최대 가/감속도 및 최대속도가 \f$A,D,V\f$인 제한조건 하에서, 초기 위치 및 속도가 \f$p_0, v_0\f$이고
목표 위치 및 속도가 \f$p_f, v_f\f$이며, 전체 주행시간이 \f$ T \f$인  위치 프로파일을 생성.

\par 미지수 결정
주행시간이 주어진 경우도, 시간최적 프로파일과 동일한 형상의 가속도 제한 프로파일로 만들 수 있다.
가속 구간, 등속구간, 감속구간 각각의 길이를 \f$ x, \hat x, \bar x \f$ 이라고 두면,
\f$ \hat x  = T - x - \bar x \f$이므로, 다음과 같이 문제를 풀 수 있다.

\li 결정해야할 미지수 : \f$ x, \bar x \f$
\li 조건 1 : 이동 거리 조건 (\f$ v_p \f$는 peak \f$ v \f$, i.e. \f$ v_1 \f$)
    \f[
        L = p_f - p_0 = \frac{v_0 + v_f}{2}x + v_p (T - x - \bar x) + \frac{v_p + v_f}{2} \bar x
    \f]
\li 조건 2 : 최대 속도 조건. scaling factor \f$ \beta \f$를 도입하여, \f$ v_p = \beta V\f$, 단 \f$ 0 \leq \beta \leq 1 \f$로 두면,
    \f[
    \begin{array}
    {l}
       x = \frac{v_p - v_0}{A} \\
       \bar x = \frac{v_0 - v_f + Ax}{D} \\
       v_p = \beta V
    \end{array}\
    \f]
    연립하여 풀면,
    \f[
        -V^2(\frac{1}{A} + \frac{1}{D} ) \beta^2 +  2V \left(T + \frac{v_0}{A} + \frac{v_f}{D} \right) \beta
            -2L - \frac{v_0^2}{A} - \frac{v_f^2}{D} = 0
    \f]
    단, \f$x \geq 0, \bar x \geq 0, (T-x-\bar x) \geq 0 \f$.
\li 참고) Time-fixed 조건의 특성상(constraint가 하나 증가) 해가 존재하지 않는 경우가 있음.
        이경우 time-optimal 조건 그대로 이용함.



\subsection mi_jointmotion joint motion

Joint motion은 서보모터를 개별로 제어하는 관점에서 본 로봇 모션이다.
로봇의 형상과 관계없이 축단위로 제어하므로 제어 수행을 위해서 기구학적인 해석이 필요 없다.

Joint 모션의 생성은 축단위 모션일 경우 각 축별로 사다리꼴 속도 프로파일을 생성하면 되고,
동기 모션의 경우 가장 수행시간이 긴 축의 시간에 맞추어 속도 프로파일을 생성하면 된다.


\subsection mi_robotmotion robot motion

Robot motion은 Task space 모션이라고도 부르며, 로봇의 End-Effector를 제어하고자 하는 관점에서 
바라본 로봇의 모션이다. 

로봇 모션을 수학적으로 표현하기 위해서는 좌표계의 정의가 필요하다.
참고로 좌표계사이의 변환은 간단한 행렬곱셈 연산을 통해 계산 가능하다.
- World 좌표계 : 로봇 및 카메라의 위치를 정의하기 위한 기본 좌표계
- Robot 좌표계 : 로봇 base를 원점으로 하는 좌표계
- Tool 좌표계 : Tool base를 원점으로 하는 좌표계
- Camera 좌표계 : 카메라를 원점으로 하는 좌표계 


로봇이 작업을 수행하기 위해서 tool을 어떤 주어진 포지션으로 이동하고,
tool 작업을 수행 하게 된다.
주어진 포지션은 world 좌표계의 어떤 지점이 되겠지만, 
로봇의 제어에 입장에서는 로봇의 모터들를 움직여야 하므로 로봇 좌표계에서 제어가 이뤄지게 된다.

로봇 포지션은 병진 방향 position인 x,y,z와 회전 방향 position인 r, p, y (roll, pitch, yaw)로 구성된다.
로봇 포지션을 서보 포지션으로 변환하는 것을 inverse kimematics라고 하고,
그 반대를 forward kinematics라고 한다.
Forward kinematics의 경우 항상 유일한 해가 존재하지만, Inverse kinematics의 경우 
해가 없거나(unreachable area) 둘 이상 존재(redundant solution area)할 수도 있다. 
직교좌표 로봇의 경우 각 축의 모션이 orthogonal하므로 로봇 포지션과 서보 포지션은 항상 1대1 매칭된다.


Joint 모션과 달리 로봇 모션은 Path와 Speed profile을 이용하여 Trajectory를 생성해야 한다.
여기서 Path란 로봇의 End-Effector가 움직이게 될 기하학적인 경로를 의미하며 
Speed는 그 경로를 어떤 속도로 움직일 지에 대한 정보이다.
Trajectory는 매 시간 로봇이 어떤 포지션에 있어야 할 지에 대한 시간 궤적을 의미한다.
로봇 모션의 경우 Path가 Task space에서 정의되므로, Trajectory도 Task Space Trajectory가 된다.
얻어진 Task Space Trajectory는 Inverse Kinematics를 이용하여 서보 좌표계로 변환하여 서보팩 reference로 전달한다.


\remark 2018년 5월 현재 Sephirot Platform을 다관절 로봇에 적용한 사례는 없음.



\section mi_robotcontrol Robot control
2018년 5월 현재 Sephirot Platform을 다관절 로봇에 적용한 사례가 없으므로 축단위 모션 제어(OHT)와 
직교좌표 로봇(Stocker) 모션 제어에 대해서 다루도록 한다.

\par RoboticModule

Sephirot의 모든 로봇 class는 RoboticModule를 상속 받는다.
RoboticModule은 로봇의 trajectory 생성을 위한 motion객체와 서보모터 제어를 위한 motor 객체들에대한 container이면서
로봇 실행자(pdoUpdateTask()) 구동의 주체이고, 사용자에게 로봇 action 제어 인터페이스를 제공한다.

Robotic Module는 다음과 같은 계층 구조를 가진다.
- RoboticModule
   - AxesModule ; ServoMotion 활용
      - SingleAxis
      - MultiAxes
      - PairedAxes : master-slave
   - CartesianModule (추후) ; RobotMotion 활용 
      - XY_Module
      - XYZ_Module
      - XYZW_Module 
      - Scara_Module
      - ArmModule 



\par Motion

RobotMotion 및 ServoMotion의 조상 클래스이다.
Motion은 Trajectory생성의 역할을 가지며, RoboticModule에 의해서 조작된다.
Motion life cycle은 다음과 같다.
  - 객체 생성
  - Set Targt : 프로파일 생성을 위한 목표값 설정
  - Init Servo : 서버를 해당 모션을 수행하기 위한 제어 상태로 변경
  - do motion : by robot module

Motion 클래스는 다음과 같은 계층 구조를 가진다.
- Motion
  - ServoMotion (추상 클래스 )
     - CSPMotion
     - CSVMotion
     - CSTMotion
     - PPMotion
  - RobotMotion (미구현)

     
\subsection mi_singleaxis Single axis robot

SingleAxis는 1개의 축을 제어하기 위한 클래스이다.
SingleAxis는 PairedAxes 및 MultiAxes 구현의 기반이기도 하다.

SingleAxis의 public 인터페이스는 다음과 같다.
\code{.cpp}
class SingleAxis : public AxesModule {
    bool initialize(int axis_no, const char* motor_model = nullptr, double gear_ratio = 1,  bool enable_on_init = true);
    bool changeControlMode(ServoOpMode new_cmode);
    bool finalize();
    bool enableServo(int axis = 0);
    bool clearFaults() override;

    bool setMax(double max_vel_deg, double max_acc_deg = -1, double max_dec_deg = -1, bool set_motor_limit = true);
    bool setMaxRPM(double max_vel_rpm, double max_acc_rpm = -1, double max_dec_rpm = -1, bool set_motor_limit = true);
    bool setPosLimit(double pos_min, double pos_max);
    
    // robot api interface
    int moveDistance(double dist, double max_v = -1, double max_a = -1, double max_d = -1, double duration = -1);
    int moveDistanceIterative(double dist, double v_end, double max_v = -1, double max_a = -1, double max_d = -1);
    int moveTo(double pos, double max_v = -1, double max_a = -1, double max_d = -1, double duration = -1);
    int rotateAt(double vel, double max_v = -1, double max_a = -1, double max_d = -1, double duration = -1);
    bool stop(double dec = -1, bool emergent = false);
    bool pause();
    bool resume();
    bool clearCommand();
    bool resetEncoder(bool soft=false);
    void setMotionChangeMode(bool immediate) { immediate_motion_change_ = immediate; }
    bool isImmediateChangeMode() {return immediate_motion_change_; }

    int lastMotionCommandId(){ return last_mc_id_; }
    int getCurrentMotionId();
    Motion::MotionState getMotionState(int axis = 0);
    virtual bool waitForMotionDone(int command_id);
\endcode
Public Interface를 살펴 보면 다음과 같이 grouping 됨을 알 수 있다.
    - 초기화/종료 관련 함수 
    - 설정 함수
    - Action 명령 함수
    - 상태 수집 함수 

SingleAxis를 잘 이해하기 위해서는 내부구현에 대해서 잘 이해할 필요가 있다.
다음은 protected method들이다.
\code{.cpp}
        // pdo context functions
        virtual void pdoUpdateTask();
        bool processCommand(); // process motion command in pdoUpdateTask context
        virtual bool updateReference();
        void stayReference();        
        virtual void logTrajectory();
        // command handler
        virtual bool onMoveD(Command& cmd);
        virtual bool onMoveA(Command& cmd);
        virtual bool onRotate(const Command& cmd);
        virtual bool onStop(const Command& cmd);
        virtual bool onPause(const Command& cmd);
        virtual bool onResume(const Command& cmd);
        // event handler
        static int onServoFault(IEventHost*ins, int evid, void *arg);
\endcode
SingleAxis Instance가 초기화 되는 도중에
RoboticModule::initialize()함수에서 SingleAxis::pdoUpdateTask()를 thread로 실행시킨다.
SingleAxis::pdoUpdateTask()함수는 로봇 명령어 처리(모션 생성과 일시정지, 종료 등) 및 서보모터 reference 갱신의 중심이 된다.
이 함수는 다음과 같은 단순한 구조로 되어 있다.
\code{.cpp}
void SingleAxis::pdoUpdateTask()
        while(!exit_) {
            pdo_arrival_event_->wait(0.1);  // 매 cycle(1ms)마다 깨어남 
            // --- 중략 ---
            if (commandq_.size() > 0) { // 사용자 명령이 있다면 처리함
                processCommand();
            }
            if (motion_ != nullptr) { // reference를 업데이트 하거나 유지함 
                updateReference();
            } else {
                stayReference();
            }
        }
\endcode
updateReference()함수는 현재 유효한 모션 객체로부터 서보 reference 좌표를 계산하는 역할을 수행한다.
또한 servo fault를 체크하고, fault 발생시 handler를 호출한다. \n
processCommand()함수는 사용자 명령에 대한 handler를 호출한다.
handler 중의 하나인 SingleAxis::onMoveA(Command& cmd) 함수를 살펴 보자.
\code{.cpp}
    bool SingleAxis::onMoveA(Command& cmd)
    {
        auto pos = (vel_ == 0) ? motors_[0]->getActual()[0] : ref_pos_;
        cmd.target = validateTargetPosition(cmd.target, "Move Absolute");
        if (control_mode_ == ServoOpMode::CSP) {
            csp_motion_->setAccLimit(cmd.max_d, cmd.max_a);
            if (cmd.duration<0) {
                csp_motion_->setMoveSpeed(pos, cmd.target, cmd.max_v, vel_);
            } else if (!csp_motion_->setMoveDuration(pos, cmd.target, cmd.duration, vel_)) {
                SEP_LOGE("[%s] Fail to make velocity profile given parameters.", name_.c_str());
                last_mc_id_ = cmd.id;
                motion_event_.wakeup();
                return false;
            }
            motion_ = csp_motion_.get();
        } else {
        // -- 중략(Profile Position Mode 일 경우 처리) ---
        }
        if (motion_) motion_->setId(cmd.id);
        return true;
    }
\endcode
CSP 모드인지 확인하고, csp_motion_ 객체를 이용하여 trajectory를 초기화 한다.
만약 duration constraint가 없으면 csp_motion_->setMoveSpeed(pos, cmd.target, cmd.max_v, vel_) 함수를 호출하여 
time-optimal trajectory를 생성하고, duration constraint가 주어진 경우는 
csp_motion_->setMoveDuration(pos, cmd.target, cmd.duration, vel_) 함수를 호출하여 주어진 time constrained trajectory를 생성한다.


\par trajectory logging

TRAJ_LOG_ENABLE(axes_module.cpp:23) 매크로가 활성화 되어 있으면 
SingleAxis는 초기화 과정에서 traj_logger_ instance를 생성한다.
traj_logger_가 생성되면 updateReference() 함수 호출때 마다 SingleAxis::logTrajectory()함수가 호출되어 
time stamp, reference position, reference velocity, actual position, actual velocity, actual torque, position following error 등을 파일에 저장하게 된다.
저장된 로그파일은 tools/plotsingleaxis.py를 이용하여 plot 가능하다.
- example
\code{.sh}
ln -s ../../../../tools/plotsingleaxis.py plot # 사용 편의를 위해 simbolic link 생성
./plot front_wheel_01180324_141948.txt
\endcode




\subsection mi_pairedaxes Paired axes robot

PaiedAxes는 master 축의 속도나 torque 추종을 위해 만들어진 확장이다. 
사용방법은 SingleAxis와 동일하며 
Instance를 생성 및 초기화 때 Slave축에 대한 부가적인 정보를 주어야 한다.
\code{.cpp}
class PairedAxes : public SingleAxis {
    PairedAxes(const char* name, ServoOpMode cmode, ServoOpMode slave_cmode = ServoOpMode::CST);
    bool initialize(int master_axis, int slave_axis, const char* master_model = nullptr, const char* slave_model = nullptr, double master_gear_ratio = 1);
    // --- 하략 ---
\endcode
다음은 사용 예이다.
\code{.cpp}
        drv_ = make_unique<PairedAxes>("drv", ServoOpMode::CSP);
        drv_->setUserUnit(0, "m", 1000000); // pulse per m
        drv_->setUserUnit(1, "m", 1000000);
        SEP_REQUIRE_RET(drv_->initialize(0,1), false);
\endcode

참고로 Slave축에 대한 속도 혹은 torque reference 생성은 following 지연현상을 최소화 하기 위해서 ecmaster단에서 이뤄진다.
\code{.cpp}
//ecmaster_service.cpp; 
void EcMasterSvc::doPdoJobs()
    //--- 중략 ---
        // 동기 축 속도 복사
        if (slave.following_axis != -1) {
            auto &master_axis = servos_[slaves_[slave.following_axis].context];
            auto &slave_axis = servos_[slave.context];
            if (slave_axis.getOperationMode() == static_cast<int>(ServoOpMode::CSV)) {
                slave_axis.setVelocityTarget(master_axis.statusWord().fault ? 0 : master_axis.getVelocity());
            } else if (slave_axis.getOperationMode() == static_cast<int>(ServoOpMode::CST)) {
                slave_axis.setTorqueTarget(master_axis.statusWord().fault ? 0 : master_axis.getTorque());
            }
        }
    //--- 하략 ---
\endcode


\subsection mi_multiaxes Multi-axes robot
MultiAxes 클래스는 내부적으로 SingleAxis vector를 가지고 있다.
사실상 모든 기능은 SingleAxis에 의해 수행되는데,
N개의 SingleAxis 객체를 생성하는 것과 N-축 MultiAxes를 사용한 것의 차이점은 
pdoUpdateTask()가 하나만 생성되므로 리소스 부하를 줄여준다는 점과
다축 동기 모션에 대한 인터페이스를 제공해 준다는 점이다.

다음은 MultiAxes를 사용한 예제 코드이다. 
\code{.cpp}
    MultiAxes ax2("ax2", 4, ServoOpMode::PP);
    ax2.addAxis(0, 0);
    ax2.addAxis(1, 1);
    ax2.addAxis(2, 2);
    ax2.addAxis(3, 3);
    SEP_REQUIRE_RET(ax2.initialize(), -1);
    ax2.setMax(0, 12000, 10000);
    ax2.setMax(1, 12000, 10000);
    ax2.setMax(2, 12000, 10000);
    ax2.setMax(3, 12000, 10000);
    printf("Single axis motion\n");
    int cmd1 = ax2.moveDistance(0, 3600);
    ax2.waitForMotionDone(0, cmd1);

    printf("multi axes with single commands\n");
    auto cmds = ax2.moveDistance({{0, 36000},{1, 36000},{2, 36000},{3, 36000}});
    ax2.waitForMotionDone(cmds[0]);
    ax2.waitForMotionDone(cmds[1]);

    printf("multi axes with single command, diffent displacement\n");
    cmds = ax2.moveDistance({{0, 3600},{1, 7200}});
    ax2.waitForMotionDone(cmds[0]);
    ax2.waitForMotionDone(cmds[1]);

    printf("multi axes with single command, with duration\n");
    cmds = ax2.moveDistance({{0, 3600, -1, -1, -1, 10},{1, 7200}});
    ax2.waitForMotionDone(cmds[0]);
    ax2.waitForMotionDone(cmds[1]);

\endcode



\section mi_ioboard GPIO control & Generic IO board interface

GPIO(General Purpose I/O)는 Digiti Input 및 Output을 말한다.
Master 제어기 보드는 38개의 Digital Input과 22개의 Digital Output 포트를 가지고 있으며,
Stocker 및 OHT IO Slave 보드에 추가적인 GPIO 포트들이 있다.
Onboard에 있는 각각의 Port control은 Gpio class에 구현되어 있으며,
IO Slave 보드들의 포트는 PDO에 매핑되어 있다.

Sephirot Platform에서는 IO보드에 상관 없이 generic하게 IO를 제어하기 위해서
IoBoard와 ECatIoInPdo/ECatIoOutPdo template class를 제공한다. 
IoBoard instance들은 Port Data를 공유메모리를 통해 접근한다.
\code{.cpp}
    enum IOBoarType {
        // ethercat slave type
        STOCKER_IO,
        OHT_FRONT_IO,
        OHT_REAR_IO,
        // on-board type
        ONBOARD_TYPE    = 0x1000,
        OHT_ONBOARD_IO,
    };
    template <int type, int IO_SLAVE_ID, int AI_PORT_COUNT, int DI_PORT_COUNT, int AO_PORT_COUNT, int DO_PORT_COUNT>
    class IoBoard {
    public:
        using IoPdoIn = ECatIoInPdo<AI_PORT_COUNT, DI_PORT_COUNT>;
        using IoPdoOut = ECatIoOutPdo<AO_PORT_COUNT, DO_PORT_COUNT>;
        //--- 중략 ---
        bool initialize() override ;
        virtual uint16_t readAi(int port);
        virtual bool writeAo(int port, uint16_t val);
        virtual bool readDi(unsigned portbit) { return readDi(portbit>>16, portbit & 0x0000FFFF);}
        virtual bool readDi(int port, int bit);
        virtual bool readDo(unsigned portbit) { return readDo(portbit>>16, portbit & 0x0000FFFF);}
        virtual bool readDo(int port, int bit);
        virtual bool writeDo(unsigned portbit, bool val) { return writeDo(portbit >> 16, portbit &0x0000FFFF, val);}
        virtual bool writeDo(int port, int bit, bool val);
        //--- 중략 ---
    protected:
        std::unique_ptr<SepShm> shm_;
        std::string name_;
        IoPdoOut* out_pdo_;
        IoPdoIn* in_pdo_;
        int bus_cycle_us_;
        int slave_id_;
        bool initialized_;
    };

    // slave type, slave id, ai port, di port byte, ao port, do port byte
    typedef IoBoard<STOCKER_IO, 4,4,6,4,5> EcStockerIo;
    typedef IoBoard<OHT_FRONT_IO,5,2,6,2,6> EcOHTFrontIo;
    typedef IoBoard<OHT_REAR_IO,4,4,6,4,6> EcOHTRearIo;        
\endcode     
GPIO read/write method를 살펴보면 인자로 port와 bit index를 받는 것을 알 수 있다.
port index는 board내의 gpio chip 인덱스이며 bit index는 chip내의 gpio 인덱스이다. 

Onboard IO의 경우 OhtOnboardIO class에 실체가 구현되어 있다.

Onboard를 포함한 각 gpio port는 board id, port index, bit index로 유일하게 정의될 수 있다. 
이렇게 정의된 각 gpio id 목록은 enum OhtIoPortId값으로 사용되고 있다.
\code{.cpp}
constexpr unsigned sephi::oht::makePortId(unsigned board, unsigned port, unsigned bit)
{ 
    return ( (board<<24) + (port<<16) + bit);
}
\endcode
OHT 제어기의 경우 위에서 정의된 portid를 다음과 같이 사용하고 있다.
\code{.cpp} 
bool OHTController::readDi(unsigned portid)
{
    unsigned board = portid >> 24;
    unsigned port_bit = portid & 0x00FFFFFF;
    bool ret(false);
    switch(board){
        case OhtIoBoardId::OHT_ONBOARD_DI:  ret = onboard_io_.readDi(port_bit); break;
        case OhtIoBoardId::OHT_FRONT_DI:    ret =  front_io_->readDi(port_bit); break;
        case OhtIoBoardId::OHT_REAR_DI:     ret = rear_io_->readDi(port_bit);break;
        default:
            throw std::runtime_error("[Read Di] Invalid Board Type");
    }
    if (active_low_ports_.find(portid) != active_low_ports_.end()) {
        return !ret;
    } else {
        return ret;
    }
}
\endcode
이제 Application code에서는 다음 코드 처럼 board type에 상관없이 gpio를 활용할 수 있다.
\code{.cpp}
    //--- 상략 ---
    if (mark_type_ == MarkType::NODE_MARK) {
        precheck = readDi(_stop_driving_precheck);
        stopcheck = readDi(_stop_driving_front);
    } else if (mark_type_ == MarkType::STATION_MARK) {
        precheck = readDi(_stop_trans_front);
        stopcheck = readDi(_stop_trans_rear);
        //SEP_LOGT("precheck = %d, stop = %d\n", precheck, stopcheck);
    }
    //--- 하략 ---
\endcode




\section mi_serial Serial port control


Sephirot platform의 Serial 통신 class는 boost.asio 라이브러리를 활용하여 구현되어 있다. 
주요 메소드는 다음과 같다.
\code{.cpp}
class SerialPort {
//--- 중략 ---
public:
    bool start(const char *com_port_name, int baud_rate = 9600,
                        int character_size = 8,
                        StopBits::type _stop_bit = StopBits::one,
                        Parity::type _parity = Parity::none,
                        FlowControl::type  _flow_control = FlowControl::none);
    int write(const char *buf, const int size);
    bool read(char *buf, const int size, int *read_size);
    bool readTimeout(char *buf, const int size, int *read_size, int64_t timeout_ms = -1, int end_of_line = 0xffff);
//--- 하략 -----    
\endcode

시리얼 통신을 시작하기 위한 함수는 start() 함수이며 
디폴트 옵션은 9600bps baud rate, 8bit char, 1 stop bit, no parity, no flow control이므로
인자를 조정하여 포트 설정에 맞추어 사용해야 한다.

Serial port로 데이터를 송신하는 함수는 write()이고, 수신 하는 함수는 readTimeout()이다.
수신시에 Timeout 및 end-of-line을 지정하여 원하는 문자까지 잘라서 수신가능하다.


*/
