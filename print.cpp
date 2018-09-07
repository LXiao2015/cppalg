#include <iostream>
#include "print.h"

using namespace std;

void printChoice() {
	for(int i = 0; i < NUM_OF_INPUT_CHAINS; ++i) {
		cout << "第 " << i+1 << " 条服务链：" << Input_Chains[i].service_type << " " << Input_Chains[i].ins << endl;
		cout << "demand：" << Input_Chains[i].demand << endl;
		cout << "NF：" << Input_Chains[i].node << endl;
		cout << "single COST: " << Input_Chains[i].fT << endl;
		for(int step = 0; step < MAX_PATH_LENGTH; ++step) {
			cout << Input_Chains[i].path[step] << " ";
		}
		cout << endl;
	}
	cout<<endl;
	for(int c = 0; c < NUM_OF_ALLOCATED_CHAINS; ++c) {
		if(c < 0) {
			break;
		}
		cout << "第 " << c+1 << " 条服务链：" << Allocated_Chains[c].service_type << " " << Allocated_Chains[c].ins << endl;
		cout << "demand：" << Allocated_Chains[c].demand << endl;
		cout << "NF：" << Allocated_Chains[c].node << endl;
		cout << "single COST: " << Allocated_Chains[c].fT << endl;
		for(int step = 0; step < MAX_PATH_LENGTH; ++step) {
			cout << Allocated_Chains[c].path[step] << " ";
		}
		cout << endl;
	}
}

void printBW() {
	for(int i = 0; i < NUM_OF_NODES; ++i) {
		for(int j = 0; j < NUM_OF_NODES; ++j) {
			cout<<BW[i][j]<<" ";
		}
		cout<<endl;
	}
}

void printRS() {
	for(int i = 0; i < NUM_OF_CLOUDS; ++i) {
		for(int j = 0; j < 2; ++j) {
			cout<<RS[i][j]<<" ";
		}
		cout<<endl;
	}
	cout << CAP << endl;
}

void printUsage() {
	cout << "node_used：" << endl;
	for(int j = 0; j < NUM_OF_NFNODES; ++j) {
		cout << node_used[j] << " ";
	}
	cout << endl << "node_vnf_demand：" << endl;
	for(int k = 0; k < NUM_OF_CLOUDS; ++k) {
		for(int j = 0; j < 3; ++j) {
			cout << node_vnf_demand[k][j] << " ";
		}
	}
	cout << endl << "node_vnf_count：" << endl;
	for(int k = 0; k < NUM_OF_CLOUDS; ++k) {
		for(int j = 0; j < 3; ++j) {
			cout << node_vnf_count[k][j] << " ";
		}
	}
	cout << endl;
}

void printCost() {
	double CR = 0.0, CI = 0.0, C = 0.0, cff = 0.0;
	for(int i = 0; i < NUM_OF_INPUT_CHAINS; ++i) {
		C += Input_Chains[i].fT;
		for(int j = 0; j < NUM_OF_FEATURES; ++j) {
			cff += (chain_types[Input_Chains[i].service_type][Input_Chains[i].ins][j] == true)? 0: feature_failure_cost[Input_Chains[i].service_type][j];
		}
	}

	for(int c = 0; c < NUM_OF_ALLOCATED_CHAINS; ++c) {
		C += Allocated_Chains[c].fT;
		for(int j = 0; j < NUM_OF_FEATURES; ++j) {
			cff += (chain_types[Allocated_Chains[c].service_type][Allocated_Chains[c].ins][j] == true)? 0: feature_failure_cost[Allocated_Chains[c].service_type][j];
		}
	}
	
//	CF = chain_failure_cost[Input_Chains[i].service_type];
	cout << cff << " + " << C - cff << " + ";
	
	for(int i = 0; i < NUM_OF_NFNODES; ++i) {
		CR += (node_used[i] > 0? 1: 0) * node_using_cost[i];
	}
	C += CR;
	cout << CR << " + ";
	for(int i = 0; i < NUM_OF_CLOUDS; ++i) {
		for(int j = 0; j < 3; ++j) {
			CI += node_vnf_count[i][j] * node_init_cost;
		}
	}
	cout << CI << " = ";
	C += CI;
	cout << C << endl;
} 

void printFeature() {
	for(int i = 0; i < NUM_OF_INPUT_CHAINS; ++i) {
		int ins = Input_Chains[i].ins, type = Input_Chains[i].service_type;
		for(int j = 0; j < NUM_OF_FEATURES; ++j) {
			cout<<chain_types[type][ins][j]<<" ";
		}
		cout<<endl;
	}
	for(int i = 0; i < NUM_OF_ALLOCATED_CHAINS; ++i) {
		int ins = Allocated_Chains[i].ins, type = Allocated_Chains[i].service_type;
		for(int j = 0; j < NUM_OF_FEATURES; ++j) {
			cout<<chain_types[type][ins][j]<<" ";
		}
		cout<<endl;
	}
}
