//
// Created by skywa on 2023/12/10.
//

#ifndef BOOKSTORE_2023_MEMORYIO_H
#define BOOKSTORE_2023_MEMORYIO_H

#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <ostream>
using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;
//file_index: 存储链表结构
//file_value: 存储值
//file_pool: 可用空间（实现中）

//for dubug only
template<typename T>
void debug(T arg){
    std::cout << arg << std::endl;
}
template<typename T, typename... Args>
void debug(T arg, Args... args){
    std::cout << arg << ", ";
    debug(args...);
}


template<class T,int info_len = 4>
class Memory {
private:
    fstream file;
    string file_name,file_index,file_pool,file_value;
    const int block_size = 120;
    const long long BASE = 131, MOD = 1e9+7;

    int sizeofT = sizeof(T);
    int sizeofint = sizeof(int), sizeofll = sizeof(long long);
    int sizeofHead = sizeofint * info_len;
    int sizeofAtom = sizeofint + sizeofll;
    int sizeofBlock = sizeofint * 4 + sizeofAtom * block_size;
    int Block_cnt = 1;
    struct Block_Info{
        long long index;
        int ptr, nxt_ptr, size, id, value_ptr, pre_ptr;
    };
    using key_value_pair = std::pair<long long, T>;
    using Atom_info = std::pair<long long, int>;
public:
    Memory() = default;
    Memory(const string& FN) : file_name(FN) {}

    bool check_File_Exists(const std::string& filePath) {
        std::fstream file_tmp(filePath);
        return file_tmp.is_open();
    }

    Block_Info get_Block_Info(int ptr){//**需保证file已处open状态**
        if (!file.is_open()) throw std::runtime_error("ERR::file not open when get_Block_Info");
        int cache_ptrp = file.tellp(), cache_ptrg = file.tellg();

        Block_Info Info;
        Info.ptr = ptr;
        file.seekg(ptr, std::ios::beg);
        file.read((char*)&Info.nxt_ptr, sizeofint);
        file.read((char*)&Info.size, sizeofint);
        file.read((char*)&Info.pre_ptr, sizeofint);
        file.read((char*)&Info.id, sizeofint);
        file.read((char*)&Info.index, sizeofll);
        file.read((char*)&Info.value_ptr, sizeofint);

        file.seekp(cache_ptrp, std::ios::beg);
        file.seekg(cache_ptrg, std::ios::beg);
        return Info;
    }

    int next_ptr(int ptr){//**需保证file已处open状态**
        if (!file.is_open()) throw std::runtime_error("ERR::file not open when get_Empty_Block");
//        int cache_ptrp = file.tellp(), cache_ptrg = file.tellg();

        int Nxt;
        file.seekg(ptr, std::ios::beg);
        file.read((char*)&Nxt, sizeofint);

//        file.seekp(cache_ptrp, std::ios::beg);
//        file.seekg(cache_ptrg, std::ios::beg);
        return Nxt;
    }
    int get_Empty_Block(){ //**需保证file已处open状态**
        if (!file.is_open()) throw std::runtime_error("ERR::file not open when get_Empty_Block");
//        int cache_ptrp = file.tellp(), cache_ptrg = file.tellg();

        file.seekg(sizeofint * 3, std::ios::beg);
        int nxt;
        file.read((char*)&nxt, sizeofint);
//        debug("NXt:"); debug(nxt);
        if (nxt == 0) {
            file.seekg(0, std::ios::end); //WRONG!!!
            nxt = file.tellg();
        } else {
            int new_nxt = next_ptr(nxt);
            file.seekp(sizeofint * 3, std::ios::beg);
            file.write(reinterpret_cast<char *>(&new_nxt), sizeofint);
        }

//        file.seekp(cache_ptrp, std::ios::beg);
//        file.seekg(cache_ptrg, std::ios::beg);
        return nxt;
    }//返回第一个可用的Empty

    long long get_Hash(const string& str1){
        long long ha = 0;
        for (int i = 0; i < str1.length(); i++)
            ha = (ha + (long long)(str1[i]) * BASE) % MOD;
        return ha;
    }

