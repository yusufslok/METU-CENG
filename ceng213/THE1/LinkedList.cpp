#include "LinkedList.h"

#include <iostream>

#include "Node.h"


template<class T>
LinkedList<T>::LinkedList()
{
    /* TODO */
    head = NULL;
    size = 0;
}

template<class T>
LinkedList<T>::LinkedList(const LinkedList<T> &obj)
{
    /* TODO */
    Node<T> *current;
    Node<T> *second;
    if(&obj == this){
        return;
    }
    head = NULL;
    size = 0;
    if(obj.size == 0){
        return;
    }
    second = obj.head;
    current = new Node<T>(obj.head -> data);
    head = current;
    size = obj.size;
    while(true){
        second = second -> next;
        if(second == obj.head){
            current -> next = head;
            head -> prev = current;
            break;}
        else{
            Node<T> *temp;
            temp = new Node<T>(second->data);
            current -> next = temp;
            temp -> prev = current;
            current = current -> next;
        }
    }
    
}

template<class T>
LinkedList<T>::~LinkedList()
{
    
    /* TODO */
    Node<T> *current;
    current = head;
    if(head){
    for(int i=0;i<size-1;i++){
        Node<T> *temp;
        temp = current;
        current = current -> next;
        delete temp;
    }
    delete current;
    size = 0;
}
}

template<class T>
LinkedList<T> &LinkedList<T>::operator=(const LinkedList<T> &rhs)
{
    /* TODO */
    Node<T> *current;
    Node<T> *second;
    if(&rhs == this){
        return *this;
    }
    removeAllNodes();
    head = NULL;
    size = 0;
    if(rhs.size == 0){
        return *this;
    }
    second = rhs.head;
    current = new Node<T>(rhs.head -> data);
    head = current;
    size = rhs.size;
    while(true){
        second = second -> next;
        if(second == rhs.head){
            current -> next = head;
            head -> prev = current;
            break;}
        else{
            Node<T> *temp;
            temp = new Node<T>(second->data);
            current -> next = temp;
            temp -> prev = current;
            current = current -> next;
        }
    }
    return *this;
    
}

template<class T>
int LinkedList<T>::getSize() const
{
    /* TODO */
    return size;
}

template<class T>
bool LinkedList<T>::isEmpty() const
{
    /* TODO */
    if(head)
        return false;
    else
        return true;
}

template<class T>
bool LinkedList<T>::containsNode(Node<T> *node) const
{
    /* TODO */
    Node<T> *current = head;
    int i = 0;
    if (!head)
        return false;
    while(i < size){
        if(current == node){
            return true;
        }
        current = current -> next;
        i++;
    }
    return false;
}

template<class T>
Node<T> *LinkedList<T>::getFirstNode() const
{
    /* TODO */
    if(isEmpty())
        return NULL;
    return head;
}

template<class T>
Node<T> *LinkedList<T>::getLastNode() const
{
    /* TODO */
    if(head)
        return head -> prev;
    else
        return NULL;
}

template<class T>
Node<T> *LinkedList<T>::getNode(const T &data) const
{
    /* TODO */
    Node<T> *current;
    current = head;
    for(int j=0; j<size;j++){
        if(current -> data == data){
            return current;
        }
        else
            current = current->next;
    }
    return NULL;
}

template<class T>
Node<T> *LinkedList<T>::getNodeAtIndex(int index) const
{
    /* TODO */
    Node<T> *current = head;
    if (index < 0 || index >= size)
        return NULL;
    for(int i=0;i<index;i++){
        current = current -> next;
    }
    return current;
}

template<class T>
void LinkedList<T>::append(const T &data)
{
    /* TODO */
    Node<T> *current;
    current = new Node<T>(data);
    if(!head){
        current -> prev = current;
        current -> next = current;
        head = current;
    }
    else{
        current -> next = head;
        current -> prev = head -> prev;
        head -> prev -> next = current;
        head -> prev = current;
    }
    size++;
}

template<class T>
void LinkedList<T>::prepend(const T &data)
{
    /* TODO */
    Node<T> *current;
    current = new Node<T>(data);
    if (!head){
        current -> prev = current;
        current -> next = current;
        head = current;
    }
    else{
        current -> next = head;
        current -> prev = head -> prev;
        head -> prev -> next = current;
        head -> prev = current;
        head = current;
    }
    size++;
}

template<class T>
void LinkedList<T>::insertAfterNode(const T &data, Node<T> *node)
{
    /* TODO */
    Node<T> *temp;
    int i=0;
    temp = head;
    if (!head || !containsNode(node))
        return;
    while(i<size){
        if(node == temp){
            Node<T> *current;
            current = new Node<T>(data);
            temp -> next -> prev = current;
            current -> next = temp -> next;
            temp -> next = current;
            current -> prev = temp;
            size++;
            break;
        }
        temp = temp -> next;
        i++;
    }
}

