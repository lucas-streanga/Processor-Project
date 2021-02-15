# Processor-Project
A virtual processor with a unique instruction set made in C++.

# How it works
This virtual processor allows the user to write programs in machine code
for it. 
The program will read in a file containing 32 bit instructions and data.
The processor is specified an amount of virtual memory to use, 
and the instructions and data will be copied into memory. The processor
will execute instructions until the end execution instruction is recieved,
opcode 255. The processor includes 16 general purpose registers with a program counter
and stack pointer. The program counter is initialized to the start of memory and the 
stack pointer to the end of the user's data. The program counter will be increamented
4 bytes (1 instruction) after every instruction execution, and can be editied directly
to create loops. The processor includes some special instructions for ease of use such 
as PRNR (print register value) and PRNM (print memory value). 

## Usage
The **size="size"** command line parameter will allow the user to set the size of memory in bytes.
If the size is invalid or no size is given, the default size of **16384** will be used. The 
**print_mem=true** parameter may be passed in to print the content of the memory before execution
begins, so that the programmer may check their data is correct. A file with instructions and data is
redirected in with **<**.

## Program File Formatting
The user program should be written in plain text and each instruction as 32 characters, 1 or 0. 
Whitespace is ignored, and # may be used for comments. The special character * **(asterisk)** may be used
as a shorthand for the end execution instruction, opcode 255. The file should end with **!** so the program
knows when to stop reading. Without an end execution instruction, the processor will execute through all of 
memory until the PC reaches the end of memory. Without an ! to indicate end of file, stdin contents will continue
to be read untill all of the memory is filled.


