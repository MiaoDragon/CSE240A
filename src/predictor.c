//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"
#include "gshare.h"

//
// TODO:Student Information
//
const char *studentName = "NAME";
const char *studentID   = "PID";
const char *email       = "EMAIL";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//

// gshare
uint8_t* ght;
int ghtSize;
uint32_t globalHistory;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//

void
init_gshare_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //

  ghtSize = 1 << ghistoryBits;
  ght = (uint8_t *)malloc(ghtSize*sizeof(uint8_t));
  memset(ght, 0, ghtSize*sizeof(uint8_t));
	globalHistory = 0;

}

void update_global_history(uint8_t in) {
	globalHistory = globalHistory << 1;
  globalHistory = globalHistory + in%2;
	globalHistory = globalHistory%ghtSize;
}

void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //

  switch (bpType) {
    case STATIC:
      return;
    case GSHARE:
      init_gshare_predictor();
      return;
    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }
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

	if (pred < 2) return NOTTAKEN;
  else return TAKEN;
}

uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
      return make_gshare_prediction(pc);
    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
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
	update_global_history(outcome);
}

void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //
  switch (bpType) {
    case STATIC:
      return;
    case GSHARE:
      train_gshare_predictor(pc, outcome);
      return;
    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }
}
