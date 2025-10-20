/**
\page pg_utilities Utilites  
\ref main_toc "back"

\par 개요
이 페이지는 차세대 제어기 SW Platform에서 활용하고 있는 기본적인 Utility Library에 대해서 설명한다.

\section util_log  Sephi Log

Multi-task모델의 프로그램 개발을 하다보면 브레이크 포인트나 
단계적 실행을 지원하는 디버기 만으로는 부족한 경우를 많이 경험하게 된다. 
브레이크 포인트 방식의 디버거는 여러 task가 concurrent하게 수행되는 가운데 발생하는 
task간 coupling에 의한 이슈들 잡지 못한다. 

Logging은 current program 디버깅 문제 해결을 위한 가장 원적인 방법이면서 강력한 방법이다.
Logging은 디버깅에 필요한 정보를 코드에 삽입하여 실행하면서 계속 log 메시지를 남긴다.
Ideal한 Logging System은 가능한 많은 디버깅 정보를 추출하면서도 task간의 coupling에 영향을 미치지 않아야 한다.

Sephirot Logging System은 
기본적으로 로그시점의 time stamp, process id, 
thread id, 소스파일 정보, log level 등의 context 정보와 사용자가 남긴 메시지 정보 모아 메시지를 생성한다.
사용자는 추가적인 노력없이 로깅 당시 상황 정보를 알수 있으며, 사용자 메시지 형식을 정형화 하면 
이후 로그메시지 분석에 필요한 정보를 더 쉽게 다를 수 있다. 

Logging system 구현에 있어서, 로깅 부하를 최소화 하기 위해서 두 단계의 double-buffering과 bottom-half 기법을 사용하고 있다. 
로그를 남기는 thread의 부하를 최소하 하도록 실행 thread는 미리 할당된 log pool에서 빈 log 객체를 꺼내고,
로그 메시지를 채운 후 queue에 넣고 바로 리턴한다. \n 
큐 데이터는 낮은 우선순위의 백그라운드 task(\ref logSender)에 의해 서버로 전송되며, 
파일에 기록하는 최종 작업은 로그 서버(\ref logsvr.py)에서 수행 된다. 

Sephrot Log의 사용자 인터페이스는 다음 매크로 형태로 제공된다. (가변인자는 printf 함수와 동일)
\code{.cpp}
#define SEP_LOGT(...) __SEP_LOG(LogLevel::TRACE, __VA_ARGS__)
#define SEP_LOGD(...) __SEP_LOG(LogLevel::DEBUG, __VA_ARGS__)
#define SEP_LOGI(...) __SEP_LOG(LogLevel::INFO, __VA_ARGS__)
#define SEP_LOGW(...) __SEP_LOG(LogLevel::WARN, __VA_ARGS__)
#define SEP_LOGE(...) __SEP_LOG(LogLevel::ERROR, __VA_ARGS__)
\endcode

로그 서버로 전송하지 않고 직접 화면에 출력하는 TRACE 매크로도 다음과 같이 제공된다.
\code{.cpp}
#define SEP_TRT(...) __SEP_TRACE('T', __VA_ARGS__)
#define SEP_TRD(...) __SEP_TRACE('D', __VA_ARGS__)
#define SEP_TRI(...) __SEP_TRACE('I', __VA_ARGS__)
#define SEP_TRW(...) __SEP_TRACE('W', __VA_ARGS__)
#define SEP_TRE(...) __SEP_TRACE('E', __VA_ARGS__)
#define SEP_TRACE SEP_TRT
\endcode 

마지막으로 condition 체크 실패시 실패 조건을 자동으로 trace로 남기는 매크로들이 다음과 같이 제공된다.
\code{.cpp}
#define SEP_REQUIRE_RET(cond,val) do{ if (!(cond)){SEP_TRACE(#cond "\n"); return (val);} } while(false)
#define SEP_REQUIRE(cond) do{ if (!(cond)){SEP_TRACE(#cond "\n"); return;} } while(false)
#define SEP_REQUIRE_THROW(cond) do{ if (!(cond)){throw std::runtime_error(#cond "\n"); } } while(false)
\endcode

\par 사용 예
\code{.cpp}
    // stack 초기화, network 설정, ...
    SEP_LOGI("Initialize EtherCAT Master\n");
    if (!opt_->emulation_mode) {
        SEP_REQUIRE_RET(initMaster(), false);
    // ....
\endcode

\par 관련 구현 파일 
- log.h 
- logger.cpp
- logsvr.py 

\section util_comm  Communication Utilities

Concurrent program을 개발하려고 하면 직면하게 되는 첫번째 문제 중의 하나는 모듈 배치이다. 
모듈은 동일 프로세스 안에서 별개의 thread로, 혹은 동일 Machine안에서 별개의 process로, 
마지막으로 다른 Machine에 배치될 수 있다. 
자유로운 모듈 배치는 resource 활용과 safety측면에서 잇점을 추구할 수 있지만, 
성능 및 개발용이성 측면에서 손해를 볼 수 있으므로 신중하게 선택해야 한다.

Sephirot는 자유로운 모듈 배치를 위해서 다양한 수준의 middleware를 제안하고 있다. 

\subsection util_comm_ipc IPC(Inter Process Communication)
리눅스는 가상메모리를 제공하는 OS이므로 다른 프로세스의 정보에 직접 접근하는 것은 불가능 하다.
고전적으로 Linux에서 많이 사용하는 방법은 POSIX Thread 라이브러리나 System-V IPC 인터페이스를 활용하는 것이다.
https://www.joinc.co.kr/w/Site/Thread/Beginning/PthreadApiReference, http://rts.gnu.ac.kr/class/2013fall/sys_perf/ch10.pdf \n 
그렇지만 위 방법은 시스템간의 이식성을 저하하므로 Sephirot에서는 고려하지 않았다.
Sephirot는 thread 및 process 관리를 위해
C++11에 표준 라이브러리로 등록된 thread 라이브러리 http://en.cppreference.com/w/cpp/thread \n 
boost 1.64버전에 추가된 boost.process 라이브러리 https://www.boost.org/doc/libs/1_66_0/doc/html/process.html \n 
및 프로세스간의 통신을 위한 boost.interproces 라이브러리 https://www.boost.org/doc/libs/1_66_0/doc/html/interprocess.html
활용을 제안한다.  

Sephirot에는 위 라이브러리 활용 편의를 위한 몇가지 wrapper class를 제공하고 있다. 
- \ref SepShm
- \ref SepMsgQ
- \ref ThreadCondition, \ref IpcCondition

다음은 공유메모리와 Codition을 활용한 예이다. 
\par example - server 
\code{.cpp}
    IpcCondition cond("shmcond", true); // create condition object
    SepShm shm("shmkey", 0x1000);

    while(true) {
        if (cond.wait(2.5)) {
            SEP_TRACE("waken up by client.; message : %s \n", shm.address);
        } else {
            SEP_TRACE("timeout\n");
        }
    }
\endcode

\par example - client
\code{.cpp}
    SepShm shm("shmkey");
    IpcCondition cond("shmcond");
    snprintf(shm.address, 100, "hello server?\n");
    cond.wakeUp();
\endcode

다음은 Message Queue를 활용한 예이다. 

\par example - message receiver
\code{.cpp}
    typedef int MsgType;
    using IntegerQ = sephi::SepMsgQ<MsgType, 1000>;
    
    IntegerQ  mq("testmq", true);    
    while(true) {
        if (mq.get(msg, 2.0)) {
            SEP_TRACE("mq waken up by client.; message : %d \n", msg);
        } else {
            SEP_TRACE("mq timeout\n");
        }
    }
\endcode

\par example - message sender
\code{.cpp}
    IntegerQ mq("testmq");
    const int size = 1000;
    SEP_LOGT("mq client send %d messages\n", size);
    for(MsgType i = 0; i < size; ++i) {
        mq.put(i);
    }
\endcode


\subsection util_comm_sock Socket 통신 
Socket 통신은 우리가 흔히 알고있는 네트워크 프로그래밍의 주요 수단이다. 
주로 Machine간의 통신에 활용 되지만 미들웨어가 잘 구비되어 사용성과 이식성이 좋으므로 
로컬 Machine내부의 프로세스간 통신에도 많이 활용된다.


소켓을 선언하기 위해서는 프로토콜 체계와 end-point인 주소를 결정해야 한다.
프로토콜 체계는 PF_INET(IPv4), PF_INET6(IPv6), PF_LOCAL(Unix domain socket) 등이 있다.
프로토콜 체계 대신 Address family를 사용하기도 하는데 사실상 같은 내용이다.
일반적으로는 PF_INET을 사용하는데, Local Machine 내부에서만 사용하는 경우
PF_LOCAL을 사용하면 좀 더 성능을 높일 수 있다. 

OS에서 제공하는 low-level socket api를 사용하여 프로그래밍 할 수도 있지만,
low-level socket api는 초보 프로그래머들이 예외처리 실수를 많이하는 것으로 악명높으므로, 더 상위 wrapper class를 사용하는 것을 권장한다.

Sephirot에 있는 Network 모듈들은 Poco.Network 및 boost.asio를 이용하여 
구현되어 있다. 
각 미들웨어의 설명은 미들웨어 홈페이지에 자세히 설명되어 있으므로 여기서는 생략한다.
참고로 Poco.Network이 사용하기는 더 간편하고, asio는 tuning할 여지가 더 많다.
Unix domain socket을 사용하기 위해서는 Poco 1.8.0이상 버전이 필요하다.
- https://pocoproject.org/slides/200-Network.pdf
- https://www.boost.org/doc/libs/1_66_0/doc/html/boost_asio.html

\subsection util_comm_rpc RPC(Remote Procedure Call)
Remote Procedure Call은 다른 프로세스나 머신의 기능을 내부 함수처럼 호출하여 
사용할수 있게 하는 미들웨어이다. 
Caller 머신과 Callee 머신은 서로 상이한 OS나 CPU 구조를 가지고 있을 수 있으며,
구현된 프로그래밍 언어도 다를 수 있다. 
또한 네트워크를 거치면서 전송 오류도 발생할 수 있다. 
이 모든 것들을 커버하면서도 통신 속도를 최대화 하는 것이 좋은 RPC 미들웨어의 조건이다.

OMG의 CORBA나 MS의 DCOM을 일종의 RPC 인프라로 볼 수 있다. 
그런데 DCOM은 MS OS에 제약되어 있고, CORBA도 개발된지 오래되어 요즘에는 
잘 사용되지 않고 있다. 
최근에는 Google에서 protobuf에 기반하여 개발한 grpc가 널리 사용되고 있으며, 
Sephirot에서도 RPC수단으로 grpc를 사용하고 있다. 

GRPC에 대한 설명은 홈페이지에 잘 되어 있으므로 생략한다.
- https://grpc.io/


\subsection util_comm_serial Serial 통신 

여전히 많은 디바이스들이 RS-232와 같은 시리얼 케이블을 통해 연결되어 있다. 
OS는 시리얼 터미널제어를 위한 termios 구조체를 직접제어 하는 저수준 api를 제공한다. 
Linux에서 저수준의 시리얼 제어는 다음 링크를 참고 바란다.
- https://wiki.kldp.org/HOWTO/html/Serial-Programming/Serial-Programming-HOWTO.html

Sephirot에서는 OS의존적인 구현을 지양하므로 
boost.asio를 활용하여 시리얼 디바이스를 제어한다. 
asio를 활용한 시리얼 포트 제어 예제는 다음을 참고 바란다.
- https://gist.github.com/yoggy/3323808


\section util_db  Database
데이터 베이스는 데이터의 저장과 공유 및 변경에 대한 안전성을 제공해주는 수단이다.
일반적으로 MySQL과 같은 RDBMS가 많이 사용되지만 Schema 관리 등 프로그래밍 입장에서는 분리한 요소들이 많으므로  
Sephirot에서는 MongoDB나 Redis와 같은 NoSQL DBMS를 지향하고 있다.

DBMS에 대한 상세 설명은 매우 길어 질수 있으니 설명은 생략한다. 
Poco.Data모듈에서 대부분의 DBMS Driver를 지원하므로 이를 활용하기를 추천한다.


\section util_fileformat  File Format Utilities 

\subsection util_yaml YAML
설정값이나 상태데이터 등 구조화된 데이터의 보관이나 전송을 위해
많이 쓰이는 방식은 json이나 yaml 형식을 사용하는 것이다. 
json보다 yaml이 가독성이 좋으몰 Sephirot에서는 yaml을 기본 configuration 포맷으로 사용하고 있다.

\par yaml parser
- C++ : https://github.com/jbeder/yaml-cpp
- python : http://pyyaml.org/wiki/PyYAMLDocumentation

\subsection util_csv CSV 
맵 데이터처럼 table화 된 데이터를 다뤄야 할 경우도 있다. 
가령 csv(comma separated value) 형식 파일이 그것이다.
Sephirot에는 Ben Strasser의 구현(BSD-3 license)을 포함하고 있으므로 
필요시 활용 바란다.
아래는 간단한 예제. 
- datafile : cid.tsv (tab separated)
\code{.sh}
LayoutID	LayoutType	ResetNode	StopNode1	StopNode2	ResetNodeType	CIDDirection1	CommStartNode1_1	CommStartNode1_2	CommStartNode1_3	CommStartNode1_4	CommStartNode1_5	CommStartNode1_Offset	CIDDirection2	CommStartNode2_1	CommStartNode2_2	CommStartNode2_3	CommStartNode2_4	CommStartNode2_5	CommStartNode2_Offset	Updated Date	
CID002	NormalCID	114	113	122	NormalNode	Left	111	112	113	0	0	1000	Right	122	0	0	0	0	300		
CID003	NormalCID	121	0	111	NormalNode	Left	0	0	0	0	0	0	Right	111	0	0	0	0	200		
CID005	NormalCID	144	143	142	CommonNode	Left	118	143	0	0	0	100	Right	142	0	0	0	0	300		
CID006	NormalCID	142	123	141	CommonNode	Left	123	0	0	0	0	0	Right	140	141	0	0	0	0		
..생략..
\endcode
- code : LayoutID, ResetNode column만 추출하여 뿌리는 예
\code{.cpp}
#include <iostream>
#include "csv.h"

int main(){
    std::string path = "cid.tsv";
    io::CSVReader<2, io::trim_chars<' ', '\t'>, io::no_quote_escape<'\t'>> in(path);
    in.read_header(io::ignore_extra_column, "LayoutID", "ResetNode");
    std::string layout; int node;
    while(in.read_row(layout, node)){
        std::cout << layout << "," << node << std::endl;
    }
    return 0;
}
\endcode

*/
