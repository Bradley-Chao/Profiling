#include <stdlib.h>
#include <stdio.h>

#include "queue.h"

int main()
{
        queue test_queue = new_queue(0);

        enqueue(test_queue, 88);
        // enqueue(test_queue, 99);
        // enqueue(test_queue, 111);
        // enqueue(test_queue, 222);


        printf("TEST SIZE: %d\n", get_length(test_queue));

        // for (size_t i = 0; i < 3; i++) {
        //     printf("index : %lu\telem: %d\n", i, get_elem(test_queue, i));
        // }

        //set_elem(test_queue, 2, 666);

        // for (size_t i = 0; i < 3; i++) {
        //     printf("index : %lu\telem: %d\n", i, get_elem(test_queue, i));
        // }

        free_queue(&test_queue);


    

}

