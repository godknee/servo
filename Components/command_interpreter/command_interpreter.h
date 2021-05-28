#ifndef __CMD_PROCESS_H
#define __CMD_PROCESS_H
#include "stm8s.h"
#include "protocal_process.h"

#define PING        0x01
#define READ_DATA   0x02
#define WRITE_DATA  0x03
#define REG_WRITE   0x04
#define ACTION      0x05
#define RESET       0x06
#define READ_CURRENT_VALUE 0x07
#define RUN_TO_MIDDLE 0x08
#define SYNC_WRITE  0x83

struct cmd_table_t {
    unsigned char cmd;
    void (*callback)(protocal_handle_t *protocal_handle);
};

void cmd_execute(protocal_handle_t *protocal_handle);

#endif
