//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h" // for including the global variables
#include "custom.h"
#include "gshare.h"
#include "tournament.h"
//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

int ghistoryBits = 9; // Number of bits used for Global History
int lhistoryBits = 10; // Number of bits used for Local History
int pcIndexBits = 10;  // Number of bits used for PC index

uint32_t custom_chooserTableSize;
int8_t custom_chooserTable[8193];  // maximal 2^13   // 0 for selecting gshare, 1 for selecting tournament
                             // initial value: WS the tournament
uint32_t custom_ghistory;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
custom_init_predictor()
{
    gshare_init_predictor(13);
    tournament_init_predictor(9, 10, 10);
    custom_ghistory = NOTTAKEN;
    custom_chooserTableSize = 1 << ghistoryBits;
    // initialize the chooser table to be WT the tournament method
    for (uint32_t i=0; i<custom_chooserTableSize; i++)
    {
        custom_chooserTable[i] = WT;
    }

}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
custom_make_prediction(uint32_t pc)
{
    //*** chooser
    // obtain the gshare masked index
    uint32_t custom_global_idx = (pc % custom_chooserTableSize) ^ custom_ghistory;
    // use the global history to index in the chooserTable
    int8_t chooser = custom_chooserTable[custom_global_idx];
    // chooser N/: select local/global
    if (chooser == SN || chooser == WN)
    {
        // select gshare
        return gshare_make_prediction(pc);
    }
    else
    {
        return tournament_make_prediction(pc);
    }
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
custom_train_predictor(uint32_t pc, uint8_t outcome)
{
    //*** train gshare
    int8_t gshareRes = gshare_make_prediction(pc);
    gshare_train_predictor(pc, outcome);
    //*** train tournament
    int8_t tournamentRes = tournament_make_prediction(pc);
    tournament_train_predictor(pc, outcome);
    //*** train chooser
    // obtain the gshare masked index
    uint32_t custom_global_idx = (pc % custom_chooserTableSize) ^ custom_ghistory;
    // use global history bits to index in the ghistoryTable for finding the current state
    int8_t chooser = custom_chooserTable[custom_global_idx];
    // if the predictions are different then update to the correct one
    custom_chooserTable[custom_global_idx] = chooser + (-1)*(gshareRes==outcome) + (1)*(tournamentRes==outcome);
    // handle boundary case
    if (custom_chooserTable[custom_global_idx] > ST)
    {
        custom_chooserTable[custom_global_idx] = ST;
    }
    if (custom_chooserTable[custom_global_idx] < SN)
    {
        custom_chooserTable[custom_global_idx] = SN;
    }
    // update global history
    custom_ghistory = (custom_ghistory << 1) + outcome;
    // mask global history to be inside bits of ghistoryBits
    custom_ghistory = custom_ghistory % custom_chooserTableSize;
}
