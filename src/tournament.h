//========================================================//
//  predictor.h                                           //
//  Header file for the Branch Predictor                  //
//                                                        //
//  Includes function prototypes and global predictor     //
//  variables and defines                                 //
//========================================================//

#ifndef TOURNAMENT_H
#define TOURNAMENT_H

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

extern uint32_t tour_ghistory;
uint32_t tour_lhistoryTableSize;
extern uint32_t tour_lhistoryTable[];
uint32_t tour_lhistoryPredTableSize;
extern int8_t tour_lhistoryPredTable[];
uint32_t tour_ghistoryTableSize;
extern int8_t tour_ghistoryTable[];
uint32_t tour_chooserTableSize;
extern int8_t tour_chooserTable[];




//------------------------------------//
//    Predictor Function Prototypes   //
//------------------------------------//

// Initialize the predictor
//
void tournament_init_predictor();

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t tournament_make_prediction(uint32_t pc);

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void tournament_train_predictor(uint32_t pc, uint8_t outcome);

#endif
