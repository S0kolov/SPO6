#include <iostream>
#include "Pool.h"


void realloc_mem();

void print_menu(){
    cout<<"menu:"<<endl;
    cout<<"1. add ne element"<<endl;
    cout<<"2. see inf"<<endl;
    cout<<"3. delete item"<<endl;
    cout<<"4. see mem stat"<<endl;
    cout<<"5. realloc"<<endl;
    cout<<"0. exit"<<endl;
}

unsigned int add_new_element(){
    int size;
    do {
        cout << "enter the size of new element (-1 foe exit):";
        fflush(stdin);
        rewind(stdin);
        cin >> size;
        if(size == -1){
            return 1;
        }
    }while(size<0);
    if(size > Pool::get_pool().get_buffer_size()){
        cout<<"not enough memory, sorry =( "<<endl;
        return 1;
    }
    item_pair p = Pool::get_pool().new_item(static_cast<unsigned int>(size));
    if(p.second == 0){
        Pool::get_pool().see_inf(static_cast<List *>(p.first));
    }
    return 0;
}

int delete_item(){
    int id;
    set<unsigned int> s = Pool::get_pool().get_set_of_id();
    do {
        cout << "enter the id of deleted element (-1 for exit):";
        fflush(stdin);
        rewind(stdin);
        cin >> id;
        if(id == -1){
            return -1;
        }
    }while(s.count(static_cast<const unsigned int &>(id)) == 0);
    Pool::get_pool().delete_item(static_cast<unsigned int>(id));
}

void mem_stat(){
    int id;
    set<unsigned int> s = Pool::get_pool().get_set_of_id();
    do {
        cout << "enter the id of element (-1 for exit):";
        fflush(stdin);
        rewind(stdin);
        cin >> id;
        if(id == -1){
            return;
        }
    }while(s.count(static_cast<const unsigned int &>(id)) == 0);
    Pool::get_pool().see_mem(static_cast<unsigned int>(id));
}

int main() {
    Pool::get_pool(1000);
    int i=0;
    bool exit= false;
    do {
        system("cls");
        fflush(stdin);
        rewind(stdin);
        print_menu();
        cin>>i;
        switch (i){
            case 1: add_new_element(); break;
            case 2: Pool::get_pool().see_inf();break;
            case 3: delete_item();break;
            case 4: mem_stat(); break;
            case 5: realloc_mem(); break;
            case 0: exit = true; break;
            default: continue;
        }
    }while(!exit);
    return 0;
}

//int main(){
//
//    Pool pool = Pool::get_pool(1000);
//
//    pool.new_item(10);  // id - 1
//    pool.new_item(20);  // id - 2
//    pool.new_item(30);  // id - 3
//    pool.new_item(100);  // id - 4
//    pool.new_item(800);  // id - 5
//
//    pool.see_inf();
//
//    pool.new_item(50);  // id - 6
//    pool.new_item(30);  // id - 7
//
//    pool.see_inf();
//
//    cout<<"delete item id =1 "<<endl;
//    pool.delete_item(1);
//    cout<<"delete item id =4 "<<endl;
//    pool.delete_item(4);
//
//    pool.see_inf();
//
//    cout<<"see inf about element id = 3"<<endl;
//    pool.see_inf(pool.find(3));
//    pool.see_mem(3);
//
//    pool.see_inf();
//
//    cout<<"realloc id = 3" << endl;
//    pool.realloc(pool.find(3),50);
//
//    pool.see_inf();
//
//    set<unsigned int> s = pool.get_set_of_id();
//
//    cout<<"delete all element"<<endl;
//    for(unsigned int element : s){
//        pool.delete_item(element);
//    }
//
//    pool.see_inf();
//
//    return 0;
//}


void realloc_mem() {
    int id;
    set<unsigned int> s = Pool::get_pool().get_set_of_id();
    do {
        cout << "enter the id of element (-1 for exit):";
        fflush(stdin);
        rewind(stdin);
        cin >> id;
        if(id == -1){
            return;
        }
    }while(s.count(static_cast<const unsigned int &>(id)) == 0);
    List * ls = Pool::get_pool().find(static_cast<unsigned int>(id));
    Pool::get_pool().see_inf(ls);
    do {
        cout << "enter new size of element (-1 for exit):";
        fflush(stdin);
        rewind(stdin);
        cin >> id;
        if(id == -1){
            return;
        }
    }while(id > Pool::get_pool().get_buffer_size() || id <= 0);
    Pool::get_pool().realloc(ls, id);
}