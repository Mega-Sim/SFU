//
// Created by master on 17. 3. 23.
//

#ifndef IPCTEST_TEST_CONFIG_H
#define IPCTEST_TEST_CONFIG_H

#undef SEP_TRACE
#define SEP_TRACE SEP_LOGT
#define TEST_SHM
//#define TEST_MSGQ
//#define TEST_RPC

typedef int MsgType;
using IntegerQ = sephi::SepMsgQ<MsgType, 10000>;

#endif //IPCTEST_TEST_CONFIG_H
