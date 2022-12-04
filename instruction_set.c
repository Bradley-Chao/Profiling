/* Name: instruction_set.c
 * Purpose: This module is responsible for executing the 32-bit word 
 * instructions, calling methods in the UM ADT to do so, abstracting runtime
 * details from the data structure itself.
 * By: Bradley Chao and Matthew Soto
 * Date: 11/16/2022
 */

#include "instruction_set.h"

/* This constant is equivalent to 2^32 and is used for modulus operation to 
   keep all arithmetic operation results in the range of 0, 2^32 - 1 */
#define mod_limit 4294967296;

/* Name: conditional_move
*  Purpose: if $r[C] != 0 then $r[A] := $r[B]
*  Parameters: Register indices
*  Returns: none
*  Effects: Checked runtime error is UM is null or
*           if A, B, or C are bigger than 8
*/
void conditional_move(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C)
{
        if (UM->registers[C] != 0)
                UM->registers[A] = UM->registers[B];
}

/* Name: segmented_load
*  Purpose: $r[A] := $m[$r[B]][$r[C]]
*  Parameters: B stores the segment ID, C stores the offset, A stores result
*  Returns: none
*  Effects: none
*/
void segmented_load(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C)
{
        uint32_t segment_ID = UM->registers[B];
        uint32_t offset = UM->registers[C];

        UM->registers[A] = UM->segments[segment_ID][offset + 1];
}

/* Name: segmented_store
*  Purpose:  Segmented Store $m[$r[A]][$r[B]] := $r[C]
*  Parameters: A holds the segment ID, B stores the offset, C stores value to
*  store in UM
*  Returns: none
*  Effects: none
*/
void segmented_store(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C)
{
        uint32_t segment_ID = UM->registers[A];
        uint32_t offset = UM->registers[B];

        UM->segments[segment_ID][offset + 1] = UM->registers[C];
}
/* Name: addition
*  Purpose: Add registers to update one
*  Parameters: UM, A, red_B, C
*  Returns: none
*  Effects: none
*/
void addition(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C)
{
        UM->registers[A] = (UM->registers[B] + UM->registers[C]) % mod_limit;
}

/* Name: multiplication
*  Purpose: multiply registers to update one 
*  Parameters: UM, A, red_B, C
*  Returns: none
*  Effects: none
*/
void multiplication(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C)
{
        UM->registers[A] = (UM->registers[B] * UM->registers[C]) % mod_limit;
}

/* Name: division
*  Purpose: divide register to update one
*  Parameters: UM, A, red_B, C
*  Returns: none
*  Effects: Checked runtime error for divide by 0
*/
void division(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C)
{
        UM->registers[A] = (UM->registers[B] / UM->registers[C]) % mod_limit;
}

/* Name: bitwise_nand
*  Purpose: get the not of register B and C
*  Parameters: UM, A, red_B, C
*  Returns: none
*  Effects: updates register A
*/
void bitwise_nand(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C)
{
        UM->registers[A] = ~(UM->registers[B] & UM->registers[C]);
}

/* Name: map_segment
*  Purpose: new segment is created
*  Parameters: UM, A, red_B, C
*  Returns: none
*  Effects: new segment is created
*/
void map(universal_machine UM, UM_Reg B, UM_Reg C)
{
        UM->registers[B] = map_segment(UM, UM->registers[C]);
}

/* Name: unmap_segment
*  Purpose: segment $m[$r[c]] is unmapped
*  Parameters: UM, A, red_B, C
*  Returns: none
*  Effects: segment $m[$r[c]] is unmapped
*/
void unmap(universal_machine UM, UM_Reg C)
{
        unmap_segment(UM, UM->registers[C]);
}

/* Name: unmap_segment
*  Purpose: segment $m[$r[c]] is unmapped
*  Parameters: UM, A, red_B, C
*  Returns: none
*  Effects: Checked runtime error if value from register c
*           is more than 255
*/
void output(universal_machine UM, UM_Reg C)
{
        putchar(UM->registers[C]);
}

/* Name: input
*  Purpose: Universal machine awaits input from I/O devise
*  Parameters: UM, A, red_B, C
*  Returns: none
*  Effects: instruction depend on I/O
*           Checked runtime error if value is
*.          out of range (has to be between 0 and 255)
*/
void input(universal_machine UM, UM_Reg C)
{
        int int_value = getchar();

        if (int_value == EOF)
                UM->registers[C] = ~0;
        else 
                UM->registers[C] = int_value;
}

/* Name: load_program
*  Purpose: segment $m[$r[B]] is duplicated and replaces $m[0]
*  Parameters: UM, A, red_B, C
*  Returns: none
*  Note: Program counter is redirected in another module 
*        Checked runtime if target or duplicates are NULL 
*/
void load_program(universal_machine UM, UM_Reg B)
{
        uint32_t reg_B_value = UM->registers[B];

        /* Not allowed to load segment zero into segment zero */
        if (reg_B_value != 0) {
                uint32_t *target_segment = UM->segments[reg_B_value];

                uint32_t num_instructions = target_segment[0];

                uint32_t *deep_copy = malloc((num_instructions + 1) * sizeof(uint32_t));
                assert(deep_copy);

                uint32_t true_size = num_instructions + 1;
                for (size_t i = 0; i < true_size; i++)
                        deep_copy[i] = target_segment[i];

                free(UM->segments[0]);

                UM->segments[0] = deep_copy;
        }       
}

/* Name: load_value
*  Purpose: set $r[A] to value
*  Parameters: UM, A, red_B, C
*  Returns: none
*  Effects: changes register A
*/
void load_value(universal_machine UM, UM_Reg A, uint32_t value)
{
        UM->registers[A] = value;
}