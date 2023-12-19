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
//index_name: 存储链表结构
//value_name: 存储值
//file_pool: 可用空间（实现中）

//for dubug only
template<typename T>
void debug(T arg){
    std::cout << arg << std::endl;
}
template<typename T, typename... Args>
void debug(T arg, Args... args){
    std::cout << arg << " ";
    debug(args...);
}

template<class T, int max_block = 1000, int info_len = 1>
class Memory {
private:
    fstream file;
    fstream file_value;
    string file_name, index_name, value_name;
    const int block_size = 5;
//    const int block_size = 300;
    const long long BASE = 131, MOD = 1e9+7;

    int sizeofT = sizeof(T);
    int sizeofint = sizeof(int), sizeofll = sizeof(long long);
    int sizeofHead = sizeofint * info_len;
    int sizeofAtom = sizeof(Atom_info);
    int sizeofBlock = sizeofAtom * block_size;
    int sizeofNode = sizeof(Link_Node);
    int Blockhead_ptr = sizeofint * info_len + max_block * sizeofNode;
    struct Link_Node{
        long long index;
        T value;
        int id, pre_node, nxt_node, size;
    };
    T T0 = T();
    using key_value_pair = std::pair<long long, T>;
    using Atom_info = std::pair<long long, int>;
public:
    Memory() = default;
    Memory(const string& FN) : file_name(FN) {}

    bool check_File_Exists(const std::string& filePath) {
        std::fstream file_tmp(filePath);
        return file_tmp.is_open();
    }

    int get_Node_ptr(int _id) const{
        return info_len * sizeofint + _id * sizeofNode;
    }
    int get_Block_ptr(int _id) const{
        return Blockhead_ptr + _id * sizeofBlock;
    }
    Link_Node get_Node(int _id){
        if (!file.is_open()) throw std::runtime_error("ERR::file not open when get_Node");
        file.seekg(get_Node_ptr(_id), std::ios::beg);
        Link_Node node;
        file.read((char*)&node, sizeofNode);
        return std::move(node);
    }

    std::vector<Atom_info> get_Block(int _id, int sz){
        if (!file.is_open()) throw std::runtime_error("ERR::file not open when get_all_Atom");
        int ptr = get_Block_ptr(_id);
        file.seekg(ptr, std::ios::beg);
        Atom_info arr[sz];
        file.read((char*)&arr, sizeof(arr));
        std::vector<Atom_info> ans(arr, arr + sz);
        return std::move(ans);
    }//得到_id的block内所有元素

    void override_Block(int _id, std::vector<Atom_info> values){
        if (!file.is_open()) throw std::runtime_error("ERR::file not open when override_Block");
        int sz = static_cast<int>(values.size());
        Atom_info arr[sz];
        std::copy(values.begin(), values.end(), arr);
        file.seekp(get_Block_ptr(_id), std::ios::beg);
        file.write(reinterpret_cast<char *>(&arr), sizeof(arr));
    }//覆写_id的block的内容，不修改linked-list

    void override_Node(int _id, Link_Node _node){
        if (!file.is_open()) throw std::runtime_error("ERR::file not open when override_Node");
        file.seekp(get_Node_ptr(_id), std::ios::beg);
        file.write(reinterpret_cast<char*>(&_node), sizeofNode);
    }

    void delete_Node(int _id){
        if (!file.is_open()) throw std::runtime_error("ERR::file not open when delete_Node");
        Link_Node node,cur_node = get_Node(_id);

        int pre_id = cur_node.pre_node;
        if (pre_id != -1){
            int pre_ptr = get_Node_ptr(pre_id);
            file.seekg(pre_ptr, std::ios::beg);
            file.read((char*)&node, sizeofNode);
            node.nxt_node = cur_node.nxt_node;
            file.seekp(pre_ptr, std::ios::beg);
            file.write(reinterpret_cast<char*>(&node), sizeofNode);
        }
        int nxt_id = cur_node.nxt_node;
        if (nxt_id != -1){
            int nxt_ptr = get_Node_ptr(nxt_id);
            file.seekg(nxt_ptr, std::ios::beg);
            file.read((char*)&node, sizeofNode);
            node.pre_node = cur_node.pre_node;
            file.seekp(nxt_ptr, std::ios::beg);
            file.write(reinterpret_cast<char *>(&node), sizeofNode);
        }
    }

