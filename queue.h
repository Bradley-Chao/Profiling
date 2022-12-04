#ifndef OUR_SEQUENCE_H
#define OUR_SEQUENCE_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "inttypes.h"

typedef struct queue {
    uint32_t num_elems;
    uint32_t size;
    uint32_t *arr;

} *queue;

queue new_queue(uint32_t size);

void free_queue(queue *qu);

uint64_t get_elem(queue qu, uint32_t index);

void set_elem(queue qu, uint32_t index, uint64_t elem);

uint32_t get_length(queue qu);

void enqueue(queue qu, uint64_t elem);

#endif