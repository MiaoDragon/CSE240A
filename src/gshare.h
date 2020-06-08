//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <string.h>
#include "predictor.h"

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

int ghistoryBits; // Number of bits used for Global History

#define NOTTAKEN  0
#define TAKEN     1

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//

uint8_t* ght;
int ghtSize;
uint32_t globalHistory;


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_gshare_predictor(int ghBits)
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //

  ghistoryBits = ghBits;
  ghtSize = 1 << ghistoryBits;
  ght = (uint8_t *)malloc(ghtSize*sizeof(uint8_t));
  memset(ght, 0, ghtSize*sizeof(uint8_t));
	globalHistory = 0;

}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_gshare_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

	uint32_t idx = (pc % ghtSize) ^ globalHistory;
	uint8_t pred = ght[idx];

	if (pred < 2) return TAKEN;
  else return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//

uint8_t inc_2_bit(uint8_t in) {
	if (in < 3) return in + 1;
	else return 3;
}

uint8_t dec_2_bit(uint8_t in) {
	if (in > 0) return in - 1;
	else return 0;
}

void
train_gshare_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //

	uint32_t idx = (pc % ghtSize) ^ globalHistory;
	if (outcome) ght[idx] = inc_2_bit(ght[idx]);
	else ght[idx] = dec_2_bit(ght[idx]);
}