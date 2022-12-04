/* Name: universal_machine.c
 * This module is responsible for implementing the universal data structure
 * Notable methods include querying, writing, and mapping/unmapping values
 * from the data structure
 * Bradley Chao and Matthew Soto
 * November 18, 2022
 */

#include "universal_machine.h"

universal_machine new_UM(uint32_t *program_instructions)
{
        universal_machine UM = malloc(sizeof(*UM));

        for (size_t i = 0; i < 8; i++) {
                UM->registers[i] = 0;
        }

        UM->program_counter = 0;

        UM->unmapped_IDs = malloc(1 * sizeof(uint32_t));
        UM->num_IDs = 0;
        UM->ID_arr_size = 1;

        UM->segments = malloc(1 * sizeof(uint32_t *));
        UM->num_segments = 1;
        UM->segment_arr_size = 1;

        /* Store pointer to first word instruction in segment zero */
        UM->segments[0] = program_instructions;

        return UM;
}

void free_UM(universal_machine *UM)
{
        /* Free malloc'd 32-bit instruction segments */

        uint32_t **spine = (*UM)->segments;

                
        for (size_t i = 0; i < (*UM)->num_segments; i++)
                free(spine[i]);

        free(spine);        

        /* Free unmapped IDs */
        free((*UM)->unmapped_IDs);

        /* Frees malloced pointer to the UM struct */
        free(*UM);
}

uint32_t map_segment(universal_machine UM, uint32_t num_words)
{
        /* Allocate (num_words + 1) * sizeof(32) bytes with words = 0 */
        uint32_t *new_segment = calloc(num_words + 1, sizeof(uint32_t));
        assert(new_segment);

        /* First elem stores the number of words */
        new_segment[0] = num_words;
        
        /* Case 1: If there are no unmapped IDs */
        if (UM->num_IDs == 0) {
                /* Check whether realloc is necessary for segments spine */
                if (UM->num_segments == UM->segment_arr_size) {
                        uint32_t bigger_arr_size = UM->segment_arr_size * 2;
                        UM->segments = realloc(UM->segments, bigger_arr_size * sizeof(uint32_t *));
                        assert(UM->segments);
                        UM->segment_arr_size = bigger_arr_size;
                }

                UM->segments[UM->num_segments] = new_segment;

                UM->num_segments++;

                return UM->num_segments - 1;
        }
        /* Case 2: There are unmapped IDs available for use */
        else {
                /* Back most element of array / top of stack */
                uint32_t available_ID = UM->unmapped_IDs[UM->num_IDs - 1];
                UM->num_IDs--;

                /* Free data that has been there */
                uint32_t *to_unmap = UM->segments[available_ID];
                free(to_unmap);

                UM->segments[available_ID] = new_segment;

                UM->num_segments++;

                return available_ID;
        }
}

/* This function purely makes the ID available does not free data */
void unmap_segment(universal_machine UM, uint32_t segment_ID)
{
        /* Add the new ID to the ID C-array */
        if (UM->num_IDs == UM->ID_arr_size) {
                uint32_t bigger_arr_size = UM->ID_arr_size * 2;
                UM->unmapped_IDs = realloc(UM->unmapped_IDs, bigger_arr_size * sizeof(uint32_t));
                assert(UM->unmapped_IDs);
                UM->ID_arr_size = bigger_arr_size;
        }

        /* Push the newly available ID to the top of the stack */
        UM->unmapped_IDs[UM->num_IDs] = segment_ID;

        /* Update number of IDs and number of segments */
        UM->num_IDs++;
        UM->num_segments--;
}