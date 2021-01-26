#include <e-hal.h>
#include <stdio.h>
#include <stdbool.h>

#define E_DARTS_OK 0
#define DARTS_RT_ALIVE 0x6c
#define MAX_PAYLOAD_SIZE 0x7f //127 for alignment
#define MAILBOX_ADDRESS 0x8e000134 //based on print statement
#define BASE_OFFSET 0x00000000
#define SU_TO_NM_OFFSET 0x00000000
#define HEADER_OFFSET 0x00000000
#define PAYLOAD_OFFSET 0x00000008
#define ACK_OFFSET (PAYLOAD_OFFSET + MAX_PAYLOAD_SIZE)
#define SIGNAL_OFFSET (ACK_OFFSET + 0x00000001)
#define NM_TO_SU_OFFSET (SIGNAL_OFFSET + 0x00000004)

// doxygen?

typedef enum message_select {
    blank = 0,
    SU_MAILBOX_REJECT = 1,      //SU rejects incoming data
    SU_MAILBOX_ACCEPT = 2,      //SU accepts incoming data
    NM_MAILBOX_REJECT = 3,      //NM rejects incoming data
    NM_MAILBOX_ACCEPT = 4,      //NM accepts incoming data
    NM_REQUEST_SU_RECEIVE = 5,  //NM requests SU receive some data, ex: invoke new closure from NM
    NM_REQUEST_SU_PROVIDE = 6,  //NM requests SU provide some data
    SU_REQUEST_NM_RECEIVE = 7,  //SU requests NM receive some data
    SU_REQUEST_NM_PROVIDE = 8,   //SU requests NM provide some data
    NM_REQUEST_STATUS = 9,
    SU_PROVIDE_STATUS = 10
    //reset?
} message;

typedef enum message_type {
    DATA = 1,
    TPCLOSURE = 2,
    STATUS = 3
} messageType;

// 4 + 4 = 8 bytes of header
typedef struct __attribute__ ((__packed__)) header_s {
    messageType msg_type;
    unsigned size;
} header_t; //on receive allocate memory for struct + size, load payload into void pointer

// 12 + MAX_PAYLOAD_SIZE + 1 + 4  = 17 + MAX_PAYLOAD_SIZE per mailbox
typedef struct __attribute__ ((__packed__)) mailbox_s {
    header_t msg_header;
    char data[MAX_PAYLOAD_SIZE];
    bool ack;
    message signal;
} mailbox_t;

// 34 + 2 * MAX_PAYLOAD_SIZE overall
typedef struct __attribute__ ((__packed__)) nodeMailbox_s {
    mailbox_t SUtoNM;
    mailbox_t NMtoSU;
} nodeMailbox_t;

typedef struct __attribute__ ((__packed__)) sigWithAck_s {
    bool ack;
    message signal;
} sigWithAck_t;

typedef union int_data_u {
    char raw[4];
    int processed;
} int_converter;

typedef union unsigned_data_u {
    char raw[4];
    unsigned processed;
} unsigned_converter;

e_platform_t platform;
e_epiphany_t dev;

extern nodeMailbox_t localMailbox;

int darts_init();

// non blocking; should change this later to allow the elf file to be independently selected
// add error checking
void darts_run(char* elf_file_name);

// wait for the runtime to close out
void darts_wait();

void darts_close();

int darts_send_message(message *signal);

int darts_send_message_wait(message *signal);

int darts_send_data(mailbox_t *data_loc);

int darts_send_data_wait(mailbox_t *data_loc);

// need to add generic tp closure to header definition and such
int darts_invoke_TP(void* closure);

message darts_receive_message(message *signal);

message darts_receive_data(mailbox_t* mailbox);

//helper function to fill mailbox data easier
void darts_fill_mailbox(mailbox_t *mailbox, messageType type, unsigned size, message signal);

int darts_set_ack(bool ack);

bool darts_get_ack();

int darts_data_convert_to_int(char *data);

unsigned darts_data_convert_to_unsigned(char *data);

void darts_int_convert_to_data(int input, char *data);

void darts_unsigned_convert_to_data(unsigned input, char *data);

//array of counts of args in following order: int, unsigned, char, float
unsigned short darts_args_encoding(unsigned short *type_array);

void darts_args_decoding(unsigned short code, unsigned short *type_array);
