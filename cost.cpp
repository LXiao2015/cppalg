#include "cost.h"
#include <iostream>
using namespace std;

int getLength(int seq[], int maxLen) {
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

float singleCost(int i, struct CFC Chains[], int ins) { 
	double cff = 0.0, cu = 0.0;
//	CF = chain_failure_cost[Input_Chains[i].service_type];
	for(int j = 0; j < NUM_OF_FEATURES; ++j) {
//		cout << chain_types[Chains[i].service_type][ins][j] << " ";
		cff += (chain_types[Chains[i].service_type][ins][j] == true)? 0: feature_failure_cost[Chains[i].service_type][j];
//		cout << cff << endl;
	}
//	cout << endl;
	
	int count = 0, start1 = 0, end1 = 0, start2 = 0, end2 = 0;
	bool firstnotsame = 1;
	if(Chains[i].update[ins].upath[start1] == 0) count = 0;    // upath ��·��, �޸��¿���
	else if(Chains[i].ini_path[start2] == 0) {
		int l1 = getLength(Chains[i].update[ins].upath, 16);
		count = l1 - 1;    // ԭ��û��·��, ���½ڵ��� upath �ĳ��ȼ�ȥ 1����۽ڵ㣩
	}
	else {
		while(Chains[i].update[ins].upath[end1] != 0 && Chains[i].ini_path[end2] != 0) {
			segment(Chains[i].update[ins].upath, &end1);    // �ص�����ڵ���۽ڵ����һ���ڵ� index 
			segment(Chains[i].ini_path, &end2);
	//		cout << start1 << " " << end1 << " " << start2 << " " << end2 << endl;
	//		int l1 = getLength(test1, start1, end1);
	//		int l2 = getLength(test2, start2, end2);
			int samepath[16] = {0};
			int l1 = end1 - start1;
			int l2 = end2 - start2;
			int samelength = lcs(Chains[i].update[ins].upath + start1, l1, Chains[i].ini_path + start2, l2, samepath);
//			cout << "samelength��" << samelength << endl;
	
			int s = samelength - 1;
			for(int step = start1; step < end1; ++step) {
//				cout << test1[step] << " ";
				if(samepath[s] != Chains[i].update[ins].upath[step]) {    // �ڵ㲻���, upath ����һ���ڵ�, ���ۼ� count 
					if(firstnotsame) {
						firstnotsame = 0;
						count++;
					}
					count++;	
				}
				else {
					firstnotsame = 1;    // ·���غ�ʱ, firstnotsame �� 1 
					s--;                 // �ڵ����, samepath �� upath ������һ���ڵ� 
				} 
//				cout << count << endl;
			}	
	
//			cout << "count��" << count << endl; 
			start1 = end1;    // �����������ڵ�, ��Ȼ��һֱѭ�� 
			start2 = end2;
		}
	}
	cu = count * update_msg_cost;
//	cout << "Changed node num: " << count << endl;
	
//	cout << Chains[i].service_type << " " << ins << endl;
//	cout << "NF: " << Chains[i].node << endl;	
//	cout << "cff: " << cff << "  cu: " << cu << endl;
	return cff + cu;
}

void totalCost() {
	double CR = 0.0, CI = 0.0;
	for(int i = 0; i < NUM_OF_INPUT_CHAINS; ++i) {
		T += Input_Chains[i].fT;
//		cout << T << endl;
	}

	for(int c = 0; c < NUM_OF_ALLOCATED_CHAINS; ++c) {
		T += Allocated_Chains[c].fT;
//		cout << T << endl;
	}

	for(int i = 0; i < NUM_OF_NFNODES; ++i) {
		CR += (node_used[i] > 0? 1: 0) * node_using_cost[i];
	}
	T += CR;
	
	for(int i = 0; i < NUM_OF_CLOUDS; ++i) {
		for(int j = 0; j < 3; ++j) {
			CI += node_vnf_count[i][j] * node_init_cost;
		}
	}
	T += CI;
	
//	printUsage();
}

float newCost(struct CFC Chains[], int i, int ins) {
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
//	if(test1[start1] == 0) count = 0;    // upath ��·��, �޸��¿���
//	else if(test2[start2] == 0) {
//		int l1 = getLength(test1, 16);
//		count = l1 - 1;    // ԭ��û��·��, ���½ڵ��� upath �ĳ��ȼ�ȥ 1����۽ڵ㣩
//	}
//	else {
//		while(test1[end1] != 0 && test2[end2] != 0) {
//			segment(test1, &end1);    // �ص�����ڵ���۽ڵ����һ���ڵ� index 
//			segment(test2, &end2);
//	//		cout << start1 << " " << end1 << " " << start2 << " " << end2 << endl;
//	//		int l1 = getLength(test1, start1, end1);
//	//		int l2 = getLength(test2, start2, end2);
//			int samepath[16] = {0};
//			int l1 = end1 - start1;
//			int l2 = end2 - start2;
//			int samelength = lcs(test1 + start1, l1, test2 + start2, l2, samepath);
//			cout << "samelength��" << samelength << endl;
//	
//			int s = samelength - 1, node;
//			for(int step = start1; step < end1; ++step) {
//				cout << test1[step] << " ";
//				if(samepath[s] != test1[step]) {    // �ڵ㲻���, upath ����һ���ڵ�, ���ۼ� count 
//					if(firstnotsame) {
//						firstnotsame = 0;
//						count++;
//					}
//					count++;	
//				}
//				else {
//					firstnotsame = 1;    // ·���غ�ʱ, firstnotsame �� 1 
//					s--;                 // �ڵ����, samepath �� upath ������һ���ڵ� 
//				} 
//				cout << count << endl;
//			}	
//	
//			cout << "count��" << count << endl; 
//			start1 = end1;    // �����������ڵ�, ��Ȼ��һֱѭ�� 
//			start2 = end2;
//		}
//	}
//	
//		
////	cout<<"��Ҫ���͸�����Ϣ�ĵ㣺"<<count<<endl;
//	cu = count * 0.2;
//	cout << "cu: " << cu << endl;
//	return 0;
//}

