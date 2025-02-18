/**
 ****************************************************************
 * @file main.c
 *
 * @brief  main entry of test.
 *
 * @author
 *
 *
 ****************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>//C语言标准库


#include "gpio.h"
#include "spi.h"
#include "uart.h"
#include "timer.h"
#include "delay.h"//基本外设头文件
#include "7816.h"


#include "rfid.h"
#include "mh523.h"
#include "iso14443_4.h"
#include "iso14443a.h"
#include "iso14443b.h"//射频卡相关头文件

#include "mhscpu_sci.h"//
#include "iso7816_3.h"//接触式卡头文件
#include "emv_errno.h"
#include "emv_hard.h"

#include "Packet.h"
#include "define.h" //上位机通讯协议相关

#include "version.h" //版本控制

Packet receivePacket;
Packet sendPacket;
MH523TypeDef MH523;//初始化一个卡片缓存区
APDUPacket APDU;
void NVIC_Configuration(void);

int main()
{

    unsigned char status;
    SYSCTRL_SYSCLKSourceSelect(SELECT_EXT12M);
    SYSCTRL_PLLConfig(SYSCTRL_PLL_96MHz);
    SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div_None);
    SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_SPI0 | SYSCTRL_APBPeriph_TIMM0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_SPI0| SYSCTRL_APBPeriph_TIMM0, ENABLE);

	  //ISO7816_Bspinit();//初始化7816接口
	
    uart_Config(115200, UART_Parity_No);
    init_timer0();
    gpio_config();
    spi_config();
    NVIC_Configuration();
	
    printf(APP_VERSION_STR);
	
    pcd_init();//NFC芯片初始化
 
	  
	  //tst_SCIColdReset(0);
    while(1)
    {
			 // tst_SCIWarmReset(0);
			 // delayms(500);
        /*delayms(1000);
        pcd_request(0x52, MH523.CardTypebuf);
        pcd_cascaded_anticoll(0x93, MH523.CardCapacity, MH523.UIDbuf);
        pcd_cascaded_select(0x93, MH523.UIDbuf,&MH523.CardCapacity);
        pcd_rats_a(0, MH523.IncrementVal);
          APDU.APDU[0]=0x00;
         APDU.APDU[1]=0xA4;
          APDU.APDU[2]=0x04;
         APDU.APDU[3]=0x00;

          APDU.APDU[4]=0x0E;

         APDU.APDU[5]=0x32;//PPSE
          APDU.APDU[6]=0x50;
         APDU.APDU[7]=0x41;

          APDU.APDU[8]=0x59;
         APDU.APDU[9]=0x2E;
          APDU.APDU[10]=0x53;
         APDU.APDU[11]=0x59;

          APDU.APDU[12]=0x53;
         APDU.APDU[13]=0x2E;
          APDU.APDU[14]=0x44;
         APDU.APDU[15]=0x44;

          APDU.APDU[16]=0x46;
         APDU.APDU[17]=0x30;
          APDU.APDU[18]=0x31;
         APDU.APDULength =19;
         APDU_Exchange(&APDU);	*/

        status = UART0_GetString(&receivePacket);//等待上位机发送数据
        if(status == TRUE)
        {
            switch(receivePacket.type)//判断通讯类型
            {
            case TYPE_M1://M1卡
                sendPacket.type = TYPE_M1;
                M1_AnalyzeReceivePacket(&receivePacket, &sendPacket);
                SendPacket(&sendPacket);
                break;
            case TYPE_CPU_CONTACT://接触式CPU卡 --理解上有点问题  0x37
                sendPacket.type = TYPE_CPU_CONTACT;
                CPU_Contact_AnalyzeReceivePacket(&receivePacket, &sendPacket);
                SendPacket(&sendPacket);
                break;
            case TYPE_CPU_PROXI://非接触式CPU卡
                sendPacket.type = TYPE_CPU_PROXI;
                CPU_Proximity_AnalyzeReceivePacket(&receivePacket, &sendPacket);
                SendPacket(&sendPacket);
                break;
            default:
                break;
            }
        }


    }
}



#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    //NVIC_SetVectorTable(NVIC_VectTab_RAM,0);
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_0);

    NVIC_InitStructure.NVIC_IRQChannel = SCI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    /*
    NVIC_InitStructure.NVIC_IRQChannel = SCI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = SCI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    */
}







