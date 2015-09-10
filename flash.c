#include "flash.h"
#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <msp430F5308.h>


//void write_2_flash(float data, unsigned short address) {
//  union {
//    float KF;
//    unsigned long KFL;
//  }koefF;
//  
//  koefF.KF=data;
//  unsigned short * Flash_ptrD;               // Initialize Flash pointer Seg D
//           
//    Flash_ptrD = (unsigned short *) address;    // Initialize Flash pointer
//    __disable_interrupt();                    // 5xx Workaround: Disable global interrupt while erasing. Re-Enable GIE if needed
//    FCTL3 = FWKEY;                            // Clear Lock bit
//    FCTL1 = FWKEY+ERASE;       //               // Set Erase bit
//    *Flash_ptrD = 0;           //               // Dummy write to erase Flash seg
//    FCTL1 = FWKEY+BLKWRT;                     // Enable long-word write
//    *Flash_ptrD = koefF.KFL;                      // Write to Flash
//    FCTL1 = FWKEY;                            // Clear WRT bit
//    FCTL3 = FWKEY+LOCK;                       // Set LOCK bit
//    __enable_interrupt(); 
//}
//
//
//void write_2_flash_buffer(unsigned char *data, unsigned short address, unsigned char floatsnumber) {
//  union {
//    unsigned long KF;
//    unsigned char KFL[4];
//  }koefF;
//  
//  char i;
//  
//  unsigned short * Flash_ptrD;               // Initialize Flash pointer Seg D
//           
//    Flash_ptrD = (unsigned short *) address;    // Initialize Flash pointer
//    __disable_interrupt();                    // 5xx Workaround: Disable global interrupt while erasing. Re-Enable GIE if needed
//    FCTL3 = FWKEY;                            // Clear Lock bit
//    FCTL1 = FWKEY+ERASE;       //               // Set Erase bit
//    *Flash_ptrD = 0;           //               // Dummy write to erase Flash seg
//    FCTL1 = FWKEY+BLKWRT;                     // Enable long-word write
//    for (i = 0; i<floatsnumber; i++) {
//      memcpy(koefF.KFL, data+(4*i), 4);
//      *Flash_ptrD = koefF.KF;                      // Write to Flash
//      Flash_ptrD = (unsigned short *) (address+(4*(i+1)));
//    }
//    FCTL1 = FWKEY;                            // Clear WRT bit
//    FCTL3 = FWKEY+LOCK;                       // Set LOCK bit
//    __enable_interrupt(); 
//}


void write_2_flash_string(unsigned char *data, unsigned long address, unsigned char charnumber) {
  union {
    unsigned long KF;
    unsigned char KFL[4];
  }koefF;
  
  char i;
  
  unsigned long * Flash_ptrD;               // Initialize Flash pointer Seg D
           
    Flash_ptrD = (unsigned long *) address;    // Initialize Flash pointer
    __disable_interrupt();                    // 5xx Workaround: Disable global interrupt while erasing. Re-Enable GIE if needed
    FCTL3 = FWKEY;                            // Clear Lock bit
    FCTL1 = FWKEY+ERASE;       //               // Set Erase bit
    *Flash_ptrD = 0;           //               // Dummy write to erase Flash seg
    FCTL1 = FWKEY+BLKWRT;                     // Enable long-word write
    for (i = 0; i<(round(charnumber/4)+1); i++) {
      memcpy(koefF.KFL, data+(4*i), 4);
      *Flash_ptrD = koefF.KF;                      // Write to Flash
      Flash_ptrD = (unsigned long *) (address+(4*(i+1)));
    }
    FCTL1 = FWKEY;                            // Clear WRT bit
    FCTL3 = FWKEY+LOCK;                       // Set LOCK bit
    __enable_interrupt(); 
}

void read_from_flash_strings(unsigned char i, unsigned char kiek) {

    char *adress;
    adress = (char *) (ADDRESS+i*512);
    memcpy(flash_data, adress, kiek);

}


//float read_from_flash_float(unsigned short address) {
//  union {
//  float KF;
//  unsigned long KFL;
//  char kf[4];
//  }koefF;
//  float laikinas;
//    char *adress;
//    adress = (char *) (address);
//    memcpy(koefF.kf, adress, 4);
//    laikinas=koefF.KF;
//    return laikinas;
//}
//
//float read_from_flash(unsigned short address) {
//
//    unsigned char kas;
//    unsigned char  laikinas;
//    char *adress;
//    adress = (char *) (address);
//    memcpy((void *)&kas, adress, 1);
//    laikinas=kas;
//    return kas;
//}