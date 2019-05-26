using namespace std;
#include "experiment.h"
#include "global.h"

#define NUM_OF_CHAIN_TYPES 5

#define NUM_OF_FEATURES 6
#define NUM_OF_NODES 45
#define NUM_OF_NFNODES 5
#define NUM_OF_CLOUDS 4

//#define NUM_OF_FEATURES 8

#define MAX_PATH_LENGTH 16

#define NUM_OF_PATH 94

// 需要用const修饰, 不然error: array bound is not an integer constant before ']' token
// const int NUM_OF_ROUTER = 4;
// const int NUM_OF_NF = 3;

/* 定义（新模型）五种服务链不同实现方式的feature选择 */
// 交换了 f5, f6 的位置, 与 CPLEX 版相同 
bool Firewall[3][NUM_OF_FEATURES] = {
	{0,0,0,0,0,0},
	{1,1,0,0,0,0},
	{1,0,1,0,0,0}
};

bool StrictFirewall[3][NUM_OF_FEATURES] = {
	{0,0,0,0,0,0},
	{1,0,1,0,0,0},
	{0,0,0,0,0,0}    // 凑整
};

bool BasicDPI[3][NUM_OF_FEATURES] = {
	{0,0,0,0,0,0},
	{1,0,0,1,1,0},
	{0,0,0,0,0,0}    // 凑整
};

bool FullDPI[3][NUM_OF_FEATURES] = {
	{0,0,0,0,0,0},
	{1,0,0,1,1,0},
	{1,0,0,1,0,1}
};

bool StrictFullDPI[3][NUM_OF_FEATURES] = {
	{0,0,0,0,0,0},
	{1,0,0,1,0,1},
	{0,0,0,0,0,0}    // 凑整 
};

bool chain_types[NUM_OF_CHAIN_TYPES][3][NUM_OF_FEATURES] = {0};

int tos[NUM_OF_CHAIN_TYPES] = {16, 20, 28, 38, 40};

int num_of_ins[NUM_OF_CHAIN_TYPES] = {3, 2, 2, 3, 2};


/* 定义feature与可选节点对应关系 */

// enum phy_feature {
	// f3 = 2, 
	// f5 = 4,
	// f6 = 5
// }; 

int phy_feature_set[3] = {f3, f5, f6};

int FW_nodes[NUM_OF_NFNODES] = {41, 42, 43, 44, 45};    // 少了一个 41 以后再说 
int IDP_nodes[NUM_OF_NFNODES] = {42, 43, 44, 45, -1};

int *service_nodes[NUM_OF_NF] = {FW_nodes, IDP_nodes, IDP_nodes};

int count_of_nfnode[NUM_OF_NF] = {5, 4, 4};

vector<CFC> Input_Chains;
vector<CFC> Allocated_Chains;

//int realc[NUM_OF_ALLOCATED_CHAINS] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

// 交换机直连的边缘云(0 代表没有直连的)(index = 节点号 - 28)
int sw_edge[9] = {-1, -1, 42, -1, -1, 43, -1, -1, 44};

// 交换机直连的 RT(index = 节点号 - 28)
int sw_RT[9] = {37, 37, 37, 38, 38, 38, 39, 39, 39};

// NF 直连的交换机(-1 代表没有直连的)(index = 节点号 - 41)
int cloud_sw[NUM_OF_NFNODES] = {-1, 30, 33, 36, -1};

// RT 之间的拓扑(index = 节点号 - 37)(0 代表路径结束) 
int RT1_RT2[5][4] = {
	{38, -1, -1, -1},
	{39, 38, -1, -1},
	{40, 38, -1, -1},
	{39, 40, 38, -1},
	{40, 39, 38, -1}
};

int RT1_RT3[5][4] = {
	{39, -1, -1, -1},
	{38, 39, -1, -1},
	{40, 39, -1, -1},
	{38, 40, 39, -1},
	{40, 38, 39, -1}
};

int RT1_RT4[5][4] = {
	{40, -1, -1, -1},
	{38, 40, -1, -1},
	{39, 40, -1, -1},
	{38, 39, 40, -1},
	{39, 38, 40, -1}
};

int RT2_RT1[5][4] = {
	{37, -1, -1, -1},
	{39, 37, -1, -1},
	{40, 37, -1, -1},
	{39, 40, 37, -1},
	{40, 39, 37, -1}
};

int RT2_RT3[5][4] = {
	{39, -1, -1, -1},
	{37, 39, -1, -1},
	{40, 39, -1, -1},
	{37, 40, 39, -1},
	{40, 37, 39, -1}
};

int RT2_RT4[5][4] = {
	{40, -1, -1, -1},
	{39, 40, -1, -1},
	{37, 40, -1, -1},
	{39, 37, 40, -1},
	{37, 39, 40, -1}
};

int RT3_RT1[5][4] = {
	{37, -1, -1, -1},
	{38, 37, -1, -1},
	{40, 37, -1, -1},
	{38, 40, 37, -1},
	{40, 38, 37, -1}
};

