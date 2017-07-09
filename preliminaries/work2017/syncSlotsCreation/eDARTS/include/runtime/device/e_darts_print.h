/**
 * @file e_darts_print.h
 * @author Jose M Monsalve
 * @date 8 July 2017
 * @brief printing service from the epiphany to darts 
 * @todo Add copyright
 * @todo add printing of variables
 *
 * Given that it is not possible to print from the device, it is necessary to
 * have a method that allows to pass information from the device to the host
 * so that the host can print it.
 */

#ifndef E_DARTS_PRINT_H
#define E_DARTS_PRINT_H

#include <stdio.h>
#include <stdlib.h>
#include "e_darts_mutex.h"
#include "common.h"

typedef struct __attribute__ ((packed)) printBuffer_s
{
    unsigned sendPrintInstruction;
    darts_mutex_t * mutex;
    char printingBufferHead[200];
} printBuffer_t;

darts_mutex_t __printingMutex __attribute__ ((section(".bss"))) = DARTS_MUTEX_NULL;
printBuffer_t __printBuffer __attribute__ ((section(".printBuffer"))) = {0, &__printingMutex, ""};

/**
 * @brief printing function
 * @todo allow printing variables
 * 
 * Send a message to this function and the message will be send to
 * the host by copying it to the printBuffer. There will be a thread 
 * on the host that will be spinning over the sendPrintInstruction flag
 * and whenver there is a 1, it will copy the information over
 * 
 */
void e_darts_print(const char * message);

#endif /* E_DARTS_PRINT_H */