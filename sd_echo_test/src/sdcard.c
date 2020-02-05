/*
 * sdcard.c
 *
 *  Created on: 2020Äê2ÔÂ3ÈÕ
 *      Author: think
 */

/****** microSD read and write ******/
#include "stdio.h"
#include "platform.h"
#include "xparameters.h"
#include "xparameters_ps.h"
#include "xil_printf.h"
#include "xil_io.h"     //DDR write and read head file
#include "xdevcfg.h"
#include "ff.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
/***********************************/
#define FILE_OCC "occ_test.txt"        // file stored in the SD card
#define FILE_SUFFIX "suffix.txt"
#define len 15                        //reference length
#define LEN_OCC 143                   //Bytes of the txt file + a byte for null
#define LEN_SUFFIX 35
#define DDR_OCCBASE XPAR_DDR_MEM_BASEADDR+0x00100000
#define DDR_SUFFIXBASE DDR_OCCBASE+0x00100200

static char dst_str[LEN_OCC];
//static char dst_str1[LEN_SUFFIX];
static FATFS fatfs;



//SD card Initialization
//Mount/Unmount a logical drive
// rc=0 mount success
// rc=0 mount error
int SD_Init()
{
FRESULT rc;
rc = f_mount(&fatfs,"",0);
if(rc)
{
xil_printf("ERROR : f_mount returned %d\r\n",rc);
return XST_FAILURE;
}
return XST_SUCCESS;
}


// To open a file from the SD card and read the data from the file and close the file
int SD_Transfer_read(char *FileName,u32 DestinationAddress,u32 ByteLength)
{
FIL fil;
FRESULT rc;
UINT br;

rc = f_open(&fil,FileName,FA_READ);
if(rc)
{
xil_printf("ERROR : f_open returned %d\r\n",rc);
return XST_FAILURE;
}
rc = f_lseek(&fil, 0); // Move file pointer of a file object
if(rc)
{
xil_printf("ERROR : f_lseek returned %d\r\n",rc);
return XST_FAILURE;
}
rc = f_read(&fil, (void*)DestinationAddress,ByteLength,&br); // Read data from a file
if(rc)
{
xil_printf("ERROR : f_read returned %d\r\n",rc);
return XST_FAILURE;
}
rc = f_close(&fil); // Close an open file object
if(rc)
{
xil_printf(" ERROR : f_close returned %d\r\n", rc);
return XST_FAILURE;
}
return XST_SUCCESS;
}



// SD card read and write functions
void SD_main()
{
   init_platform();
   int i;
   int r;
   int toint;
   int rev[(len+1)*4];
   //int occ_mat[len+1][4];
   //int suffix_mat[len];

   r  = SD_Init();
   if( XST_SUCCESS != r )
   {
	   printf("fail to open SD1\n\r");
   }

//SD_Init();

   //xil_printf("read OCC MATRIX from SD1\n\r");
   SD_Transfer_read(FILE_OCC,(void*)dst_str,(LEN_OCC));

   //xil_printf("start writing OCC MATRIX to DDR memory\r\n");
   int count=0;
   int toint1=0;

   for(i=0;i<LEN_OCC;++i)
  {
     if(dst_str[i]>='0')
    {
	   //xil_printf("%c",dst_str[i]);
	   toint=dst_str[i]-'0';
       toint1=toint1*10+toint;
    }
     else if((dst_str[i]==32) | (dst_str[i]==13) | (dst_str[i]==0) )
     {
	     //xil_printf("data :%d\n",toint1);
         Xil_Out32(DDR_OCCBASE+count*4,toint1);
	     count++;
         toint1=0;
     }

  }

//xil_printf("read suffix MATRIX from SD1\n\r");
//SD_Transfer_read(FILE_SUFFIX,(void*)dst_str1,(LEN_OCC));
//
//count=0;
//toint1=0;
//
//for(i=0;i<LEN_SUFFIX;++i)
//{
//   if(dst_str1[i]>='0')
//  {    toint=dst_str1[i]-'0';
//       toint1=toint1*10+toint;
//  }
//   else if((dst_str1[i]==32) | (dst_str1[i]==0) )
//       {
//	     //xil_printf("data :%d\n",toint1);
//         Xil_Out32(DDR_SUFFIXBASE+count*4,toint1);
//	     count++;
//         toint1=0;
//        }
//}

//xil_printf("\r\n SD read over!\r\n");



//////////////////////////////////////////////////////////////////////////
//xil_printf("start reading data from DDR memory\r\n");
//xil_printf("-----read OCC MATRIX----- \r\n");

for(i=0;i<(len+1)*4;++i)
{
    rev[i]=Xil_In32(DDR_OCCBASE+i*4);
	//xil_printf("DDR_OCCBASE:%x:  %d\r",DDR_OCCBASE+i*4,rev[i]);
}

////////////////////////////////////////////////////////
//
//xil_printf("-----read SUFFIX MATRIX----- \r\n");
//for(i=0;i<len;++i)
//{
//    suffix_mat[i]=Xil_In32(DDR_SUFFIXBASE+i*4);
//	xil_printf("DDR_SUFFIXBASE:%x:  %d\r",DDR_SUFFIXBASE+i*4,suffix_mat[i]);
//}


cleanup_platform();
//return 0;
}

