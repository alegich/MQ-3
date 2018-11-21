
#define TM4C123GH6PM
#include <TM4C123.h>
#include "..//..//Labware//FlagUtils.h"

// MQ-3 is connected to PE-2 (Analog input Ain1)
void Mq3Init() {
	SYSCTL->RCGCGPIO |= (1U << 4); //Enable clock for PORTE 
	GPIOE->DIR &= ~PIN2; // input
	GPIOE->AFSEL |= PIN2;
	GPIOE->DEN &= PIN2;
	GPIOE->AMSEL |= PIN2;
	SYSCTL->RCGC0 |= 0x00010000;   // 6) activate ADC0 
	SYSCTL->RCGC0 &= ~0x00000300;  // 7) configure for 125K
	ADC0->SSPRI = 0x0123;          // 8) Sequencer 3 is highest priority
	ADC0->ACTSS &= ~0x0008;        // 9) disable sample sequencer 3
	ADC0->EMUX &= ~0xF000;         // 10) seq3 is software trigger
	ADC0->SSMUX3 = (ADC0->SSMUX3 & 0xFFFFFFF0) + 1; // 11) channel Ain1 (PE2)
	ADC0->SSCTL3 = 0x0006;         // 12) no TS0 D0, yes IE0 END0
	ADC0->ACTSS |= 0x0008;         // 13) enable sample sequencer 3
}

unsigned long Mq3In(void){  
	unsigned long result;
  ADC0->PSSI = 0x0008;            // 1) initiate SS3
  while ((ADC0->RIS & 0x08) == 0) {};   // 2) wait for conversion done
  result = ADC0->SSFIFO3 & 0xFFF; // 3) read result
  ADC0->ISC = 0x0008;             // 4) acknowledge completion
  return result;
}

float getInputVoltage() {
    float sensor_volt;
    float sensorValue;
	  int x = 0;

    /*--- Get a average data by testing 100 times ---*/
    for(x = 0; x < 100 ; ++x) {
        sensorValue = sensorValue + Mq3In();
    }
    sensorValue = sensorValue/100.0;
    /*-----------------------------------------------*/

    sensor_volt = sensorValue / 4096 * 5.0;
		
		return sensor_volt;
}

float getR0(float voltage) {
    float RS_air; //  Get the value of RS via in a clear air
    float R0;  // Get the value of R0 via in Alcohol

    RS_air = (5.0 - voltage) / voltage; // omit *RL
    R0 = RS_air / 60.0; // The ratio of RS/R0 is 60 in a clear air from Graph (Found using WebPlotDigitizer)
	
	return R0;
}

float getGasRatio(float R0) {
    float sensor_volt;
    float RS_gas; // Get value of RS in a GAS
    float ratio; // Get ratio RS_GAS/RS_air

	  sensor_volt = getInputVoltage();
    RS_gas = (5.0 - sensor_volt) / sensor_volt; // omit *RL

    /*-Replace the name "R0" with the value of R0 in the demo of First Test -*/
    ratio = RS_gas / R0;  // ratio = RS/R0
	
	  return ratio;
}

float getConcentration(float gasRatio) {
	float result = 0.1896 * gasRatio * gasRatio - gasRatio * 0.86178 + 1.0792;   //BAC in mg/L
	return result;
}

typedef struct {
	float x;
	float y;
} Func;

float getConcentrationDiscrete(float gasRatio) {
	const unsigned int Count = 13;
  float X[Count] = {0.0, 0.02, 0.04, 0.07, 0.2, 0.3, 0.4, 0.53, 1.0, 1.7, 2.2, 60.0, 100.0};
	float Y[Count] = {20, 10.0, 8.0, 6.0, 4.0, 2.6, 1.7, 1.0, 0.4, 0.22, 0.1, 0.0, 0.0};
	float result = 0;
	
	unsigned int i = 1;
	
	while (i < Count && gasRatio > X[i]) { ++i; }
	
	result = Y[i - 1] + (gasRatio - X[i - 1]) * (Y[i] - Y[i - 1]) / (X[i] - X[i - 1]);
	return result;
}
