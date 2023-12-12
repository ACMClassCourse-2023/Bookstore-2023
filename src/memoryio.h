//
// Created by skywa on 2023/12/10.
//

#ifndef BOOKSTORE_2023_MEMORYIO_H
#define BOOKSTORE_2023_MEMORYIO_H

#pragma once
#include <vector>
#include <fstream>
#include <ostream>
using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;
using ul = unsigned long;
//file_index: 存储链表结构
//file_value: 存储值
//file_pool: 可用空间
template<class T,int info_len = 5>
class Memory {
private:
    fstream file;
    string file_name,file_index,file_pool,file_value;
    const int block_size = 300;
    char index[65];
    int sizeofT = sizeof(T);
    int sizeofint = sizeof(int);
    int sizeofBlock = sizeofint * 5;
    int sizeofPtr = sizeofint * 4 + sizeof(index);
public:
    Memory() = default;
    Memory(const string& file_name) : file_name(file_name) {}

    void to_char(string str1, char (&arr)[65]){
        strncpy(arr , str1.c_str(), sizeof(arr) - 1);
        arr[sizeof(arr) - 1] = '\0';
    }

    int add_value(int val){
        T tmp(val);
        fstream file_tmp;
        file_tmp.open(file_value, std::ios::in | std::ios::out | std::ios::binary);
        file_tmp.seekp(0, std::ios::end);
        int ptr = file_tmp.tellp();
        file_tmp.write(reinterpret_cast<char *>(&tmp), sizeofT);
        file_tmp.close();
        return ptr;
    }
    T get_value(int ptr){
        fstream file_tmp;
        file_tmp.open(file_value, std::ios::in | std::ios::binary);
        file_tmp.seekg(ptr);
        T tmp;
        file_tmp.read((char*)&tmp, sizeofT);
        file_tmp.close();
        return tmp;
    }

    void initialise(string FN = "") {
        if (FN != "") {
            file_name = FN;
        }
        file_index = file_name + "_index.txt";
        file_value = file_name + "_value.txt";

        file.open(file_index, std::ios::out | std::ios::binary);
        int v_0 = 0, v_sz = block_size , v_2 = 2 , v_1 = 1, value_ptr;
        int head_block = sizeofint * info_len, end_block = sizeofint * (info_len + 5);
        //总数据据，总块数，块的大小，head块指针,end块指针
        file.write(reinterpret_cast<char *>(&v_2), sizeofint);//数据总数
        file.write(reinterpret_cast<char *>(&v_2), sizeofint);//总块数
        file.write(reinterpret_cast<char *>(&v_sz), sizeofint);//块的大小
        file.write(reinterpret_cast<char *>(&head_block), sizeofint);//head_ptr
        file.write(reinterpret_cast<char *>(&end_block), sizeofint);//end_ptr
        //写入两个块
        //head_block
        int ptr1 = info_len * sizeofint + sizeofBlock * 2;
        file.write(reinterpret_cast<char *>(&end_block), sizeofint);//block_nxt
        file.write(reinterpret_cast<char *>(&v_0), sizeofint);//block_bef
        file.write(reinterpret_cast<char *>(&ptr1), sizeofint);//block_head_ptr
        file.write(reinterpret_cast<char *>(&v_1), sizeofint);//block_num
        file.write(reinterpret_cast<char *>(&v_1), sizeofint);//block_id
        //end_block
        int ptr2 = info_len * sizeofint + sizeofBlock * 2 + sizeofPtr;
        file.write(reinterpret_cast<char *>(&v_0), sizeofint);//block_nxt
        file.write(reinterpret_cast<char *>(&head_block), sizeofint);//block_bef
        file.write(reinterpret_cast<char *>(&ptr2), sizeofint);//block_head_ptr
        file.write(reinterpret_cast<char *>(&v_1), sizeofint);//block_num
        file.write(reinterpret_cast<char *>(&v_2), sizeofint);//block_id
        //写入两个节点
        string str_min(64,' '), str_max(64,'~');
        to_char(str_min, index);
        file.write(reinterpret_cast<char *>(&ptr2), sizeofint);//ptr_nxt
        file.write(reinterpret_cast<char *>(&v_0), sizeofint);//ptr_bef
        file.write(reinterpret_cast<char *>(&v_1), sizeofint);//block_id
        file.write(reinterpret_cast<char *>(&str_min), sizeof(index));//最小的index
        value_ptr = add_value(0);//value为0
        file.write(reinterpret_cast<char *>(&value_ptr), sizeofint);

        to_char(str_max, index);
        file.write(reinterpret_cast<char *>(&v_0), sizeofint);//ptr_nxt
        file.write(reinterpret_cast<char *>(&ptr1), sizeofint);//ptr_bef
        file.write(reinterpret_cast<char *>(&v_2), sizeofint);//block_id
        file.write(reinterpret_cast<char *>(&str_max), sizeof(index));//最小的index
        value_ptr = add_value(0);//value为0
        file.write(reinterpret_cast<char *>(&value_ptr), sizeofint);

        file.close();
    }

