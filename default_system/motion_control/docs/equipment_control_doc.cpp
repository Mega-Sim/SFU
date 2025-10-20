/** 

\page pg_eq_control Equipment Control

\ref main_toc "back"


\section eq_stk Stocker 

Stocker 제어시스템은 크레인 제어를 위한 4개의 서보모터와 하나의 IO 보드로 구성되어 있다. 

각 축의 limit설정 및 application 데이터 설정은 config/stocker_config.yml에 있으며
StockerConfig class를 이용해 load하여 사용할 수 있다.

src/control/test/stocker_demo.cpp는 Stocker 제어 기능 테스트를 위해 만들어진  코드이다. 



\section eq_oht OHT 

OHT는 주행 및 이적재를 위한 4개의 서보 모터와 2개의 IO 보드 및 마스터제어기 내장 IO로 구성되어 있다.
OHT제어기의 주 기능은 주행과 이적재 이다.

OHTController class는 OHT simulator를 검증하기 위해서 만들어진 클래스인데,
OHT 제어기의 이해를 위해서는 이 class 및 하위 클래스들을  살펴보는 것이 좋다.

- OhtConfig
- OHTMap
- OhtOnboardIO, OhtIoPortId
- Driving
    - Localization
    - MarkDetector
- Transfer
    - PioControl
    - TaskStepHandlerSet
    - TaskTimer
    - TransferTask
    


\section eq_amc AMC Compliant Interface 

차세대 모션 제어기의 요구사항 중의 하나는 AMC 보드를 사용하는 상위 제어기(IPC)의 SW 변경 없이 
AMC 보드만 차세대 모션 제어기로 교체하여 사용가능하게 하는 것이었다.

기존 AMC 제어기는 IPC와 PC104버스로 연결된 DPRAM(Dual Port RAM)을 통해 통신을 한다.
AMC제어기는 DPRAM에 접근하기 위한 디바이스드라이버 및 User Library를 제공했다.
차세대 모션 제어기는 PC104버스 및 DPRAM대신 Gbit Ethernet을 통해 IPC와 연결되어 있다. 
상위 User Library 수준의 호환성을 위해서는 DPRAM동작을 emulation하는 계층이 필요하다.

이를 위해서 IPC쪽에서는 amclib에 amc_client.cpp에서 모션 제어기와 통신을 구현하였고,
모션 제어기 쪽에는 src/app/amc/amc_dpram.cpp가 dpram을 emulation하도록 만들어져 있다.


AMC 제어기 동작을 emulation하는 부분은 AmcEmulator 클래스에서 구현되어 있다.
Stocker용 AMC 제어기와 OHT용 AMC 제어기 동작이 일부 다르기 때문에 
이를 위해서 AmcEmulator를 상속 받는 Amc4Stocker와 Amc4OHT 클래스가 분리되어 있다.


기본적인 동작은 IPC User Library가 호출되면 
DPRAM emulator가 dpram 이미지를 업데이트 하고,
모션제어기 쪽에서 AmcEmulator::dispatch()를 통해 필요한 서비스 핸들러가 호출되는 방식이다.
서비스 핸들러는 AMC 제어기에서 구현되어 있던 기능들을 차세대 모션제어기의 기능을 활용하여 새로 구현한 것들이다.


상세 내용은 소스 코드(src/app/amc)를 통해 참고 바란다.


*/