template<class T>
void LinkedList<T>::insertAtIndex(const T &data, int index)
{
    /* TODO */
    if (index > size || index < 0){
        return;
    }
    else{
        Node<T> *temp, *current;
        current = head;
        for(int i=0;i<index;i++){
            current = current -> next;
        }
        if(index == 0){
            prepend(data);
        }
        else{
            temp = new Node<T>(data);
            current -> prev -> next = temp;
            temp -> prev = current -> prev;
            temp -> next = current;
            current -> prev = temp;
            size++;
        }
    }
}

template<class T>
int LinkedList<T>::getIndex(Node<T> *node) const
{
    /* TODO */
    int index =0,i=0;
    if(!containsNode(node)){
        return -1;
    }
    else{
        Node<T> *current;
        current = head;
        while(i<size){
            if(node == current){
                return index;
            }
            else{
                current = current -> next;
                index++;
            }
            i++;
        }
    }
}

template<class T>
void LinkedList<T>::moveToIndex(int currentIndex, int newIndex)
{
    /* TODO */
    if(currentIndex >= size ||  currentIndex == newIndex || newIndex<0 || size == 0 || size == 1)
        return;
    if(newIndex >= size){
        Node<T> *current = head;
        Node<T> *temp;
        for(int i=0;i<currentIndex;i++){
            current = current -> next;
        }
        if(current == head){
            head = head -> next;
        }
        current -> prev -> next = current -> next;
        current -> next ->prev = current -> prev;
        head -> prev -> next = current;
        current -> prev = head -> prev;
        head -> prev = current;
        current -> next = head;
        
    }
    else{
        Node<T> *current = head;
        Node<T> *second = head;
        for(int i=0;i<currentIndex;i++){
            current = current -> next;
        }
        if(current == head){
            head = head -> next;
            newIndex++;
        }
        current -> prev -> next = current -> next;
        current -> next -> prev = current -> prev;
        for(int j=0;j<newIndex;j++){
            second = second -> next;
        }
        if(newIndex == 0){
            head = current;
        }
        second -> prev -> next = current;
        current -> next = second ;
        current -> prev = second -> prev;
        second -> prev = current;
    }
}

template<class T>
void LinkedList<T>::removeNode(Node<T> *node)
{
    /* TODO */
    Node<T> *current;
    int a = size;
    current = head;
    if (size == 0 || !containsNode(node))
        return;
    for(int i=0;i<a;i++){
        if(current == node){
            if(current == head && size > 1){
                head -> prev -> next = head -> next;
                head -> next -> prev = head -> prev;
                head = head -> next;
                delete current;
                size--;
                break;
            }
            else if (size > 1){
                current -> prev -> next = current -> next;
                current -> next -> prev = current -> prev;
                delete current;
                size--;
                break;}
            else{
                delete current;
                head = NULL;
                size = 0;
                break;
            }
                
        }
        current = current -> next;
    }
    
    
}

template<class T>
void LinkedList<T>::removeNode(const T &data)
{
    /* TODO */
    int a=size;
    Node<T> *current = head;
    if (!head)
        return;
    for(int i=0;i<a;i++){
        if(current -> data == data){
            if(current == head && size > 1){
                head -> prev -> next = head -> next;
                head -> next -> prev = head -> prev;
                head = head -> next;
                delete current;
                current = head;
                size--;
            }
            else if (size > 1){
                Node<T> *temp;
                temp = current;
                current -> prev -> next = current -> next;
                current -> next -> prev = current -> prev;
                size--;
                current = current -> next;
                delete temp;}
            else{
                delete current;
                head = NULL;
                size = 0;
                break;
            }
            
        }
        else{
            current = current -> next;
        }
    }
    
}

template<class T>
void LinkedList<T>::removeNodeAtIndex(int index)
{
    /* TODO */
    if (index >= size || size == 0 || index < 0){
        return;
    }
    else{
        Node<T> *current = head;
        for(int i=0;i<index;i++){
            current = current -> next;
        }
        removeNode(current);
    }
}

template<class T>
void LinkedList<T>::removeAllNodes()
{
    /* TODO */
    Node<T> *current;
    current = head;
    if (head){
    for(int i=0;i<size-1;i++){
        Node<T> *temp;
        temp = current;
        current = current -> next;
        delete temp;
    }
    delete current;
    head =  NULL;
    size = 0;}
}

template<class T>
void LinkedList<T>::print() const
{
    if (this->isEmpty()) {
        std::cout << "The list is empty." << std::endl;
        return;
    }

    Node<T> *node = this->getFirstNode();

    do {
        std::cout << *node << std::endl;
        node = node->next;
    }
    while (node != this->getFirstNode());
}
