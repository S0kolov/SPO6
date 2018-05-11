//
// Created by sasha on 01.05.2018.
//

#include "Pool.h"

item_pair Pool::new_item(unsigned int size) {
    List *item= nullptr;
    plase_pair p = find_plase(size);
    if(p.second){
        item = static_cast<List *>(p.first);
        init_mem(item);
        return make_pair(item,0);
    }else{
        cout<< "not enough memory"<<endl;
    }
    return make_pair(item,-1);
}

void Pool::delete_item(unsigned int id,bool flag) {
    List * ls = find(id);

    if(ls->perent == nullptr && ls->next == nullptr){
        _busy_mem = nullptr;
    }else if(ls->perent == nullptr){
        _busy_mem = ls->next;
        _busy_mem->perent= nullptr;
    }else {
        ls->perent->next=ls->next;
        if(ls->next != nullptr) {
            ls->next->perent = ls->perent;
        }
    }

    if(!flag){
        return;
    }
    if(ls->st == status :: ON_DISK ){
        return;
    }else {
        ls->perent = nullptr;
        ls->next = _free_mem;
        _free_mem = ls;
        ls->next->perent = ls;
    }
    fold_list();

}

void Pool::fold_list(){
    List * ls = _free_mem;
    while(ls != nullptr){
        if(ls->next != nullptr){
            List * temp = ls->next;
            if(ls->pointer > temp->pointer){
                if(ls->perent != nullptr) {
                    temp->perent->next = temp;
                    temp->perent = ls->perent;
                    ls->perent = temp;
                    ls->next = temp->next;
                    ls->next->perent = ls;
                    temp->next = ls;

                    List * t = ls;
                    ls = temp;
                    temp = t;
                }else{
                    temp->perent->next = temp;
                    temp->perent = ls->perent;
                    ls->perent = temp;
                    ls->next = temp->next;
                    ls->next->perent = ls;
                    temp->next = ls;

                    List * t = ls;
                    ls = temp;
                    temp = t;
                    this->_free_mem = ls;
                }
            }
            if((ls->pointer+ls->size) == ls->next->pointer){
                ls->next=temp->next;
                if(temp->next!= nullptr){
                temp->next->perent=ls;
                }
                ls->size +=temp->size;
                delete temp;
            }else{
                ls= ls->next;
            }
        }else{
            break;
        }
    }
}

void Pool::see_inf() {
    List * ls = _busy_mem;
    cout<< "free memory = "<< free_memory_size(_free_mem)<<endl;
    cout<<"\n\tname:"<<"\tsize:"<<"\tpointer"<<"\tstatus"<<endl;
    while(ls){
        cout<<"\t"<<ls->id<<"\t"<<ls->size<<"\t"<<(int)(ls->pointer-_buffer)<<"\t"<<str_status(ls->st)<<endl;
        ls=ls->next;
    }
}

unsigned int Pool::free_memory_size(List *ls) {
    if(ls== nullptr){
        return 0;
    }else {
        return ls->size + free_memory_size(ls->next);
    }
}

void Pool::see_inf(List *ls) {
    cout<<"\n\tname:"<<"\tsize:"<<"\tpointer"<<"\tstatus"<<endl;
    cout<<"\t"<<ls->id<<"\t"<<ls->size<<"\t"<<(int)(ls->pointer- Pool::get_buffer_start_pointer())<<"\t"<<str_status(ls->st)<<endl;
}

Pool::~Pool() {
    delete [] _buffer;
}

int Pool::get_buffer_start_pointer() {
    return reinterpret_cast<int>(get_pool()._buffer);
}

Pool &Pool::get_pool(unsigned int size) {
    static Pool  pool(size);
    return pool;
}

set<unsigned int> &Pool::get_set_of_id() {
    set<unsigned int> * s = new set<unsigned int>();
    List * ls = _busy_mem;
    while(ls!= nullptr){
        s->insert(ls->id);
        ls=ls->next;
    }
    return *s;
}

