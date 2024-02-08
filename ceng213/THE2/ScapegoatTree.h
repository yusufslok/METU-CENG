#ifndef TREE_H
#define TREE_H

#include <iostream>
#include <cmath>

#include "Node.h"
#include "NoSuchItemException.h"

enum TraversalMethod {preorder, inorder, postorder};

template<class T>
class ScapegoatTree {
public: // DO NOT CHANGE THIS PART.
    ScapegoatTree();

    ScapegoatTree(const ScapegoatTree<T> &obj);

    ~ScapegoatTree();

    bool isEmpty() const;

    int getHeight() const;

    int getSize() const;

    bool insert(const T &element);

    bool remove(const T &element);

    void removeAllNodes();

    const T &get(const T &element) const;

    void print(TraversalMethod tp=inorder) const;

    void printPretty() const;

    ScapegoatTree<T> &operator=(const ScapegoatTree<T> &rhs);

    void balance();

    const T &getCeiling(const T &element) const;

    const T &getFloor(const T &element) const;

    const T &getMin() const;

    const T &getMax() const;

    const T &getNext(const T &element) const;

private: // YOU MAY ADD YOUR OWN UTILITY MEMBER FUNCTIONS HERE.
    void ScapegoatTreehelper(Node<T> *&r, Node<T> *second){
        Node<T> *temp;
        if(second == NULL) {r = NULL;return;}
        temp = new Node<T>(second->element,NULL,NULL);
        r = temp;
        if(second->left) ScapegoatTreehelper(r->left,second->left);
        if(second->right) ScapegoatTreehelper(r->right,second->right);
    }
    void removeAllNodesh(Node<T> *&r){
        Node<T> *temp;
        if(r == NULL) return;
        temp = r;
        if(r-> left) removeAllNodesh(r->left);
        if(r->right) removeAllNodesh(r->right);
        delete temp;
    }
    int getHeighth(Node<T> *r) const{
        int left=-1,right=-1,max=-1;
        if(r == NULL) return -1;
        if(r->left){
            left = getHeighth(r->left);
        }
        if(r->right){
            right = getHeighth(r->right);
        }
        if(left > right){
            max = left;
        }
        else max = right;
        return max+1;
    }
    
    int getSizeh(Node<T> *r) const{
        if(r == NULL) return 0;
        return getSizeh(r->left) + getSizeh(r->right) + 1;
    }
    
    bool geth(Node<T> *r, const T &val) const{
        if(r == NULL) return false;
        if(r->element == val)
            return true;
        if(val < r->element){
            return geth(r->left,val);
        }
        else if(val > r->element){
            return geth(r->right,val);
        }
    } 
    
    const T &gethelement(Node<T> *r, const T &val) const{
        if(r->element == val)
            return r->element;
        if(val < r->element){
            return gethelement(r->left,val);
        }
        else if(val > r->element){
            return gethelement(r->right,val);
        }
    } 
    
    void inserth(Node<T> *&r, const T &val){
        if(val < r->element and r->left == NULL){
            Node<T> *node;
            node = new Node<T>(val,NULL,NULL);
            r->left = node;
        }
        else if(val > r->element and r->right == NULL){
            Node<T> *node;
            node = new Node<T>(val,NULL,NULL);
            r->right = node;
        }
        else if(val < r->element){
            inserth(r->left,val);
        }
        else if(val > r->element){
            inserth(r->right,val);
        }
    }
    
    void removeh(Node<T> *&r, const T &val){
        if(val < r->element){
            removeh(r->left,val);
        }
        else if(val > r->element){
            removeh(r->right,val);
        }
        else{
            if(r->left != NULL and r->right != NULL ){
                r->element = getMaxh(r->left);
                removeh(r->left,r->element);
            }
            else if(r->left != NULL){
                Node<T> *temp;
                temp = r;
                r = r->left;
                delete temp;
            }
            else{
                Node<T> *temp;
                temp = r;
                r = r->right;
                delete temp;
            }
        }
    }
    
