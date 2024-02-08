#include "the3.h"

// do not add extra libraries, but you can define helper functions below.




/* 
PART 1
you are expected to call recursive_alignment (as the name suggests) recursively to find an alignment. 
initial call_count value given to you will be 0.
you should check if call_count >= 1000000, if so, set possible_alignment string to "STACK LIMIT REACHED", return INT_MIN (or anything - it will not be checked).
*/

std::pair <int,std::string> recursion_helper(std::string A, std::string B, std::string result, int gap, int mismatch, int match, int& call_count, int i, int j, int depth, int& flag){
    if (call_count >= 1000000){
        std::pair <int,std::string> temp;
        temp.second = "STACK LIMIT REACHED";
        flag = 1;
        temp.first = INT_MIN;
        return temp;
    }
    if(i <= -1 && j <= -1){
        std::pair <int,std::string> temp;
        temp.first = 0;
        temp.second = result;
        return temp;
    }
    else if(i <= -1){
        std::pair <int,std::string> temp;
        result.insert(0, "_");
        call_count++;
        temp = recursion_helper(A, B, result, gap, mismatch, match, call_count, i, j-1, depth, flag);
        temp.first -= gap;
        return temp;
    }
    else if(j <= -1){
        std::pair <int,std::string> temp;
        result.insert(0, ".");
        call_count++;
        temp = recursion_helper(A, B, result, gap, mismatch, match, call_count, i-1, j, depth, flag);
        temp.first -= gap;
        return temp;
    }
    std::pair <int,std::string> temp1;
    std::pair <int,std::string> temp2;
    std::pair <int,std::string> temp3;
    //first state
    int res=0;
    if(A[i] == B[j]){ 
        res = match;
        std::string s(1, A[i]);
        result.insert(0, s);
    }
    else{ 
        res = -mismatch;
        result.insert(0, "!");
    }
    call_count++;
    temp1 = recursion_helper(A, B, result, gap, mismatch, match, call_count, i-1, j-1, depth, flag);
    temp1.first += res;
    //second state
    result[0] = '.';
    call_count++;
    temp2 = recursion_helper(A, B, result, gap, mismatch, match, call_count, i-1, j, depth, flag);
    temp2.first -= gap;
    //third state
    result[0] = '_';
    call_count++;
    temp3 = recursion_helper(A, B, result, gap, mismatch, match, call_count, i, j-1, depth, flag);
    temp3.first -= gap;
    return std::max(std::max(temp1, temp2), temp3);
}







int recursive_alignment(std::string sequence_A, std::string sequence_B, int gap, int mismatch, int match, std::string &possible_alignment, int call_count){
    int highest_alignment_score, depth=0;
    std::string result;
    int flag=0;
    std::pair <int,std::string> temp = recursion_helper(sequence_A, sequence_B, result, gap, mismatch, match, call_count, sequence_A.size()-1, sequence_B.size()-1, depth, flag);
    if(flag) {
        possible_alignment = "STACK LIMIT REACHED";
    }
    else possible_alignment = temp.second;
    return temp.first;    
}

/* 
PART 2
you are expected to create a dynamic programming table to find the highest alignment score. 
then you will need to reconstruct a possible alignment string from the table.
*/
int dp_table_alignment(std::string sequence_A, std::string sequence_B, int gap, int mismatch, int match, std::string &possible_alignment){
    int highest_alignment_score;
    std::vector<std::vector<std::pair<int, char>>> table(sequence_A.size()+1, std::vector<std::pair<int, char>>(sequence_B.size()+1));
    std::pair<int, char> temp;
    temp.first = 0;
    table[0][0] = temp;
    for(int j=1; j<=sequence_B.size();j++){
        std::pair<int, char> t;
        t.first = j*-gap;
        t.second = '_';
        table[0][j] = t;
    }
    for(int j=1; j<=sequence_A.size();j++){
        std::pair<int, char> t;
        t.first = j*-gap;
        t.second = '.';
        table[j][0] = t;
    }
    for(int i=1;i<=sequence_A.size();i++){
        for(int j=1;j<=sequence_B.size();j++){
            int res1,res2,res3;
            int flag=0;
            if(sequence_A[i-1] == sequence_B[j-1]){
                res1 = match+table[i-1][j-1].first;
                flag=1;
            }
            else{
                res1 = -mismatch+table[i-1][j-1].first;
            }
            res2 = -gap + table[i][j-1].first;
            res3 = -gap + table[i-1][j].first;
            if(res1 >= res2 && res1 >= res3){
                if(flag == 1) {
                    std::pair<int, char> t1;
                    t1.first = res1;
                    t1.second = sequence_A[i-1];
                    table[i][j] = t1;
                }
                else{
                    std::pair<int, char> t1;
                    t1.first = res1;
                    t1.second = '!';
                    table[i][j] = t1;
                }
            }
            else if(res2 >= res1 && res2 >= res3){
                std::pair<int, char> t1;
                t1.first = res2;
                t1.second = '_';
                table[i][j] = t1;
            }
            else if(res3 >= res1 && res3 >= res2){
                std::pair<int, char> t1;
                t1.first = res3;
                t1.second = '.';
                table[i][j] = t1;
            }
        }
    }
    highest_alignment_score = table[sequence_A.size()][sequence_B.size()].first;
    int i=sequence_A.size(),j=sequence_B.size();
    while(i>0 || j>0){
        char t = table[i][j].second;
        std::string s(1,t);
        if(t == '_'){
            possible_alignment.insert(0, s);
            j--;
        }
        else if(t == '.'){
            possible_alignment.insert(0, s);
            i--;
        }
        else if(t == '!'){
            possible_alignment.insert(0, s);
            i--;j--;
        }
        else{
            possible_alignment.insert(0, s);
            i--;j--;
        }
    }
    return highest_alignment_score;    
}
