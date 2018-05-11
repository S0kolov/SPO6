//
// Created by sasha on 01.05.2018.
//
#pragma once
#ifndef SPO6_POOL_H
#define SPO6_POOL_H

#include <iostream>
#include <set>

#define plase_pair pair<void* ,bool>
#define item_pair pair<void*,int>


using namespace std;

enum status {IN_MEMORY ,ON_DISK };

struct List{
    unsigned int id;
    unsigned int size;
    char * pointer;
    List * perent;
    List * next;
    status st;
};

auto str_status = [](status i){
    if(i == status::IN_MEMORY){
        return "IN_MEMORY";
    }else{
        return "ON_DISK";
    }
};



class Pool {
private:
    int _counter;
    const int _max_count;

    unsigned int _buf_size;
    char * _buffer;
    unsigned int _id_counet;
    List * _free_mem;
    List * _busy_mem;

    FILE * _swap_file;
    const char * _swap_file_name;

    List* last(List* elem);

    List * init_struct(List * item, unsigned int id, unsigned int size,char * pointer,List * next,List * perent);

    plase_pair find_plase(unsigned int size, int id = -1,bool f = false);

    explicit Pool(unsigned int buf_size );

    void init_mem(List * ls);

    void refresh_file();

    void fold_list();

    List * free_list(List * ls);

    int list_item_count(List * ls, int count =0);

    List* first_in_memory();

    List * get_first(List * tail);

public:

    void realloc(List * ls,int size);

    List * find(unsigned int id);

    void defragmentation();

    void make_free(List * ls);

    void load_to_disk(List *ls);

    List * load_to_memory(unsigned int id);

    int get_buffer_size();

    set<unsigned int>& get_set_of_id();

    static int get_buffer_start_pointer();

    static Pool & get_pool(unsigned int size = 1000000);

    item_pair new_item(unsigned int size);

    void delete_item(unsigned int id, bool flag = true);

    unsigned int free_memory_size(List * ls);

    void see_inf();

    void see_inf(List * ls);

    void see_mem(unsigned int id);

    void swap(unsigned int size);

    ~Pool();
};


#endif //SPO6_POOL_H
