//#include "global.h"
#include "read_chains.h" 
#include <ctime>
#include <cmath>
#include <time.h>
#include <iostream>
#include <algorithm>

using namespace std;

vector<int> realc;
int session_num[NUM_OF_NFNODES] = {0};
vector<vector<int>> session_set(NUM_OF_NFNODES, vector<int>(0));

void swap(int array[], int i, int j) {  
    int temp = array[i];  
    array[i] = array[j];  
    array[j] = temp;  
}

void sortMaxLeftMemCloud(int cloud[], int n) {
	for(int i = 0; i < n - 1; i++) {
        for(int j = i + 1; j < n; j++) {  
            if(RS[cloud[i]-42][1] < RS[cloud[j]-42][1])  
                swap(cloud, j, i);  
        }  
    }
//    for(int i = 0; i < n; i++) {  
//        cout << cloud[i] << " "; 
//    }
}

void chooseNode(int i, bool *nf_done, struct CFC Chains[], int type, int ins) {
	Chains[i].update[ins].succ = true;
//	cout << "The " << i+1 << "th chain: " << type << " " << Chains[i].ins << " " << ins << endl; 

	int phy = 0;    // 第 n 个物理特征
	for(int f : phy_feature_set) {

		if(chain_types[type][ins][f] == 1) {
			*nf_done = false;
//			cout << "nf_done: " << *nf_done << endl; 
//			cout << "physical feature: " << chain_types[type][Chains[i].ins][f] << endl;
			
			Chains[i].update[ins].unode = service_nodes[phy][rand()%count_of_nfnode[phy]];    // NF 节点序号 
			Chains[i].update[ins].uphy = phy;
//			cout << "new node: " << Chains[i].update[ins].unode << endl;
			if(!checkCapacity(Chains, i, ins)) {
//				cout << "New node is not usable!" << endl;
				Chains[i].update[ins].unode = Chains[i].node;
//				Chains[i].update[ins].uphy = Chains[i].phy;
				if(Chains[i].node <= 0 || !checkCapacity(Chains, i, ins)) {    // 原先也没有 NF 节点, 或用原先的节点也不够完成，这种实现方式失败 
//					cout << "Old node is not usable either!" << endl;
					*nf_done = true;
					Chains[i].update[ins].succ = false;
					Chains[i].update[ins].uphy = Chains[i].phy;
				}
				return;
			}
			
//			cout << "update node: " << Chains[i].update[ins].unode << endl;
			
			return;
		}
		phy++;    // 换了位置 
	}
}

