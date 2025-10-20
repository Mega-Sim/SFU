//---------------------------------------------------------------------------

#ifndef CheckerUnitH
#define CheckerUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
// ------------- 테스트 시뮬레이션 진행 커맨드 -------------
namespace Sim_Cmd_ST
{
	enum Reserved
	{
		TEST_NONE					= 0, // 테스트 진행중이 아님
		TEST_START					= 1, // 테스트 시작. Status Machine은 초기화
		TEST_GOING				    = 2, // 테스트를 진행해 나감
		TEST_PAUSE				    = 3, // 일시정지
		TEST_STOP					= 4,
		// 주의 : 이 부분에 01234 식으로 써 넣으면 컴파일러는 8진수로 해석해 버림!
	};
};

// ------------- 테스트 시뮬레이션 스텝 정의 -------------
namespace Sim_Step_ST
{
	enum Reserved
	{
	    // (0) Not work
		None		= 0,

	    // ### (1) VHL 외부베터리를 연결하고, CID-O-S 양쪽이 깜빡이는지 확인하고, 테스트 벤치로 이동후 OK버튼을 눌러 주세요
		StartSteeringMove,

		// ### (2)
	    StartSteeringFRight,
		StartSteeringFRight_Confirm,		// CID-O-S 양쪽이 깜박거리고 있었습니까? (그림참고)

		// (3)
		StartSteeringRRight,
		StartSteeringRRight_Confirm,		// CID-O-S 양쪽이 깜박거리고 있었습니까? (그림참고)

		// ### (4) O1(Left Select) On
		StartSteeringFLeft,
		StartSteeringFLeft_Confirm,		// CID-O-S 양쪽이 깜박거리고 있었습니까? (그림참고)

		StartSteeringRLeft,
		StartSteeringRLeft_Confirm,		// CID-O-S 양쪽이 깜박거리고 있었습니까? (그림참고)

		SteeringCheckComplete,		// (28) Cid Check Complete

	};

};


// ------------- 테스트 시뮬레이션 스텝 정의 -------------
namespace Sim_Step_Shift
{
	enum Reserved
	{
	    // (0) Not work
		None		= 0,

	    // ### (1) VHL 외부베터리를 연결하고, CID-O-S 양쪽이 깜빡이는지 확인하고, 테스트 벤치로 이동후 OK버튼을 눌러 주세요
		StartShiftMove,

		// ### (2)
		StartShiftRight,
		StartShiftRight_Confirm,		// CID-O-S 양쪽이 깜박거리고 있었습니까? (그림참고)

		StartShiftHome1,
		StartShiftHome1_Confirm,		// CID-O-S 양쪽이 깜박거리고 있었습니까? (그림참고)

		// ### (4) O1(Left Select) On
		StartShiftLeft,
		StartShiftLeft_Confirm,		// CID-O-S 양쪽이 깜박거리고 있었습니까? (그림참고)

		StartShiftHome2,
		StartShiftHome2_Confirm,		// CID-O-S 양쪽이 깜박거리고 있었습니까? (그림참고)

		ShiftCheckComplete,		// (28) Cid Check Complete

	};

};


// ------------- 테스트 시뮬레이션 스텝 정의 -------------
namespace Sim_Step_Hoist
{
	enum Reserved
	{
	    // (0) Not work
		None		= 0,

	    // ### (1) VHL 외부베터리를 연결하고, CID-O-S 양쪽이 깜빡이는지 확인하고, 테스트 벤치로 이동후 OK버튼을 눌러 주세요
		StartHoistMove,

		// ### (2)
		StartHoistDown,
		StartHoistDown_Confirm,		// CID-O-S 양쪽이 깜박거리고 있었습니까? (그림참고)

		StartHoistHome,
		StartHoistHome_Confirm,		// CID-O-S 양쪽이 깜박거리고 있었습니까? (그림참고)

		HoistCheckComplete,		// (28) Cid Check Complete

	};

};
class TUnitChecker : public TForm
{
__published:	// IDE-managed Components
	TBitBtn *btnSMPSCheck;
	TGroupBox *grpSteer;
	TButton *btnStart;
	TButton *btnStop;
	TGroupBox *grpSteeringIO;
	TPanel *Panel18;
	TShape *RStrRightLt;
	TShape *FStrRightLt;
	TLabel *Label17;
	TLabel *Label18;
	TPanel *Panel19;
	TShape *RStrLeftLt;
	TShape *FStrLeftLt;
	TLabel *Label19;
	TLabel *Label20;
	TMemo *mmoMemo;
	TStringGrid *strngrd1;
	TTimer *tmrIOUI;
	TTimer *tmrSimulation;
	TButton *btnClose;
	TGroupBox *grpShiftHome;
	TGroupBox *GroupBox1;
	TPanel *Panel2;
	TShape *Shp_ShiftHome;
	TLabel *Label3;
	TMemo *mmoShiftHome;
	TButton *btnShiftHomeStart;
	TButton *btnShiftHomeStop;
	TStringGrid *strngrdShiftHome;
	TGroupBox *GroupBox2;
	TGroupBox *GroupBox3;
	TPanel *Panel1;
	TShape *ShpHoist;
	TLabel *Label1;
	TMemo *mmoHoist;
	TButton *btnHoistStart;
	TButton *btnHoistStop;
	TStringGrid *strngrdHoist;
	TTimer *tmrSimulationShift;
	TTimer *tmrSimulationHoist;
	TEdit *edtHoistStandard;
	TLabel *Label2;
	void __fastcall tmrIOUITimer(TObject *Sender);
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall btnStartClick(TObject *Sender);
	void __fastcall btnStopClick(TObject *Sender);
	void __fastcall tmrSimulationTimer(TObject *Sender);
	void __fastcall btnSMPSCheckClick(TObject *Sender);
	void __fastcall btnShiftHomeStopClick(TObject *Sender);
	void __fastcall tmrSimulationShiftTimer(TObject *Sender);
	void __fastcall btnShiftHomeStartClick(TObject *Sender);
	void __fastcall btnHoistStopClick(TObject *Sender);
	void __fastcall tmrSimulationHoistTimer(TObject *Sender);
	void __fastcall btnHoistStartClick(TObject *Sender);
private:	// User declarations
	void StatusMachine(int iCmd);
	void Log(UnicodeString strLog, int Type =0);
	void Pass(int iIndex, bool bPass);

	void MaxDistShiftHoist(int iIndex, int MaxDist, bool IsPass = false, bool IsHoist =false);

	void StatusMachineShift(int iCmd);
	void ShiftINCMove(int MoveDist,bool bLeft);

	void StatusMachineHoist(int iCmd);
	void HoistINCMove(int MoveDist);

	void OACISDataSendTP2OHT(short type, short typeNum,int Data1,int Data2=0,int Data3=0,int Data4=0);
public:		// User declarations
	__fastcall TUnitChecker(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TUnitChecker *UnitChecker;
//---------------------------------------------------------------------------
#endif
