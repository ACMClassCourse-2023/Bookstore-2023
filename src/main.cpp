//
// Created by skywa on 2023/12/10.
//
#include <iostream>
#include <cstring>
#include <cstdio>
#include <vector>
#include "memoryio.h"
class Number{
public:
    int num;
    Number(int b=0):num(b){}
    bool operator <(const Number &other) const{
        return (*this).num<other.num;
    }
};
int main(){
    Memory<Number> memory;
    memory.initialise("test");
    int T,x,y;
    std::string s1,s2;
    std::cin>>T;
    while (T--){
        std::cin>>s1;
        if (s1 == "insert"){
            std::cin>>s2>>x;
            Number a(x);
            memory.push(s2,a);
        }else if (s1 == "delete"){
            std::cin>>s2>>x;
            memory.del(s2,x);
        }else if (s1 == "find"){
            std::cin>>s1;
            std::vector<Number> val = memory.search(s1);
        }
    }
    return 0;
}