int RT3_RT2[5][4] = {
	{38, -1, -1, -1},
	{37, 38, -1, -1},
	{40, 38, -1, -1},
	{37, 40, 38, -1},
	{40, 37, 38, -1}
};

int RT3_RT4[5][4] = {
	{40, -1, -1, -1},
	{37, 40, -1, -1},
	{38, 40, -1, -1},
	{37, 38, 40, -1},
	{38, 37, 40, -1}
};

int RT4_RT1[5][4] = {
	{37, -1, -1, -1},
	{38, 37, -1, -1},
	{39, 37, -1, -1},
	{38, 39, 37, -1},
	{39, 38, 37, -1}
};

int RT4_RT2[5][4] = {
	{38, -1, -1, -1},
	{37, 38, -1, -1},
	{39, 38, -1, -1},
	{37, 39, 38, -1},
	{39, 37, 38, -1}
};

int RT4_RT3[5][4] = {
	{39, -1, -1, -1},
	{37, 39, -1, -1},
	{38, 39, -1, -1},
	{37, 38, 39, -1},
	{38, 37, 39, -1}
};

int RT_Paths[NUM_OF_ROUTER][NUM_OF_ROUTER][5][4];

int node_used[NUM_OF_NFNODES] = {0};

double node_vnf_demand[NUM_OF_CLOUDS][3]; 
int node_vnf_count[NUM_OF_CLOUDS][3];

//int chain_failure_cost[NUM_OF_CHAIN_TYPES] = {1, 2, 4, 8, 16};

int feature_failure_cost[NUM_OF_CHAIN_TYPES][NUM_OF_FEATURES] = {
	{8, 4, 8, 0, 0, 0},
	{8, 0, 8, 0, 0, 0},
	{12, 0, 0, 0, 12, 0},
	{12, 0, 0, 0, 6, 12},
	{12, 0, 0, 0, 0, 12}
};

double update_msg_cost = 0.2;

int num_of_realc = 0;

// 拓扑带宽
double h_s = 2500.0;
double s_r = 2500.0;
double s_e = 2500.0;
double r_r = 2500.0;
double n_r = 5000.0;

double BW[NUM_OF_NODES][NUM_OF_NODES] = {
	/*
	 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45
	*/
	
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	
	/*
	 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45
	*/
	
	{h_s, h_s, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, s_r, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, h_s, h_s, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, s_r, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, h_s, h_s, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, s_r, 0, 0, 0, 0, s_e, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, h_s, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, s_r, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, h_s, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, s_r, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, h_s, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, s_r, 0, 0, 0, 0, s_e, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, h_s, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, s_r, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, h_s, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, s_r, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, h_s, h_s, h_s, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, s_r, 0, 0, 0, 0, s_e, 0},
	
	/*
	 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45
	*/
	
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, s_r, s_r, s_r, 0, 0, 0, 0, 0, 0, 0, r_r, r_r, r_r, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, s_r, s_r, s_r, 0, 0, 0, r_r, 0, r_r, r_r, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, s_r, s_r, s_r, r_r, r_r, 0, r_r, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, r_r, r_r, r_r, 0, n_r, 0, 0, 0, n_r},
	
	/*
	 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45
	*/	
	
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, n_r, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, s_e, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, s_e, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, s_e, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, n_r, 0, 0, 0, 0, 0}
};

double RS[NUM_OF_CLOUDS][2] = {
	{20.0, 3000.0},
	{20.0, 3000.0},
	{20.0, 3000.0},
	{80.0, 12000.0}
};

double node_using_cost[NUM_OF_NFNODES] = {0.1, 0.2, 0.2, 0.2, 0.1};
double node_init_cost = 0.4;
double unit_rps[NUM_OF_NF] = {1000.0, 300.0, 100.0};

double node_resource[NUM_OF_NF][2] = {
	{1.0, 100.0},
	{1.0, 200.0},
	{1.0, 500.0}
};
double CAP = 15000;

//float resource_cost_of_node[NUM_OF_NFNODES] = {1, 2, 2, 2, 3};    // 这里需要改

double T = 0.0;
double CF = 0.0;
double CU = 0.0;
double CH = 0.0;
double CI = 0.0;

double prop[NUM_OF_NF] = {0.9, 0.8, 0.7}; 

int from[NUM_OF_PATH] = {0}, to[NUM_OF_PATH] = {0};

vector<vector<string>> sfp(5, vector<string>(3));

string sname[5] = {"Firewall", "StrictFirewall", "BasicDPI", "FullDPI", "StrictFullDPI"};

string nname[5] = {"nf_name_41", "nf_name_42", "nf_name_43", "nf_name_44", "nf_name_45"};

// 类型 实现方式 与对应的prop
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

//f3  (0,2) (1,1)
//f5  (2,1) (3,1)
//f6  (3,2) (4,1)
