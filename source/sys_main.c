/** @file sys_main.c 
*   @brief Application main file
*   @date 11-Dec-2018
*   @version 04.07.01
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com 
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/
/*Note: Frequency: 80Mhz */

/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Include Files */
#include "sys_common.h"
#include "sci.h"
#include "spi.h"
#include "rti.h"
#include "gio.h"
#include <math.h>
#include "uartstdio.h"
#include "sdc-hercules.h"
#include <string.h>
#include <stdio.h>
#include "ff.h"
#include "sd_defs.h"
/* USER CODE BEGIN (1) */
#define TEST_FILENAME "Futr.TXT"

FATFS Fatfs;
FIL fil;
/**** *******/

#define noSCell 8
#define noPCell 7
#define noTemp 8

struct cell{
    float voltage;
    float current;
    float z;
    float temp;
    float Capacity;
};
struct cell c[8];
float zAvg, vPack, iPack, TempPack;
int i;

int Factor = 1E5;
int vCell[noSCell], zCell[noSCell], tCell[noTemp];

/* USER CODE END */
//DWORD get_fattime (void)
//{
  //  time_t t;
//    struct tm *stm;
//     DWORD tmr;
//     Calendar curTime = sdInterface->sdGetRTCTime();


  //  t = time(0);
  // stm = localtime(&t);
//
//    tmr =  (DWORD)(curTime.Year - 80) << 25 |
//           (DWORD)(curTime.Month) << 21 |
//           (DWORD)(curTime.DayOfMonth) << 16 |
//           (DWORD)(curTime.Hours << 11) |
//           (DWORD)(curTime.Minutes << 5) |
//           (DWORD) (curTime.Seconds >> 1);

//    return tmr;
//}

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/
int SD_Test(void);


/******* sd_card call functionality *********/

int SD_Card_Save(int,int,int,int,int,int,int[],int[],int[]);







//int SD_Card_Save(int,int,int);
/* USER CODE BEGIN (2) */
// bframe one byte header then cell voltage, aux channel values, die temperature and vpack.

/////*code for set RTC timer //////

//FRESULT set_timestamp (
//    char *obj,     /* Pointer to the file name */
//    int year,
//    int month,
//    int mday,
//    int hour,
//    int min,
//    int sec
//)
//{
//    FILINFO fno;
//
//    fno.fdate = (WORD)(((year - 1980) * 512U) | month * 32U | mday);
//    fno.ftime = (WORD)(hour * 2048U | min * 32U | sec / 2U);
//
//    return f_utime(obj, &fno);
//}

float getCellVoltage(struct cell c[]){
//    int i, nSent;
    float packV = 0;
    for(i=1; i<=noSCell; i++){
        c[noSCell-i].voltage = 3.75;
        vCell[noSCell-i] = (c[noSCell-i].voltage)*Factor;
        packV+=c[noSCell-i].voltage;
    }
    return packV;
}

float getCellCurrent(struct cell c[]){
//    int i;
    float pack_I;
    pack_I = 10;
    for(i = 0; i < noSCell; i++){
        c[i].current = (pack_I/noPCell);
    }
    return (pack_I);
 }

float getCellTemp(struct cell c[]){
//    int i;
    float TempTolal = 0;
    for(i = 1; i<=noTemp; i++){
        c[noTemp-i].temp = 29;
        tCell[noTemp-i] = (c[noTemp-i].temp)*Factor;
        TempTolal+=c[8-i].temp;
    }
    return (TempTolal/noTemp);
}

float getInitialSOC(struct cell c[]){
//    int i;
    float zTotal=0;
    for(i = 0; i < noSCell; i++){
        c[i].z = 0.75;
        zCell[i] = (c[i].z)*Factor;
        zTotal+=c[i].z;
    }
    return (zTotal/noSCell);
}

float calculateCellSOC(struct cell c[]){
//    int i;
    float zTotal=0;
    for(i = 0; i< noSCell; i++){
        c[i].z = 0.85;
        zCell[i] = (c[i].z)*Factor;
        zTotal += c[i].z;
    }
    return (zTotal/noSCell);
}



/* USER CODE END */

int main(void)
{

//UINT bw;


      gioInit();                              //Initializes the GPIO driver
      sciInit(); // To Initalize LIN/SCI2 routines to receive commands and transmit data
      //  spiInit(); // Use it  in Fatfs port

      UARTprintf("Futronics Limited");

       rtiInit();
      /* Enable RTI Compare 3 interrupt notification */
      rtiEnableNotification(rtiNOTIFICATION_COMPARE3);
      /* Enable IRQ - Clear I flag in CPS register */
      _enable_IRQ();
      /* Start RTI Counter Block 1 */
      rtiStartCounter(rtiCOUNTER_BLOCK1);

      mmcSelectSpi(spiPORT2, spiREG2);  // SD card is on the SPI1
      iPack = getCellCurrent(c);
      vPack = getCellVoltage(c);
      TempPack = getCellTemp(c);
      zAvg = getInitialSOC(c);
//      SD_Test();

//      SD_Card_Save(noSCell, noTemp, vPack*100);

//      f_mount(&Fatfs, "", 0);     /* Give a work area to the default drive */
//
//      fr = f_open(&fil, "xyz.txt", FA_WRITE | FA_CREATE_ALWAYS);  /* Create a file */
//
//      if (fr == FR_OK) {
//            f_write(&fil, "It works!\r\n", 11, &bw);    /* Write data to the file */
//            fr = f_close(&fil);                         /* Close the file */
//
//            }
      UARTprintf("\n SD Card Access Done");

      while(1){
          gioToggleBit(gioPORTA,2);
//          SD_Test();
          iPack = getCellCurrent(c);
          vPack = getCellVoltage(c);
          TempPack = getCellTemp(c);
          zAvg = calculateCellSOC(c);
          SD_Card_Save(noSCell, noTemp, vPack*Factor, iPack*Factor, zAvg*Factor, TempPack*Factor, vCell, zCell, tCell);
//          SD_Card_Save(noSCell, noTemp, vPack*100);
          UARTprintf("\n SD Card Access Done");
      }


}


/* USER CODE BEGIN (4) */
/* USER CODE END */