    std::vector<Link_Node> get_Nodes(){
        if (!file.is_open()) throw std::runtime_error("ERR::file not open when get_Nodes");
        file.seekg(sizeofint * info_len);
        Link_Node all_nodes[max_block];
        file.read((char*)&all_nodes, sizeof(all_nodes));
        std::vector<Link_Node> values(all_nodes, all_nodes + max_block);
        return std::move(values);
    }//调取linked-list中所有nodes, 以vector形式返回

    int get_empty_Block(){
        if (!file.is_open()) throw std::runtime_error("ERR::file not open when get_empty_Block");
        std::vector<Link_Node> nodes(get_Nodes());
        int ptr = 0;
        int flag[max_block];
        memset(flag, 0, sizeof(flag));
        Link_Node cur_node;
        while (ptr != -1){
            flag[ptr] = 1;
            cur_node = nodes[ptr];
            ptr = cur_node.nxt_node;
        }
        for (int i = 0; i < max_block; i++)
            if (!flag[i]){
                ptr = i;
                break;
            }
        return ptr;
    }
    void set_new_Block(int _id){
        if (!file.is_open()) throw std::runtime_error("ERR::file not open when set_new_Block");
        file.seekp(get_Block_ptr(_id), std::ios::beg);
        Atom_info arr[block_size];
        memset(arr, 0, sizeof(arr));
        file.write(reinterpret_cast<char *>(&arr), sizeof(arr));
    }

    T get_value(int ptr){
        fstream file_tmp(value_name, std::ios::in | std::ios::binary);
        file_tmp.seekg(ptr, std::ios::beg);
        T tmp;
        file_tmp.read((char*)&tmp, sizeofT);
        file_tmp.close();
        return tmp;
    }
    std::vector<T> get_values(std::vector<int> ptrs){
        fstream file_tmp(value_name, std::ios::in | std::ios::binary);
        std::vector<T> values = {};
        int sz = static_cast<int>(ptrs.size());
        T tmp;
        values.reserve(sz);
        for (int i = 0; i < sz; i++){
            file_tmp.seekg(ptrs[i], std::ios::beg);
            file_tmp.read((char*)&tmp, sizeofT);
            values.push_back(tmp);
        }
        file_tmp.close();
        return std::move(values);
    }

    long long get_Hash(const string& str1){
        long long ha = 0;
        for (int i = 0; i < str1.length(); i++)
            ha = (ha + (long long)(str1[i]) * BASE) % MOD;
        return ha;
    }

    int new_Value(T value){
        fstream file_tmp(value_name, std::ios::in | std::ios::out | std::ios::binary);
        file_tmp.seekp(0, std::ios::end);
        int ptr = file_tmp.tellp();
        file_tmp.write(reinterpret_cast<char *>(&value), sizeofT);
        file_tmp.close();
        return ptr;
    }

