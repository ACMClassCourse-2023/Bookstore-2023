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
    bool operator <(const Number &other) const{ return (*this).num<other.num; }
    bool operator >(const Number &other) const{ return (*this).num>other.num; }
    bool operator ==(const Number &other) const{ return (*this).num==other.num; }
    bool operator <=(const Number &other) const{ return (*this).num<=other.num; }
    bool operator >=(const Number &other) const{ return (*this).num>=other.num; }
    bool operator !=(const Number &other) const{ return (*this).num!=other.num; }
};
void debug(std::string str1){
    std::cout<<str1<<std::endl;
}
int main(){
    Memory<Number> memory;
    memory.initialise("test");
    debug("init_good?");
    int T,x,y;
    std::string s1,s2;
    std::cin>>T;
    while (T--){
        std::cin>>s1;
        if (s1 == "insert"){
            std::cin>>s2>>x;
            Number a(x);
            memory.add_Atom(s2,a);
        }else if (s1 == "delete"){
            std::cin>>s2>>x;
            Number a(x);
            memory.delete_Atom(s2,a);
        }else if (s1 == "find"){
            std::cin>>s2;
            std::vector<Number> val = memory.search(s2);
            if (val.size() > 0){
                for (int i = 0; i < val.size(); i++)
                    std::cout<<val[i].num<<' ';
                std::cout<<std::endl;
            }else{
                std::cout<<"null"<<std::endl;
            }
        }
        debug("One operation");
    }
    return 0;
}