    int next(int ptr){
        fstream file_tmp;
        file_tmp.open(file_index, std::ios::in | std::ios::binary);
        file.seekg(ptr,std::ios::beg);
        file.read((char*)&ptr, sizeofint);
        file_tmp.close();
        return ptr;
    }


    void push(string str1, T value){
        /*
        strncpy(index , str1.c_str(), sizeof(index) - 1);
        index[sizeof(index) - 1] = '\0';
         */
        to_char(str1, index);

        int pre_ptr = seek(str1, value);
        int nxt_ptr = next(pre_ptr);

        while (get_index(nxt_ptr) == str1 && get_value(nxt_ptr) < value){
            pre_ptr = nxt_ptr;
            nxt_ptr = next(nxt_ptr);
        }

        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);

        file.seekg(nxt_ptr + sizeofint, std::ios::beg);

        int block_ptr;
        file.read((char*)&block_ptr, sizeofint);
        //将新的值插入在pre和nxt之间 如果 块size>block_size 则裂块split()

        file.seekp(0, std::ios::end);
        int new_ptr = file.tellp();
        file.write(reinterpret_cast<char *>(&nxt_ptr),sizeofint);//next_ptr
        file.write(reinterpret_cast<char *>(&block_ptr),sizeofint);//块指针
        file.write(reinterpret_cast<char *>(&index), sizeof(index));//index
        file.write(reinterpret_cast<char *>(&value), sizeofT); //value

        //n+1;
        //更新pre
        file.seekp(pre_ptr + sizeofint, std::ios::beg);
        file.write(reinterpret_cast<char *>(&new_ptr),sizeofint);
        //更新block_size
        file.seekp(block_ptr + sizeofint * 2, std::ios::beg);



        file.close();
    }

    string get_index(int ptr){
        fstream file_tmp;
        file_tmp.open(file_index, std::ios::in | std::ios::binary);
        file_tmp.seekg(ptr + sizeofint * 2,std::ios::beg);
        char chararray[65];
        file_tmp.read((char*)&chararray, sizeof(chararray));
        string str(chararray);
        file_tmp.close();
        return str;
    }


    int seek(const string& str1){//返回index < str1的最后一个位置（如果为0表示str1应为head）
        fstream file_tmp;
        file_tmp.open(file_index, std::ios::in | std::ios::binary);

        file_tmp.seekg(sizeofint * info_len, std::ios::beg);
        int headptr = sizeofint * info_len,  nxtptr , valueptr;
        string nxt_index;
        file_tmp.read((char*)&nxtptr, sizeofint);

        while (nxtptr > 0){
            file_tmp.seekg(nxtptr + sizeofint, std::ios::beg);
            file_tmp.read((char*)&valueptr, sizeofint);
            nxt_index = get_index(valueptr);
            if (nxt_index >= str1) {
                break;
            }
            file_tmp.seekg(nxtptr, std::ios::beg);
            headptr = nxtptr;
            file_tmp.read((char*)&nxtptr, sizeofint);
        }
        //从head_ptr开始找
        file_tmp.seekg(headptr+sizeofint, std::ios::beg);
        file_tmp.read((char*)&headptr, sizeofint);
        file_tmp.seekg(headptr, std::ios::beg);
        file_tmp.read((char*)&nxtptr, sizeofint);
        while (nxtptr > 0 && get_index(nxtptr) < str1) {
            headptr = nxtptr;
            file_tmp.seekg(nxtptr, std::ios::beg);
            file_tmp.read((char *) &nxtptr, std::ios::beg);
        }
        file_tmp.close();
        return headptr;
    }

    std::vector<T> search(string str1){
        std::vector<T> ans = {};
        int ptr = seek(str1);
        while (ptr > 0 && )
    }

    void del(string str1, T value){
        int ptr = seek(str1);

    }

};




#endif //BOOKSTORE_2023_MEMORYIO_H