List *Pool::find(unsigned int id) {
    List * ls = _busy_mem;
    while(ls != nullptr){
        if(ls->id == id){
            return ls;
        }
        ls= ls->next;
    }
    return nullptr;
}

Pool::Pool(unsigned int buf_size) : _max_count(10) {
    _counter = 0;
    _swap_file_name = "dump_of_memory.list";
    _id_counet=0;
    _buf_size = buf_size;
    _buffer = new char [_buf_size];
    _free_mem  = init_struct(new List(),_id_counet++,_buf_size,_buffer, nullptr, nullptr);
    _busy_mem = nullptr;
    _swap_file = fopen(_swap_file_name,"w");
    fclose(_swap_file);
}

List *Pool::init_struct(List *item, unsigned int id, unsigned int size, char *pointer, List *next, List *perent) {
    item->id=id;
    item->size=size;
    item->pointer=pointer;
    item->next=next;
    item->perent= perent;
    if(perent!= nullptr){
        perent->next=item;
    }
    item->st = status::IN_MEMORY;
    return item;
}

plase_pair Pool::find_plase(unsigned int size,int id, bool f) {
    bool res = false;
    List * res_l = this->_free_mem;
    List * new_l = nullptr;
    do{
        if(res_l == nullptr){
            break;
        }
        if(res_l->size >= size){
            if(id == -1){
                id = _id_counet++;
            }else {
                if (f) {
                    delete_item(id);
                    res_l = _free_mem;
                } else {
                new_l = find(static_cast<unsigned int>(id));
                if (new_l->perent == nullptr) {
                    _busy_mem = new_l->next;
                    _busy_mem->perent = nullptr;
                } else {
                    new_l->perent->next = new_l->next;
                    if (new_l->next != nullptr) {
                        new_l->next->perent = new_l->perent;
                    }
                }
                delete new_l;
                new_l = nullptr;
                }
            }
            new_l=init_struct(new List(), static_cast<unsigned int>(id), size, res_l->pointer, nullptr,
                              (_busy_mem == nullptr)? _busy_mem: last(_busy_mem));
            _busy_mem = (_busy_mem== nullptr)? new_l:_busy_mem;
            res_l->size-=size;
            res_l->pointer+=size;
            res=true;
            if(res_l->size==0){
                if(res_l->perent == nullptr){
                    _free_mem = res_l->next;
                    delete res_l;
                }else{
                    res_l->perent->next=res_l->next;
                }
            }
            break;
        }else{
            res_l=res_l->next;
        }
    }while(res_l != nullptr);
    if(res == false){
        if(size > free_memory_size(_free_mem)) {
            swap(size);
        }else{
            defragmentation();
        }
        return find_plase(size,id);
    }
    return make_pair(new_l,res);
}

void Pool::init_mem(List *ls) {
    char * pointer= ls->pointer;
    for(int i =0;i<ls->size;i++){
        pointer[i]= static_cast<char>('0' + ls->id);
    }
}

void Pool::see_mem(unsigned int id) {
    List * ls = find(id);
    if(ls->st == status::ON_DISK){
        ls = load_to_memory(ls->id);
    }
    for(int i = 0 ;i<ls->size;i++){
        printf("%c",ls->pointer[i]);
    }
    puts("");
}

List *Pool::last(List *elem) {
    if(elem->next == nullptr){
        return elem;
    }else{
        return last(elem->next);
    }
}

void Pool::swap(unsigned int size) {
    List * ls = this->_busy_mem;
    unsigned int ssize = size;
    do{
        if(ls->st == status::IN_MEMORY){
            size -= ls->size;
            load_to_disk(ls);
            make_free(ls);
        }
        ls = ls->next;
        if(ssize <= free_memory_size(this->_free_mem)){
            defragmentation();
            return;
        }
        if(ls == nullptr){
            break;
        }
    }while(size < Pool::get_pool()._buf_size );
    fold_list();
}

int Pool::get_buffer_size() {
    return _buf_size;
}

