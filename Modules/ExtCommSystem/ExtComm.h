/**
**************************************************************************************************
* @file				ExtComm.h
* @author			SmartOs Team, ASBG, University of Minho
* @contacts 	paulo.felix.oliveira@gmail.com, pnlfernandes@gmail.com, joana.figueiredob@gmal.com
* @version 		V1
* @date				16-April-2018
* @brief			External Communication header.
**************************************************************************************************
*/

#ifndef _EXTCOMM_H_
#define _EXTCOMM_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "defines.h"
	
/* Global variables ---------------------------------------------------------*/
extern QueueHandle_t hqueueReceivedExternal;
	
/* Global function/task prototypes ------------------------------------------*/
/** @addtogroup EMG_Public_Functions
	* @{
	*/
/* Peripheral Control functions  *********************************************/
void ExtComm_Init(void);
void ExtComm_USART2_Init(void);
/* Communicatiokn functions -------------------------------------------------*/
uint8_t ExtComm_GetExternalMsg(ExtComm_Data *Msg, uint8_t c);
/* Callbacks and Tasks *******************************************************/
void ExtComm_USART2_RxCpltCallback(void);    // isr
void ExtComm_ReceiveExternal_Task(void * pvParameters);   //task 

#endif

