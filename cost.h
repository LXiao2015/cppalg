#pragma once
#include "lcs.h"
#include "global.h"

int getLength(int seq[], int maxLen);

void segment(int p[], int* i);

float singleCost(int i, struct CFC Chains[], int ins);

void totalCost();

float newCost(struct CFC Chains[], int i, int ins);

