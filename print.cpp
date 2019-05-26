#include <iostream>
#include "print.h"
#include <fstream>
#include <iomanip>
#include <cstring>
using namespace std;

void storePolicy() {
	ofstream Outfile;   //声明一个 ofstream 对象
    Outfile.open("/home/ubuntu/cppalg/output/policy.log");
	for(CFC c: Allocated_Chains) {
        Outfile << c.src << " " << c.sink << " " << c.service_type << " " << c.demand << "  ";
		Outfile << c.ins << " " << c.phy << " " << c.node << "  ";
		for(int step = 0; step < MAX_PATH_LENGTH; ++step) {
			if (c.path[step] > 0) {
				Outfile << c.path[step] << " ";
			}
		}
		Outfile << endl;
	}
}

void printChoice() {
	// cout << "Print detailed choices: " << endl;
    ofstream Outfile;   //声明一个 ofstream 对象
    Outfile.open("/home/ubuntu/cppalg/output/demandAndPath.txt");  //将OF与“study.txt”文件关联起来
	for(CFC c: Input_Chains) {
		// cout << "[ The " << i+1 << " th new chain: ] " << Input_Chains[i].service_type << " " << Input_Chains[i].ins << endl;
		// cout << "demand: " << Input_Chains[i].demand << endl;
		// cout << "NF: " << Input_Chains[i].node << endl;
		// cout << "single COST: " << Input_Chains[i].fT << endl;

        Outfile << tos[c.service_type] << " ";
		for(int step = 0; step < MAX_PATH_LENGTH; ++step) {
			// cout << Input_Chains[i].path[step] << " ";
			if (c.path[step] > 0) {
				Outfile << c.path[step] << " ";
			}
		}
		Outfile << endl;
		// cout << endl;
	}
	cout<<endl;
	for(CFC c: Allocated_Chains) {
		// cout << "[ The " << c+1 << " th existed chain: ] " << Allocated_Chains[c].service_type << " " << Allocated_Chains[c].ins << endl;
		// cout << "demand: " << Allocated_Chains[c].demand << endl;
		// cout << "NF node: " << Allocated_Chains[c].node << endl;
		// cout << "single COST: " << Allocated_Chains[c].fT << endl;

        Outfile << c.demand << " ";
		for(int step = 0; step < MAX_PATH_LENGTH; ++step) {
			// cout << Allocated_Chains[c].path[step] << " ";
			if (c.path[step] > 0) {
				Outfile << c.path[step] << " ";
			}
		}
		Outfile << endl;
		// cout << endl;
	}
}

void printBW() {
	cout << "Print Bandwidth left: " << endl;
	for(int i = 0; i < NUM_OF_NODES; ++i) {
		for(int j = 0; j < NUM_OF_NODES; ++j) {
			cout<<BW[i][j]<<" ";
		}
		cout<<endl;
	}
}

void printRS() {
	cout << "Print the left resources: " << endl;
	for(int i = 0; i < NUM_OF_CLOUDS; ++i) {
		for(int j = 0; j < 2; ++j) {
			cout<<RS[i][j]<<" ";
		}
		cout<<endl;
	}
	cout << CAP << endl;
}

void printUsage() {
	cout << "node_used: " << endl;
	for(int j = 0; j < NUM_OF_NFNODES; ++j) {
		cout << node_used[j] << " ";
	}
	cout << endl << "node_vnf_demand: " << endl;
	for(int k = 0; k < NUM_OF_CLOUDS; ++k) {
		for(int j = 0; j < 3; ++j) {
			cout << node_vnf_demand[k][j] << " ";
		}
	}
	cout << endl << "node_vnf_count: " << endl;
	for(int k = 0; k < NUM_OF_CLOUDS; ++k) {
		for(int j = 0; j < 3; ++j) {
			cout << node_vnf_count[k][j] << " ";
		}
	}
	cout << endl;
}

void printSession(int session_num[], vector<vector<int>>& session_set) {
	cout << "Print sessions of each nfnode: " << endl;
	for (int i = 0; i < NUM_OF_NFNODES; ++i) {
		cout << "Node " << i + 41 << " carries " << session_num[i] << " sessions: ";
		for (int j = 0; j < session_num[i]; ++j) {
			cout << " " << session_set[i][j];
		}
		cout << endl;
	}
}

void printCost() {
	cout << "CF(" << CF << ") + CU(" << CU << ") + CH(" << CH << ") + CI(" << CI << ") = " << T << endl;
} 

void printFeature() {
	for(int i = 0; static_cast<unsigned long>(i) < Input_Chains.size(); ++i) {
		int ins = Input_Chains[i].ins, type = Input_Chains[i].service_type;
		for(int j = 0; j < NUM_OF_FEATURES; ++j) {
			cout<<chain_types[type][ins][j]<<" ";
		}
		cout<<endl;
	}
	for(int i = 0; static_cast<unsigned long>(i) < Allocated_Chains.size(); ++i) {
		int ins = Allocated_Chains[i].ins, type = Allocated_Chains[i].service_type;
		for(int j = 0; j < NUM_OF_FEATURES; ++j) {
			cout<<chain_types[type][ins][j]<<" ";
		}
		cout<<endl;
	}
}

void printChainInfo(CFC& chain) {
	cout << "------+------+------+------+------+------+------+------+------" << endl;
        char basic[48] = "host ";
        strcat(basic, to_string(chain.src).c_str());
        char t1[] = " -> host ";
        strcat(basic, t1);
        strcat(basic, to_string(chain.sink).c_str());
        char t2[] = "  ";
        strcat(basic, t2);
        strcat(basic, to_string(int(chain.demand)).c_str());
        char t3[] = " MB  ";
        strcat(basic, t3);
        strcat(basic, sname[chain.service_type].c_str());
        cout << setw(48) << setiosflags(ios::left) << basic;
	cout << sfp[chain.service_type][chain.ins] << "\t" << nname[chain.node - 41] << endl;
	cout << "Former  path: ";
	for(int step = 0; step < MAX_PATH_LENGTH; ++step) {
		// cout << Allocated_Chains[c].path[step] << " ";
		if (chain.ini_path[step] > 0) {
			cout << chain.ini_path[step] << " ";
		}
	}
	cout << "\t";
	cout << "Current path: ";
	for(int step = 0; step < MAX_PATH_LENGTH; ++step) {
		// cout << Allocated_Chains[c].path[step] << " ";
		if (chain.path[step] > 0) {
			cout << chain.path[step] << " ";
		}
	}
	cout << endl;
}
