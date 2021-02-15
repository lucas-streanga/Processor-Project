#ifndef _ERROR_HANDLER_H
#define _ERROR_HANDLER_H

#include"proc_defines.h"
#include<iostream>
#include"CPU.h"

#define FATALERROR(x) std::cerr << TERM_COLOR_ERROR << "ERROR: " << x << "\nExitting now..." << TERM_COLOR_RESET << std::endl
#define NONFATALERROR(x) std::cerr << TERM_COLOR_WARN << "WARNING: " << x << TERM_COLOR_RESET << std::endl


/* Prototypes */
void error_handler(word, struct CPU *);
void exit_procedure(struct CPU *);


#endif