    void set_new_Block(int ptr, int id, int pre_ptr=0, int nxt_ptr=0, int size = 0){ //**需保证file已处open状态**
        if (!file.is_open()) throw std::runtime_error("ERR::file not open when new_Block");
//        int cache_ptrp = file.tellp(), cache_ptrg = file.tellg();

        file.seekp(ptr, std::ios::beg);
        file.write(reinterpret_cast<char *>(&nxt_ptr), sizeofint);
        file.write(reinterpret_cast<char *>(&size), sizeofint);
        file.write(reinterpret_cast<char *>(&pre_ptr), sizeofint);
        file.write(reinterpret_cast<char *>(&id), sizeofint);
        long long tmp1 = 0; int tmp2 = 0;
        for (int i = 0; i < block_size; i++){
            file.write(reinterpret_cast<char *>(&tmp1), sizeofll);
            file.write(reinterpret_cast<char *>(&tmp2), sizeofint);
        }
        int block_num;
        file.seekg(0, std::ios::beg);
        file.read((char*)&block_num, sizeofint);
        block_num++;
        file.seekp(0, std::ios::beg);
        file.write(reinterpret_cast<char *>(&block_num), sizeofint);

//        file.seekp(cache_ptrp, std::ios::beg);
//        file.seekg(cache_ptrg, std::ios::beg);
    }//在ptr位置新建Block的头信息，传入参数：ptr,id,pre_ptr,nxt_ptr,size

    void override_Atom(int ptr, long long index, T value){//**需保证file已处open状态**
//        if (!file.is_open()) throw std::runtime_error("ERR::file not open when override_Atom");
//        int cache_ptrp = file.tellp(), cache_ptrg = file.tellg();
        file.seekp(ptr, std::ios::beg);
        file.write(reinterpret_cast<char *>(&index), sizeofll);
        file.write(reinterpret_cast<char *>(&value), sizeofT);
//        file.seekp(cache_ptrp, std::ios::beg);
//        file.seekg(cache_ptrg, std::ios::beg);
    }

    void initialise(string FN = "", int clear_file = 0) {
        if (!FN.empty()) {
            file_name = FN;
        }
        file_index = "./" + file_name + "_index.txt";
        file_value = "./" + file_name + "_value.txt";

        if (check_File_Exists(file_index) && check_File_Exists(file_value) && (!clear_file)) return; //文件已经存在就无需初始化

        int tmpt = 0;
        file.open(file_index, std::ios::out);
        for (int i = 0; i < info_len; i++)
            file.write(reinterpret_cast<char *>(&tmpt), sizeofint);
        file.close();
        file.open(file_value, std::ios::out);
        for (int i = 0; i < 1; i++)
            file.write(reinterpret_cast<char *>(&tmpt), sizeofint);
        file.close();

        file.open(file_index, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()){
            file.open(file_index, std::ios::in | std::ios::out | std::ios::binary);
            file.close();
            file.open(file_index, std::ios::in | std::ios::out | std::ios::binary);
        }
        //head of file
        int value1 = 1, valuesz = block_size, valueheadptr = sizeofHead, value0 = 0;
        file.seekp(0,std::ios::beg);
        file.write(reinterpret_cast<char *>(&value1), sizeofint);
        file.write(reinterpret_cast<char *>(&valuesz), sizeofint);
        file.write(reinterpret_cast<char *>(&valueheadptr), sizeofint);
        file.write(reinterpret_cast<char *>(&value0), sizeofint);
        file.flush();
        //the first block (with two Atoms:index_min and index_max)
        int new_ptr = get_Empty_Block();
//        debug("init_new_ptr:");debug(new_ptr);
        set_new_Block(new_ptr, 1, 0, 0, 2); //id:1_base
        T v0(0);
        override_Atom(new_ptr + sizeofint * 4, 0, v0);
        override_Atom(new_ptr + sizeofint * 4 + sizeofAtom, MOD, v0);

        file.close();
    }

    T get_value(int ptr){
        fstream file_tmp(file_value, std::ios::in | std::ios::binary);
        file_tmp.seekg(ptr);
        T tmp;
        file_tmp.read((char*)&tmp, sizeofT);
        file_tmp.close();
        return tmp;
    }

    key_value_pair get_key_value_pair(int ptr){
//        if (!file.is_open()) throw std::runtime_error("ERR::file not open when get_key_value_pair");
//        int cache_ptrp = file.tellp(), cache_ptrg = file.tellg();

        key_value_pair ans;
        int value_ptr;
        file.seekg(ptr, std::ios::beg);
        file.read((char*)&ans.first, sizeofll);
        file.read((char*)&value_ptr, sizeofint);
        ans.second = get_value(value_ptr);

//        file.seekp(cache_ptrp, std::ios::beg);
//        file.seekg(cache_ptrg, std::ios::beg);
        return ans;
    }