void Pool::load_to_disk(List *ls) {
    _swap_file = fopen(_swap_file_name,"a+");
    fwrite((ls), sizeof(List),1,_swap_file);
    fwrite((ls->pointer), sizeof(char),ls->size,_swap_file);
    ls->st = status ::ON_DISK;
    fclose(_swap_file);
}

List * Pool::load_to_memory(unsigned int id) {
    _counter++;
    List * ls = new List();
    _swap_file= fopen(_swap_file_name,"r");
    do{
        fread(ls, sizeof(List),1,_swap_file);
        if(feof(_swap_file)){
            break;
        }
        if(ls->id == id){
            int pos = ftell(_swap_file);
            plase_pair p = find_plase(ls->size,ls->id);
            if(p.second){
                _swap_file = fopen(_swap_file_name,"r");
                fseek(_swap_file,pos,SEEK_SET);
                fread(ls->pointer,ls->size,1,_swap_file);
            }
            break;
        }else{
            int p = ftell(_swap_file);
            long p1 =static_cast<long>(ftell(_swap_file) + ls->size);
            fseek(_swap_file,p1, SEEK_SET);
        }
    }while(id != ls->id);
    if(_counter == _max_count){
        _counter=0;
        refresh_file();
    }
    fclose(_swap_file);
    return ls;
}

void Pool::refresh_file() {

}

void Pool::make_free(List * ls) {
    List * l = new List();
    l->id = ls->id;
    l->size = ls->size;
    l->pointer = ls->pointer;
    l->perent= nullptr;
    l->next = this->_free_mem;
    l->st = status :: ON_DISK;
    this->_free_mem = l;
    if(l->next != nullptr){
        l->next->perent=l;
    }
}

void Pool::defragmentation() {

    List * list = nullptr;
    List * buise;
    free_list(this->_free_mem);
    _free_mem = init_struct(new List(),0,this->_buf_size,this->_buffer, nullptr, nullptr);
    List * temp ;
    while(list_item_count(_busy_mem) != 0) {
        temp = first_in_memory();
        buise= temp;
        while (buise != nullptr) {
            if(buise->st != status::ON_DISK){
                if (temp->pointer > buise->pointer) {
                    temp = buise;
                }
            }
            buise = buise->next;
        }
        _free_mem->pointer+=temp->size;
        _free_mem->size-= temp->size;

        list = init_struct(new List(),temp->id,temp->size,_free_mem->pointer-temp->size, nullptr,list);
        for(int i =0 ; i < temp->size;i++){
            list->pointer[i] = temp->pointer[i];
        }
        delete_item(temp->id,false);
    }
    List * l = get_first(list);
    l->perent = last(_busy_mem);
    last(_busy_mem)->next= l;
}

void Pool::realloc(List *ls, int size) {
    if(ls == nullptr || size == ls->size){
        return;
    }
    if(size < ls->size){
        List * lis = init_struct(new List(),0,
                                ls->size - size,ls->pointer+size,_free_mem, nullptr);
        ls->size = static_cast<unsigned int>(size);
        _free_mem = lis;
    }else {
        //save memory value
        plase_pair p = find_plase(size,ls->id,true);
        if(p.second){
            ls = static_cast<List *>(p.first);
        }
        init_mem(ls);
        //reestablish memory value
    }
}

List *Pool::free_list(List *ls) {
    if(ls == nullptr){
        return nullptr;
    }
    if(ls->next == nullptr){
        return ls;
    }else{
        free_list(ls->next);
        free(ls);
    }
    return nullptr;
}

int Pool::list_item_count(List *ls, int count) {
    if(ls == nullptr){
        return count;
    }else{
        return (ls->st == status::IN_MEMORY)?
               list_item_count(ls->next,count+1) : list_item_count(ls->next,count);
    }
}

List *Pool::first_in_memory() {
    List * ls =this->_busy_mem;
    while(ls->st != status::IN_MEMORY){
        ls= ls->next;
    }
    return ls;
}

List *Pool::get_first(List *tail) {
    if(tail->perent == nullptr){
        return tail;
    }else{
        return get_first(tail->perent);
    }
}
