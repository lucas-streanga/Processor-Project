/*
Defines for the project.
The Opcodes will defined here,
as well as the error codes
and the calculation constants
*/

#ifndef _PROC_DEFINES_H
#define _PROC_DEFINES_H

#include<stdint.h>

/* Terminal Colors */
#define TERM_COLOR_RESET "\033[0m"
#define TERM_COLOR_WARN "\033[1m\033[33m"
#define TERM_COLOR_LOG "\033[35m"
#define TERM_COLOR_ERROR "\033[1m\033[31m"
#define TERM_COLOR_TIMER "\033[32m"

#if DEBUG == 1
#include<iostream>
#define LOG(x) std::cout << TERM_COLOR_LOG << x << TERM_COLOR_RESET << std::endl
#include<iomanip>
#else
#define LOG(x)
#endif

/* Constants */
#define MAX_U         4294967295L
#define MAX_S         2147483647L
#define SIGN_BIT      (-(2147483648L))

/* Constants for decoding */
#define DEC_OP    (0b11111111000000000000000000000000)
#define DEC_COND  (0b00000000111100000000000000000000)
#define DEC_I     (0b00000000000010000000000000000000)
#define DEC_S     (0b00000000000001000000000000000000)
#define DEC_RN    (0b00000000000000111100000000000000)
#define DEC_RD    (0b00000000000000000011110000000000)
#define DEC_IMMED (0b00000000000000000000001111111111)
#define DEC_OPTWO (0b00000000000000000000001111000000)
#define DEC_SHIFT (0b00000000000000000000000000111111)

/* Error Codes */
// Out of memory
#define ERR_OOM   0
//Unable to alloc memory
#define ERR_UTA   1
//Execution reached end of memory
#define ERR_EOM   2
//Unknown opcode
#define ERR_UOC   3
//Virtual segmentation fault
#define ERR_SEG   4
//Incomplete instruction warning
#define ERR_ICI   5

/* Opcodes */
#define OP_ADD    0
#define OP_SUB    1
#define OP_LDR    2
#define OP_LDRH   3
#define OP_LDRB   4
#define OP_STR    5
#define OP_STRH   6
#define OP_STRB   7
#define OP_MOV    8
#define OP_CMP    9
#define OP_PRNR   10
#define OP_PRNM   11
#define TOTAL_INS 11

#define OP_END    255

/* Conditionals */
#define COND_AL 0
#define COND_EQ 1
#define COND_NE 2
#define COND_CS 3
#define COND_CC 4
#define COND_MI 5
#define COND_PL 6
#define COND_OS 7
#define COND_OC 8
#define COND_HI 9
#define COND_LS 10
#define COND_GE 11
#define COND_LT 12
#define COND_GT 13
#define COND_LE 14
#define COND_NV 15




/* Default Values */
#define DEFAULT_MEM_SIZE    16384
#define REGISTERS           16



/* Typedefs */
typedef uint8_t byte;
typedef uint16_t half;
typedef uint32_t word;
typedef uint64_t dword;
/* END */

#endif