void choosePath(int i, bool *nf_done, struct CFC Chains[], int ins) {
	
//	cout << *nf_done << " " << Chains[i].update[ins].unode << endl; 

	int step = 0; 
	int src_sw = (Chains[i].src-1)/3 + 28;
	int sink_sw = (Chains[i].sink-1)/3 + 28;
//	cout << src_sw << " " << sink_sw << endl;
	Chains[i].update[ins].upath[step++] = Chains[i].src;
//	cout << Chains[i].update_path[step-1] << " ";
	Chains[i].update[ins].upath[step++] = src_sw;
//	cout << Chains[i].update_path[step-1] << " ";

	if(sw_edge[src_sw - 28] == Chains[i].update[ins].unode) {    // 若选择了本网的边缘云 
		Chains[i].update[ins].upath[step++] = Chains[i].update[ins].unode;
//		cout << Chains[i].update[ins].upath[step-1] << " ";
		Chains[i].update[ins].upath[step++] = src_sw;
//		cout << Chains[i].update[ins].upath[step-1] << " ";
		*nf_done = true; 
	}
	
	int target_sw;    // 确定将要通向的交换机
	int t_RT = 40;    // 直连中心云的路由器 
	if(*nf_done == false) {    // 若仍需经过云端, 确定对面的交换机和路由器 	
		target_sw = cloud_sw[Chains[i].update[ins].unode - 41];
		if(target_sw != -1) {    // node 是边缘云 
			t_RT = sw_RT[target_sw - 28];
		} 
	}
	else {    // 无 NF、已经过 NF 
		target_sw = sink_sw;
		t_RT = sw_RT[target_sw - 28];
	}

	// 无 NF、已经过 NF、未经过 NF 都需要这一步
	int s_RT = sw_RT[src_sw - 28]; 
	Chains[i].update[ins].upath[step++] = s_RT;
//	cout << Chains[i].update_path[step-1] << " ";
	if(s_RT != t_RT) {
		int rd = rand()%5;
		memcpy(Chains[i].update[ins].upath + step, RT_Paths[s_RT-37][t_RT-37][rd], 4*4);

//		cout << "Router" << " ";
		
		for(; Chains[i].update[ins].upath[step] > 0; ++step) {
//			cout << Chains[i].update[ins].upath[step] << " ";
		}
	}
	
	
	if(target_sw == sink_sw && *nf_done == true) {  // 无 NF、已经过 NF 的路径添加完毕 
		Chains[i].update[ins].upath[step++] = sink_sw;
//		cout << Chains[i].update_path[step-1] << " ";
		Chains[i].update[ins].upath[step] = Chains[i].sink;
//		cout << Chains[i].update_path[step] << " ";
	}
	
	else {
		// 添加 NF 这一段的路径 
		if(target_sw > 0) {
			Chains[i].update[ins].upath[step++] = target_sw;
//			cout << Chains[i].update_path[step-1] << " ";
		}
		Chains[i].update[ins].upath[step++] = Chains[i].update[ins].unode;
//		cout << Chains[i].update_path[step-1] << " ";
		*nf_done = true;
		if(target_sw > 0) {
			Chains[i].update[ins].upath[step++] = target_sw;
//			cout << Chains[i].update_path[step-1] << " ";
		}
		
		if(target_sw == sink_sw) {    // NF 在汇聚节点侧的路径添加完毕 

//			cout << Chains[i].update_path[step-1] << " ";
			Chains[i].update[ins].upath[step] = Chains[i].sink;
//			cout << Chains[i].update_path[step] << " ";
		}
		
		else{
			s_RT = t_RT;
			t_RT = sw_RT[sink_sw - 28];
			Chains[i].update[ins].upath[step++] = s_RT;
//			cout << Chains[i].update_path[step-1] << " ";

			if(s_RT != t_RT) {
				int rd = rand()%5;
				memcpy(Chains[i].update[ins].upath + step, RT_Paths[s_RT-37][t_RT-37][rd], 4*4);

//				cout << "Router" << " ";
				for(; Chains[i].update[ins].upath[step] > 0; ++step) {
//					cout << Chains[i].update[ins].upath[step] << " ";
				}
			}
			
			Chains[i].update[ins].upath[step++] = sink_sw;
//			cout << Chains[i].update_path[step-1] << " ";
			Chains[i].update[ins].upath[step] = Chains[i].sink;
//			cout << Chains[i].update_path[step] << " ";
		}	
	} 
	
//	cout << Chains[i].update[ins].unode << endl;
//	for(int step = 0; step < MAX_PATH_LENGTH; ++step) {
//		cout << Chains[i].update[ins].upath[step] << " ";
//	}
//	cout << endl;
	if(!checkTraffic(Chains[i].path, Chains[i].update[ins].upath, Chains[i].demand, Chains[i].phy, Chains[i].update[ins].uphy)) {
		memcpy(Chains[i].update[ins].upath, Chains[i].path, MAX_PATH_LENGTH*4);
		Chains[i].update[ins].unode = Chains[i].node;

		Chains[i].update[ins].uphy = Chains[i].phy;
		Chains[i].update[ins].succ = false;
//		cout << "This path is not available!" << endl;
	}
	else {
		Chains[i].update[ins].succ &= true;
//		cout << "Path available" << endl;
	}
//	cout << Chains[i].update_node << endl;
//	for(int step = 0; step < MAX_PATH_LENGTH; ++step) {
//		cout << Chains[i].update_path[step] << " ";
//	}
//	cout << endl;
}

