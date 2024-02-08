#include <stdio.h>
#define W_INDEX 0
#define H_INDEX 1
#define T_INDEX 2
#define P_INDEX 3
#define S_INDEX 4
#define A_INDEX 5
#define N_INDEX 6
int wall,numberoftribe;

int main() {
    int i=0;
    scanf("%d ",&wall);
    scanf("%d ",&numberoftribe);
    return helper(wall, numberoftribe);
}

int helper(wall, numberoftribe){
    int arr[2000000],i=0,j=0,k=0,w,h,t,p,pp,s,a,n;
    for(i=0;i<wall+1;i++){
        arr[i] = 1;
    }
    for(i=0;i<numberoftribe;i++){
        scanf("%d %d %d %d %d %d %d ",&w,&h,&t,&p,&s,&a,&n);
        for(j=0;j<n;j++){
            for(k=0,pp=p;k<w;k++,pp++){
            	if (pp >= 0 && pp <= wall){
                if (h > arr[pp]){
                    arr[pp] = h;
                }
            
        }}
        p += s*a;
    }}
    for(i=0;i<wall;i++){
        printf("%d ",arr[i]);
    }
    printf("%d\n",arr[wall]);
    return 0;
}