    const T &getMaxh(Node<T> *r) const{
        if(r->right == NULL) return r->element;
        else return getMaxh(r->right);
    }
    
    void makearr(T *&arr, int &i, Node<T> *r){
        if(r->left){
            makearr(arr,i,r->left);
        }
        arr[i] = r->element;
        i++;
        if(r->right){
            makearr(arr,i,r->right);
        }
    }
    
    T *partial(T *arr, int i, int size){
        T *ar;
        int eosize = size;
        if(size%2 == 0) eosize = eosize/2 - 1;
        else eosize = eosize/2;
        ar = new T[eosize+1];
        if(i==0){
        for(int j=0;j<eosize;j++){
            ar[j] = arr[j];
        }}
        else if(i==1){
            for(int j=eosize+1,k=0;j<size;j++,k++){
                ar[k] = arr[j];
            }
        }
        return ar;
    }

    
    void balanced(T *arr, Node<T> *&r, int size){
        T * temp1,*temp2;
        int eosize = size;
        if(size%2 == 0) eosize = eosize/2 - 1;
        else eosize = eosize/2;
        if(size == 0) return;
        r = new Node<T>(arr[eosize],NULL,NULL);
        temp1 = partial(arr,0,size);
        temp2 = partial(arr,1,size);
        if(size%2 == 0){
        balanced(temp1,r->left,size/2-1);
        balanced(temp2,r->right,size/2);}
        else{
            balanced(temp1,r->left,size/2);
            balanced(temp2,r->right,size/2);
        }
        delete [] temp1;
        delete [] temp2;
    }
    
    
    void findsg(Node<T> *&r, T element, Node<T> **&sg){
        if(element < r->element){
            findsg(r->left,element,sg);
        }
        else if(element > r-> element){
            findsg(r->right,element,sg);
        }
        if(sg == NULL ){
            if(element < r->element and r->left){
                if(3*getSizeh(r->left) > 2*getSizeh(r)){
                    sg = &r;
                }
            }
            else if(element > r-> element and r->right){
                if(3*getSizeh(r->right) > 2*getSizeh(r)){
                    sg = &r;
                }
            }
        }
    }
    
    void balancehh(Node<T> *&r) {
    int i=0,size;
    T * arr;
    arr = new T[getSizeh(r)];
    makearr(arr,i,r);
    size = getSizeh(r);
    removeAllNodesh(r);
    r = NULL;
    balanced(arr,r,size);
    delete [] arr;
}

    const T &getCeilingh(Node<T> *r, const T &element) const{
        Node<T> *ceil;
        while(r){
            if(element < r->element){
                ceil=r;
                r=r->left;
            }
            else{
                r=r->right;
            }
        }
        return ceil->element;
    }
    
    const T &getMinh(Node<T> *r) const{
        if(r->left == NULL) return r->element;
        return getMinh(r->left);
    }
    
    const T &getFloorh(Node<T> *r, const T &element) const{
        Node<T> *floor;
        while(r){
            if(element > r->element){
                floor=r;
                r=r->right;
            }
            else{
                r=r->left;
            }
        }
        return floor->element;
    }
    
    void print(Node<T> *node, TraversalMethod tp) const;
    void printPretty(Node<T> *node, int indentLevel, bool isLeftChild) const;

private: // DO NOT CHANGE THIS PART.
    Node<T> *root;

    int upperBound;
};

#endif //TREE_H

template<class T>
ScapegoatTree<T>::ScapegoatTree() {
    /* TODO */
    root = NULL;
    upperBound = 0;
}

template<class T>
ScapegoatTree<T>::ScapegoatTree(const ScapegoatTree<T> &obj) {
    /* TODO */
    upperBound = obj.upperBound;
    ScapegoatTreehelper(root,obj.root);
}

