#ifndef TREEMAP_H
#define TREEMAP_H

#include <iostream>
#include <vector>

#include "KeyValuePair.h"
#include "ScapegoatTree.h"

template<class K, class V>
class TreeMap {
public: // DO NOT CHANGE THIS PART.
    TreeMap();

    void clear();

    const V &get(const K &key) const;

    bool pop(const K &key);

    bool update(const K &key, const V &value);

    const KeyValuePair<K, V> &ceilingEntry(const K &key);

    const KeyValuePair<K, V> &floorEntry(const K &key);

    const KeyValuePair<K, V> &firstEntry();

    const KeyValuePair<K, V> &lastEntry();

    void pollFirstEntry();

    void pollLastEntry();

    std::vector<KeyValuePair<K, V> > subMap(K fromKey, K toKey) const;

    void print() const;

private: // YOU MAY ADD YOUR OWN UTILITY MEMBER FUNCTIONS HERE.
void subMaph(K fromKey, K toKey, std::vector<KeyValuePair<K, V> > &v, ScapegoatTree<KeyValuePair<K, V> > stree, KeyValuePair<K, V> element) const{
    if(stree.getMax() == element){
        if(element >= fromKey and element <= toKey){
            v.push_back(element);
            return;
        }
        return;
    }
    else if(element >= fromKey and element <= toKey){
        v.push_back(element);
    }
    subMaph(fromKey, toKey, v, stree, stree.getNext(element));
    
}

private: // DO NOT CHANGE THIS PART.
    ScapegoatTree<KeyValuePair<K, V> > stree;
};

template<class K, class V>
TreeMap<K, V>::TreeMap() {}

template<class K, class V>
void TreeMap<K, V>::clear() {
    /* TODO */
    stree.removeAllNodes();
}

template<class K, class V>
const V &TreeMap<K, V>::get(const K &key) const {
    /* TODO */
    KeyValuePair<K,V> a(key);
    return (stree.get(a)).getValue();
}

template<class K, class V>
bool TreeMap<K, V>::pop(const K &key) {
    /* TODO */
    KeyValuePair<K,V> a(key);
    return stree.remove(a);
}

template<class K, class V>
bool TreeMap<K, V>::update(const K &key, const V &value) {
    /* TODO */
        KeyValuePair<K,V> a(key,value);
        return stree.insert(a);
}

template<class K, class V>
const KeyValuePair<K, V> &TreeMap<K, V>::ceilingEntry(const K &key) {
    /* TODO */
    KeyValuePair<K,V> a(key);
    return stree.getCeiling(a);
}

template<class K, class V>
const KeyValuePair<K, V> &TreeMap<K, V>::floorEntry(const K &key) {
    /* TODO */
    KeyValuePair<K,V> a(key);
    return stree.getFloor(a);
}

template<class K, class V>
const KeyValuePair<K, V> &TreeMap<K, V>::firstEntry() {
    /* TODO */
    return stree.getMin();
}

template<class K, class V>
const KeyValuePair<K, V> &TreeMap<K, V>::lastEntry() {
    /* TODO */
    return stree.getMax();
}

template<class K, class V>
void TreeMap<K, V>::pollFirstEntry() {
    /* TODO */
    stree.remove(stree.getMin());
}

template<class K, class V>
void TreeMap<K, V>::pollLastEntry() {
    /* TODO */
    stree.remove(stree.getMax());
}

template<class K, class V>
std::vector<KeyValuePair<K, V> > TreeMap<K, V>::subMap(K fromKey, K toKey) const {
    /* TODO */
    std::vector<KeyValuePair<K, V> > v;
    KeyValuePair<K, V> element;
    element = stree.getMin();
    subMaph( fromKey, toKey, v, stree, element);
    return v;
}

template<class K, class V>
void TreeMap<K, V>::print() const {

    std::cout << "# Printing the tree map ..." << std::endl;

    std::cout << "# ScapegoatTree<KeyValuePair<K, V> > stree:" << std::endl;
    stree.printPretty();

    std::cout << "# Printing is done." << std::endl;
}

#endif //TREEMAP_H

