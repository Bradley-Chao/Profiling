#include "queue.h"

queue new_queue(uint32_t size)
{
    /* Array of 64-bit pointers to individual segments */
    uint64_t *new_arr = malloc(sizeof(*new_arr) * size);

    queue ret = malloc(sizeof(*ret));

    ret->arr = new_arr;
    ret->size = size;
    ret->num_elems = 0;

    return ret;
}

void free_queue(queue *qu)
{
    free((*qu)->arr);

    free(*qu);
}

uint64_t get_elem(queue qu, uint32_t index)
{       
    assert(index < qu->size);
    return qu->arr[index];
}

void set_elem(queue qu, uint32_t index, uint64_t elem)
{
    assert(index < qu->size);
    qu->arr[index] = elem;
}

uint32_t get_length(queue qu)
{
    return qu->size;
}

void enqueue(queue qu, uint64_t elem)
{
    if (qu->num_elems == qu->size) {

        uint32_t old_size = qu->size;

        uint32_t bigger_size = old_size * 2 + 1;

        qu->arr = realloc(qu->arr, bigger_size * sizeof(uint64_t));

        qu->arr[old_size] = elem;

        qu->size = bigger_size;

        qu->num_elems++;
    }
    else {

        qu->arr[qu->num_elems] = elem;
        qu->num_elems++;
    }
}