void chooseLargestNode(int i, bool *nf_done, struct CFC Chains[], int type, int ins) {
	Chains[i].update[ins].succ = true;
//	cout << "The " << i+1 << "th chain: " << type << " " << Chains[i].ins << " " << ins << endl; 

	int phy = 0;    // 第 n 个物理特征
	for(int f : phy_feature_set) {

		if(chain_types[type][ins][f] == 1) {
			*nf_done = false;
//			cout << "nf_done：" << *nf_done << endl; 
//			cout << "physical feature: " << chain_types[type][ins][f] << endl;
			
//			Chains[i].update[ins].unode = service_nodes[phy][rand()%count_of_nfnode[phy]];    // 随机挑选 NF 节点 
			
			// 挑选最近的 NF 节点 
			if(type < 2) {
				Chains[i].update[ins].unode = 41;
				if(!checkCapacity(Chains, i, ins)) {
//					cout << "Node 41 is not usable!" << endl;
					Chains[i].update[ins].unode = Chains[i].node;
				}
				else return;
			}
//			switch(Chains[i].src) {
//				case 7: case 8: case 9:  
//					Chains[i].update[ins].unode = 42; break;
//				case 16: case 17: case 18:
//					Chains[i].update[ins].unode = 43; break;
//				case 25: case 26: case 27:
//					Chains[i].update[ins].unode = 44; break;
//				default: Chains[i].update[ins].unode = 45; break;
//			} 
			int maxLeftMemCloud[4] = {42, 43, 44, 45};
			sortMaxLeftMemCloud(maxLeftMemCloud, 4);
			
			Chains[i].update[ins].uphy = phy;
//			cout << "new node: " << Chains[i].update[ins].unode<<endl;
			for(int node = 0; node < 4; node++) {
				Chains[i].update[ins].unode = maxLeftMemCloud[node];
				if(!checkCapacity(Chains, i, ins)) {
					Chains[i].update[ins].succ = false; 
//					cout << "Node is not usable!" << endl;
				}
					
				else {	
					Chains[i].update[ins].succ = true;
//					cout << "Node is usable!" << endl;
					return;
				}
			}
		}
		phy++;    // 换了位置 
	}
}

void init() {

	for(int i = 0; i < NUM_OF_INPUT_CHAINS; ++i) {	
		
		bool nf_done = true;    // NF 是否已经过(或根本没有 NF) 
		
		int type = Input_Chains[i].service_type;    // 五种服务链中的哪一种
		int ins = rand()%num_of_ins[type];    // 选择一种实现方式(考虑服务链分配失败)
//		Input_Chains[i].ins = ins;
//		cout << "[Input Chain] init - " << i << endl;
		if(ins != 0) {
			// 先随机选，失败则选一个剩余最大的 
			chooseNode(i, &nf_done, Input_Chains, type, ins);   // 失败后恢复 
			if(Input_Chains[i].update[ins].succ == false) {
				chooseLargestNode(i, &nf_done, Input_Chains, type, ins);   // 失败后恢复
			} 
			if(Input_Chains[i].update[ins].succ != false) {
				// 挑选路径
				choosePath(i, &nf_done, Input_Chains, ins);   // 失败后恢复
			}	
			if(Input_Chains[i].update[ins].succ != false) {
				// 扣除资源和带宽 
				updateTraffic(Input_Chains[i].path, Input_Chains[i].update[ins].upath, Input_Chains[i].demand, Input_Chains[i].phy, Input_Chains[i].update[ins].uphy);
				updateCapacity(Input_Chains, i, ins);
				memcpy(Input_Chains[i].path, Input_Chains[i].update[ins].upath, 4*MAX_PATH_LENGTH);
				
				Input_Chains[i].ins = ins;
				Input_Chains[i].phy = Input_Chains[i].update[ins].uphy;
				Input_Chains[i].node = Input_Chains[i].update[ins].unode;
				
			}
		}
 		if(Input_Chains[i].update[ins].succ != false) {
			Input_Chains[i].fT = singleCost(i, Input_Chains, ins);	
		}
		else {
			Input_Chains[i].fT = singleCost(i, Input_Chains, 0);
		}
	}
} 

void resetSession(struct CFC Chians[], int i) {
	Chians[i].service_type = -1;
	Chians[i].ins = -1;
	Chians[i].phy = -1;    // 第几个物理特征 
	Chians[i].node = -1;

	memset(Chians[i].ini_path, 0, MAX_PATH_LENGTH);
	memset(Chians[i].path, 0, MAX_PATH_LENGTH);

	Chians[i].fT = 0;
}

double calcLoad(int i) {
	double load = 0.0;
	for (int j = 0; j <= 1; ++j) {    // resource type
		int resourceLoad = 0;
		for (int k = 0; k <= 2; ++k) {    // VNF type
			resourceLoad += (node_vnf_demand[i][k] + 1) / node_resource[k][j] * node_resource[k][j];
		}
		load = max(load, resourceLoad/RS[i][j]);
	}
	return load;
}

