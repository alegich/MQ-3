#ifndef HR_SR04_H
#define HR_SR04_H

#include <stdint.h>

void InitRegisters(void);
void Timer0_init(void);
void delay_Microsecond(uint32_t time);
int32_t measureD(void);
void OutSignal(uint32_t value);
void TriggerSignal(void);
void LedToggle(void);

#endif // HR_SR04_H
