#ifndef __EEPROM_H_
#define __EEPROM_H_
#include "stm8s_flash.h"

void eeprom_init(void); 
int eeprom_write_data(uint32_t addr, uint8_t *buf, uint16_t len);
int eeprom_read_data(uint32_t addr, uint8_t *buf, uint16_t len);
#endif
