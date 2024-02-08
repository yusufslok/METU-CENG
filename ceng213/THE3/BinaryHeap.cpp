#include "BinaryHeap.h"

BinaryHeap::BinaryHeap()
{
    size = 0;
    // TODO: or not
}


bool BinaryHeap::Add(int uniqueId, double weight)
{
    HeapElement element;
    element.uniqueId = uniqueId;
    element.weight=weight;
    for(int i=0;i<size;i++){
        if(elements[i].uniqueId == element.uniqueId) return false;
    }
    elements.push_back(element);
    size++;
    int cur = size;
        for(;cur > 1;){
            int parent = cur /2;
            if(elements[cur-1].weight < elements[parent-1].weight){
                HeapElement tmp = elements[cur-1];
                elements[cur-1] = elements[parent-1];
                elements[parent-1] = tmp;
            }
            else break;
            cur=parent;
        }
    return true;
    // TODO:
}

bool BinaryHeap::PopHeap(int& outUniqueId, double& outWeight)
{
    if(size){
        outWeight = elements[0].weight;
        outUniqueId = elements[0].uniqueId;
        int cur = 1;
        elements[0] = elements[size-1];
        size--;
        for(;cur*2<=size;){
            int child = cur*2;
            if(child+1<=size && elements[child].weight < elements[child-1].weight){
                child++;
                }
            if(elements[cur-1].weight > elements[child-1].weight){
                HeapElement temp = elements[cur-1];
                elements[cur-1] = elements[child-1];
                elements[child-1] = temp;
            }
            else break;
            cur = child;
        }
            
        return true;
    }
    return false;
    // TODO:
}

bool BinaryHeap::ChangePriority(int uniqueId, double newWeight)
{
    for(int i=0;i<size;i++){
        if(elements[i].uniqueId == uniqueId){
            elements[i].weight = newWeight;
            if(i+1 != 1 and newWeight < elements[(i+1)/2-1].weight){
                int cur = i+1;
                for(;cur > 1;){
                    int parent = cur /2;
                    if(elements[cur-1].weight < elements[parent-1].weight){
                    HeapElement tmp = elements[cur-1];
                    elements[cur-1] = elements[parent-1];
                    elements[parent-1] = tmp;
                }
                else break;
                cur=parent;
            }
            }
            else if(((i+1)*2-1 <= size and newWeight > elements[(i+1)*2-1].weight) || (i+1)*2 <= size and newWeight > elements[(i+1)*2].weight){
                int cur = i+1;
                for(;cur*2<=size;){
                    int child = cur*2;
                    if(child+1<=size && elements[child].weight < elements[child-1].weight){
                        child++;
                    }
                    if(elements[cur-1].weight > elements[child-1].weight){
                        HeapElement temp = elements[cur-1];
                        elements[cur-1] = elements[child-1];
                        elements[child-1] = temp;
                    }
                    else break;
                    cur = child;
                }
            }
            return true;
        }
    }
    return false;
    // TODO:
}

int BinaryHeap::HeapSize() const
{
    return size;
    // TODO:
}