//========================================================//
//  predictor.h                                           //
//  Header file for the Branch Predictor                  //
//                                                        //
//  Includes function prototypes and global predictor     //
//  variables and defines                                 //
//========================================================//

#ifndef GSHARE_H
#define GSHARE_H

#include <stdint.h>
#include <stdlib.h>

//------------------------------------//
//      Global Predictor Defines      //
//------------------------------------//
#define NOTTAKEN  0
#define TAKEN     1

// Definitions for 2-bit counters
#define SN  0			// predict NT, strong not taken
#define WN  1			// predict NT, weak not taken
#define WT  2			// predict T, weak taken
#define ST  3			// predict T, strong taken


//------------------------------------//
//    Data Structure for Prediction   //
//------------------------------------//

extern int gshare_ghistoryBits; // Number of bits used for Global History

extern uint32_t gshare_ghistory;
uint32_t gshare_ghistoryTableSize;
extern int8_t gshare_ghistoryTable[];

//------------------------------------//
//    Predictor Function Prototypes   //
//------------------------------------//

// Initialize the predictor
//
void gshare_init_predictor(int ghistoryBits_in);

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t gshare_make_prediction(uint32_t pc);

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void gshare_train_predictor(uint32_t pc, uint8_t outcome);

#endif
