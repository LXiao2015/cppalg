#pragma once
#include "experiment.h"
#include <vector>

using namespace std;
using std::vector;

#define NUM_OF_CHAIN_TYPES 5

#define NUM_OF_FEATURES 6
#define NUM_OF_NODES 45
#define NUM_OF_NFNODES 5
#define NUM_OF_CLOUDS 4
#define NUM_OF_ROUTER 4
#define NUM_OF_NF 3


#define MAX_PATH_LENGTH 16

#define NUM_OF_PATH 94

// ��Ҫ��const����, ��Ȼerror: array bound is not an integer constant before ']' token
//extern const int NUM_OF_ROUTER;
//extern const int NUM_OF_NF;

/* ���壨��ģ�ͣ����ַ�������ͬʵ�ַ�ʽ��featureѡ�� */
// ������ f5, f6 ��λ��, �� CPLEX ����ͬ 
extern bool Firewall[3][NUM_OF_FEATURES];

extern bool StrictFirewall[3][NUM_OF_FEATURES];

extern bool BasicDPI[3][NUM_OF_FEATURES];

extern bool FullDPI[3][NUM_OF_FEATURES];

extern bool StrictFullDPI[3][NUM_OF_FEATURES];

extern bool chain_types[NUM_OF_CHAIN_TYPES][3][NUM_OF_FEATURES];

extern int tos[NUM_OF_CHAIN_TYPES];
extern int mtos[3];

extern int num_of_ins[NUM_OF_CHAIN_TYPES];


/* ����feature���ѡ�ڵ��Ӧ��ϵ */

enum phy_feature {
	f3 = 2, 
	f5 = 4,
	f6 = 5
}; 

extern int phy_feature_set[3];

extern int FW_nodes[NUM_OF_NFNODES];    // ����һ�� 41 �Ժ���˵ 
extern int IDP_nodes[NUM_OF_NFNODES];

extern int *service_nodes[NUM_OF_NF];

extern int count_of_nfnode[NUM_OF_NF];

struct Update {
	int uphy = -1;
	int unode = 0;
	int upath[MAX_PATH_LENGTH] = {0};
	bool succ = false;
	double uT;
	double cff;
	double cu;
};

struct CFC {
	int src;
	int sink;
	int service_type;
	int ins = 0;
	int phy = -1;    // �ڼ����������� 
	int node = 0;
//	int ini_node = 0;
	double demand;    // ��ʱ�̶�
	int ini_path[MAX_PATH_LENGTH] = {0};
	int path[MAX_PATH_LENGTH] = {0};    // ����� 14 ����
	
	struct Update update[3]; 
//	int update_ins = 0;
//	int update_phy = -1;
//	int update_node = 0;
//	int update_path[MAX_PATH_LENGTH] = {0};
	double fT;
	double cff;
	double cu;
}; 

extern vector<CFC> Input_Chains;
extern vector<CFC> Allocated_Chains;

// extern struct CFC Input_Chains[NUM_OF_INPUT_CHAINS];
// extern struct CFC Allocated_Chains[NUM_OF_ALLOCATED_CHAINS];

//extern int realc[NUM_OF_ALLOCATED_CHAINS];

// ������ֱ���ı�Ե��(0 ����û��ֱ����)(index = �ڵ�� - 28)
extern int sw_edge[9];

// ������ֱ���� RT(index = �ڵ�� - 28)
extern int sw_RT[9];

// NF ֱ���Ľ�����(-1 ����û��ֱ����)(index = �ڵ�� - 41)
extern int cloud_sw[NUM_OF_NFNODES];

// RT ֮�������(index = �ڵ�� - 37)(0 ����·������) 
extern int RT1_RT2[5][4];

extern int RT1_RT3[5][4];

extern int RT1_RT4[5][4];

extern int RT2_RT1[5][4];

extern int RT2_RT3[5][4];

extern int RT2_RT4[5][4];

extern int RT3_RT1[5][4];

extern int RT3_RT2[5][4];

extern int RT3_RT4[5][4];

extern int RT4_RT1[5][4];

extern int RT4_RT2[5][4];

extern int RT4_RT3[5][4];

extern int RT_Paths[NUM_OF_ROUTER][NUM_OF_ROUTER][5][4];

extern int node_used[NUM_OF_NFNODES];

extern double node_vnf_demand[NUM_OF_CLOUDS][3]; 
extern int node_vnf_count[NUM_OF_CLOUDS][3];

//int chain_failure_cost[NUM_OF_CHAIN_TYPES] = {1, 2, 4, 8, 16};

extern int feature_failure_cost[NUM_OF_CHAIN_TYPES][NUM_OF_FEATURES];

extern double update_msg_cost;

//extern int num_of_realc;

// ���˴���
extern double h_s;
extern double s_r;
extern double s_e;
extern double r_r;
extern double n_r;

extern double BW[NUM_OF_NODES][NUM_OF_NODES];

extern double RS[NUM_OF_CLOUDS][2];

extern double node_using_cost[NUM_OF_NFNODES];

extern double node_init_cost;
extern double unit_rps[NUM_OF_NF];

extern double node_resource[NUM_OF_NF][2];
extern double CAP;

//float resource_cost_of_node[NUM_OF_NFNODES] = {1, 2, 2, 2, 3};    // ������Ҫ��

extern double T;
extern double CF;
extern double CU;
extern double CH;
extern double CI;

extern double prop[NUM_OF_NF]; 

extern int from[NUM_OF_PATH], to[NUM_OF_PATH];


// extern int session_set[NUM_OF_NFNODES][80];

// ���� ʵ�ַ�ʽ ���Ӧ��prop
//0 0
//0 1  1
//0 2  0.9
//
//1 0
//1 1  0.9
//
//2 0
//2 1  0.8
//
//3 0
//3 1  0.8
//3 2  0.7
//
//4 0
//4 1  0.7 

extern vector<vector<string>> sfp;
extern string sname[5];
extern string nname[5];

//f3  (0,2) (1,1)
//f5  (2,1) (3,1)
//f6  (3,2) (4,1)
