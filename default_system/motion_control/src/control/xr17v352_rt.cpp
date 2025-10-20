/*
 * xr17v352_rt.cpp
 *
 *  Created on: 2018. 8. 8.
 *      Author: master
 */



#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <chrono>
#include <iostream>
#include <sched.h>
#include <sys/mman.h>

#include "xr17v352_rt.h"

#ifdef __cplusplus
extern "C" {
#endif

// atlas보드의 XR17V53
const char* devfile[2] = {  "/sys/bus/pci/devices/0000:04:00.0/resource0",
                            "/sys/bus/pci/devices/0000:05:00.0/resource0"};

// IO값이 저장되는 글로벌 변수
uint64_t dioreg;
// XR17V32 디바이스는 억세스 하는 핸들
pXR17V352Dev xr17v352devp;

// input : output
// 0 : LOW 32bit memory address
// 1 : HIGH 32bit memory address
// output
//
uint32_t* PseudoMmap(int position)
{
    uint32_t *addr = (uint32_t*)&dioreg;
    return  (addr + position);
}

pXR17V352Dev GetDIOHandle(void)
{
    return xr17v352devp;
}

int XR17V352Init(void)
{
    int ret = 0;

    xr17v352devp = xr17v352_open();

    if (xr17v352devp == nullptr) {
        ret = -1;
    }

    return ret;
}

pXR17V352Dev xr17v352_open(void)
{
    int fd;
    void *map_base;
    //uint8_t bdata;
    pXR17V352Dev pdev;

    pdev = (pXR17V352Dev)malloc(sizeof(XR17V32Dev));
    memset(pdev, 0, sizeof(*pdev));
    if (pdev == NULL) {
        fprintf(stderr, "Can't malloc\n");
        return NULL;
    }

    for (int i=0; i < 2; i++) {
        //i번째 XR
        fd = open(devfile[i], O_RDWR | O_SYNC);
        if (fd < 0) {
            fprintf(stderr, "file open error:%s\n", devfile[i]);
            free(pdev);
            return NULL;
        }
        pdev->dev[i].fd = fd;

        map_base = mmap(0, 4*1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (map_base == (void *)-1) {
            fprintf(stderr, "mmap failed\n");
            free(pdev);
            return NULL;
        }

        pdev->dev[i].virt_addr = (uint8_t*)map_base;
        pdev->dev[i].lvlctrl[0] = pdev->dev[i].virt_addr + MPIOLVLL;
        pdev->dev[i].lvlctrl[1] = pdev->dev[i].virt_addr + MPIOLVLH;
        pdev->dev[i].selctrl[0] = pdev->dev[i].virt_addr + MPIOSELL;
        pdev->dev[i].selctrl[1] = pdev->dev[i].virt_addr + MPIOSELH;
        pdev->dev[i].trictrl[0] = pdev->dev[i].virt_addr + MPIO3TL;
        pdev->dev[i].trictrl[1] = pdev->dev[i].virt_addr + MPIO3TH;
        pdev->dev[i].odctrl[0]  = pdev->dev[i].virt_addr + MPIOODL;
        pdev->dev[i].odctrl[1]  = pdev->dev[i].virt_addr + MPIOODH;
    }
    // 입력 포트 및 출력 포트를 설정 합니다.
    tUI64 mpiosel;
    tUI64 ompiolvl;

    mpiosel.bit.mpioout1    = 0b0000000000000000;
    mpiosel.bit.mpioout2    = 0b00;
    mpiosel.bit.mpioin1     = 0b111111111111;
    mpiosel.bit.mpioin2     = 0b111111111111;
    ompiolvl.all = 0;
#if 0 // default로 Low 레벨로 합니다.
    ompiolvl.outbits.OHT_EN              = 1;
    ompiolvl.outbits.OBS_EN              = 1;
    ompiolvl.outbits.QRR1_IN1            = 1;
    ompiolvl.outbits.BCR1_IN2            = 1;
    ompiolvl.outbits.QRR2_IN3            = 1;
    ompiolvl.outbits.BCR2_IN4            = 1;
    ompiolvl.outbits.Driving_Stop_EN1    = 1;
    ompiolvl.outbits.Trans_Stop_EN2      = 1;
#endif
    xr17v352_setmpiosel(pdev, mpiosel.all);
    xr17v352_write(pdev, ompiolvl.all);

    return pdev;
}

int xr17v352_close(pXR17V352Dev pdev)
{
    if (pdev == NULL) {
        fprintf(stderr, "pdev is NULL\n");
        return -1;
    }

    for (int i = 0; i < 2; i++) {
        close(pdev->dev[i].fd);
    }

    free(pdev);

    return 0;
}


// 하위 32bit만 의미가 있습니다.
int xr17v352_setmpiosel(pXR17V352Dev pdev, uint64_t selval)
{
    int ret = 0;

    if (pdev == NULL) {
        return -1;
    }

    tReg64 tempval;
    tVal64 regval;

    regval.all = selval;

    tempval.bit.mpioout1    = regval.bit.mpioout1;
    tempval.bit.mpioout2    = regval.bit.mpioout2;
    tempval.bit.mpioctl     = 0x00; // 모두 output
    tempval.bit.mpioin1     = regval.bit.mpioin1;


    *pdev->dev[0].selctrl[0] = tempval.byte[0];
    *pdev->dev[0].selctrl[1] = tempval.byte[1];
    *pdev->dev[1].selctrl[0] = tempval.byte[2];
    *pdev->dev[1].selctrl[1] = tempval.byte[3];

    return ret;
}

int xr17v352_getmpiosel(pXR17V352Dev pdev, uint64_t *selval)
{
    int ret = 0;

    if (pdev == NULL) {
		return -1;
    }

    tReg64 tempval;
    tVal64 regval;
    uint64_t *p = selval;

	tempval.byte[0] = *pdev->dev[0].selctrl[0]; // 7-0
	tempval.byte[1] = *pdev->dev[0].selctrl[1]; // 15-8
    tempval.byte[2] = *pdev->dev[1].selctrl[0]; // 23-26
    tempval.byte[3] = *pdev->dev[1].selctrl[1]; // 31-24

    regval.all = 0; // 초기화
    regval.bit.mpioout1 = tempval.bit.mpioout1;
    regval.bit.mpioout2 = tempval.bit.mpioout2;
    regval.bit.mpioin1  = tempval.bit.mpioin1;
    regval.bit.mpioin2  = tempval.bit.mpioin1;

    *p = regval.all;


    return ret;
}

// 하위 32bit만 의미가 있습니다.
int xr17v352_setmpioinv(pXR17V352Dev pdev, uint64_t invval)
{
    int ret = 0;

    if (pdev == NULL) {
		return -1;
    }

    tReg64 tempval;
    tVal64 regval;

    regval.all = invval;

    tempval.bit.mpioout1    = regval.bit.mpioout1;
    tempval.bit.mpioout2    = regval.bit.mpioout2;
    tempval.bit.mpioctl     = 0x00; // 모두 output
    tempval.bit.mpioin1     = regval.bit.mpioin1;


    *pdev->dev[0].invctrl[0] = tempval.byte[0];
    *pdev->dev[0].invctrl[1] = tempval.byte[1];
    *pdev->dev[1].invctrl[0] = tempval.byte[2];
    *pdev->dev[1].invctrl[1] = tempval.byte[3];

    return ret;
}

int xr17v352_getmpioinv(pXR17V352Dev pdev, uint64_t *invval)
{
    int ret = 0;

    if (pdev == NULL) {
		return -1;
    }

    tReg64 tempval;
    tVal64 regval;
    uint64_t *p = invval;

    tempval.byte[0] = *pdev->dev[0].invctrl[0]; // 7-0
    tempval.byte[1] = *pdev->dev[0].invctrl[1]; // 15-8
    tempval.byte[2] = *pdev->dev[1].invctrl[0]; // 23-26
    tempval.byte[3] = *pdev->dev[1].invctrl[1]; // 31-24

    regval.all = 0;
    regval.bit.mpioout1 = tempval.bit.mpioout1;
    regval.bit.mpioout2 = tempval.bit.mpioout2;
    regval.bit.mpioin1  = tempval.bit.mpioin1;
    regval.bit.mpioin2  = tempval.bit.mpioin1;

    *p = regval.all;

    return ret;
}

int xr17v352_setmpiotri(pXR17V352Dev pdev, uint64_t trival)
{
    int ret = 0;

    if (pdev == NULL) {
		return -1;
    }

    tReg64 tempval;
    tVal64 regval;

    regval.all = trival;

    tempval.bit.mpioout1    = regval.bit.mpioout1;
    tempval.bit.mpioout2    = regval.bit.mpioout2;
    tempval.bit.mpioctl     = 0x00; // 모두 output
    tempval.bit.mpioin1     = regval.bit.mpioin1;


    *pdev->dev[0].trictrl[0] = tempval.byte[0];
    *pdev->dev[0].trictrl[1] = tempval.byte[1];
    *pdev->dev[1].trictrl[0] = tempval.byte[2];
    *pdev->dev[1].trictrl[1] = tempval.byte[3];


    return ret;
}

int xr17v352_getmpiotri(pXR17V352Dev pdev, uint64_t *trival)
{
    int ret = 0;

    if (pdev == NULL) {
		return -1;
    }

    tReg64 tempval;
    tVal64 regval;
    uint64_t *p = trival;

    tempval.byte[0] = *pdev->dev[0].trictrl[0]; // 7-0
    tempval.byte[1] = *pdev->dev[0].trictrl[1]; // 15-8
    tempval.byte[2] = *pdev->dev[1].trictrl[0]; // 23-26
    tempval.byte[3] = *pdev->dev[1].trictrl[1]; // 31-24

    regval.all = 0;
    regval.bit.mpioout1 = tempval.bit.mpioout1;
    regval.bit.mpioout2 = tempval.bit.mpioout2;
    regval.bit.mpioin1  = tempval.bit.mpioin1;
    regval.bit.mpioin2  = tempval.bit.mpioin1;

    *p = regval.all;

    return ret;
}

int xr17v352_setmpiood(pXR17V352Dev pdev, uint64_t odval)
{
    int ret = 0;

    if (pdev == NULL) {
		return -1;
    }

    tReg64 tempval;
    tVal64 regval;

    regval.all = odval;

    tempval.bit.mpioout1    = regval.bit.mpioout1;
    tempval.bit.mpioout2    = regval.bit.mpioout2;
    tempval.bit.mpioctl     = 0x00; // 모두 output
    tempval.bit.mpioin1     = regval.bit.mpioin1;


    *pdev->dev[0].odctrl[0] = tempval.byte[0];
    *pdev->dev[0].odctrl[1] = tempval.byte[1];
    *pdev->dev[1].odctrl[0] = tempval.byte[2];
    *pdev->dev[1].odctrl[1] = tempval.byte[3];

    return ret;
}

int xr17v352_getmpiood(pXR17V352Dev pdev, uint64_t *odval)
{
    int ret = 0;

    if (pdev == NULL) {
		return -1;
    }

    tReg64 tempval;
    tVal64 regval;
    uint64_t *p = odval;

    tempval.byte[0] = *pdev->dev[0].odctrl[0]; // 7-0
    tempval.byte[1] = *pdev->dev[0].odctrl[1]; // 15-8
    tempval.byte[2] = *pdev->dev[1].odctrl[0]; // 23-26
    tempval.byte[3] = *pdev->dev[1].odctrl[1]; // 31-24

    regval.all = 0;
    regval.bit.mpioout1 = tempval.bit.mpioout1;
    regval.bit.mpioout2 = tempval.bit.mpioout2;
    regval.bit.mpioin1  = tempval.bit.mpioin1;
    regval.bit.mpioin2  = tempval.bit.mpioin1;

    *p = regval.all;

    return ret;
}

// 코딩 순서를 바꾸지 말아 주세요.
int xr17v352_read(pXR17V352Dev pdev, uint64_t* val)
{
    int ret = 0;

    if (pdev == NULL) {
        return -1;
    }

    tReg64  tempval, tempvalH;
    tVal64  regval;

#if 1
    // Low 영역 읽기
    *pdev->dev[1].lvlctrl[0] &= 0b11111011;
    tempval.all = 0;    // 레지스터 세팅 후 잠시 딜레이
    tempval.byte[0] = *pdev->dev[0].lvlctrl[0];
    tempval.byte[1] = *pdev->dev[0].lvlctrl[1];
    tempval.byte[2] = *pdev->dev[1].lvlctrl[0];
    tempval.byte[3] = *pdev->dev[1].lvlctrl[1];

    // High 영역 일기
    *pdev->dev[1].lvlctrl[0] |= 0b00001100;
    tempvalH.all = 0;   // 레지스터 세팅 후 잠시 딜레이
    tempvalH.byte[0] = *pdev->dev[0].lvlctrl[0];
    tempvalH.byte[1] = *pdev->dev[0].lvlctrl[1];
    tempvalH.byte[2] = *pdev->dev[1].lvlctrl[0];
    tempvalH.byte[3] = *pdev->dev[1].lvlctrl[1];

    regval.all = 0;
    regval.bit.mpioout1 = tempval.bit.mpioout1;
    regval.bit.mpioout2 = tempval.bit.mpioout2;
    regval.bit.mpioin1  = tempval.bit.mpioin1;
    regval.bit.mpioin2  = tempvalH.bit.mpioin1;

    *val = regval.all;
#else
    // High 영역 일기
    *pdev->dev[1].lvlctrl[0] |= 0b00001100;
    tempval.all = 0;    // 레지스터 세팅 후 잠시 딜레이
    tempval.byte[0] = *pdev->dev[0].lvlctrl[0];
    tempval.byte[1] = *pdev->dev[0].lvlctrl[1];
    tempval.byte[2] = *pdev->dev[1].lvlctrl[0];
    tempval.byte[3] = *pdev->dev[1].lvlctrl[1];

    // High 영역 일기
    *pdev->dev[1].lvlctrl[0] |= 0b00001100;
    tempvalH.all = 0;   // 레지스터 세팅 후 잠시 딜레이
    tempvalH.byte[0] = *pdev->dev[0].lvlctrl[0];
    tempvalH.byte[1] = *pdev->dev[0].lvlctrl[1];
    tempvalH.byte[2] = *pdev->dev[1].lvlctrl[0];
    tempvalH.byte[3] = *pdev->dev[1].lvlctrl[1];

    regval.all = 0;
    regval.bit.mpioout1 = tempval.bit.mpioout1;
    regval.bit.mpioout2 = tempval.bit.mpioout2;
    regval.bit.mpioin1  = tempval.bit.mpioin1;
    regval.bit.mpioin2  = tempvalH.bit.mpioin1;

    *val = regval.all;
#endif
    return ret;
}

int xr17v352_write(pXR17V352Dev pdev, uint64_t val)
{
    int ret = 0;

    if (pdev == NULL) {
        return -1;
    }

    tReg64  tempval;
    tVal64  regval;

    regval.all = val;

    tempval.bit.mpioout1    = regval.bit.mpioout1;
    tempval.bit.mpioout2    = regval.bit.mpioout2;
    tempval.bit.mpioctl     = 0b10;
    tempval.bit.mpioin1     = regval.bit.mpioin1;
    tempval.bit.mpioin2     = regval.bit.mpioin2;

    *pdev->dev[0].lvlctrl[0] = tempval.byte[0];
    *pdev->dev[0].lvlctrl[1] = tempval.byte[1];
    *pdev->dev[1].lvlctrl[0] = tempval.byte[2];
    *pdev->dev[1].lvlctrl[1] = tempval.byte[3];

    return ret;
}

int ReadXR17V352Data(void)
{
    int ret;
    static uint64_t dioreg_old;
    ret = xr17v352_read(xr17v352devp, &dioreg);
    if (dioreg_old != dioreg) {
        //printf("ReadXR17V352Data: %016lx %016lx\n", dioreg_old, dioreg);
        dioreg_old = dioreg;
    }
    return ret;
}

int WriteXR17V352Data(void)
{
    int ret;
    ret = xr17v352_write(xr17v352devp, dioreg);
    return ret;
}

#ifdef __cplusplus
}
#endif
