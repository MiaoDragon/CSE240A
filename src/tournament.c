//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h" // for including the global variables
#include "tournament.h" // for including the global variables

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

int tour_ghistoryBits; // Number of bits used for Global History
int tour_lhistoryBits; // Number of bits used for Local History
int tour_pcIndexBits;  // Number of bits used for PC index



uint32_t tour_ghistory;
uint32_t tour_lhistoryTableSize;
uint32_t tour_lhistoryTable[1025];
uint32_t tour_lhistoryPredTableSize;
int8_t tour_lhistoryPredTable[1025];  // maximal 2^10
uint32_t tour_ghistoryTableSize;
int8_t tour_ghistoryTable[8193];  // maximal 2^13
uint32_t tour_chooserTableSize;
int8_t tour_chooserTable[8193];  // maximal 2^13   // 0 for selecting lhistory, 1 for selecting ghistory
                             // initial value: WS the global history, i.e. 10=2


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
tournament_init_predictor(int ghistoryBits_in, int lhistoryBits_in, int pcIndexBits_in)
{
    tour_ghistoryBits = ghistoryBits_in;
    tour_lhistoryBits = lhistoryBits_in;
    tour_pcIndexBits = pcIndexBits_in;
    // initialize sizes for each table
    tour_lhistoryTableSize = 1 << tour_pcIndexBits;
    tour_lhistoryPredTableSize = 1 << tour_lhistoryBits;
    tour_ghistoryTableSize = 1 << tour_ghistoryBits;
    tour_chooserTableSize = 1 << tour_ghistoryBits;

    // initialize global history bits to be NOTTAKEN
    tour_ghistory = NOTTAKEN;

    // initialize local historyto be NOTTAKEN
    for (uint32_t i=0; i<tour_lhistoryTableSize; i++)
    {
        tour_lhistoryTable[i] = NOTTAKEN;
    }


    // initialize 2-bit predictors to be WN
    for (uint32_t i=0; i<tour_lhistoryPredTableSize; i++)
    {
        tour_lhistoryPredTable[i] = WN;
    }

    for (uint32_t i=0; i<tour_ghistoryTableSize; i++)
    {
        tour_ghistoryTable[i] = WN;
    }

    // initialize the chooser table to be WT the global history
    for (uint32_t i=0; i<tour_chooserTableSize; i++)
    {
        tour_chooserTable[i] = WT;
    }

}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
tournament_make_prediction(uint32_t pc)
{
    //*** chooser
    // use the global history to index in the chooserTable
    int8_t chooser = tour_chooserTable[tour_ghistory];
    // chooser N/: select local/global
    if (chooser == SN || chooser == WN)
    {
        // select local hisotry
        //*** local prediction
        // use program counter to index in the lhistoryTable for finding the local history
        uint32_t masked_pc = pc % tour_lhistoryTableSize;
        uint32_t lhistory = tour_lhistoryTable[masked_pc];
        // use the local history to index in the lhistoryPredTable for finding the current state
        int8_t lhistoryPred = tour_lhistoryPredTable[lhistory];
        // use the state for making prediction
        if (lhistoryPred == SN || lhistoryPred == WN)
        {
            return NOTTAKEN;
        }
        else
        {
            return TAKEN;
        }

    }
    else
    {
        // select global history
        //*** global prediction
        // use global history bits to index in the ghistoryTable for finding the current state
        int8_t ghistoryPred = tour_ghistoryTable[tour_ghistory];
        if (ghistoryPred == SN || ghistoryPred == WN)
        {
            return NOTTAKEN;
        }
        else
        {
            return TAKEN;
        }
    }
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
tournament_train_predictor(uint32_t pc, uint8_t outcome)
{
    //*** train Local
    // use program counter to index in the lhistoryTable for finding the local history
    uint32_t masked_pc = pc % tour_lhistoryTableSize;
    // extract the previous history value
    uint32_t lhistory = tour_lhistoryTable[masked_pc];
    // use the local history to index in the lhistoryPredTable for finding the current state
    int8_t lhistoryPred = tour_lhistoryPredTable[lhistory];
    // update the value of local history prediction based on the outcome
    // if the outcome is 1, then increment the prediction. Otherwise decrement the prediction
    tour_lhistoryPredTable[lhistory] = lhistoryPred + 1*(outcome == 1) + (-1)*(outcome == 0);
    // handle boundary case
    if (tour_lhistoryPredTable[lhistory] > ST)
    {
        tour_lhistoryPredTable[lhistory] = ST;
    }
    if (tour_lhistoryPredTable[lhistory] < SN)
    {
        tour_lhistoryPredTable[lhistory] = SN;
    }
    // update local history table: shift and add
    lhistory = (lhistory << 1) + outcome;
    // mask local history to be inside bits of lhistoryBits;
    lhistory = lhistory % tour_lhistoryPredTableSize;

    tour_lhistoryTable[masked_pc] = lhistory;


    //*** train global
    // use global history bits to index in the ghistoryTable for finding the current state
    int8_t ghistoryPred = tour_ghistoryTable[tour_ghistory];
    // update the value of global history prediction based on the outcome
    // if the outcome is 1, then increment the prediction. Otherwise decrement the prediction
    tour_ghistoryTable[tour_ghistory] = ghistoryPred + 1*(outcome == 1) + (-1)*(outcome==0);
    // handle boundary case
    if (tour_ghistoryTable[tour_ghistory] > ST)
    {
        tour_ghistoryTable[tour_ghistory] = ST;
    }
    if (tour_ghistoryTable[tour_ghistory] < SN)
    {
        tour_ghistoryTable[tour_ghistory] = SN;
    }

    //*** train chooser
    int8_t chooser = tour_chooserTable[tour_ghistory];
    // if the predictions are different then update to the correct one
    uint8_t lhistoryResult = 0;
    if (lhistoryPred == SN || lhistoryPred == WN)
    {
        lhistoryResult = 0;
    }
    else
    {
        lhistoryResult = 1;
    }
    uint8_t ghistoryResult = 0;
    if (ghistoryPred == SN || ghistoryPred == WN)
    {
        ghistoryResult = 0;
    }
    else
    {
        ghistoryResult = 1;
    }
    //printf("in training...\n");
    //printf("lhistory prediction: %d\n", lhistoryPred);
    //printf("ghistory predction: %d\n", ghistoryPred);
    //printf("lhistory result: %d\n", lhistoryResult);
    //printf("ghistory result: %d\n", ghistoryResult);
    //printf("outcome: %d\n", outcome);
    // when local history result is correct (== outcome), then decrement
    // when global history result is correct (==outcome), then increment
    // when predictions are the same then the increment are cancelled
    tour_chooserTable[tour_ghistory] = chooser + (-1)*(lhistoryResult==outcome) + (1)*(ghistoryResult==outcome);
    // handle boundary case
    if (tour_chooserTable[tour_ghistory] > ST)
    {
        tour_chooserTable[tour_ghistory] = ST;
    }
    if (tour_chooserTable[tour_ghistory] < SN)
    {
        tour_chooserTable[tour_ghistory] = SN;
    }

    // update global history
    tour_ghistory = (tour_ghistory << 1) + outcome;
    // mask global history to be inside bits of ghistoryBits
    tour_ghistory = tour_ghistory % tour_ghistoryTableSize;
    //tour_ghistory = (1<<ghistoryBits-1) & tour_ghistory;
}