    int find_Block(long long index){//**需保证file已处open状态**
        if (!file.is_open()) throw std::runtime_error("ERR::file not open when find_Block");
//        int cache_ptrp = file.tellp(), cache_ptrg = file.tellg();
        int ptr, ans;
        file.seekg(sizeofint * 2, std::ios::beg);
        file.read((char*)&ptr, sizeofint);
        long long Block_index;
        while (ptr > 0){
            file.seekg(ptr + sizeofint * 4, std::ios::beg);
            file.read((char*)&Block_index, sizeofll);
            if (Block_index >= index) break;
            ans = ptr;
            ptr = next_ptr(ptr);
        }
//        file.seekp(cache_ptrp, std::ios::beg);
//        file.seekg(cache_ptrg, std::ios::beg);
        return ans;
    }//返回其lower_bound（<=该元素的最后一个元素）所在的块的头指针
    int find_Block(long long index, T value){//**需保证file已处open状态**
        if (!file.is_open()) throw std::runtime_error("ERR::file not open when find_Block");
//        int cache_ptrp = file.tellp(), cache_ptrg = file.tellg();
        int ptr, ans;
        file.seekg(sizeofint * 2, std::ios::beg);
        file.read((char*)&ptr, sizeofint);
        while (ptr > 0){
            key_value_pair Pair = get_key_value_pair(ptr + sizeofint * 4);
            if (Pair.first > index || Pair.first == index && Pair.second > value) break;
            ans = ptr;
            ptr = next_ptr(ptr);
        }
//        file.seekp(cache_ptrp, std::ios::beg);
//        file.seekg(cache_ptrg, std::ios::beg);
        return ans;
    }//返回其lower_bound（<=该元素的最后一个元素）所在的块的头指针

    std::vector<Atom_info> get_all_Atom(int ptr){//**需保证file已处open状态**
        if (!file.is_open()) throw std::runtime_error("ERR::file not open when get_all_Atom");
//        int cache_ptrp = file.tellp(), cache_ptrg = file.tellg();

        int sz,tmp2;
        long long tmp1;
        std::vector <Atom_info> ans = {};
        file.seekg(ptr + sizeofint, std::ios::beg);
        file.read((char*)&sz, sizeofint);
        file.seekg(ptr + sizeofint * 4, std::ios::beg);
        for (int i = 0; i <  sz; i++){
            file.read((char*)&tmp1, sizeofll);
            file.read((char*)&tmp2, sizeofint);
            ans.push_back(std::make_pair(tmp1, tmp2));
        }

//        file.seekp(cache_ptrp, std::ios::beg);
//        file.seekg(cache_ptrg, std::ios::beg);
        return ans;
    }

    int new_Value(T value){
        fstream file_tmp(file_value, std::ios::in | std::ios::out | std::ios::binary);
        file_tmp.seekp(0, std::ios::end);
        int ptr = file_tmp.tellp();
        file_tmp.write(reinterpret_cast<char *>(&value), sizeofT);
        file_tmp.close();
        return ptr;
    }

    void override_Block(int ptr, const std::vector<Atom_info>& values, int nxt_ptr = 0){//**需保证file已处open状态**
        if (!file.is_open()) throw std::runtime_error("ERR::file not open when get_all_Atom");
//        int cache_ptrp = file.tellp(), cache_ptrg = file.tellg();

        int sz = values.size();
        file.seekp(ptr + sizeofint, std::ios::beg);
        file.write(reinterpret_cast<char *>(&sz), sizeofint);
        file.seekp(ptr + sizeofint * 4, std::ios::beg);
        long long tmp1; int tmp2;
        for (int i = 0; i < sz ; i++){
            tmp1 = values[i].first; tmp2 = values[i].second;
            file.write(reinterpret_cast<char *>(&tmp1), sizeofll);
            file.write(reinterpret_cast<char *>(&tmp2), sizeofint);
        }
        if (nxt_ptr != 0){
            file.seekp(ptr, std::ios::beg);
            file.write(reinterpret_cast<char *>(&nxt_ptr), sizeofint);
        }//需要修改nxt_ptr

//        file.seekp(cache_ptrp, std::ios::beg);
//        file.seekg(cache_ptrg, std::ios::beg);
    }

