#include <stdio.h>
#include <stdlib.h>

char *Curpos;
char* p;
int* pp;
int len=0;
int space(){
    if (Curpos[0] == ' ' || Curpos[0] == '\n'){
        while(Curpos[0] == ' ' || Curpos[0] == '\n'){
            Curpos++;
        }
    }
    return 0;
}
int poww(int alt, int us){
    int result=1;
    while (us != 0){
        result*=2;
        us--;
    }
    return result;
}
int index(char x,char* p){
    int i=0;
    for(i=0;p[i]!='\0';i++){
        if (x == p[i]){
            return i;
        }
    }
    return -1;
}
int evalexp(){
    int left,right,t;
    char opr;
    space();
    if (Curpos[0] == '('){
        Curpos++;
        space();
        left = evalexp();
        space();
        opr = Curpos[0];
        Curpos++;
        space();
        right = evalexp();
        Curpos++;
        space();
        switch (opr){
            case '&': return left && right ;break;
            case '|': return left || right ;break;
            case '=': return left == right ;break;
            case '>': return left <= right ;break;
        }
    }
    else{

        if (Curpos[0] == '-'){
            Curpos++;
            space();
            if (index(Curpos[0],p) != -1){
                if (pp[index(Curpos[0],p)] == 0){
                    Curpos++;
                    space();
                    return 1;}
                else if (pp[index(Curpos[0],p)] == 1){
                    Curpos++;
                    space();
                    return 0;}}
            else {
                if (evalexp() == 0)
                    return 1;
                else
                    return 0;}
            
        }
        else{
        t = pp[index(Curpos[0],p)];
        Curpos++;
        space();
        return t;}
    }
    return 0;
}
int helper(){
    int number=0,i=0,j=0,result;
    float a;
    while(p[i] != '\0'){
        number++;
        i++;
    }
    pp = (int*) malloc(number*sizeof(int));
    for(i=0;i<poww(2,number);i++){
        for(j=0;j<number-1;j++){
            a = (float) i / poww(2,number-j-1);
            if ((int) a % 2 == 0){
                printf("T ");
                pp[j] = 1;
            }
            else{
                printf("F ");
                pp[j] = 0;
            }
        }
        if (i % 2 == 0){
            printf("T ");
            pp[number-1] = 1;
        }
        else{
            printf("F ");
            pp[number-1] = 0;
        }
        result = evalexp();
        Curpos -= len;
        if (result == 1){
            printf("T\n");
        }
        else
            printf("F\n");
    }
    return 0;
}



int main() {
    int i=0,j=0,k=0,h=0,min,index,x,a,d=0,numberofbyte=1;
    p = (char *) malloc(25*sizeof(char)+1);
    Curpos = (char *) malloc(1*sizeof(char));
    while((Curpos[i] = getchar())!=EOF){
        numberofbyte++;
        Curpos = (char *) realloc(Curpos,sizeof(char)*numberofbyte);
        i++;
        len++;
    }
    for(i=0,d=0;Curpos[i] != EOF ;i++,d=0){ 
        if (Curpos[i] >= 97 && Curpos[i] <= 122){
            for(a=0;a<j;a++){
                if(p[a] == Curpos[i]){
                    d++;
                    break;
                }
            }
            if (d == 0){
            p[j] = Curpos[i];
            j++;}
        }
    }
    p[j] = '\0';
    for(h=0,index=0;p[h] != '\0';h++,index = 0){
        for(k=h+1,min = p[h];p[k] != '\0';k++){
            if (p[k] < min){
                index = k;
                min =p[k];
            }
        }
        if (index != 0){
        x = p[h];
        p[h] = p[index];
        p[index] = x;}
    }
    for(i=0;p[i] != '\0';i++){
        printf("%c ",p[i]);
    }
    printf("R\n");
    return helper();
}



