#pragma once
#include "print.h"

bool checkTraffic(int path[], int upath[], double demand, int phy, int uphy);

void updateTraffic(int path[], int upath[], double demand, int phy, int uphy);

bool checkCapacity(struct CFC Chains[], int i, int ins);

void updateCapacity(struct CFC Chains[], int i, int ins);