void classifyByLoad() {
	double load[NUM_OF_NFNODES] = {0};
	// calculate load for PNF node
	for (int i = 0; i < session_num[0]; ++i) {
		load[0] += Allocated_Chains[session_set[0][i]].demand;
	}
	load[0] /= CAP;
	// calculate load for each server node
	for (int i = 1; i <= NUM_OF_CLOUDS; ++i) {
		// for (int j = 0; j <= 1; ++j) {    // resource type
			// int resourceLoad = 0;
			// for (int k = 0; k <= 2; ++k) {    // VNF type
				// resourceLoad += node_vnf_count[i][k] * node_resource[k][j];
			// }
			// load[i] = max(load[i], resourceLoad/RS[i][j]);
		// }
		load[i] = calcLoad(i - 1);
	}
	
	double avgLoad = (load[0]+load[1]+load[2]+load[3]+load[4])/5;
	
	// classify
	for (int i = 0; i < NUM_OF_NFNODES; ++i) {
		if (load[i] > avgLoad) {
			if (i == 0) {    // 是PNF
				// 从大到小排序, 删最后的
				sort(session_set[i].begin(), session_set[i].end(), greater<int>());
				int index = 0;
				while (load[0] > avgLoad) {
					double newLoad = 0.0;
					for (int i = 0; i < session_num[0] - 1; ++i) {
						newLoad += Allocated_Chains[session_set[0][i]].demand;
					}
					newLoad /= CAP;
					if (newLoad > avgLoad) {
						load[0] = newLoad;
						index = *(session_set[i].end() - 1);
						resetSession(Allocated_Chains, index);
						realc.push_back(index);    // 添加进再分配集合
						session_set[i].pop_back();    // 从节点上移除
						session_num[i]--;
					}
				}
			}
			else {    // 是VNF
				// 从小到大排序, 删最后的
				sort(session_set[i].begin(), session_set[i].end());
				int index = 0;
				while (load[i] > avgLoad) {
					// 减去这条流的新load
					double newLoad = 0.0;
					index = *(session_set[i].end() - 1);
					node_vnf_demand[i - 1][Allocated_Chains[index].phy] -= Allocated_Chains[index].demand;
					newLoad = calcLoad(i - 1);
					if (newLoad > avgLoad) {
						load[i] = newLoad;
						resetSession(Allocated_Chains, index);
						realc.push_back(index);    // 添加进再分配集合
						session_set[i].pop_back();    // 从节点上移除
						session_num[i]--;
						node_vnf_count[i - 1][Allocated_Chains[index].phy] = node_vnf_demand[i - 1][Allocated_Chains[index].phy] / unit_rps[Allocated_Chains[index].phy];
					}
					else {
						node_vnf_demand[i - 1][Allocated_Chains[index].phy] += Allocated_Chains[index].demand;
						break;
					}
				}
			}
		}
	}
	for (auto val : realc) {
		cout << val << endl;
	}
}

/*
void classify() {
//	cout << "classify" << endl;

	int lowC = 1000, highC = 0;
	for(int i = 0; i < NUM_OF_ALLOCATED_CHAINS; ++i) {
		lowC = lowC >= Allocated_Chains[i].fT? Allocated_Chains[i].fT: lowC;
		highC = highC <= Allocated_Chains[i].fT? Allocated_Chains[i].fT: highC;
	}
	int count = 0;
	for(int i = 0; i < NUM_OF_ALLOCATED_CHAINS; ++i) {
//		double Pc = pow(Allocated_Chains[i].cost/sumC, 1.0/(double)NUM_OF_ALLOCATED_CHAINS);
//		double Pc = Allocated_Chains[i].cost/sumC;
		double Pc = Allocated_Chains[i].fT;
		int pr = rand()%(highC - lowC) + lowC;

		if(pr - Pc < 0) {
			realc[count++] = i;
		}
//		realc[count++] = i;  // 全加进来试试 
	}
	num_of_realc = count;
	cout << "Allocated chains chosen:" << endl;
	for(int i = 0; realc[i] >= 0; ++i) {
		cout << realc[i] << " ";
	}
}
*/