    void add_Atom(string str1, T value){
        long long index = get_Hash(str1);
        file.open(file_index, std::ios::in | std::ios::out | std::ios::binary);
        int Block_ptr = find_Block(index, value);
        std::vector <Atom_info> values = get_all_Atom(Block_ptr);
        int value_ptr = new_Value(value);
        int sz = values.size();
        int pre,is_same = 0;
        for (pre = 0; pre < sz; pre++)
            if (values[pre].first > index || (values[pre].first == index) && (get_value(values[pre].second) >= value)){
                if ((values[pre].first == index) && (get_value(values[pre].second) == value)) is_same = 1;
                break;
            }
        if (is_same){
            file.close();
            return;
        }
        if (pre == sz){
            values.push_back(std::make_pair(index, value_ptr));
        }else{
            values.insert(values.begin() + pre, std::make_pair(index, value_ptr));
        }

        if (sz >= block_size){
            Block_Info info = get_Block_Info(Block_ptr);
            auto mid  = values.begin() + values.size() / 2;
            std::vector <Atom_info> half1(values.begin(), mid);
            std::vector <Atom_info> half2(mid, values.end());

            int new_Block_ptr = get_Empty_Block();
            set_new_Block(new_Block_ptr, (++Block_cnt), Block_ptr, info.nxt_ptr, half2.size());
            override_Block(new_Block_ptr, half2);

            override_Block(Block_ptr, half1, new_Block_ptr); //修改前半部分
            //从中间split
        }else{
            override_Block(Block_ptr, values);
        }
        file.close();
    }
    void delete_Block(int ptr){//**需保证file已处open状态**
        if (!file.is_open()) throw std::runtime_error("ERR::file not open when delete_Block");
//        int cache_ptrp = file.tellp(), cache_ptrg = file.tellg();
        //
        int tmp;
        Block_Info info = get_Block_Info(ptr);
        if (info.pre_ptr > 0){
            file.seekp(info.pre_ptr, std::ios::beg);
            file.write(reinterpret_cast<char *>(&info.nxt_ptr), sizeofint);
        }
        if (info.nxt_ptr > 0){
            file.seekp(info.nxt_ptr + sizeofint * 2, std::ios::beg);
            file.write(reinterpret_cast<char *>(&info.pre_ptr), sizeofint);
        }
        //更改前后块指针
        file.seekg(sizeofint * 3, std::ios::beg);
        file.read((char*)&tmp,sizeofint);
        file.seekp(sizeofint * 3, std::ios::beg);
        file.write(reinterpret_cast<char *>(&ptr), sizeofint);
        file.seekp(ptr, std::ios::beg);
        file.write(reinterpret_cast<char *>(&tmp), sizeofint);
        //加入pool
        file.seekg(0, std::ios::beg);
        file.read((char*)&tmp,sizeofint);
        tmp--;
        file.seekp(0, std::ios::beg);
        file.write(reinterpret_cast<char *>(&tmp), sizeofint);
        //更改block总数
//        file.seekp(cache_ptrp, std::ios::beg);
//        file.seekg(cache_ptrg, std::ios::beg);
    }

    void delete_Atom(string str1, T value){
        long long index = get_Hash(str1);
        file.open(file_index, std::ios::in | std::ios::out | std::ios::binary);
        int Block_ptr = find_Block(index, value);
//        debug("Block_ptr", Block_ptr);
        Block_Info info = get_Block_Info(Block_ptr);
        std::vector <Atom_info> values = get_all_Atom(Block_ptr);
        int sz = values.size();
        int del_pos = -1;
        for (int i = 0; i < sz; i++){
            if (values[i].first < index) continue;
            if (values[i].first == index)
                if (get_value(values[i].second) == value){
                    del_pos = i;
                    break;
                }
            if (values[i].first > index) break;
        }
        if (del_pos == -1){
            file.close();
            return;
        }//未找到则直接关文件退出
        values.erase(values.begin() + del_pos);
        //values更新完成
        if (values.size() == 0){
            delete_Block(Block_ptr);
        }//销毁块

        override_Block(Block_ptr, values);
        //TODO: 如果和前后块size加起来不到block_size的2/3，则合并块
        file.close();
    }

    std::vector<T> search(string str1){
        long long index = get_Hash(str1);
        file.open(file_index, std::ios::in | std::ios::out | std::ios::binary);
        int Block_ptr = find_Block(index), sz;
        std::vector <Atom_info> values, ans = {};
        std::vector <T> ans_T = {};
        int flag = 0;
        while (Block_ptr != 0){
            values = get_all_Atom(Block_ptr);
            sz = values.size();
            for (int i = 0; i < sz; i++){
//                debug(values[i].first);
                if (values[i].first == index) ans.push_back(values[i]);
                if (values[i].first > index) {
                    flag = 1;
                    break;
                }
            }
            if (flag) break;
            Block_ptr = next_ptr(Block_ptr);
        }
        sz = ans.size();
        for (int i = 0; i < sz; i++)
            ans_T.push_back(get_value(ans[i].second));
        file.close();
        return ans_T;
    }

};

#endif //BOOKSTORE_2023_MEMORYIO_H
