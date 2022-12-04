/* Name: universal_machine.h
 * Interface for universal_machine.c
 * Bradley Chao and Matthew Soto
 * November 18, 2022
 */

#ifndef UNIVERSAL_MACHINE
#define UNIVERSAL_MACHINE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

typedef uint32_t UM_instruction;

typedef struct universal_machine {
        uint32_t registers[8]; 
        uint32_t program_counter;

        /* C-Array with first 32-bit integer as size */
        uint32_t *unmapped_IDs;
        uint32_t num_IDs;
        uint32_t ID_arr_size;
        
        /* contiguous block of 64-bit addresses of queues which represent segments of 32-bit word instructions */
        uint32_t **segments;
        uint32_t num_segments;
        uint32_t segment_arr_size;

} *universal_machine;

universal_machine new_UM(uint32_t *program_instructions);
void free_UM(universal_machine *UM);

uint32_t map_segment(universal_machine UM, uint32_t num_words);
void unmap_segment(universal_machine UM, uint32_t segment_ID);

#endif