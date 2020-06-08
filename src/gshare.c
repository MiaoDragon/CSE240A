//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h" // for including the global variables

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//
uint32_t gshare_ghistory;
uint32_t gshare_ghistoryTableSize;
int8_t gshare_ghistoryTable[8193];  // maximal 2^13

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
gshare_init_predictor()
{
    // initialize sizes for each table
    gshare_ghistoryTableSize = 1 << ghistoryBits;

    // initialize global history bits to be NOTTAKEN
    gshare_ghistory = NOTTAKEN;

    // initialize local historyto be NOTTAKEN
    for (uint32_t i=0; i<gshare_ghistoryTableSize; i++)
    {
        gshare_ghistoryTable[i] = WN;
    }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
gshare_make_prediction(uint32_t pc)
{

    uint32_t idx = (pc % gshare_ghistoryTableSize) ^ gshare_ghistory;
    //printf("prediction index %d\n", idx);
	int8_t pred = gshare_ghistoryTable[idx];
    //printf("prediction: %d\n", pred);

    //printf("after prediction...\n");

	if (pred < 2)
    {
        return NOTTAKEN;
    }
    else
    {
        return TAKEN;
    }
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
gshare_train_predictor(uint32_t pc, uint8_t outcome)
{
    uint32_t idx = (pc % gshare_ghistoryTableSize) ^ gshare_ghistory;

    // if the outcome is 1, then increment the prediction. Otherwise decrement the prediction
    gshare_ghistoryTable[idx] = gshare_ghistoryTable[idx] + (1)*(outcome==1) + (-1)*(outcome==0);
    if (gshare_ghistoryTable[idx] > ST)
    {
        gshare_ghistoryTable[idx] = ST;
    }
    if (gshare_ghistoryTable[idx] < SN)
    {
        gshare_ghistoryTable[idx] = SN;
    }
    //printf("train index %d\n", idx);
    //printf("updated prediction: %d\n", gshare_ghistoryTable[idx]);
    gshare_ghistory = (gshare_ghistory << 1) + outcome;
    gshare_ghistory = gshare_ghistory % gshare_ghistoryTableSize;
    //printf("after training...");
}
