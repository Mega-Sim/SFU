/*
 * xr17v352_rt.h
 *
 *  Created on: 2018. 10. 10.
 *      Author: master
 */

#ifndef XR17V352_RT_XR17V352_RT_H_
#define XR17V352_RT_XR17V352_RT_H_

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
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MPIOINTL    (0x8f)
#define MPIOLVLL    (0x90)
#define MPIO3TL     (0x91)
#define MPIOINVL    (0x92)
#define MPIOSELL    (0x93)
#define MPIOODL     (0x94)

#define MPIOINTH    (0x95)
#define MPIOLVLH    (0x96)
#define MPIO3TH     (0x97)
#define MPIOINVH    (0x98)
#define MPIOSELH    (0x99)
#define MPIOODH     (0x9a)

#define MPIOOUT     (0)
#define MPIOIN      (1)

typedef struct _XR17V32Dev {
    int fd;
    uint8_t *virt_addr;
    uint8_t *intmask[2];
    uint8_t *lvlctrl[2];
    uint8_t *trictrl[2];
    uint8_t *invctrl[2];
    uint8_t *selctrl[2];
    uint8_t *odctrl[2];
} XR17V32, *pXR17V32;

typedef struct {
    XR17V32 dev[4];
} XR17V32Dev, *pXR17V352Dev;

typedef struct {
    uint64_t mpioout1   : 16;
    uint64_t mpioout2   :  2;
    uint64_t reserved   : 46;
} sXRValOut64;

typedef struct {
    uint64_t mpioin1    : 12;
    uint64_t mpioin2    : 12;
    uint64_t reserved   : 40;
} sXRValIn64;

typedef struct {
    uint64_t mpioout1   : 16;
    uint64_t mpioout2   :  2;
    uint64_t mpioctl    :  2;
    uint64_t mpioin1    : 12;
    uint64_t mpioin2    : 12;
    uint64_t reserved   : 20;
} sXRSelReg;

typedef struct {
    uint64_t mpioout1   : 16;
    uint64_t mpioout2   : 2;
    uint64_t mpioin1    : 12;
    uint64_t mpioin2    : 12;
    uint64_t reserved   : 22;
} sXRSelVal;

typedef struct {
    uint64_t    OBS_Area1_Select        :   1;
    uint64_t    OBS_Area2_Select        :   1;
    uint64_t    OBS_Area3_Select        :   1;
    uint64_t    OBS_Area4_Select        :   1;
    uint64_t    OBS_Area5_Select        :   1;
    uint64_t    OHT_DET_Area1_Select    :   1;
    uint64_t    OHT_DET_Area2_Select    :   1;
    uint64_t    OHT_DET_Area3_Select    :   1;
    uint64_t    OHT_DET_Area4_Select    :   1;
    uint64_t    OHT_DET_Area5_Select    :   1;
    uint64_t    OHT_EN                  :   1;
    uint64_t    OBS_EN                  :   1;
    uint64_t    QRR1_IN1                :   1;
    uint64_t    BCR1_IN2                :   1;
    uint64_t    QRR2_IN3                :   1;
    uint64_t    BCR2_IN4                :   1;
    uint64_t    Driving_Stop_EN1        :   1;
    uint64_t    Trans_Stop_EN2          :   1;
    uint64_t    reserved                :   46;
} sOutBits;

typedef struct {
    uint64_t    inbits                  :   18;
    // Low 12 bit
    uint64_t    OHT_Detect_Short        :   1;
    uint64_t    OHT_Detect_long         :   1;
    uint64_t    OHT_Detect_Middle       :   1;
    uint64_t    OHT_Detect_Fail         :   1;
    uint64_t    Driving_Front_Stop_Sen  :   1;
    uint64_t    PreCheck_Sensor         :   1;
    uint64_t    Driving_BCR_OK_OUT      :   1;
    uint64_t    Driving_BCR_NG_OUT      :   1;
    uint64_t    Trans_Front_Stop_Sen    :   1;
    uint64_t    Trans_Rear_Stop_Sen     :   1;
    uint64_t    Trans_BCR_OK_OUT        :   1;
    uint64_t    Trans_BCR_NG_OUT        :   1;
    // High 12 bit
    uint64_t    OHT_ST                  :   1;
    uint64_t    OBS_ST                  :   1;
    uint64_t    Driving_Stop_ST1        :   1;
    uint64_t    Trans_Stop_ST2          :   1;
    uint64_t    QRR1_ST1                :   1;
    uint64_t    BCR1_ST2                :   1;
    uint64_t    QRR2_ST3                :   1;
    uint64_t    BCR2_ST4                :   1;
    uint64_t    OBS_long                :   1;
    uint64_t    OBS_Middle              :   1;
    uint64_t    OBS_Short               :   1;
    uint64_t    OBS_Fail                :   1;
    uint64_t    reserved                :   22;
} sInBits;

typedef union {
    uint64_t    all;
    uint8_t     byte[8];
    sXRSelReg   bit;
} tReg64;

typedef union {
    uint64_t    all;
    uint8_t     byte[8];
    sXRSelVal   bit;
} tVal64;

typedef union {
    uint64_t    all;
    uint8_t     byte[8];
    uint16_t    word[4];
    uint32_t    dword[2];
    sXRSelVal   bit;
    sOutBits    outbits;
    sInBits     inbits;
} tUI64;

typedef union {
    uint32_t    all;
    uint8_t     byte[4];
    uint16_t    word[2];
} tUI32;

uint32_t* PseudoMmap(int position);
pXR17V352Dev GetDIOHandle(void);
int XR17V352Init(void);
pXR17V352Dev xr17v352_open(void);
int xr17v352_close(pXR17V352Dev pdev);
int xr17v352_setmpiosel(pXR17V352Dev pdev, uint64_t);
int xr17v352_getmpiosel(pXR17V352Dev pdev, uint64_t *);
int xr17v352_setmpioinv(pXR17V352Dev pdev, uint64_t);
int xr17v352_getmpioinv(pXR17V352Dev pdev, uint64_t *);
int xr17v352_setmpiotri(pXR17V352Dev pdev, uint64_t);
int xr17v352_getmpiotri(pXR17V352Dev pdev, uint64_t *);
int xr17v352_setmpiood(pXR17V352Dev pdev, uint64_t);
int xr17v352_getmpiood(pXR17V352Dev pdev, uint64_t *);
int xr17v352_read(pXR17V352Dev pdev, uint64_t* val);
int xr17v352_write(pXR17V352Dev pdev, uint64_t val);
int ReadXR17V352Data(void);
int WriteXR17V352Data(void);

#ifdef __cplusplus
}
#endif

#endif /* XR17V352_RT_XR17V352_RT_H_ */
