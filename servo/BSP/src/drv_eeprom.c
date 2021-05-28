#include "drv_eeprom.h"

#define EEPROM_ADDR 0x4000
#define EEPROM_SIZE 128

void eeprom_init(void) {
  FLASH_DeInit();
}

int eeprom_write_data(uint32_t addr, uint8_t *buf, uint16_t len) {
  uint16_t i = 0;
  
  if(len > (EEPROM_SIZE-addr)) {
    return -1;
  }
  
  if(addr >= EEPROM_SIZE) {
    return -2;
  }
  
//  FLASH_Unlock(FLASH_MEMTYPE_DATA);
  /* Unlock data memory */
  FLASH->DUKR = FLASH_RASS_KEY2; /* Warning: keys are reversed on data memory !!! */
  FLASH->DUKR = FLASH_RASS_KEY1;
  
  for(i=0; i<len; i++) {
     FLASH_ProgramByte(EEPROM_ADDR+addr+i, buf[i]);
  }
  
  FLASH_Lock(FLASH_MEMTYPE_DATA);
  
  return 0;
}

int eeprom_read_data(uint32_t addr, uint8_t *buf, uint16_t len) {
  uint16_t i = 0;
  
  if(len > (EEPROM_SIZE-addr)) { 
    return -1;
  }
  
  if(addr >= EEPROM_SIZE) {
    return -2;
  }
  
  for(i=0; i<len; i++) {
    buf[i] = FLASH_ReadByte(EEPROM_ADDR+addr+i);
  }
  
  return 0;
}