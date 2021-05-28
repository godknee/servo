#include <stdlib.h>

#include "cyclic_queue.h"

/* global variable */
rr_queue_t queue;

unsigned char queue_buf[300];

void rr_queue_init(rr_queue_t *queue, unsigned short size)
{
    queue->data = /*(unsigned char *)malloc(size)*/queue_buf;
    queue->head = queue->tail = 0;
    queue->capacity = size;
} 

unsigned char queue_in(rr_queue_t *queue, unsigned char data)
{
    if ((queue->tail+1) % (queue->capacity) == queue->head){
        return 0;
    }else{
        queue->data[queue->tail] = data;
        queue->tail = (queue->tail+1) % (queue->capacity);
        return 1;
    }
}


unsigned char queue_out(rr_queue_t *queue)
{
    if (queue->head == queue->tail){
        return 0;
    }else{
        unsigned char d = queue->data[queue->head];
        queue->head = (queue->head+1) % (queue->capacity);
        return d;
    }
}

unsigned char queue_is_empty(rr_queue_t *queue)
{
    if(queue->head == queue->tail){
        return 1;
    }else{
        return 0;
    }
}

//int main()
//{
//    rr_queue_t queue;
//
//    rr_queue_init(&queue,100);
//
//    unsigned int i=0;
//
//    for(i=0;i<100;i++){
//        queue_in(&queue,i);
//    }
//
//    for(i=0;i<100;i++){
//        printf("%d ",queue_out(&queue));
//    }
//    printf("\n");
//    return 0;
//}
