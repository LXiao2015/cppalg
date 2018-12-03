#pragma once
#include <fstream>
#include <string>
#include "cost.h"
#include "update.h"

#include <set>

bool cmp(bool f_choice[], bool model[]);

int composite(int path[], bool p_choice[][5][NUM_OF_PATH], int src, int sink);

void allocated_chains(int session_num[], vector<vector<int>>& session_set);

void input_chains();

void read(int session_num[], vector<vector<int>>& session_set);
