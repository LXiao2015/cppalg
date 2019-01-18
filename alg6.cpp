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

void swap(int array[], int i, int j) {    // ����������
    int temp = array[i];  
    array[i] = array[j];  
    array[j] = temp;  
}

void sortMaxLeftMemCloud(int cloud[], int n) {    // ���ڴ�Ӵ�С����
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

void chooseNode(int i, bool *nf_done, vector<CFC>& Chains, int type, int ins) {
	Chains[i].update[ins].succ = true;
//	cout << "The " << i+1 << "th chain: " << type << " " << Chains[i].ins << " " << ins << endl; 

	int phy = 0;    // �� n ����������
	for(int f : phy_feature_set) {

		if(chain_types[type][ins][f] == 1) {
			*nf_done = false;
//			cout << "nf_done: " << *nf_done << endl; 
//			cout << "physical feature: " << chain_types[type][Chains[i].ins][f] << endl;
			
			Chains[i].update[ins].unode = service_nodes[phy][rand()%count_of_nfnode[phy]];    // NF �ڵ���� 
			Chains[i].update[ins].uphy = phy;
//			cout << "new node: " << Chains[i].update[ins].unode << endl;
			if(!checkCapacity(Chains, i, ins)) {
//				cout << "New node is not usable!" << endl;
				Chains[i].update[ins].unode = Chains[i].node;
//				Chains[i].update[ins].uphy = Chains[i].phy;
				if(Chains[i].node <= 0 || !checkCapacity(Chains, i, ins)) {    // ԭ��Ҳû�� NF �ڵ�, ����ԭ�ȵĽڵ�Ҳ������ɣ�����ʵ�ַ�ʽʧ�� 
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
		phy++;    // ����λ�� 
	}
}

void choosePath(int i, bool *nf_done, vector<CFC>& Chains, int ins) {
	
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
		int rd = rand()%5;
		memcpy(Chains[i].update[ins].upath + step, RT_Paths[s_RT-37][t_RT-37][rd], 4*4);

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

void chooseLargestNode(int i, bool *nf_done, vector<CFC>& Chains, int type, int ins) {
	Chains[i].update[ins].succ = true;
//	cout << "The " << i+1 << "th chain: " << type << " " << Chains[i].ins << " " << ins << endl; 

	int phy = 0;    // �� n ����������
	for(int f : phy_feature_set) {

		if(chain_types[type][ins][f] == 1) {
			*nf_done = false;
//			cout << "nf_done��" << *nf_done << endl; 
//			cout << "physical feature: " << chain_types[type][ins][f] << endl;
			
//			Chains[i].update[ins].unode = service_nodes[phy][rand()%count_of_nfnode[phy]];    // �����ѡ NF �ڵ� 
			
			// ��ѡ����� NF �ڵ� 
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
		phy++;    // ����λ�� 
	}
}

void initPerChain(vector<CFC>& Chains, int i, int offset) {

	bool nf_done = true;    // NF �Ƿ��Ѿ���(�����û�� NF) 

	int type = Chains[i].service_type;    // ���ַ������е���һ��

	int ins = rand()%num_of_ins[type];    // ѡ��һ��ʵ�ַ�ʽ(���Ƿ���������ʧ��)

	if(ins != 0) {
		// �����ѡ��ʧ����ѡһ��ʣ������ 
		chooseNode(i, &nf_done, Chains, type, ins);   // ʧ�ܺ�ָ� 
		if(Chains[i].update[ins].succ == false) {
			chooseLargestNode(i, &nf_done, Chains, type, ins);   // ʧ�ܺ�ָ�
		} 
		if(Chains[i].update[ins].succ != false) {
			// ��ѡ·��
			choosePath(i, &nf_done, Chains, ins);   // ʧ�ܺ�ָ�
		}	
		if(Chains[i].update[ins].succ != false) {
			// �۳���Դ�ʹ��� 
			// cout << "Try updating..." << endl;
			updateTraffic(Chains[i].path, Chains[i].update[ins].upath, Chains[i].demand, Chains[i].phy, Chains[i].update[ins].uphy);
			updateCapacity(Chains, i, ins);
			memcpy(Chains[i].path, Chains[i].update[ins].upath, 4*MAX_PATH_LENGTH);
			
			Chains[i].ins = ins;
			Chains[i].phy = Chains[i].update[ins].uphy;
			Chains[i].node = Chains[i].update[ins].unode;
			// cout << "Updating succeed..." << endl;
		}
	}

	if(Chains[i].update[ins].succ != false) {
		Chains[i].fT = singleCost(Chains[i], ins);
		session_set[Chains[i].node - 41].push_back(i + offset);
		session_num[Chains[i].node - 41]++;    // ָ����һ����λ
	}
	else {
		Chains[i].fT = singleCost(Chains[i], 0);
	}
}

void init() {
	cout << "Start initing process..." << endl;
	// new chains
	for (int i = 0; static_cast<unsigned long>(i) < Input_Chains.size(); ++i) {	
		initPerChain(Input_Chains, i, 45);
	}
	// old chains to be reallocate
	for (int i = 0; static_cast<unsigned long>(i) < realc.size(); ++i) {
		initPerChain(Allocated_Chains, realc[i], 0);
	}
	cout << "Initing process done." << endl;
} 


void update(int i, vector<CFC>& Chains, int ins, float update_cost) {
	// cout << "Updating..." << endl;
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
	memcpy(Chains[i].path, Chains[i].update[ins].upath, 4 * MAX_PATH_LENGTH);
//	cout << "singlecost changes: " << Chains[i].fT << " " << Chains[i].update[ins].uT << endl;
	Chains[i].fT = Chains[i].update[ins].uT;
	Chains[i].ins = ins;
	Chains[i].phy = Chains[i].update[ins].uphy;
	Chains[i].cu = Chains[i].update[ins].cu;
	Chains[i].cff = Chains[i].update[ins].cff;
	// update T
	totalCost();;
//	cout << T << endl;
//	printCost();
}

void resetSession(vector<CFC>& Chains, int i) {
	Chains[i].ins = 0;
	Chains[i].update[0].uphy = -1;    // �ڼ����������� 
	Chains[i].update[0].uphy = -1;
	Chains[i].update[0].unode = -1;

	Chains[i].fT = singleCost(Chains[i], 0);
	memset(Chains[i].update[0].upath, 0, MAX_PATH_LENGTH * 4);
	
	update(i, Chains, 0, T);
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

bool ASC(int a, int b)
{
	return Allocated_Chains[a].demand < Allocated_Chains[b].demand;
}

bool DESC(int a, int b)
{
	return Allocated_Chains[a].demand > Allocated_Chains[b].demand;
}

void classifyByLoad() {
	cout << "Start classifying the old chains..." << endl;
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
			if (i == 0) {    // ��PNF
				// �Ӵ�С����, ɾ����
				sort(session_set[i].begin(), session_set[i].end(), ASC);    // Ϊʲô������Ҫ����д����
				// cout << "The first one in PNF: " << session_set[i][0] << endl;
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
						realc.push_back(index);    // ��ӽ��ٷ��伯��
						session_set[i].pop_back();    // �ӽڵ����Ƴ�
						session_num[i]--;
					}
				}
			}
			else {    // ��VNF
				// ��С��������, ɾ����
				sort(session_set[i].begin(), session_set[i].end(), DESC);
				// cout << "The first one in VNF: " << session_set[i][0] << endl;
				int index = 0;
				while (load[i] > avgLoad) {
					// ��ȥ����������load
					double newLoad = 0.0;
					index = *(session_set[i].end() - 1);
					node_vnf_demand[i - 1][Allocated_Chains[index].phy] -= Allocated_Chains[index].demand;
					newLoad = calcLoad(i - 1);
					if (newLoad > avgLoad) {
						load[i] = newLoad;
						node_vnf_demand[i - 1][Allocated_Chains[index].phy] += Allocated_Chains[index].demand;
						resetSession(Allocated_Chains, index);
						realc.push_back(index);    // ��ӽ��ٷ��伯��
						session_set[i].pop_back();    // �ӽڵ����Ƴ�
						session_num[i]--;
					}
					else {
						node_vnf_demand[i - 1][Allocated_Chains[index].phy] += Allocated_Chains[index].demand;
						break;
					}
				}
			}
		}
	}
	cout << "The old chains to be reallocated: ";
	for (auto val : realc) {
		cout << val << " ";
	}
	cout << endl << "Classify process done." << endl;
}

void action() { 

	for(int i = 0; static_cast<unsigned long>(i) < Input_Chains.size(); ++i) {
		int type = Input_Chains[i].service_type;    // ���ַ������е���һ��
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
				Input_Chains[i].update[ins].uT = singleCost(Input_Chains[i], ins);
			}
		}
	}

	for(int c: realc) {
		if(c < 0) {
			break;
		}
		int type = Allocated_Chains[c].service_type;    // ���ַ������е���һ��
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
				Allocated_Chains[c].update[ins].uT = singleCost(Allocated_Chains[c], ins);
			}
		}	
	}

