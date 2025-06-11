/*************************************************** 
  This is a library for the Adafruit DRV2605L Haptic Driver

  ----> http://www.adafruit.com/products/2306
 
  Check out the links above for our tutorials and wiring diagrams
  This motor/haptic driver uses I2C to communicate

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#include "Adafruit_DRV2605.h"


/**************************************************************************/
/*! 
    @brief  Setups the HW
*/
/**************************************************************************/
uint8_t Adafruit_DRV2605_begin() {
  
  //HAL_I2C_MspInit( &hi2c1 );

  uint8_t id = Adafruit_DRV2605_readRegister8(DRV2605_REG_STATUS);
  
  Adafruit_DRV2605_writeRegister8(DRV2605_REG_MODE, 0x00); // out of standby 
  
  Adafruit_DRV2605_writeRegister8(DRV2605_REG_RTPIN, 0x00); // no real-time-playback
  
  Adafruit_DRV2605_writeRegister8(DRV2605_REG_WAVESEQ1, 0x00); 
  Adafruit_DRV2605_writeRegister8(DRV2605_REG_WAVESEQ2, 0x00);
  
  Adafruit_DRV2605_writeRegister8(DRV2605_REG_OVERDRIVE, 0x00); // no overdrive
  
  Adafruit_DRV2605_writeRegister8(DRV2605_REG_SUSTAINPOS, 0x00);
  Adafruit_DRV2605_writeRegister8(DRV2605_REG_SUSTAINNEG, 0x00);
  Adafruit_DRV2605_writeRegister8(DRV2605_REG_BREAK, 0x00);
  Adafruit_DRV2605_writeRegister8(DRV2605_REG_AUDIOMAX, 0xFF);
  
	// Calibration outputs
	Adafruit_DRV2605_writeRegister8(DRV2605_REG_FEEDBACK, 0x2A);
	Adafruit_DRV2605_writeRegister8(DRV2605_REG_AUTOCALCOMP, 0x0A);
	Adafruit_DRV2605_writeRegister8(DRV2605_REG_AUTOCALEMP, 0x74);
	
	uint8_t deb1 =	Adafruit_DRV2605_readRegister8(DRV2605_REG_AUTOCALEMP);

	
	// Initizalition
	Adafruit_DRV2605_writeRegister8(DRV2605_REG_CONTROL1, 0x93); //default
	
	//PWM Mode
	Adafruit_DRV2605_writeRegister8(DRV2605_REG_MODE, 0x03); // PWM Mode
	Adafruit_DRV2605_writeRegister8(DRV2605_REG_CONTROL3, 0xA0); // NG_Threshold =  10; ERM_OPEN_LOOP = 1; [4:0]=0
  
	uint8_t deb2 =	Adafruit_DRV2605_readRegister8(DRV2605_REG_MODE);

	// ERM open loop
  
  // turn off N_ERM_LRA
  //Adafruit_DRV2605_writeRegister8(DRV2605_REG_FEEDBACK, Adafruit_DRV2605_readRegister8(DRV2605_REG_FEEDBACK) & 0x7F);
  // turn on ERM_OPEN_LOOP
  //Adafruit_DRV2605_writeRegister8(DRV2605_REG_CONTROL3, Adafruit_DRV2605_readRegister8(DRV2605_REG_CONTROL3) | 0x20);
  //Adafruit_DRV2605_writeRegister8(DRV2605_REG_MODE, 0x03); // PWM Mode
  //Adafruit_DRV2605_writeRegister8(DRV2605_REG_FEEDBACK, 0x2A); // ERM Mode ; FB_BRAKE_FACTOR = 010; LOOP_GAIN = 10; BEMF_GAIN = 10
  //Adafruit_DRV2605_writeRegister8(DRV2605_REG_CONTROL3, 0x60); // NG_Threshold =  01; ERM_OPEN_LOOP = 1; [4:0]=0



  return 1;
}

void Adafruit_DRV2605_setWaveform(uint8_t slot, uint8_t w) {
  Adafruit_DRV2605_writeRegister8(DRV2605_REG_WAVESEQ1+slot, w);
}

void Adafruit_DRV2605_selectLibrary(uint8_t lib) {
  Adafruit_DRV2605_writeRegister8(DRV2605_REG_LIBRARY, lib);
}

void Adafruit_DRV2605_go() {
  Adafruit_DRV2605_writeRegister8(DRV2605_REG_GO, 1);
}

void Adafruit_DRV2605_setMode(uint8_t mode) {
  Adafruit_DRV2605_writeRegister8(DRV2605_REG_MODE, mode);
}

void Adafruit_DRV2605_setRealtimeValue(uint8_t rtp) {
  Adafruit_DRV2605_writeRegister8(DRV2605_REG_RTPIN, rtp);
}

/********************************************************************/

uint8_t Adafruit_DRV2605_readRegister8(uint8_t reg) {

  uint8_t x;
  HAL_StatusTypeDef result;
  
  result = HAL_I2C_Master_Transmit( &hi2c3, (DRV2605_ADDR << 1), &reg, 1, 10 );
  if( result != HAL_OK ) 
  {
    /*Failed to send register value to driver*/
    x = 0;
  }
  
  result = HAL_I2C_Master_Receive ( &hi2c3, (DRV2605_ADDR << 1), &x  , 1, 10 );
  if( result != HAL_OK ) 
  {
    /*Failed to send register value to driver*/
    x = 0;
  }
  
  return x;
}

void Adafruit_DRV2605_writeRegister8(uint8_t reg, uint8_t val) {
  
  HAL_StatusTypeDef result;
  
  uint8_t data[2];
  
  data[0] = reg;
  data[1] = val;
  
  result = HAL_I2C_Master_Transmit( &hi2c3, (DRV2605_ADDR << 1), data, 2, 10 );
  if( result != HAL_OK ) 
  {
    /*Failed to send register value to driver*/    
  }
  
}

/****************/


// Allow users to use ERM motor or LRA motors

void Adafruit_DRV2605_useERM ()
{
  Adafruit_DRV2605_writeRegister8(DRV2605_REG_FEEDBACK, Adafruit_DRV2605_readRegister8(DRV2605_REG_FEEDBACK) & 0x7F);
}

void Adafruit_DRV2605_useLRA ()
{
  Adafruit_DRV2605_writeRegister8(DRV2605_REG_FEEDBACK, Adafruit_DRV2605_readRegister8(DRV2605_REG_FEEDBACK) | 0x80);
}



