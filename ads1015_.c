
#include <unistd.h>
#include <wiringPiI2C.h>
#include "ads1015_.h"
#include <arpa/inet.h>
#include "time.h"
#include <wiringPi.h>


uint8_t   m_conversionDelay;
uint8_t   m_bitShift;
/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
static uint8_t i2cread(uint8_t i2cFd) {
  return wiringPiI2CRead(i2cFd);
}

/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
static void i2cwrite(uint8_t i2cFd, uint8_t x) {
  wiringPiI2CWrite(i2cFd, x);

}

/**************************************************************************/
/*!
    @brief  Writes 16-bits to the specified destination register
*/
/**************************************************************************/
static void writeRegister(uint8_t i2cFd, uint8_t i2cAddress, uint8_t reg, uint16_t value) {
  wiringPiI2CWriteReg16(i2cFd, reg, (value>>8) | (value<<8));
}

/**************************************************************************/
/*!
    @brief  Reads 16-bits from the specified destination register
*/
/**************************************************************************/
static uint16_t readRegister(uint8_t i2cFd, uint8_t i2cAddress, uint8_t reg) {
  wiringPiI2CWrite(i2cFd, ADS1015_REG_POINTER_CONVERT);
  uint16_t reading = wiringPiI2CReadReg16(i2cFd, reg);
  reading = (reading>>8) | (reading<<8); // yes, wiringPi did not assemble the bytes as we want
  return reading;

}

int m_i2cFd ;
int m_i2cAddress ;
adsGain_t m_gain;

void ads_init()
{
    m_i2cAddress = 0x48; // 48
    m_i2cFd = -1;
    m_conversionDelay = ADS1115_CONVERSIONDELAY; // 8
    m_bitShift = 0;
    m_gain = GAIN_EIGHT; /* +/- 6.144V range (limited to VDD +0.3V max!) */
}

int ads_begin()
{
    printf("setting up ADS with m_i2cAddress = %d\n", m_i2cAddress);
    m_i2cFd = wiringPiI2CSetup(m_i2cAddress);
    printf("m_i2cFd = %d\n", m_i2cFd);
    return m_i2cFd ;
}

uint32_t ads_GetTick()
{
	return millis() ;
}

static bool locked = false ;
uint32_t lock_time = 0 ;

void ads_SetADCChannel(int fd, uint8_t channel)
{
	if(locked == false )
	{
			
		uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
						  ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
						  ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
						  ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
						  ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
						  ADS1015_REG_CONFIG_MODE_SINGLE  ; // Single-shot mode (default)

		config |= ADS1015_REG_CONFIG_PGA_6_144V;

		// Set single-ended input channel
		switch (channel)
		{
			case (0):
				config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
				break;
			case (1):
				config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
				break;
			case (2):
				config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
				break;
			case (3):
				config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
				break;
		}

		config |= ADS1015_REG_CONFIG_OS_SINGLE;
		wiringPiI2CWriteReg16(fd, ADS1015_REG_POINTER_CONFIG, ntohs(config));
	   
		locked = true ;
		lock_time = ads_GetTick() ;
	}
	else
	{
		
	}
}

bool ads_IsDataReady()
{
	bool ret ;
	
	if(ads_GetTick() - lock_time > ADS1015_CONVERSIONDELAY)
	{
		locked = false ;
		ret = true ;
	}
	else{
		ret = false ;
	}
	
	return ret ;
}

uint16_t ads_GetADCChannel(int fd , uint8_t channel) 
{
	return ntohs(wiringPiI2CReadReg16(fd, ADS1015_REG_POINTER_CONVERT) >> 4);
}
