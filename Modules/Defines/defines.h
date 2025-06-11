/**
  ******************************************************************************
  * File Name          : defines.h
  * Description        : Global Constants
	* Author(s)					 : Paulo Félix, Joana Figueiredo
	* Contact						 : paulo.felix.oliveira@gmail.com, joana.figueiredob@gmail.com
	* Organization		   : University of Minho, Guimarães / CSIC - Cajal Institute, Madrid
	* Project						 : 
  ******************************************************************************
**/

#ifndef _DEFINES_H_
#define _DEFINES_H_

/* Includes ------------------------------------------------------------------*/
	#include "stm32f4xx_hal.h"
	
	//#define COMPUT_TEST
	
	//#define SQUARE_WAVE_TEST
	/***********************************************/
	/*   																					 */	
	/*          Control Unit Callback: PD8   			 */
	/*          	Low-Level_Control:	 PD9			   */
	/*  				 	Mid-Level Contro:	 	 PD10 			 */
  /* 					IMU Callback:				   PD11				 */
	/* 					ORTH Callback:				 PD12			   */
	/*   																					 */
	/* 					Control Unit Task:     PD13				 */
  /* 					IMU Taks:				       PD14				 */
	/* 					ORTH Taks:					   PD15		     */
	/*																						 */	
	/***********************************************/
	
	#define SOF_ 	0x0d
	#define EOF_ 	0x0a

	typedef struct ExtComm_Data {
		uint8_t length; 				  
		uint8_t data[128];
	} ExtComm_Data; 	
	
	union float_t{
		
		uint8_t c_t[4];
		float f_t;
	};
	
	
	
#endif
