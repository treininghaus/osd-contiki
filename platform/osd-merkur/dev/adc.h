#ifndef __ADC_ARCH_H__
#define __ADC_ARCH_H__

int readADC(uint8_t pin);
long readVcc();
int readInternalTemp(void);

#endif /* __ADC_ARCH_H__ */