    void initialise_file(){
        int tmpt = 0;
        file.open(index_name, std::ios::out);
        for (int i = 0; i < info_len; i++)
            file.write(reinterpret_cast<char *>(&tmpt), sizeofint);
        file.close();

        file.open(value_name, std::ios::out);
        for (int i = 0; i < 1; i++)
            file.write(reinterpret_cast<char *>(&tmpt), sizeofint);
        file.close();
    }
    void initialise(string FN = "", int clear_file = 0) {
        if (!FN.empty())
            file_name = FN;
        index_name = "./" + file_name + "_index.txt";
        value_name = "./" + file_name + "_value.txt";

        if (check_File_Exists(index_name) && check_File_Exists(value_name) && (!clear_file)) return; //文件已经存在就无需初始化

        initialise_file();

        file.open(index_name, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()){
            file.open(index_name, std::ios::in | std::ios::out | std::ios::binary);
            file.close();
            file.open(index_name, std::ios::in | std::ios::out | std::ios::binary);
        }
        //head of file
        int value1 = 1, valuesz = block_size, valueheadptr = sizeofHead, value0 = 0;
        file.seekp(0,std::ios::beg);
        file.write(reinterpret_cast<char *>(&valueheadptr), sizeofint);
        Link_Node init_nodes[max_block];
        Link_Node node1;
        memset(init_nodes, 0, sizeof(init_nodes));
        node1.size = 2;
        node1.nxt_node = -1;
        node1.id = 0;
        node1.index = 0;
        node1.value = T0;
        node1.pre_node = -1;
        init_nodes[0] = node1;
        file.write(reinterpret_cast<char *>(&init_nodes), sizeof(init_nodes));
        //the first block (with two Atoms:index_min and index_max)
        int ptr1 = new_Value(T0), ptr2 = new_Value(T0);
        std::vector<Atom_info> init_arr ={std::make_pair(0, ptr1), std::make_pair(MOD, ptr2)};
        override_Block(0, init_arr);
        file.close();
    }


    Link_Node find_Block(long long _index){//**需保证file已处open状态**
        if (!file.is_open()) throw std::runtime_error("ERR::file not open when find_Block");
        std::vector<Link_Node> nodes = get_Nodes();
        int sz = nodes.size();
        Link_Node cur_node;
        int ptr = 0, ans = 0;
        while (ptr != -1){
            cur_node = nodes[ptr];
            if (cur_node.index >= _index) break;
            ans = ptr;
            ptr = cur_node.nxt_node;
        }
        return nodes[ans];
    }//返回其lower_bound（<=该元素的最后一个元素）所在的块的id
    Link_Node find_Block(long long _index, T _value){//**需保证file已处open状态**
        if (!file.is_open()) throw std::runtime_error("ERR::file not open when find_Block");
        std::vector<Link_Node> nodes = get_Nodes();
        int sz = nodes.size();
        Link_Node cur_node;
        int ptr = 0, ans = 0;
        while (ptr != -1){
            cur_node = nodes[ptr];
            if (cur_node.index > _index || cur_node.index == _index && cur_node.value > _value) break;
            ans = ptr;
            ptr = cur_node.nxt_node;
        }
        return nodes[ans];
    }//返回其lower_bound（<=该元素的最后一个元素）所在的块的信息