template<class T>
ScapegoatTree<T>::~ScapegoatTree() {
    /* TODO */
    removeAllNodes();
}

template<class T>
bool ScapegoatTree<T>::isEmpty() const {
    /* TODO */
    if(root == NULL) return true;
    else return false;
}

template<class T>
int ScapegoatTree<T>::getHeight() const {
    /* TODO */
    return getHeighth(root);
}

template<class T>
int ScapegoatTree<T>::getSize() const {
    /* TODO */
    return getSizeh(root);
}

template<class T>
bool ScapegoatTree<T>::insert(const T &element) {
    /* TODO */
    Node<T> *r=root,**sg;
    if(!geth(r,element)){
        if(root == NULL){
            Node<T> *node;
            node = new Node<T>(element,NULL,NULL);
            root = node;
            upperBound++;
            return true;
        }
        inserth(r,element);
        upperBound++;
        sg = NULL;
        if(pow(1.5,getHeight()) > upperBound){
        findsg(root,element,sg);
        if(*sg != NULL){
            if(*sg == root){balance();}
            else balancehh(*sg);
            }
        }
        return true;
    }
    return false;
}

template<class T>
bool ScapegoatTree<T>::remove(const T &element) {
    /* TODO */
    Node<T> *r = root;
    if(geth(r,element)){
        if(getSize() == 1){
            delete root;
            root = NULL;
            upperBound = 0;
            return true;
        }
        else if(root-> element == element){
        if(root->left != NULL and root->right != NULL ){
                root->element = getMaxh(root->left);
                removeh(root->left,root->element);
            }
            else if(root->left != NULL){
                Node<T> *temp;
                temp = root;
                root = root->left;
                delete temp;}
                
            else{
                Node<T> *temp;
                temp = root;
                root = root->right;
                delete temp;
                
            }
             if(upperBound > 2*getSize() or getSize() > upperBound){
            balance();
            upperBound = getSize();
        }
            return true;
        }
        removeh(r,element);
        if(upperBound > 2*getSize() or getSize() > upperBound){
            balance();
            upperBound = getSize();
        }
        return true;
    }
    return false;
}

template<class T>
void ScapegoatTree<T>::removeAllNodes() {
    /* TODO */
    upperBound = 0;
    removeAllNodesh(root);
    root = NULL;
}

template<class T>
const T &ScapegoatTree<T>::get(const T &element) const {
    /* TODO */
    if(geth(root,element)){
        return gethelement(root,element);
    }
    throw NoSuchItemException();
}

template<class T>
void ScapegoatTree<T>::print(TraversalMethod tp) const {

    if (tp == preorder) {
        /* TODO */
        if(isEmpty()){
            std::cout << "BST_preorder{}" << std::endl;
            return;
        }
        std::cout << "BST_preorder{" << std::endl;
        print(root,tp);
        std::cout << std::endl << "}" << std::endl;
    } else if (tp == inorder) {
        // check if the tree is empty?
        if (isEmpty()) {
            // the tree is empty.
            std::cout << "BST_inorder{}" << std::endl;
            return;
        }

        // the tree is not empty.

        // recursively output the tree.
        std::cout << "BST_inorder{" << std::endl;
        print(root, tp);
        std::cout << std::endl << "}" << std::endl;
    } else if (tp == postorder) {
        /* TODO */
        if(isEmpty()){
            std::cout << "BST_postorder{}" << std::endl;
            return;
        }
        std::cout << "BST_postorder{" << std::endl;
        print(root,tp);
        std::cout << std::endl << "}" << std::endl;
    }
}

