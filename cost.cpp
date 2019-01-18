#include "cost.h"
#include <iostream>
using namespace std;

double rs_using_cost[2] = {0.2, 0.0001};

int getLength(int seq[], int maxLen) {    // get path length
	int l = 0;
	while(l < maxLen && seq[l] > 0) l++;
	return l;
}

void segment(int p[], int* i) {
	while(*i < 16 && p[*i] > 26 && p[*i] < 41) {
//		cout << "The " << *i << "th node is: " << p[(*i)] << endl;
		(*i)++;
	}
	(*i)++;
}

float singleCost(CFC& Chain, int ins) {    // cost per chain includes cff and cu
	double cff = 0.0, cu = 0.0;
//	CF = chain_failure_cost[Input_Chains[i].service_type];
	for(int j = 0; j < NUM_OF_FEATURES; ++j) {
//		cout << chain_types[Chains[i].service_type][ins][j] << " ";
		cff += (chain_types[Chain.service_type][ins][j] == true)? 0: feature_failure_cost[Chain.service_type][j];
//		cout << cff << endl;
	}
//	cout << endl;
	
	int count = 0, start1 = 0, end1 = 0, start2 = 0, end2 = 0;
	bool firstnotsame = 1;
	if(Chain.update[ins].upath[start1] == 0) count = 0;    // upath 无路径, 无更新开销
	else if(Chain.ini_path[start2] == 0) {
		int l1 = getLength(Chain.update[ins].upath, 16);
		count = l1 - 1;    // 原先没有路径, 更新节点是 upath 的长度减去 1（汇聚节点）
	}
	else {
		while(Chain.update[ins].upath[end1] != 0 && Chain.ini_path[end2] != 0) {
			segment(Chain.update[ins].upath, &end1);    // 截到服务节点或汇聚节点的下一个节点 index 
			segment(Chain.ini_path, &end2);
	//		cout << start1 << " " << end1 << " " << start2 << " " << end2 << endl;
	//		int l1 = getLength(test1, start1, end1);
	//		int l2 = getLength(test2, start2, end2);
			int samepath[16] = {0};
			int l1 = end1 - start1;
			int l2 = end2 - start2;
			int samelength = lcs(Chain.update[ins].upath + start1, l1, Chain.ini_path + start2, l2, samepath);
//			cout << "samelength：" << samelength << endl;
	
			int s = samelength - 1;
			for(int step = start1; step < end1; ++step) {
//				cout << test1[step] << " ";
				if(samepath[s] != Chain.update[ins].upath[step]) {    // 节点不相等, upath 到下一个节点, 并累加 count 
					if(firstnotsame) {
						firstnotsame = 0;
						count++;
					}
					count++;	
				}
				else {
					firstnotsame = 1;    // 路径重合时, firstnotsame 置 1 
					s--;                 // 节点相等, samepath 和 upath 都到下一个节点 
				} 
//				cout << count << endl;
			}	
	
//			cout << "count：" << count << endl; 
			start1 = end1;    // 跳过这个服务节点, 不然会一直循环 
			start2 = end2;
		}
	}
	cu = count * update_msg_cost;
	
	// cout << Chains[i].service_type << " " << ins << endl;
	// cout << "NF node: " << Chains[i].node << endl;	
	// cout << "cff: " << cff << "  cu: " << cu << endl;
	Chain.update[ins].cff = cff;
	Chain.update[ins].cu = cu;
	return cff + cu;
}

void totalCost() {
	T = 0.0;
	CF = 0.0;
	CU = 0.0;
	CH = 0.0;
	CI = 0.0;    // cost for hardware and instantiating

	// add (cff + cu)
	for (int i = 0; static_cast<unsigned long>(i) < Input_Chains.size(); ++i) {
		CF += Input_Chains[i].cff;
		// cout << Input_Chains[i].cu << endl;
		CU += Input_Chains[i].cu;
	}
	// cout << "Cost for Input Chains: " << CF + CU << endl;
	for (int c = 0; static_cast<unsigned long>(c) < Allocated_Chains.size(); ++c) {
		CF += Allocated_Chains[c].cff;
		// cout << Allocated_Chains[c].cu << endl;
		CU += Allocated_Chains[c].cu;
	}
	// cout << "CF and CU: " << CF << " " << CU << endl;
	// add ch
	
	for (int i = 0; i < NUM_OF_CLOUDS; ++i) {
		for (int j = 0; j <= 1; ++j) {    // resource type
			for (int k = 0; k <= 2; ++k) {    // VNF type
				// cout << node_vnf_count[i][k] << "*" << node_resource[k][j] << "*" << rs_using_cost[j] << endl;
				CH += node_vnf_count[i][k] * node_resource[k][j] * rs_using_cost[j];
			}
		}
	}
	// cout << "Cost for hardwares: " << CH << endl;

	// add ci
	for (int i = 0; i < NUM_OF_NFNODES; ++i) {    // node running cost
		CI += node_used[i] > 0? node_using_cost[i]: 0;
	}
	// cout << "Cost for running: " << CI << endl;

	T = CF + CU + CH + CI;
//	printUsage();
}

