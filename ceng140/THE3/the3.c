#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "the3.h"
CountNode* head;
QueryNode** querytable;
int index;
int countoftable=1;
#define MAX_SIZE 10
int isthere(int number, CountNode* head){
    CountNode* h;
    h = head;
    while(1){
        if (h != NULL){
            if (number == (h -> count)){
                return 1;
            }
        }
        else
            return 0;
        h = h -> next;
    }
}
int compare(char* f, char* s){
    int i=0;
    while(1){
        if (f[i] == s[i]){
            i++;
        }
        else if (f[i] < s[i]){
            return 1;
        }
        else{
            return 0;
        }
    }
}
int istherestr(char* str,QueryNode** querytable){
    int i = 0;
    for(i=0;i<countoftable;i++){
        if (!strcmp(querytable[i] -> query, str)){
            return 1;
        }
    }
    return 0;
}
void removefromtable(char* str){
    int i=0;
    for(i=0;i<countoftable;i++){
        if (!strcmp(querytable[i] -> query,str)){
            index = i;
            break;
        }
    }
}
void inserttable(QueryNode* n){
    querytable[index] = n;
}
void deletecountnode(CountNode* h){
    if (h -> next == NULL){
        if(h -> prev == NULL){
            head = NULL;
        }
        else{
            h -> prev -> next = NULL;
        }
    }
    else{
        if(h -> prev == NULL){
            head = head -> next;
            head -> prev = NULL;
        }
        else{
            h -> prev -> next = h -> next;
            h -> next -> prev = h -> prev;
        }
    }
}

void deletequerynode(QueryNode* list,CountNode* h){
    if (list -> next == NULL){
        if(list -> prev == NULL){
            deletecountnode(h);}
        else{
            list -> prev -> next = NULL;
        }
    }
    else{
        if(list -> prev == NULL){
            (h -> querylist)[0] = list -> next;
            (h -> querylist)[0] -> prev = NULL;
        }
        else{
            list -> prev -> next = list -> next;
            list -> next -> prev = list -> prev;
        }
    }
        
}

void printt(CountNode* h){
    QueryNode* list;
    list = (h -> querylist)[0];
    while(1){
        if (list == NULL){
            printf("\n");
            break;
        }
        printf(" %s",list -> query);
        list = list -> next;
    }
}
void insertquery(char* str,CountNode* h){
    int number = 0;
    number = (*h).count +1 ;
    if (isthere(number,head)){
            QueryNode* newquery;
            CountNode* h;
            newquery = malloc(sizeof(QueryNode));
            newquery -> query = malloc(sizeof(char)*6);
            newquery -> query = str;
            h = head;
            while(1){
                if ((*h).count == number){
                    QueryNode* list;
                    list = (h -> querylist)[0]; 
                    while(1){
                        if (list -> next == NULL){
                            if (compare(str,(list -> query))){
                                if (list -> prev != NULL){
                                    list -> prev -> next = newquery;
                                    newquery -> prev = list -> prev;
                                    newquery -> next = list;
                                    list -> prev = newquery;
                                    break;
                                }
                                else{
                                    newquery -> next = list;
                                    list -> prev = newquery;
                                    newquery -> prev = NULL;
                                    (h -> querylist)[0] = newquery;
                                    break;
                                }
                            }
                            else{
                                newquery -> next = NULL;
                                newquery -> prev = list;
                                list -> next = newquery;
                                break;
                            }
                        }
                        else if (compare(str,(list -> query))){
                            if (list -> prev != NULL){
                                list -> prev -> next = newquery;
                                newquery -> prev = list -> prev;
                                newquery -> next = list;
                                list -> prev = newquery;
                                break;
                            }
                            else{
                                newquery -> next = list;
                                list -> prev = newquery;
                                newquery -> prev = NULL;
                                (h -> querylist)[0] = newquery;
                                break;
                            }
                        }
                        else
                            list = list -> next;
                    }
                    break;
                }
                else{
                    
                    h = h -> next;}
            }
        
    }
    else{
            CountNode* h;
            CountNode* neww;
            neww = malloc(sizeof(CountNode));
            ((neww -> querylist)) = malloc(sizeof(QueryNode*));
            *(neww -> querylist) = malloc(sizeof(QueryNode));
            (*neww).count = number;
            h = head;
            while(1){
                if (h == NULL){
                    head = neww;
                    neww -> next = NULL;
                    neww -> prev = NULL;
                    break;
                }
                else if(h -> next == NULL){
                    if(number < (*h).count){
                        if (h->prev != NULL){
                            h -> prev -> next = neww;
                            neww -> prev = h -> prev;
                            neww -> next = h;
                            h -> prev = neww;
                            break;
                        }
                        else{
                            neww -> next = head;
                            neww -> prev = NULL;
                            head = neww;
                            break;
                        }

                    }
                    else{
                        h -> next = neww;
                        neww -> prev = h;
                        neww -> next = NULL;
                        break;
                    }
                }
                else if (number < (*h).count){
                    if ((h -> prev) != NULL){
                    h -> prev -> next = neww;
                    neww -> prev = h -> prev;
                    neww -> next = h;
                    h -> prev = neww;
                    break;
                    }
                    else{
                        neww -> next = h;
                        h -> prev = neww;
                        neww -> prev = NULL;
                        head = neww;
                        break;
                    }
                }
                else{
                    h = h -> next;
                }
            }
            (neww -> querylist)[0] -> query = str;
            (neww -> querylist)[0] -> prev = NULL;
            (neww -> querylist)[0] -> next = NULL;
            
            
        
    }
}