template<class T>
void ScapegoatTree<T>::print(Node<T> *node, TraversalMethod tp) const {

    if (tp == preorder) {
        /* TODO */
        if(node == NULL) return;
        std::cout << "\t" << node->element;
        if (node->left) {
            std::cout << "," << std::endl;
        }
        print(node->left,tp);
        if (node->right) {
            std::cout << "," << std::endl;
        }
        print(node->right,tp);
        
       
    } else if (tp == inorder) {
        // check if the node is NULL?
        if (node == NULL)
          return;

        // first, output left subtree and comma (if needed).
        print(node->left, inorder);
        if (node->left) {
            std::cout << "," << std::endl;
        }

        // then, output the node.
        std::cout << "\t" << node->element;

        // finally, output comma (if needed) and the right subtree.
        if (node->right) {
            std::cout << "," << std::endl;
        }
        print(node->right, inorder);
    } else if (tp == postorder) {
        /* TODO */
        if(node == NULL) return;
        print(node->left,tp);
        if (node->left) {
            std::cout << "," << std::endl;
        }
        print(node->right,tp);
        if (node->right) {
            std::cout << "," << std::endl;
        }
        std::cout << "\t" << node->element;
    }
}

template<class T>
void ScapegoatTree<T>::printPretty() const {

    // check if the tree is empty?
    if (isEmpty()) {
        // the tree is empty.
        std::cout << "BST_pretty{}" << std::endl;
        return;
    }

    // the tree is not empty.

    // recursively output the tree.
    std::cout << "BST_pretty{" << std::endl;
    printPretty(root, 0, false);
    std::cout << "}" << std::endl;

}

template<class T>
void ScapegoatTree<T>::printPretty(Node<T> *node, int indentLevel, bool isLeftChild) const {

    // check if the node is NULL?
    if (node == NULL)
        return;

    // output the indentation and the node.
    std::cout << "\t";
    for (int i = 0; i < indentLevel; ++i) {
        std::cout << "---";
    }
    std::cout << (indentLevel == 0 ? "root:" : (isLeftChild ? "l:" : "r:")) << node->element << std::endl;

    // first, output left subtree with one more indentation level.
    printPretty(node->left, indentLevel + 1, true);

    // then, output right subtree with one more indentation level.
    printPretty(node->right, indentLevel + 1, false);
}

template<class T>
ScapegoatTree<T> &ScapegoatTree<T>::operator=(const ScapegoatTree<T> &rhs) {
    /* TODO */
    if(this == &rhs) return *this;
    removeAllNodes();
    upperBound = rhs.upperBound;
    ScapegoatTreehelper(root,rhs.root);
    return *this;
}

template<class T>
void ScapegoatTree<T>::balance() {
    /* TODO */
    int i=0,size;
    T * arr;
    arr = new T[getSize()];
    makearr(arr,i,root);
    size = getSize();
    removeAllNodes();
    balanced(arr,root,size);
    delete [] arr;
}

template<class T>
const T &ScapegoatTree<T>::getCeiling(const T &element) const {
    /* TODO */
    Node<T> *r= root;
    if(geth(r,element)) return gethelement(root,element);
    if(element > getMax()) throw NoSuchItemException();
    else return getCeilingh(r,element);
}

template<class T>
const T &ScapegoatTree<T>::getFloor(const T &element) const {
    /* TODO */
    Node<T> *r= root;
    if(geth(r,element)) return gethelement(root,element);
    if(element < getMin()) throw NoSuchItemException();
    else return getFloorh(r,element);
}

template<class T>
const T &ScapegoatTree<T>::getMin() const {
    /* TODO */
    Node<T> *r=root;
    if(isEmpty()) throw NoSuchItemException();
    else
        return getMinh(r);
}

template<class T>
const T &ScapegoatTree<T>::getMax() const {
    /* TODO */
    Node<T> *r=root;
    if(isEmpty()) throw NoSuchItemException();
    else
        return getMaxh(r);
}

template<class T>
const T &ScapegoatTree<T>::getNext(const T &element) const {
    /* TODO */
    Node<T> *r= root;
    if(element >= getMax()) throw NoSuchItemException();
    else return getCeilingh(r,element);
}