//	double max_perform = -1000.0;
	double min_perform = 1000.0;
	double Qf; 
	int update_chain = -1, update_ins = -1;
	float update_cost = 0.0;
	double b = 7.0, t = 3.0;

	for(int i = 0; static_cast<unsigned long>(i) < Input_Chains.size(); ++i) {
		int type = Input_Chains[i].service_type;
		for(int ins = 0; ins < num_of_ins[type]; ++ins) {
			if(Input_Chains[i].update[ins].succ == false) {
				// cout << i << " do not participate int update procedure" << endl;
				continue;
			}
//			cout << newCost(Input_Chains, i, ins) << " + " << Input_Chains[i].update[ins].uT << " - " << Input_Chains[i].fT << endl;
			float new_cost = newCost(Input_Chains, i, ins) + Input_Chains[i].update[ins].uT - Input_Chains[i].fT;
			// cout << i << "!!!!!" << new_cost << " " << T << endl;
//			Qf = exp(t - b*(Input_Chains[i].cost - Input_Chains[i].update[ins].ucost))/num_of_ins[type];
			Qf = exp(t - b*(1/new_cost - 1/T));    // ����ķ�ĸӦ���ǿ�ѡ�� ins 
//			Qf = T - new_cost;
//			Qf = exp(t - b*(1/T - 1/new_cost));
//			cout << Qf << endl;	
//			if(Input_Chains[i].update[ins].ucost != Input_Chains[i].cost && min_perform > Qf) {
			if(min_perform > Qf) {
				// cout << "I - " << i << " 's cost reduced: " << " " << Input_Chains[i].fT << " " << Input_Chains[i].update[ins].uT << endl;
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
		int type = Allocated_Chains[c].service_type;    // ���ַ������е���һ��
		
		for(int ins = 0; ins < num_of_ins[type]; ++ins) {
			if(Allocated_Chains[c].update[ins].succ == false) {
//				cout << c << " do not participate int update procedure" << endl;
				continue;
			}
//			cout << newCost(Input_Chains, c, ins) << " + " << Allocated_Chains[c].update[ins].uT << " - " << Allocated_Chains[c].fT << endl;
			float new_cost = newCost(Allocated_Chains, c, ins) + Allocated_Chains[c].update[ins].uT - Allocated_Chains[c].fT;
//			cout << c << "!!!!!" << new_cost << " " << T << endl;
//			Qf = exp(t - b*(Allocated_Chains[c].cost - Allocated_Chains[c].update[ins].ucost))/num_of_ins[type];
			Qf = exp(t - b*(1/new_cost - 1/T));    // ����ķ�ĸӦ���ǿ�ѡ�� node 
//			Qf = T - new_cost;
//			Qf = exp(t - b*(1/T - 1/new_cost));
//			cout << Qf << endl;		
//			if(Allocated_Chains[c].update[ins].ucost != Allocated_Chains[c].cost && min_perform > Qf) {
			if(min_perform > Qf) {
	//			cout << "A - " << c << " 's cost reduced: " << " " << Allocated_Chains[c].cost << " " << Allocated_Chains[c].update_cost << endl;
				min_perform = Qf;
				update_chain = c + Input_Chains.size();
				update_ins = ins;
				update_cost = new_cost;
			}
		}
	}

//	printUsage();

	if (update_chain < 0) {
//		cout << "Didn't update this round." << T << endl;
		return;
	}
	else if(static_cast<unsigned long>(update_chain) >= Input_Chains.size()) {
		update(update_chain - Input_Chains.size(), Allocated_Chains, update_ins, update_cost);

	}
	else if(update_chain >= 0){
		update(update_chain, Input_Chains, update_ins, update_cost);
    }
}

int main() {
	read(session_num, session_set);
	
    time_t timer = time(NULL);
    time_t start = timer;


	// ѡ�����˴ε������ѷ�������� 
	if (Allocated_Chains.size() > 3) {
		classifyByLoad();
	}
   
	// printChoice();
	// printUsage();
	printSession(session_num, session_set);
	// printRS();
	
	// �����д��������������ѡһ�ֳ�ʼ���� 
	init();
	
	totalCost();
	// printCost();

	// ���Ը��� 
	for(int times = 0; times < 300; ++times) {
//		printRS();
		// printCost();
		if((time(NULL) - timer) >= 5) {
			printf("\nRuntime: %ld ms\n", time(NULL) - start);
			cout << "times: " << times << endl;
			// printCost();
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
	// printBW();
	printRS();
	
	// �ϲ� Input_Chains �� Allocated_Chains, �����ǰ��
	Allocated_Chains.insert(Allocated_Chains.end(), Input_Chains.begin(), Input_Chains.end());
	for (auto c: Allocated_Chains) {
		printChainInfo(c);
	}
	storePolicy();
	
	return 0; 

}
