#pragma once
#include "lcs.h"
#include "global.h"

int getLength(int seq[], int maxLen);

void segment(int p[], int* i);

float singleCost(CFC& Chain, int ins);

void totalCost();

float newCost(vector<CFC>& Chains, int i, int ins);

