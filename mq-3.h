#ifndef MQ_3_H
#define MQ_3_H

void Mq3Init(void);
float getInputVoltage(void);
float getR0(float voltage);

float getGasRatio(float R0);
float getConcentration(float gasRatio);
float getConcentrationDiscrete(float gasRatio);

#endif // MQ_3_H
