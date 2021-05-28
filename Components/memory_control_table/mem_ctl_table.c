#include <string.h>

#include "mem_ctl_table.h"
#include "drv_eeprom.h"

/* 
   ---------

   ROM

   --------- MEM_CTL_TABLE_RAM_START_ADDR

   RAM

   ---------
 */
uint8_t mem_ctl_table[MEM_CTL_TABLE_SIZE];
uint8_t mem_ctl_table_cache[MEM_CTL_TABLE_SIZE];

void servo_mem_ctl_table_init(void) 
{
  /* clear the RAM table */
  memset(mem_ctl_table+MEM_CTL_TABLE_RAM_START_ADDR, 0x00, MEM_CTL_TABLE_SIZE-MEM_CTL_TABLE_RAM_START_ADDR+1);
  /* restore the ROM table from eeprom */
  eeprom_read_data(0, mem_ctl_table, MEM_CTL_TABLE_ROM_END_ADDR+1);
}