float newCost(vector<CFC>& Chains, int i, int ins) {
	int node = Chains[i].node, unode = Chains[i].update[ins].unode;
	int phy = Chains[i].phy, uphy = Chains[i].update[ins].uphy;
	if(node == unode && uphy == phy) {
		return T;
	}
	
	double newT = T;
	double demand = Chains[i].demand;
//	cout << "here " << newT << endl;
	// node using cost
	if(unode != node) {
		if(node_used[node - 41] == 1) {
			newT -= node_using_cost[node - 41];
		}
		if(node_used[unode - 41] == 0) {
			newT += node_using_cost[unode - 41];
		}
	}
//	cout << newT << endl;
	// vnf initiating cost
	
	if(node > 41) {
		newT -= node_vnf_count[node - 42][phy] * node_init_cost;
//		cout << newT << endl;
		newT += (int)((node_vnf_demand[node - 42][phy] - demand + unit_rps[phy] - 1)/unit_rps[phy]) * node_init_cost;
//		cout << newT << endl;
	}
	if(unode > 41) {
		newT -= node_vnf_count[unode - 42][uphy] * node_init_cost;	
//		cout << newT << endl;
		newT += (int)((node_vnf_demand[unode - 42][uphy] + demand + unit_rps[uphy] - 1)/unit_rps[uphy]) * node_init_cost;
//		cout << newT << " here" << endl;
	}
//	cout << newT << endl;
	return newT;
}

//int main() {
//	double cu = 0.0;
//
////	int test2[16] = {1, 28, 37, 40, 41, 40, 38, 31, 10, 0, 0, 0, 0, 0, 0, 0};     // ini_path
////	int test1[16] = {1, 28, 37, 39, 40, 41, 40, 38, 31, 10, 0, 0, 0, 0, 0, 0};    // upath
//	
////	int test2[16] = {9, 30, 37, 40, 45, 40, 38, 33, 18, 0, 0, 0, 0, 0, 0, 0};     // ini_path
////	int test1[16] = {9, 30, 42, 30, 37, 38, 33, 18, 0, 0, 0, 0, 0, 0, 0, 0};      // upath
//
////	int test2[16] = {0};     // ini_path
////	int test1[16] = {1, 28, 37, 39, 40, 41, 40, 38, 31, 10, 0, 0, 0, 0, 0, 0};    // upath
//
//	int test2[16] = {1, 28, 37, 40, 41, 40, 38, 31, 10, t0, 0, 0, 0, 0, 0, 0};     // ini_path
//	int test1[16] = {0};    // upath
//
//	int count = 0, start1 = 0, end1 = 0, start2 = 0, end2 = 0;
//	bool firstnotsame = 1;
//	if(test1[start1] == 0) count = 0;    // upath 无路径, 无更新开销
//	else if(test2[start2] == 0) {
//		int l1 = getLength(test1, 16);
//		count = l1 - 1;    // 原先没有路径, 更新节点是 upath 的长度减去 1（汇聚节点）
//	}
//	else {
//		while(test1[end1] != 0 && test2[end2] != 0) {
//			segment(test1, &end1);    // 截到服务节点或汇聚节点的下一个节点 index 
//			segment(test2, &end2);
//	//		cout << start1 << " " << end1 << " " << start2 << " " << end2 << endl;
//	//		int l1 = getLength(test1, start1, end1);
//	//		int l2 = getLength(test2, start2, end2);
//			int samepath[16] = {0};
//			int l1 = end1 - start1;
//			int l2 = end2 - start2;
//			int samelength = lcs(test1 + start1, l1, test2 + start2, l2, samepath);
//			cout << "samelength：" << samelength << endl;
//	
//			int s = samelength - 1, node;
//			for(int step = start1; step < end1; ++step) {
//				cout << test1[step] << " ";
//				if(samepath[s] != test1[step]) {    // 节点不相等, upath 到下一个节点, 并累加 count 
//					if(firstnotsame) {
//						firstnotsame = 0;
//						count++;
//					}
//					count++;	
//				}
//				else {
//					firstnotsame = 1;    // 路径重合时, firstnotsame 置 1 
//					s--;                 // 节点相等, samepath 和 upath 都到下一个节点 
//				} 
//				cout << count << endl;
//			}	
//	
//			cout << "count：" << count << endl; 
//			start1 = end1;    // 跳过这个服务节点, 不然会一直循环 
//			start2 = end2;
//		}
//	}
//	
//		
////	cout<<"需要发送更新消息的点："<<count<<endl;
//	cu = count * 0.2;
//	cout << "cu: " << cu << endl;
//	return 0;
//}

