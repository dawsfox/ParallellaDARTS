#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "e-lib.h"
#include "codelet.h"
#include "codeletsQueue.h"
#include "threadedProcedure.h"
#include "tpClosuresQueue.h"
#include "e_darts_rt.h"
#include "e_darts_mailbox.h"
#include "e_DARTS.h"


#define N 4

extern codelet_t _dartsFinalCodelet;
extern nodeMailbox_t _dartsNodeMailbox;
void computeElement();

typedef struct int_matrix_s {
    int data[N*N];
} int_matrix_t;

    DEFINE_TP_MEM_REGIONS(mat_mult_tp,
		              //DRAM
			      int_matrix_t x;
			      int_matrix_t y;
			      int_matrix_t out;
			      ,
			      //INTERNAL
			      ,
			      //DIST
		              ) 

    DEFINE_THREADED_PROCEDURE(mat_mult_tp,3, {
			      memDRAM->x = x;
			      memDRAM->y = y;
			      memDRAM->out = out;
			      //start with one codelet per element in result matrix
			      ASSIGN_SYNC_SLOT_CODELET(*this,0,computeElement,1,1,N*N);
			      ASSIGN_SYNC_SLOT_CODELET(*this,1,printResult,N*N,N*N,1);
			      syncSlot_t *first_slot = GET_SYNC_SLOT(*this, 0);
			      e_darts_print("initializing TP\n");
			      DEC_DEP(first_slot);
		              }
			      , int_matrix_t x, int_matrix_t y, int_matrix_t out)

    DEFINE_TP_CLOSURE(mat_mult_tp,int_matrix_t,int_matrix_t,int_matrix_t);

void pushFinalCodelet() {
    codeletsQueue_t * suCodeletQueue = (codeletsQueue_t *) DARTS_APPEND_COREID(0x808,&(_dartsCUElements.mySUElements->darts_rt_codeletsQueue));
    while (pushCodeletQueue(suCodeletQueue, &(_dartsFinalCodelet)) != CODELET_QUEUE_SUCCESS_OP);
}

void computeElement()
{
    e_darts_print("computeElement running\n");
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    mat_mult_tp_memDRAM_t *memDRAM = (mat_mult_tp_memDRAM_t *) actualTP->memDRAM;
    syncSlot_t *printSlot = (syncSlot_t *) GET_SYNC_SLOT(*actualTP, 1);
    //perform multiplication
    DEC_DEP(printSlot);
}

void printResult()
{
    e_darts_print("printResult running\n");
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    mat_mult_tp_memDRAM_t *memDRAM = (mat_mult_tp_memDRAM_t *) actualTP->memDRAM;
    int *result = (int *) memDRAM->out.data;
    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            //print element
        }
    }
}

int main(void)
{
    unsigned base0_0Address = (unsigned) e_get_global_address(0, 0, 0x0000);

    e_darts_cam_t CAM = {SU, CU, CU ,CU,\
	                 CU, CU, CU, CU,\
                         CU, CU, CU, CU,\
			 CU, CU, CU, CU};
    e_darts_rt(CAM, CU_ROUND_ROBIN, SU_ROUND_ROBIN);
    if (e_group_config.core_row == 0 && e_group_config.core_col == 0) {
        e_darts_node_mailbox_init();
        e_darts_print("Address of mailbox: %x\n", &(_dartsNodeMailbox));
	e_darts_print("Address of SUtoNM's data: %x\n", &(_dartsNodeMailbox.SUtoNM.data));
	e_darts_print("Address of SUtoNM's ack: %x\n", &(_dartsNodeMailbox.SUtoNM.ack));
	e_darts_print("Address of NMtoSU: %x\n", &(_dartsNodeMailbox.NMtoSU));
	e_darts_print("Address of NMtoSU's ack: %x\n", &(_dartsNodeMailbox.NMtoSU.ack));
        e_darts_print("message at start: %d\n", _dartsNodeMailbox.NMtoSU.signal);
	int localInt;
	syncSlot_t localSlot;
	syncSlot_t *finalSyncSlot = (syncSlot_t *) &localSlot;
        initSyncSlot(finalSyncSlot, 0, 1, 1, _dartsFinalCodelet, 1);
        e_darts_print("final codelet ID: %x, final syncSlot address: %x\n", finalSyncSlot->codeletTemplate.codeletID, finalSyncSlot);
        //INVOKE(fib_tp,2,0,0,(int *)DARTS_APPEND_COREID(0x808,&localInt),(syncSlot_t *)DARTS_APPEND_COREID(0x808,finalSyncSlot));
        INVOKE(fib_tp,9,0,0,(int *)DARTS_APPEND_COREID(0x808,&localInt),(syncSlot_t *) NULL);
    }
    e_darts_run();
    return 0;
}