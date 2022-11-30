/* Name: run_UM.h
 * Int for run_UM.h
 * Bradley Chao and Matthew Soto
 * November 18, 2022
 */


#ifndef RUN_UM_H
#define RUN_UM_H

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include "universal_machine.h"
#include "instruction_set.h"

//static inline uint64_t shl(uint64_t word, unsigned bits);
//static inline uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value);

universal_machine read_program_file(FILE *fp);
void run_program(universal_machine UM);
void run_helper(universal_machine UM, int OP_CODE, UM_Reg A,
                 UM_Reg B, UM_Reg C);

/* Private Helper */
uint32_t read_word_helper(FILE *fp);



#endif