void update(int i, struct CFC Chains[], int ins, float update_cost) {
//	printUsage();
//	cout << "Path changes: " << endl;
//	
//	for(int step = 0; step < MAX_PATH_LENGTH; ++step) {
//		cout<<Chains[i].path[step]<<" ";
//	}
//	cout << endl;
//	for(int step = 0; step < MAX_PATH_LENGTH; ++step) {
//		cout<<Chains[i].update[ins].upath[step]<<" ";
//	}
//	cout << endl;

//	cout << "node selection changes: " << Chains[i].node << " " << Chains[i].update[ins].unode << endl;
//	cout << "type: " << Chains[i].service_type << "  " << "ins changes: " << Chains[i].ins << " " << ins << endl; 
//	cout << "phy changes: " << Chains[i].phy << " " << Chains[i].update[ins].uphy << endl; 
	
//	cout << "traffic demand: " << Chains[i].demand << endl;
	updateTraffic(Chains[i].path, Chains[i].update[ins].upath, Chains[i].demand, Chains[i].phy, Chains[i].update[ins].uphy);
	updateCapacity(Chains, i, ins);
	
//	printUsage();
//	cout << "update - " << i << endl;
	Chains[i].node = Chains[i].update[ins].unode;
	memcpy(Chains[i].path, Chains[i].update[ins].upath, 4*MAX_PATH_LENGTH);
//	cout << "singlecost changes: " << Chains[i].fT << " " << Chains[i].update[ins].uT << endl;
	Chains[i].fT = Chains[i].update[ins].uT;
	Chains[i].ins = ins;
	Chains[i].phy = Chains[i].update[ins].uphy;
	
	// update T
	T = update_cost;
//	cout << T << endl;
//	printCost();
}