int main() {
    int b=0,i=0,j=0,number=0,l=0;
    char* a;
    a = malloc(sizeof(char)*6);
    head = malloc(sizeof(CountNode));
    querytable = malloc(sizeof(*querytable)*MAX_SIZE);
    scanf("%d",&b);
        j=0;
        while(1){
            char k;
            scanf("%c",&k);
            if (k == ' ') {
                a[j] = '\0';
                break;}
            if (k == '\n'){
                continue;
            }
            a[j] = k;
            j++;
        }
    scanf("%d",&number);
    head -> next = NULL;
    head -> prev = NULL;
    (*head).count = number;
    ((head -> querylist)) = malloc(sizeof(QueryNode*));
    *(head -> querylist) = malloc(sizeof(QueryNode));
    (*(head -> querylist)) -> query = a;
    (*(head -> querylist)) -> next = NULL;
    (head -> querylist)[0] -> prev = NULL;
    querytable[0] = (head -> querylist)[0];
    for(i=0;i<b-1;i++){
        char* a;
        j=0;
        a = malloc(sizeof(char)*6);
        while(1){
            char k;
            scanf("%c",&k);
            if (k == ' ') {
                a[j] = '\0';
                break;}
            if (k == '\n'){
                continue;
            }
            a[j] = k;
            j++;
        }
    scanf("%d",&number);
        if (!isthere(number,head)){
            CountNode* h;
            CountNode* neww;
            neww = malloc(sizeof(CountNode));
            ((neww -> querylist)) = malloc(sizeof(QueryNode*));
            *(neww -> querylist) = malloc(sizeof(QueryNode));
            (*neww).count = number;
            h = head;
            while(1){
                
                if (h == NULL){
                    head = neww;
                    neww -> next = NULL;
                    neww -> prev = NULL;
                    break;
                }
                else if(h -> next == NULL){
                    
                    if(number < (*h).count){
                        if (h -> prev != NULL){
                            h -> prev -> next = neww;
                            neww -> prev = h -> prev;
                            neww -> next = h;
                            h -> prev = neww;
                            break;}
                        else{
                            h -> prev = neww;
                            neww -> next = head;
                            neww -> prev = NULL;
                            head = neww;
                            break;
                        }
                    }
                    else{
                        h -> next = neww;
                        neww -> prev = h;
                        neww -> next = NULL;
                        break;
                    }
                }
                else if (number < (*h).count){
                    if (h -> prev != NULL){
                    h -> prev -> next = neww;
                    neww -> prev = h -> prev;
                    neww -> next = h;
                    h -> prev = neww;
                    break;
                    }
                    else{
                        neww -> next = h;
                        h -> prev = neww;
                        neww -> prev = NULL;
                        head = neww;
                        break;
                    }
                }
                else{
                    h = h -> next;
                }
            }
            querytable[countoftable++] = *(neww -> querylist);
            (neww -> querylist)[0] -> query = a;
            (neww -> querylist)[0] -> prev = NULL;
            (neww -> querylist)[0] -> next = NULL;
            
            
        }
        else{
            QueryNode* newquery;
            CountNode* h;
            newquery = malloc(sizeof(QueryNode));
            newquery -> query = a;
            h = head;
            querytable[countoftable++] = newquery;
            while(1){
                if ((*h).count == number){
                    QueryNode* list;
                    list = (h -> querylist)[0]; 
                    while(1){
                        if (list -> next == NULL){
                            if (compare(a,(list -> query))){
                                if (list -> prev != NULL){
                                    list -> prev -> next = newquery;
                                    newquery -> prev = list -> prev;
                                    newquery -> next = list;
                                    list -> prev = newquery;
                                    break;
                                }
                                else{
                                    newquery -> next = list;
                                    list -> prev = newquery;
                                    newquery -> prev = NULL;
                                    (h -> querylist)[0] = newquery;
                                    break;
                                }
                            }
                            else{
                                newquery -> next = NULL;
                                newquery -> prev = list;
                                list -> next = newquery;
                                break;
                            }
                        }
                        else if (compare(a,(list -> query))){
                            if (list -> prev != NULL){
                                list -> prev -> next = newquery;
                                newquery -> prev = list -> prev;
                                newquery -> next = list;
                                list -> prev = newquery;
                                break;
                            }
                            else{
                                newquery -> next = list;
                                list -> prev = newquery;
                                newquery -> prev = NULL;
                                (h -> querylist)[0] = newquery;
                                break;
                            }
                        }
                        else
                            list = list -> next;
                    }
                    break;
                }
                else{
                    
                    h = h -> next;}
            }
        }
    }
    scanf("%d",&b);
    getchar();
    for(l=0;l<b;l++){
        char* str;
        char temp;
        str = malloc(sizeof(char)*6);
        j=0;
        while ((temp=getchar())!= EOF  && temp != ' ' ){
            str[j] = temp;
            j++;
        }
        str[j] = '\0';
        if(!istherestr(str,querytable)){
            if (countoftable == MAX_SIZE){
                char* cc;
                cc = (head -> querylist)[0] -> query;
                removefromtable(cc);
                if ((head -> querylist)[0] -> next == NULL){
                    head = head -> next;
                }
                else{
                    (head -> querylist)[0] = (head -> querylist)[0] -> next;
                    (head -> querylist)[0] -> prev = NULL;
                }
                
            }
                if(!(isthere(1,head))){
                    CountNode* neww;
                    neww = malloc(sizeof(CountNode));
                    (neww -> querylist) = malloc(sizeof(QueryNode*));
                    (neww -> querylist)[0] = malloc(sizeof(QueryNode));
                    (neww -> querylist)[0] -> query = str;
                    (neww -> querylist)[0] -> next = NULL;
                    (neww -> querylist)[0] -> prev = NULL;
                    (*neww).count = 1;
                    head -> prev = neww;
                    neww -> next = head;
                    neww -> prev = NULL;
                    head = neww;
                    if (countoftable == MAX_SIZE){
                        inserttable((neww -> querylist)[0]);
                    }
                    else {
                        querytable[countoftable++] = (neww -> querylist)[0];
                    }
                    
                }
                else{
                    QueryNode* newquery;
                    QueryNode* list;
                    CountNode* h;
                    newquery = malloc(sizeof(QueryNode));
                    newquery -> query = str;
                    h = head;
                    list = (head -> querylist)[0];
                    if (countoftable == MAX_SIZE){
                        inserttable(newquery);
                    }
                    else{
                        querytable[countoftable++] = newquery;
                    }
                    while(1){
                        if (list -> next == NULL){
                            if (compare(str,(list -> query))){
                                if (list -> prev != NULL){
                                    list -> prev -> next = newquery;
                                    newquery -> prev = list -> prev;
                                    newquery -> next = list;
                                    list -> prev = newquery;
                                    break;
                                }
                                else{
                                    newquery -> next = list;
                                    list -> prev = newquery;
                                    newquery -> prev = NULL;
                                    (h -> querylist)[0] = newquery;
                                    break;
                                }
                            }
                            else{
                                newquery -> next = NULL;
                                newquery -> prev = list;
                                list -> next = newquery;
                                break;
                            }
                        }
                        else if (compare(str,(list -> query))){
                            if (list -> prev != NULL){
                                list -> prev -> next = newquery;
                                newquery -> prev = list -> prev;
                                newquery -> next = list;
                                list -> prev = newquery;
                                break;
                            }
                            else{
                                newquery -> next = list;
                                list -> prev = newquery;
                                newquery -> prev = NULL;
                                (h -> querylist)[0] = newquery;
                                break;
                            }
                        }
                        else{
                            list = list -> next;
                        }
                    }
                }
        }
        else{
             CountNode* h;
             QueryNode* list;
             int a= 0;
             h = head;
             while(1){
                 if (a != 0)
                    break;
                 list = (h -> querylist)[0];
                 while(1){
                     if (list == NULL){
                         break;
                     }
                    else if (!strcmp(list -> query,str)){
                        a++;
                        insertquery(str,h);
                        deletequerynode(list,h);
                        
                        break;
                    }
                    else{
                        list = list -> next;
                    }
                }
                h = h -> next;
             }
        }
    }
    while(1){
        if (head == NULL){
            break;
        }
        printf("%d",head -> count);
        printt(head);
        head = head -> next;
    }
    return 0;
}