    key_value_pair get_key_value_pair(int ptr){
        if (!file.is_open()) throw std::runtime_error("ERR::file not open when get_key_value_pair");
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

    void insert_Atom(string _str1, const T& _value){
        long long index = get_Hash(_str1);
        file.open(index_name, std::ios::in | std::ios::out | std::ios::binary);
        Link_Node node_info = find_Block(index, _value);
        int Block_id = node_info.id;
        std::vector<Atom_info> values = get_Block(Block_id, node_info.size);
//        debug("values:");for (auto i:values){debug("   ",i.first, i.second);}debug("values_end.");
        int value_ptr = new_Value(_value);
        int sz = static_cast<int>(values.size());
        std::vector<int> values_ptr = {};
        values_ptr.reserve(sz);
        for (int i = 0; i < sz; i++){
            values_ptr.push_back(values[i].second);
        }
        std::vector<T> T_values = get_values(values_ptr);
        int pre, is_same = 0;
        for (pre = 0; pre < sz; pre++){
            if (values[pre].first > index || (values[pre].first == index) && (T_values[pre] >= _value)){
                if ((values[pre].first == index) && (T_values[pre] == _value)) is_same = 1;
                break;
            }
        }

        if (is_same){
            file.close();
            return;
        }

        Atom_info insert_val = std::make_pair(index, value_ptr);
        if (pre == sz){
            values.push_back(insert_val);
        }else{
            values.insert(values.begin() + pre, insert_val);
        }
//        debug("values:");for (auto i:values){debug("   ",i.first, i.second);}debug("values_end.");

        if (sz >= block_size){
            auto mid  = values.begin() + static_cast<int>(values.size()) / 2;
            std::vector <Atom_info> half1(values.begin(), mid);
            std::vector <Atom_info> half2(mid, values.end());
            //2 parts of values

            int new_id = get_empty_Block();
            Link_Node node1 = node_info, node2;

            node2.nxt_node = node1.nxt_node;
            node2.pre_node = node1.id;
            node2.size = half2.size();
            node2.id = new_id;
            node2.index = half2[0].first;
            node2.value = get_value(half2[0].second);

            node1.nxt_node = new_id;
            node1.size = half1.size();

            override_Node(node1.id, node1);
            override_Block(node1.id, half1);
            override_Node(node2.id, node2);
            override_Block(node2.id, half2);
        }else{
            node_info.size++;
            override_Node(Block_id, node_info);
            override_Block(Block_id, values);
        }
        //不可能在block开头插入，无需修改link-list
        file.close();
    }


    void delete_Atom(string str1, T _value){
        long long index = get_Hash(str1);
        file.open(index_name, std::ios::in | std::ios::out | std::ios::binary);
        Link_Node node_info = find_Block(index, _value);
        int Block_id = node_info.id;
//        debug("delete::Block_id::", Block_id);
        std::vector <Atom_info> values = get_Block(Block_id, node_info.size);
        int sz = static_cast<int>(values.size());
        int del_pos = -1;
        for (int i = 0; i < sz; i++){
            if (values[i].first < index) continue;
            if (values[i].first == index)
                if (get_value(values[i].second) == _value){
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

        if (values.empty()){
            delete_Node(Block_id);
            file.close();
            return;
        }//销毁块

        Link_Node node = node_info;
        node.size--;
        if (del_pos == 0){
            node.index = values[0].first;
            node.value = get_value(values[0].second);
        }
        override_Node(Block_id, node);
        override_Block(Block_id, values);
        //TODO: 如果和前后块size加起来不到block_size的2/3，则合并块
        file.close();
    }

    std::vector<T> search(string str1){
        long long index = get_Hash(str1);
        file.open(index_name, std::ios::in | std::ios::out | std::ios::binary);
        Link_Node node_info = find_Block(index);
        int Block_id = node_info.id;
        std::vector<Atom_info> values;
        std::vector<int> ptrs = {};
        std::vector<T> ans_T = {};
        int flag = 0;
        while (Block_id != -1){
            values = get_Block(Block_id, node_info.size);
            for (auto i: values){
                if (i.first == index) {
                    ptrs.push_back(i.second);
                }else if (i.first > index){
                    flag = 1;
                    break;
                }
            }
            if (flag || node_info.nxt_node == -1) break;
            node_info = get_Node(node_info.nxt_node);
            Block_id = node_info.id;
        }
        ans_T = get_values(ptrs);
        file.close();
        return ans_T;
    }

    //for debug only
    /*
    void print_all(){
        file.open(index_name, std::ios::in | std::ios::out | std::ios::binary);
        int tmp;long long tmp2;int tnum;
        file.seekg(0, std::ios::beg);
        for (int i = 0; i < 4; i++){
            file.read((char*)&tmp,sizeofint);std::cout<<tmp<<' ';std::cout<<std::endl;
        }
        std::cout<<std::endl;
        for (int i = 0; i < 4; i++){
            file.read((char*)&tmp,sizeofint);std::cout<<tmp<<' ';std::cout<<std::endl;
        }
    }*/
};

#endif //BOOKSTORE_2023_MEMORYIO_H
