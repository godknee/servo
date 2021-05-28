#ifndef __CYCLIC_QUEUE_H
#define __CYCLIC_QUEUE_H


typedef struct{
    unsigned char *data;
    unsigned short head;
    unsigned short tail;
    unsigned short capacity;

}rr_queue_t;

/* global variable */
extern rr_queue_t queue;

void rr_queue_init(rr_queue_t *queue, unsigned short size);
unsigned char queue_in(rr_queue_t *queue, unsigned char data);
unsigned char queue_out(rr_queue_t *queue);
unsigned char queue_is_empty(rr_queue_t *queue);

#endif