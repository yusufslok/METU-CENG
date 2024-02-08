#include "the1.h"
//You may write your own helper functions here

int lomutoPartition(unsigned short* arr, int low, int high, bool median_of_3, long& swap, double& avg_dist, double& max_dist){
    int size = high-low+1;
    int pivot = high;
    unsigned short temp;
    if(median_of_3 == true){
        if (((arr[low] < arr[high]) ^ (arr[low] < arr[low+(size-1)/2])) && (arr[low] != arr[high] and arr[low] != arr[low+(size-1)/2] and arr[high] != arr[low+(size-1)/2] )){ 
            temp = arr[low];
            arr[low] = arr[high];
            arr[high] = temp;
            int distance = high-low;
            if(distance > max_dist) max_dist = distance;
            avg_dist += distance;
            swap++;
        }
        else if(((arr[low+(size-1)/2] < arr[low]) ^ (arr[low+(size-1)/2] < arr[high])) && (arr[low] != arr[high] and arr[low] != arr[low+(size-1)/2] and arr[high] != arr[low+(size-1)/2] )){
            temp = arr[low+(size-1)/2];
            arr[low+(size-1)/2] = arr[high];
            arr[high] = temp;
            int distance = high-(low+(size-1)/2);
            if(distance > max_dist) max_dist = distance;
            avg_dist += distance;
            swap++;
        }
    }
    int i = low-1;
    for(int j=i+1;j<high;j++){
        if(arr[j] >= arr[pivot]){
            unsigned short temp;
            i++;
            temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
            int distance = j-i;
            if(distance > max_dist) max_dist = distance;
            avg_dist += distance;
            swap++;
        }
    }
    temp = arr[i+1];
    arr[i+1] = arr[pivot];
    arr[pivot] = temp;
    int distance = pivot-i-1;
    if(distance > max_dist) max_dist = distance;
    avg_dist += distance;
    swap++;
    return i+1;
}


int hoarePartition(unsigned short* arr, int low, int high, bool median_of_3, long& swap, double& avg_dist, double& max_dist){
    int size = high-low+1;
    int i=low-1;
    int j=low+size; 
    int pivot = low+(size-1)/2;
    int x = arr[pivot];
    unsigned short temp;
    if(median_of_3 == true){
        if (((arr[low] < arr[high]) ^ (arr[low] < arr[low+(size-1)/2])) && (arr[low] != arr[high] and arr[low] != arr[low+(size-1)/2] and arr[high] != arr[low+(size-1)/2] ) ){ 
            temp = arr[low];
            arr[low] = arr[low+(size-1)/2];
            arr[low+(size-1)/2] = temp;
            int distance = (low+(size-1)/2)-low;
            if(distance > max_dist) max_dist = distance;
            avg_dist += distance;
            swap++;
        }
        else if (((arr[high] < arr[low]) ^ (arr[high] < arr[low+(size-1)/2])) && (arr[low] != arr[high] and arr[low] != arr[low+(size-1)/2] and arr[high] != arr[low+(size-1)/2] )){
            temp = arr[high];
            arr[high] = arr[low+(size-1)/2];
            arr[low+(size-1)/2] = temp;
            int distance = high-(low+(size-1)/2);
            if(distance > max_dist) max_dist = distance;
            avg_dist += distance;
            swap++;
        }
    }
    x = arr[pivot];
    while(true){
        do {j--;}
        while(arr[j] < x);
        do {i++;}
        while(arr[i] > x);
        if(i<j){
            unsigned short temp;
            temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
            int distance = j-i;
            if(distance > max_dist) max_dist = distance;
            avg_dist += distance;
            swap++;
        }
        else return j;
    }
}



int quickSort1(unsigned short* arr, long& swap, double& avg_dist, double& max_dist, bool hoare, bool median_of_3, int size, int low, int high){
    //Your code here
    int pivot;
	if(size > 1){
	    if(hoare == false) {
	        pivot = lomutoPartition(arr, low, high, median_of_3, swap, avg_dist, max_dist);
	        return quickSort1(arr, swap, avg_dist, max_dist, hoare, median_of_3, pivot-low, low, pivot-1)+
	        quickSort1(arr, swap, avg_dist, max_dist, hoare, median_of_3, high-pivot, pivot+1, high)+2;
	    }
	    else {
	        pivot = hoarePartition(arr, low, high, median_of_3, swap, avg_dist, max_dist);
	        return quickSort1(arr, swap, avg_dist, max_dist, hoare, median_of_3, pivot-low+1, low, pivot)+
	        quickSort1(arr, swap, avg_dist, max_dist, hoare, median_of_3, high-pivot, pivot+1, high)+2;
	    }
	}
	return 0;
}



int quickSort(unsigned short* arr, long& swap, double& avg_dist, double& max_dist, bool hoare, bool median_of_3, int size){
    avg_dist = 0; swap=0; max_dist = 0;
    int numberOfRec = quickSort1(arr, swap, avg_dist, max_dist, hoare, median_of_3, size, 0, size-1)+1;
    avg_dist /= swap;
    return numberOfRec;
}
