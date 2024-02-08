#include "the2.h"

// do not add extra libraries here
/*
    arr       : array to be sorted, in order to get points this array should contain be in sorted state before returning
    ascending : true for ascending, false for descending 
    size      : number of elements in the array
*/

void counting_sort(std::string* A , int n, int k, int index, int& number_iter){
    int* C = new int[k+1+1];
    std::string* B = new std::string[n+1];
    for(int i=0;i<k+1+1;i++){
        C[i] = 0;
    }
    for(int j=1; j<n+1;j++){
        if(A[j].size() <= index){
            C[0]++;
        }
        else
            C[A[j][index] -64] += 1;
        number_iter++;
    }
    
    for(int i=1; i<k+1+1;i++){
        C[i] += C[i-1];
        number_iter++;
    }
    
    for(int j=n;j>0;j--){
        if(A[j].size() <= index){
            B[C[0]] = A[j];
            C[0] -= 1;
        }
        else{
        B[C[A[j][index] -64]] = A[j];
        C[A[j][index]-64] -= 1;
        }
        number_iter++;
    }
    for (int i = 1; i < n + 1; i++) {
        A[i] = B[i];
        number_iter++;
    }

    delete[] C;
    delete[] B;
}


int max(std::string* arr, int size){
    int max = 0;
    for(int i=0; i<size; i++){
        if(arr[i].length() > max) max = arr[i].size();
    }
    return max;
}


int radix_string_sort(std::string* arr, int size, bool ascending){
    int number_iter = 0;
    int max_digit = max(arr, size);
    std::string* A = arr-1;
    for(int i=max_digit-1;i>=0;i--){
        counting_sort(A, size, 25, i, number_iter);
    }
    arr = A+1;
    if(!ascending){
        std::string* temp = new std::string[size];
        for(int i=0; i<size ;i++){
            temp[i] = (arr[i]);
        }
        for(int i=0;i<size;i++){
            arr[i] = temp[size-i-1];
        }
    }
    return number_iter;    
    
}
