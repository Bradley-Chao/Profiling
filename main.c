
/* Name: main.c
 * Purpose: main executes a virtual machine program which reads in a user file
 * and executes 32-bit word instructions
 * By: Bradley Chao and Matthew Soto
 * Date: 11/16/2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>

#include "universal_machine.h"
#include "instruction_set.h"

Except_T Bitpack_Overflow = { "Overflow packing bits" };

static inline uint64_t shl(uint64_t word, unsigned bits)
{
        assert(bits <= 64);
        if (bits == 64)
                return 0;
        else
                return word << bits;
}

static inline uint64_t shr(uint64_t word, unsigned bits)
{
        assert(bits <= 64);
        if (bits == 64)
                return 0;
        else
                return word >> bits;
}

static inline bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        assert(width <= 64);
        /* thanks to Jai Karve and John Bryan  */
        /* clever shortcut instead of 2 shifts */
        return shr(n, width) == 0; 
}

static inline uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                      uint64_t value)
{
        assert(width <= 64);
        unsigned hi = lsb + width; /* one beyond the most significant bit */
        assert(hi <= 64);
        if (!Bitpack_fitsu(value, width)) {
                printf("Value : %lu\n", value);
                printf("width : %u\n", width);

                RAISE(Bitpack_Overflow);
        }
        return shl(shr(word, hi), hi)                 /* high part */
                | shr(shl(word, 64 - lsb), 64 - lsb)  /* low part  */
                | (value << lsb);                     /* new part  */
}

static inline uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        unsigned hi = lsb + width; /* one beyond the most significant bit */
        assert(hi <= 64);
        /* different type of right shift */
        return shr(shl(word, 64 - hi),
                   64 - width); 
}

universal_machine read_program_file(FILE *fp)
{
        assert(fp != NULL);

        uint32_t *segment_zero = malloc(100 * sizeof(uint32_t));
        uint32_t segment_size = 100;
        uint32_t num_elems = 0;

        uint32_t word = 0;

        int byte = fgetc(fp);

        while (byte != EOF) {
                word = 0;

                for (int i = 3; i >= 0; i--) {
                        word = Bitpack_newu(word, 8, i * 8, byte);
                        byte = fgetc(fp);
                }

                /* If num elems + size elem is equal to total malloc'd size */
                if (num_elems + 1 == segment_size) {
                        uint32_t bigger_size = segment_size * 2;
                        segment_zero = realloc(segment_zero, bigger_size * sizeof(uint32_t));
                        segment_zero[num_elems + 1] = word;
                        segment_size = bigger_size;
                }
                else
                        segment_zero[num_elems + 1] = word;

                num_elems++;
        }

        segment_zero[0] = num_elems;

        universal_machine UM = new_UM(segment_zero);

        return UM;
}

/* Name: run_helper
*  Purpose: Handles non load value instruction cases  op codes 0-12
*  Parameters: UM, OP_CODE, A, B, C
*  Returns: none
*  Effects: helps choose which case/function to call
*/
void run_helper(universal_machine UM, int OP_CODE, UM_Reg A,
                UM_Reg B, UM_Reg C)
{
        switch (OP_CODE) {
                case 0:
                        conditional_move(UM, A, B, C);
                        break;
                case 1:
                        segmented_load(UM, A, B, C);
                        break;
                case 2:
                        segmented_store(UM, A, B, C);
                        break;
                case 3:
                        addition(UM, A, B, C);
                        break;
                case 4:
                        multiplication(UM, A, B, C);
                        break;
                case 5:
                        division(UM,  A, B, C);
                        break;
                case 6:
                        bitwise_nand(UM, A, B, C);
                        break;
                case 8:
                        map(UM, B, C);
                        break;
                case 9:
                        unmap(UM, C);
                        break;
                case 10:
                        output(UM, C);
                        break;
                case 11:
                        input(UM, C);
                        break;
                case 12:
                        load_program(UM, B);
                        break;
        }
}

void print_registers(universal_machine UM)
{
        for (size_t i = 0; i < 8; i++) {
                printf("R[%lu] = %d\n", i, UM->registers[i]);
        }
}

void print_spine(universal_machine UM)
{
        for (size_t i = 0; i < UM->num_segments; i++) {
                printf("SEGMENT ID%lu\n", i);

                for (size_t j = 0; j < UM->segments[i][0]; j++)
                        printf("Word[%lu]: %d\n", j, UM->segments[i][j + 1]);
        }
}

/* Name: run_program
 * Purpose: Command loop for each machine cycle 
 * Parameters: Pointer to instance of universal machine
 * Returns: Void
 * Effects: Checked runtime error if program counter is out of bounds, invalid
 * OP_CODE, and if segment zero was unavailable 
 */
void run_program(universal_machine UM)
{
        assert(UM != NULL);

        while (true) {
                uint32_t *segment_zero = UM->segments[0];

                UM_instruction word = segment_zero[UM->program_counter + 1];

                int OP_CODE = Bitpack_getu(word, 4, 28);
               
                UM_Reg A, B, C;

                /* Halt Command, exit function to free data */
                if (OP_CODE == 7) {
                        return;
                }
                /* Special Load Value Command */
                else if (OP_CODE == 13) {
                        A = Bitpack_getu(word, 3, 25);
                        int load_val = Bitpack_getu(word, 25, 0);
                        
                        load_value(UM, A, load_val);
                }
                /* Other 12 instructions */
                else {
                        A = Bitpack_getu(word, 3, 6);
                        B = Bitpack_getu(word, 3, 3);
                        C = Bitpack_getu(word, 3, 0);

                        run_helper(UM, OP_CODE, A, B, C);
                }
        
                if (OP_CODE == 12)
                        UM->program_counter = UM->registers[C];
                else
                        UM->program_counter++;

                
        }
}

int main(int argc, char *argv[])
{
        assert(argc == 2);

        FILE *fp = fopen(argv[1], "rb");
        
        universal_machine UM = read_program_file(fp);

        run_program(UM);

        free_UM(&UM);

        fclose(fp);

        return 0;
}