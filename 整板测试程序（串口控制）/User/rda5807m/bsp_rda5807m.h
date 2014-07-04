#ifndef __RDA5807M_H
#define	__RDA5807M_H

#include "stm32f10x.h"

void RDA5807_write_reg(void);
void RDA5807_read_reg(uint8_t *reg_buf);
void RDA5807_power(void);
void RDA5807_FM_seek(void);
void RDA5807_FM_Test(void);

#endif /* __RDA5807M_H */