void action() { 
	for(int i = 0; i < NUM_OF_INPUT_CHAINS; ++i) {
		int type = Input_Chains[i].service_type;    // 五种服务链中的哪一种
//		cout << "i = " << i << endl;
		for(int ins = 0; ins < num_of_ins[type]; ++ins) {
			bool nf_done = true;
//			cout << "Choosing node..." << endl;
			chooseNode(i, &nf_done, Input_Chains, type, ins);
//			cout << "Done!" << endl;
//			cout << "Clearing path..." << endl;
			memset(Input_Chains[i].update[ins].upath, 0, MAX_PATH_LENGTH*4);
//			cout << "Done!" << endl; 

			if(Input_Chains[i].update[ins].succ != false) {
//				cout  <<  "Choosing path..."  <<  endl;
				choosePath(i, &nf_done, Input_Chains, ins);
//			    cout << "Done!" << endl;		
			}
			if(Input_Chains[i].update[ins].succ != false) {
//				cout << "Calculating input chain " << i << " 's " << ins << " implementation cost..." << endl;
				Input_Chains[i].update[ins].uT = singleCost(i, Input_Chains, ins);
			}
		}
	}
	
	for(int c: realc) {
		if(c < 0) {
			break;
		}
		int type = Allocated_Chains[c].service_type;    // 五种服务链中的哪一种
//		cout << "c = " << c << endl;
		for(int ins = 0; ins < num_of_ins[type]; ++ins) {
			bool nf_done = true;
//			cout << "Choosing node..." << endl;
			chooseNode(c, &nf_done, Allocated_Chains, type, ins);
//			cout << "Done!" << endl;
//			cout << "Clearing path..." << endl;
			memset(Allocated_Chains[c].update[ins].upath, 0, MAX_PATH_LENGTH*4);
//			cout << "Done!" << endl; 

			if(Allocated_Chains[c].update[ins].succ != false) {
//				cout << "Choosing path..." << endl;
				choosePath(c, &nf_done, Allocated_Chains, ins);
//			    cout << "Done!" << endl;
//				
			}
			if(Allocated_Chains[c].update[ins].succ != false) {
//				cout << "Calculating allocated chain " << c << " 's " << ins << " implementation cost..." << endl;
				Allocated_Chains[c].update[ins].uT = singleCost(c, Allocated_Chains, ins);
			}
		}	
	}

//	double max_perform = -1000.0;
	double min_perform = 1000.0;
	double Qf; 
	int update_chain = -1, update_ins = -1;
	float update_cost = 0.0;
	double b = 7.0, t = 3.0;

	for(int i = 0; i < NUM_OF_INPUT_CHAINS; ++i) {
		int type = Input_Chains[i].service_type;
		for(int ins = 0; ins < num_of_ins[type]; ++ins) {
			if(Input_Chains[i].update[ins].succ == false) {
//				cout << i << " do not participate int update procedure" << endl;
				continue;
			}
//			cout << newCost(Input_Chains, i, ins) << " + " << Input_Chains[i].update[ins].uT << " - " << Input_Chains[i].fT << endl;
			float new_cost = newCost(Input_Chains, i, ins) + Input_Chains[i].update[ins].uT - Input_Chains[i].fT;
//			cout << i << "!!!!!" << new_cost << " " << T << endl;
//			Qf = exp(t - b*(Input_Chains[i].cost - Input_Chains[i].update[ins].ucost))/num_of_ins[type];
			Qf = exp(t - b*(1/new_cost - 1/T));    // 这里的分母应该是可选的 ins 
//			Qf = T - new_cost;
//			Qf = exp(t - b*(1/T - 1/new_cost));
//			cout << Qf << endl;	
//			if(Input_Chains[i].update[ins].ucost != Input_Chains[i].cost && min_perform > Qf) {
			if(min_perform > Qf) {
	//			cout << "I - " << i << " 's cost reduced: " << " " << Input_Chains[i].cost << " " << Input_Chains[i].update_cost << endl;
				min_perform = Qf;
				update_chain = i;
				update_ins = ins; 
				update_cost = new_cost;
			}
		} 
	}
	for(int c: realc) {
		if(c < 0) {
			break;
		}
		int type = Allocated_Chains[c].service_type;    // 五种服务链中的哪一种
		
		for(int ins = 0; ins < num_of_ins[type]; ++ins) {
			if(Allocated_Chains[c].update[ins].succ == false) {
//				cout << c << " do not participate int update procedure" << endl;
				continue;
			}
//			cout << newCost(Input_Chains, c, ins) << " + " << Allocated_Chains[c].update[ins].uT << " - " << Allocated_Chains[c].fT << endl;
			float new_cost = newCost(Allocated_Chains, c, ins) + Allocated_Chains[c].update[ins].uT - Allocated_Chains[c].fT;
//			cout << c << "!!!!!" << new_cost << " " << T << endl;
//			Qf = exp(t - b*(Allocated_Chains[c].cost - Allocated_Chains[c].update[ins].ucost))/num_of_ins[type];
			Qf = exp(t - b*(1/new_cost - 1/T));    // 这里的分母应该是可选的 node 
//			Qf = T - new_cost;
//			Qf = exp(t - b*(1/T - 1/new_cost));
//			cout << Qf << endl;		
//			if(Allocated_Chains[c].update[ins].ucost != Allocated_Chains[c].cost && min_perform > Qf) {
			if(min_perform > Qf) {
	//			cout << "A - " << c << " 's cost reduced: " << " " << Allocated_Chains[c].cost << " " << Allocated_Chains[c].update_cost << endl;
				min_perform = Qf;
				update_chain = c + NUM_OF_INPUT_CHAINS;
				update_ins = ins;
				update_cost = new_cost;
			}
		}
	}
	
//	cout << "T before updating —— " << T << endl;
//	printUsage();
//	cout << endl;
	if(update_chain >= NUM_OF_INPUT_CHAINS) {
//		cout << "update A  " << update_chain - NUM_OF_INPUT_CHAINS << " " << update_ins << endl;
//		cout << Allocated_Chains[update_chain - NUM_OF_INPUT_CHAINS].node << " " << Allocated_Chains[update_chain - NUM_OF_INPUT_CHAINS].update[update_ins].unode << endl;
		update(update_chain - NUM_OF_INPUT_CHAINS, Allocated_Chains, update_ins, update_cost);
//		cout << "Done!" << endl;
	}
	else if(update_chain >= 0){
//		cout << "update I  " << update_chain << " " << update_ins << endl;
//		cout << Input_Chains[update_chain].node << " " << Input_Chains[update_chain].update[update_ins].unode << endl;
		update(update_chain, Input_Chains, update_ins, update_cost);
//		cout << "Done!" << endl;
	}
	else {
//		cout << "Didn't update this round." << T << endl;
		return;
	}
}

int main() {
//	cout << "Before read..." << endl;
	read(session_num, session_set);
//	cout << "Read over!" << endl;
	printRS();
//	printBW();
//	printUsage();
    time_t timer = time(NULL);
    time_t start = timer;
//  printRS();

	totalCost();
	cout << T << endl;
//	cout << "Cost is caculated!" << endl;

	// 选择参与此次调整的已分配服务链 
	classifyByLoad();

	
	// 将所有新服务链的先选一种初始配置 
	init();
//	cout << "Init over!" << endl;

//	printRS();
//	printChoice();
//	printUsage();

	// 策略更新 
	for(int times = 0; times < 300; ++times) {
//		printRS();
		if((time(NULL) - timer) >= 10) {
			printf("\nRuntime: %ld ms\n", time(NULL) - start);
			cout << "times: " << times << endl;
			cout << T << endl;
			timer = time(NULL);
		}
		action();
//		printUsage();
//		cout<<endl;
	}

	printChoice();
  
    printf("\nTotal runtime: %ld ms\n", time(NULL) - start);
//	printFeature();
	printUsage();
	printCost(); 
	printBW();
	printRS();
}
