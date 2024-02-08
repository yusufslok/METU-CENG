#include "the4.h"

// do not add extra libraries here
int divide_land(int X, int Y, bool** possible_plots) {
    std::vector<std::vector<int>> dp(X+1, std::vector<int>(Y+1, 0));
    
    for (int i=0; i<X+1; i++) {
        for (int j=0; j<Y+1; j++) {
            if (possible_plots[i][j]) {
                dp[i][j] = 0;
            } else if(!possible_plots[i][j]){
                dp[i][j] = i * j;
            }
        }
    }
    
    
    for (int i=1; i<X+1; i++) {
        for (int j=1; j<Y+1; j++) {
            for (int k=1; k<i; k++) {
                if (dp[i][j] > dp[k][j] + dp[i-k][j]) {
                    dp[i][j] = dp[k][j] + dp[i-k][j];
                }
            }
            for (int k=1; k<j; k++) {
                if (dp[i][j] > dp[i][k] + dp[i][j-k]) {
                    dp[i][j] = dp[i][k] + dp[i][j-k];
                }
            }
        }
    }

    return dp[X][Y];
}