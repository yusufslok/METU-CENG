#include "Browser.h"

Browser::Browser() {
    windows.append(Window());
}

void Browser::newWindow() {
    // TODO
    windows.prepend(Window());
}

void Browser::closeWindow() {
    // TODO
    if(windows.getSize())
            windows.removeNodeAtIndex(0);
}

void Browser::switchToWindow(int index) {
    // TODO
    windows.moveToIndex(index,0);
}

Window &Browser::getWindow(int index) {
    return windows.getNodeAtIndex(index) -> data;
}

void Browser::moveTab(Window &from, Window &to) {
    // TODO
    Tab win;
    win = from.getActiveTab();
    from.closeTab();
    to.newTab(win);
}

void Browser::mergeWindows(Window &window1, Window &window2) {
    // TOD
   while(!(window2.isEmpty())){
       window2.changeActiveTabTo(0);
       window1.newTab(window2.getActiveTab());
       window2.closeTab();
   }
}

void Browser::mergeAllWindows() {
    // TODO 
    
    Node<Window> *first;
    Node<Window> *win;
    Window *firstwin;
    if(windows.getSize() == 0){
        return;
    }
    first = windows.getFirstNode();
    win = windows.getFirstNode();
    firstwin = &(first -> data);
    win = win -> next;
    while(!(win == windows.getFirstNode())){
        while(!((win -> data).isEmpty())){
            Window *window;
            window = &(win -> data);
            window -> changeActiveTabTo(0);
            firstwin -> newTab(window -> getActiveTab());
            window -> closeTab();
        }
        win = win -> next;
    }
    
    
}

void Browser::closeAllWindows() {
    // TODO
    while(windows.getSize()){
        closeWindow();
    }
}

void Browser::closeEmptyWindows() {
    // TODO
    Node<Window> *win;
    int a = windows.getSize();
    win = windows.getFirstNode();
    for(int i=0,j=0;i<a;i++){
        if((win -> data).isEmpty()){
            win = win -> next;
            windows.removeNodeAtIndex(j);
        }
        else{
            win = win -> next;
            j++;
        }
    }
    
}

void Browser::print() {
    Node<Window> *head = windows.getFirstNode();
    if(head == NULL) {
        std::cout << "The browser is empty" << std::endl;
    } else {
        (head -> data).print();
        head = head -> next;
        while(head != windows.getFirstNode()) {
            (head -> data).print();
            head = head -> next;
        }
    }
}