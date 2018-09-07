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

	int phy = 0;    // �� n ����������
	for(int f : phy_feature_set) {

		if(chain_types[type][ins][f] == 1) {
			*nf_done = false;
//			cout << "nf_done: " << *nf_done << endl; 
//			cout << "physical feature: " << chain_types[type][Chains[i].ins][f] << endl;
			
//			Chains[i].update[ins].unode = service_nodes[phy][rand()%count_of_nfnode[phy]];    // �����ѡ NF �ڵ� 
			
			// ��ѡ����� NF �ڵ� 
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
		phy++;    // ����λ�� 
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

	if(sw_edge[src_sw - 28] == Chains[i].update[ins].unode) {    // ��ѡ���˱����ı�Ե�� 
		Chains[i].update[ins].upath[step++] = Chains[i].update[ins].unode;
//		cout << Chains[i].update[ins].upath[step-1] << " ";
		Chains[i].update[ins].upath[step++] = src_sw;
//		cout << Chains[i].update[ins].upath[step-1] << " ";
		*nf_done = true; 
	}
	
	int target_sw;    // ȷ����Ҫͨ��Ľ�����
	int t_RT = 40;    // ֱ�������Ƶ�·���� 
	if(*nf_done == false) {    // �����辭���ƶ�, ȷ������Ľ�������·���� 	
		target_sw = cloud_sw[Chains[i].update[ins].unode - 41];
		if(target_sw != -1) {    // node �Ǳ�Ե�� 
			t_RT = sw_RT[target_sw - 28];
		} 
	}
	else {    // �� NF���Ѿ��� NF 
		target_sw = sink_sw;
		t_RT = sw_RT[target_sw - 28];
	}

	// �� NF���Ѿ��� NF��δ���� NF ����Ҫ��һ��
	int s_RT = sw_RT[src_sw - 28]; 
	Chains[i].update[ins].upath[step++] = s_RT;
//	cout << Chains[i].update_path[step-1] << " ";
	if(s_RT != t_RT) {
//		int rd = rand()%5;
//		memcpy(Chains[i].update[ins].upath + step, RT_Paths[s_RT-37][t_RT-37][rd], 4*4);
		memcpy(Chains[i].update[ins].upath + step, RT_Paths[s_RT-37][t_RT-37][pathno], 4*4);    // ֱ�������·�� 
		
		// ����ѡ��������·��
//		while(t_RT != Chains[i].update[ins].upath[step-1]) {
//			
//		}
//		cout << "Router" << " ";
		
		for(; Chains[i].update[ins].upath[step] > 0; ++step) {
//			cout << Chains[i].update[ins].upath[step] << " ";
		}
	}
	
	
	if(target_sw == sink_sw && *nf_done == true) {  // �� NF���Ѿ��� NF ��·�������� 
		Chains[i].update[ins].upath[step++] = sink_sw;
//		cout << Chains[i].update_path[step-1] << " ";
		Chains[i].update[ins].upath[step] = Chains[i].sink;
//		cout << Chains[i].update_path[step] << " ";
	}
	
	else {
		// ��� NF ��һ�ε�·�� 
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
		
		if(target_sw == sink_sw) {    // NF �ڻ�۽ڵ���·�������� 

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
				memcpy(Chains[i].update[ins].upath + step, RT_Paths[s_RT-37][t_RT-37][pathno], 4*4);    // ֱ�������·�����ζ�·�� 

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
		
		bool nf_done = true;    // NF �Ƿ��Ѿ���(�����û�� NF) 
		
		int type = Input_Chains[i].service_type;    // ���ַ������е���һ��
//		int ins = rand()% (num_of_ins[type] - 1) + 1;    // ѡ��һ��ʵ�ַ�ʽ(�����Ƿ���������ʧ��)
		int ins = num_of_ins[type] - 1;    // ѡ����̰����ʵ�ַ�ʽ
//		Input_Chains[i].ins = ins;
//		cout << "[Input Chain] init - " << i << endl;
//		while(ins >= 0) {
//			if(ins > 0) {
				chooseLargestNode(i, &nf_done, Input_Chains, type, ins);   // ʧ�ܺ�ָ� 
				if(Input_Chains[i].update[ins].succ != false) {
					// ��ѡ·��
					int pathno = 0;
					chooseShortestPath(i, &nf_done, Input_Chains, ins, pathno);   // ʧ�ܺ�ָ�
//					while(Input_Chains[i].update[ins].succ == false) {
//						pathno++;
//						chooseShortestPath(i, &nf_done, Input_Chains, ins, pathno);
//					}
				}
//			} 
//			else {
//				Input_Chains[i].fT = singleCost(i, Input_Chains, ins);    // ���۳ɹ�ʧ�ܶ�Ҫ���㿪��
//				nf_done = true;
//			}
			if(Input_Chains[i].update[ins].succ != false) {
				// �۳���Դ�ʹ��� 
				
				updateTraffic(Input_Chains[i].path, Input_Chains[i].update[ins].upath, Input_Chains[i].demand, Input_Chains[i].phy, Input_Chains[i].update[ins].uphy);
				updateCapacity(Input_Chains, i, ins);
				memcpy(Input_Chains[i].path, Input_Chains[i].update[ins].upath, 4*MAX_PATH_LENGTH);
				
				Input_Chains[i].ins = ins;
				Input_Chains[i].phy = Input_Chains[i].update[ins].uphy;
				Input_Chains[i].node = Input_Chains[i].update[ins].unode;
				
				Input_Chains[i].fT = singleCost(i, Input_Chains, ins);    // ���۳ɹ�ʧ�ܶ�Ҫ���㿪��
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
	// �������·���������ѡһ�ֳ�ʼ���� 
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
