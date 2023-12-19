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
    Number():num(0) {}
    Number(int b):num(b) {}
    bool operator <(const Number &other) const{ return (*this).num<other.num; }
    bool operator >(const Number &other) const{ return (*this).num>other.num; }
    bool operator ==(const Number &other) const{ return (*this).num==other.num; }
    bool operator <=(const Number &other) const{ return (*this).num<=other.num; }
    bool operator >=(const Number &other) const{ return (*this).num>=other.num; }
    bool operator !=(const Number &other) const{ return (*this).num!=other.num; }
};



int main(){
    Memory<Number> memory;
    memory.initialise("test",0);
//    debug("__init__done__");
    int T,x,y;
    std::string s1,s2;
    std::cin>>T;
    while (T--){
        std::cin>>s1;
        if (s1 == "insert"){
            std::cin>>s2>>x;
            Number a(x);
            memory.insert_Atom(s2,a);
        }else if (s1 == "delete"){
            std::cin>>s2>>x;
            Number a(x);
            memory.delete_Atom(s2,a);
        }else if (s1 == "find"){
            std::cin>>s2;
            std::vector<Number> val = memory.search(s2);
            if (!val.empty()){
                for (auto i:val)
                    std::cout<<i.num<<' ';
                std::cout<<std::endl;
            }else{
                std::cout<<"null"<<std::endl;
            }
        }
    }
    return 0;
}