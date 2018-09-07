#include "global.h"
#include "read_chains.h" 
#include <ctime>
#include <cmath>
#include <time.h>
#include <iostream>

using namespace std;

void swap(int array[], int i, int j) {  
    int temp = array[i];  
    array[i] = array[j];  
    array[j] = temp;  
}

void sortMinCostCloud(int cloud[], int n) {
	for(int i = 0; i < n - 1; i++) {  
        for(int j = i + 1; j < n; j++) {  
            if(node_using_cost[cloud[i]-42] > node_using_cost[cloud[j]-42])  
                swap(cloud, j, i);  
        }  
    }
//    for(int i = 0; i < n; i++) {  
//        cout << cloud[i] << " "; 
//    }
}

void chooseLargestNode(int i, bool *nf_done, struct CFC Chains[], int type, int ins) {
	Chains[i].update[ins].succ = true;
	cout << "The " << i+1 << "th chain: " << type << " " << Chains[i].ins << " " << ins << endl; 

	int phy = 0;    // 第 n 个物理特征
	for(int f : phy_feature_set) {

		if(chain_types[type][ins][f] == 1) {
			*nf_done = false;
//			cout << "nf_done: " << *nf_done << endl; 
//			cout << "physical feature: " << chain_types[type][Chains[i].ins][f] << endl;
			
//			Chains[i].update[ins].unode = service_nodes[phy][rand()%count_of_nfnode[phy]];    // 随机挑选 NF 节点 
			
			// 挑选最近的 NF 节点 
			if(type < 2) {
				Chains[i].update[ins].unode = 41;
				if(!checkCapacity(Chains, i, ins)) {
					cout << "New node 41 is not usable!" << endl;
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
			int minCostCloud[4] = {42, 43, 44, 45};
			sortMinCostCloud(minCostCloud, 4);
			
			Chains[i].update[ins].uphy = phy;

			for(int node = 0; node < 4; node++) {
				Chains[i].update[ins].unode = minCostCloud[node];
				if(!checkCapacity(Chains, i, ins)) {
					Chains[i].update[ins].succ = false; 
					cout << "New node is not usable!" << endl;
				}
					
				else {	
					Chains[i].update[ins].succ = true;
					cout << "New node is usable!" << endl;
					return;
				}
			}
		}
		phy++;    // 换了位置 
	}
}

void chooseShortestPath(int i, bool *nf_done, struct CFC Chains[], int ins, int pathno) {
	
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
//		int rd = rand()%5;
//		memcpy(Chains[i].update[ins].upath + step, RT_Paths[s_RT-37][t_RT-37][rd], 4*4);
		memcpy(Chains[i].update[ins].upath + step, RT_Paths[s_RT-37][t_RT-37][pathno], 4*4);    // 直接用最短路径 
		
		// 总挑选带宽最大的路径
//		while(t_RT != Chains[i].update[ins].upath[step-1]) {
//			
//		}
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
//				int rd = rand()%5;
//				memcpy(Chains[i].update[ins].upath + step, RT_Paths[s_RT-37][t_RT-37][rd], 4*4);
				memcpy(Chains[i].update[ins].upath + step, RT_Paths[s_RT-37][t_RT-37][pathno], 4*4);    // 直接用最短路径、次短路径 

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
		cout << "This path is not available!" << endl;
	}
	else {
		Chains[i].update[ins].succ &= true;
		cout << "This path is available!" << endl;
	}
//	cout << Chains[i].update_node << endl;
//	for(int step = 0; step < MAX_PATH_LENGTH; ++step) {
//		cout << Chains[i].update_path[step] << " ";
//	}
//	cout << endl;
}

void init() {

	for(int i = 0; i < NUM_OF_INPUT_CHAINS; ++i) {	
		
		bool nf_done = true;    // NF 是否已经过(或根本没有 NF) 
		
		int type = Input_Chains[i].service_type;    // 五种服务链中的哪一种
//		int ins = rand()% (num_of_ins[type] - 1) + 1;    // 选择一种实现方式(不考虑服务链分配失败)
		int ins = num_of_ins[type] - 1;    // 选择最贪婪的实现方式
//		Input_Chains[i].ins = ins;
//		cout << "[Input Chain] init - " << i << endl;
//		while(ins >= 0) {
//			if(ins > 0) {
				chooseLargestNode(i, &nf_done, Input_Chains, type, ins);   // 失败后恢复 
				if(Input_Chains[i].update[ins].succ != false) {
					// 挑选路径
					int pathno = 0;
					chooseShortestPath(i, &nf_done, Input_Chains, ins, pathno);   // 失败后恢复
//					while(Input_Chains[i].update[ins].succ == false) {
//						pathno++;
//						chooseShortestPath(i, &nf_done, Input_Chains, ins, pathno);
//					}
				}
//			} 
//			else {
//				Input_Chains[i].fT = singleCost(i, Input_Chains, ins);    // 无论成功失败都要计算开销
//				nf_done = true;
//			}
			if(Input_Chains[i].update[ins].succ != false) {
				// 扣除资源和带宽 
				
				updateTraffic(Input_Chains[i].path, Input_Chains[i].update[ins].upath, Input_Chains[i].demand, Input_Chains[i].phy, Input_Chains[i].update[ins].uphy);
				updateCapacity(Input_Chains, i, ins);
				memcpy(Input_Chains[i].path, Input_Chains[i].update[ins].upath, 4*MAX_PATH_LENGTH);
				
				Input_Chains[i].ins = ins;
				Input_Chains[i].phy = Input_Chains[i].update[ins].uphy;
				Input_Chains[i].node = Input_Chains[i].update[ins].unode;
				
				Input_Chains[i].fT = singleCost(i, Input_Chains, ins);    // 无论成功失败都要计算开销
//				cout << "calculate COST: " << Input_Chains[i].fT;
//				break;	
			}
			else {
				Input_Chains[i].fT = singleCost(i, Input_Chains, 0);
//				ins--;
			}	
//		}	 
	}
} 


int main() {
	time_t start = time(NULL);
	
//	cout << "Before read..." << endl;
	read();
//	cout << "Read over!" << endl;
	printRS();
//	printBW();
	printUsage();

//    printRS();
	// 将所有新服务链的先选一种初始配置 
	init();
//	cout << "Init over!" << endl;

	totalCost();
	cout << T << endl;

	printChoice();
	time_t stop2 = time(NULL);  
    printf("\nRuntime: %lld ms\n", stop2 - start);
//	printFeature();
	printUsage();
	printCost(); 
	printBW();
	printRS();
}
