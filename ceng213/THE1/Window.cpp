#include "Window.h"

Window::Window() {
    this->activeTab = 0;
    tabs.append(Tab());
}

Tab Window::getActiveTab() {
    // TODOo, you are not allowed to create new nodes
    if(tabs.getSize() == 0){
        return Tab();}
    else{
        return (tabs.getNodeAtIndex(activeTab)) -> data;
    }
}

bool Window::isEmpty() const {
    // TODO
    if(tabs.getSize() == 0){
        return true;
    }
    else
        return false;
}

void Window::newTab(const Tab &tab) {
    // TODO
    if(isEmpty()){
        activeTab = 0;
        tabs.prepend(tab);
    }
    else{
    tabs.insertAfterNode(tab,tabs.getNodeAtIndex(activeTab));
    activeTab++;
    }
    
}

void Window::closeTab() {
    // TODO
    int a = activeTab;
    if(activeTab == 0){
    }
    else if(activeTab == tabs.getSize() - 1){
        activeTab--;
    }
    else{
        activeTab;
    }
    tabs.removeNodeAtIndex(a);
    if(isEmpty()){
        activeTab = -1;
    }
}

void Window::moveActiveTabTo(int index) {
    // TODO
    if(index < 0 || isEmpty())
        return;
    else{
        tabs.moveToIndex(activeTab,index);
        if(index >= tabs.getSize())
            activeTab = tabs.getSize() - 1;
        else if(index < tabs.getSize())
            activeTab = index;
    }
}

void Window::changeActiveTabTo(int index) {
    // TODO
    if(index >= tabs.getSize() || index < 0){
        return;
    }
    else{
        activeTab = index;
    }
}

void Window::addTab(Node<Tab> &tab) {
    // TODO
    if(isEmpty()){
        activeTab = 0;
    }
    tabs.append(tab.data);
}

void Window::print() {
    if(tabs.isEmpty()) std::cout << "Window is empty" << std::endl;
    else std::cout << "The active tab is tab " << activeTab << std::endl;
    tabs.print();
}
