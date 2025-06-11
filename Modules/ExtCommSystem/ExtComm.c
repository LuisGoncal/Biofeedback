/**
**************************************************************************************************
* @file				ExtComm.c
* @author			SmartOs Team, ASBG, University of Minho
* @contacts 	paulo.felix.oliveira@gmail.com, pnlfernandes@gmail.com, joana.figueiredob@gmail.com
* @version 		V1
* @date				16-April-2018
* @brief			External Communication source.
**************************************************************************************************
*
*  ===============================================================================================
*                     					##### Module Overview #####
*  ===============================================================================================
*		External Communication module is responsible to manage the arrival of new chars via UART2.
*		UART2 Rx callback runs every time a new char arrives, sending it to a specific reception task 
*   which builds the messages and forwards them to control unit module. 
*
*	(#) To sart the module:
*			(##) Call ExtComm_Init()
*			
*	(#) ExtComm_USART2_RxCpltCallback() runs when a new char arrives, sending it to hqueueSerialData
*			queue that awakes ExtComm_ReceiveExternal_Task(). Within this task, a command message is
*			built until a stop char arrives, signaling the task that can forward the arrived message to
*			hqueueReceivedExternal queue so control unit module can process it.
*			
*/

/* Includes ------------------------------------------------------------------*/
#include "ExtComm.h"

/* External Functions --------------------------------------------------------*/
void Error_Handler(void);	
	
/* Global variables ----------------------------------------------------------*/

/* Private variables *********************************************************/
UART_HandleTypeDef huart2;
SemaphoreHandle_t hsemEXTCOMMSendMsg; 
QueueHandle_t hqueueReceivedExternal;  //Stores messages received externally (command/data)
QueueHandle_t hqueueSerialData;    //Stores data received from serial interface
/* Debug variables ***********************************************************/
int rx_count = 0;
int miss_count = 0;
int invalid_count = 0;
float currentPosition = 0.0;
long MSG_RECEIVED = 0, AVAILABLE;
uint8_t rx_cnt=0;
long tick_db_Comm = 0;

/* Peripheral Control functions  *********************************************/

/**
  * @brief  Module Init Function. Initializes USART driver linking it to DMA.
	*					It creates a communication semaphore, the receive and send externally 
	*					queues and each task.
	* @param  None.
  * @retval None.
  */
void ExtComm_Init(void)
{
	ExtComm_USART2_Init();
	
	hsemEXTCOMMSendMsg = xSemaphoreCreateCounting(0xffff, 0);
	
	hqueueReceivedExternal = xQueueCreate(128, sizeof(struct ExtComm_Data));
	
	hqueueSerialData = xQueueCreate(512, sizeof(uint8_t));

	xTaskCreate(ExtComm_ReceiveExternal_Task, "ExtComm_ReceiveExternal_Task", 128, (void*)NULL, 6, NULL);
}

/**
  * @brief  Module USART2 Init Function. Driver set with 230400 baudrate, 8 bits of resolution,
	*					and in Rx and Tx mode (IRS).
  * @param  None.
  * @retval None.
  */
void ExtComm_USART2_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 230400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  
	if (HAL_UART_Init(&huart2) != HAL_OK)
    Error_Handler();
	
	SET_BIT(huart2.Instance->CR1, USART_CR1_RXNEIE);  //enable RXNE interrupt
}

/* Communicatiokn functions -------------------------------------------------*/

/**
  * @brief  Module USART2_DMA Init Function (Tx mode).
	* @param  Msg: message to build (receivesd).
	* @param  c: char received throught uart.
  * @retval State.
	*		@arg 0: Finished
	*		@arg 1: Building msg
	*		@arg 2: Msg to long
	*		@arg 3: Missing EOF char
  */
uint8_t ExtComm_GetExternalMsg(ExtComm_Data *Msg, uint8_t c)
{
	static uint8_t state = 0;
	static uint8_t index = 0;
	
	switch (state)
	{
		case 0:
			if (c==0x00){
				state = 1;
				return 1;
			}
		case 1:
			if (c <=128){
				Msg ->length  = c;
				state = 2;
				return 1;
			}
			else{
				return 2;
			}
		case 2:
			if (index < Msg -> length){
					Msg ->data[index] = c;
					
					if (index == (Msg ->length -1)){
						state =3;
					}
					return 1;
			}
			else{
				return 3;
			}
		case 3:
			/*FFFFFAAAZEEEEERRRRRRRR
	}
}

/* Callbacks and Tasks *******************************************************/

/**
  * @brief  USART2 Rx Callback. Asynchronous function responsible for place a received char
	*					into queue of ReceiveExternal_Task. 
	* @param  None
  * @retval None.
  */
void ExtComm_UART2_RxCpltCallback(void)
{
	uint8_t c;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if(__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_RXNE) && __HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE)){
		
		/*	Reads incoming char	*/
		c = READ_REG(huart2.Instance->DR);
		rx_cnt++;
		/*	Send char to hqueueSerialData queue	*/
		xQueueSendFromISR( hqueueSerialData, &c, &xHigherPriorityTaskWoken );
		
		__HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_RXNE); 
		
		if(xHigherPriorityTaskWoken == pdTRUE)
			portYIELD_FROM_ISR( xHigherPriorityTaskWoken );	
	}
}

/**
	* @brief  Builds messages, when chars are received (respecting a predefined protocol).
	* @param  None.
	* @retval None.
	*/
void ExtComm_ReceiveExternal_Task(void * pvParameters)
{
	ExtComm_Data Msg;
	uint8_t c = 0;
	
	uint8_t returnValue;
	
	while(1)
	{
		while(1)
		{
			/*	Waits for new char to arrive	*/
			if(xQueueReceive(hqueueSerialData, &c, portMAX_DELAY) == pdTRUE)
			{
				returnValue = ExtComm_GetExternalMsg(&Msg, c);
				
				switch (returnValue)
				{
					/*	Finished message build */
					case 0:
						xQueueSend(hqueueReceivedExternal, (void*)&Msg, portMAX_DELAY);
						rx_count++;
						break;
					/*	Continue to build message	*/
					case 1:
						continue;
					case 2:
						invalid_count++;
						//send status message
						break;
					case 3:
						miss_count++;
						break;
				}
			}		
		}
	}
}