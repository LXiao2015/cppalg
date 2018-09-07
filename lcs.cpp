#include <iostream>
#include "lcs.h"

using namespace std;

int lcs(int now[], int m, int former[], int n, int same[]) {  // 返回的 same 是逆序的路径 
	int dp[m + 1][n + 1];
	memset(dp, 0, sizeof(dp));
    char dir[m + 1][n + 1];
    
    for(int i = 1; i <= m; ++i){
        for(int j = 1; j <= n; ++j){
            if(now[i - 1] == former[j - 1]){
                dp[i][j] = dp[i - 1][j - 1] + 1;
                dir[i][j] = '#';
            }else if(dp[i - 1][j] >= dp[i][j - 1]){
                dp[i][j] = dp[i - 1][j];
                dir[i][j] = '|';
            }else{
                dp[i][j] = dp[i][j - 1];
                dir[i][j] = '-';
            }
        }
    }
//    
//    for(int i = 0; i <= m; ++i){
//        for(int j = 0; j <= n; ++j){
//            cout<<dp[i][j]<<" ";
//        }
//        cout<<endl;
//    }
//    
//    for(int i = 1; i <= m; ++i){
//        for(int j = 1; j <= n; ++j){
//            cout<<dir[i][j]<<" ";
//        }
//        cout<<endl;
//    }
    
    int i = m, j = n;
    int count = 0;
    while(i > 0 && j > 0) {
    	switch(dir[i][j]) {
    		case '#': same[count++] = now[i - 1]; i--; j--; break;
    		case '|': i--; break;
			case '-': j--; break;
			default: cout << "no" << endl;
		}		
	}

//    for(int i = 0; i < count; ++i) {
//    	cout << same[i] << " ";
//	}
//    cout << endl;
    return dp[m][n];
}

//int main() {
//	int a[6] = {43, 23, 3, 5, 6, 18};
//	int b[6] = {43, 23, 13, 15, 6, 10};
//	int s[6] = {0};
//	int samelength = lcs(a, 6, b, 6, s);
//	cout<<samelength<<endl;
//}
