#include "lsmTree.hpp"

#include <iostream>
#include <chrono>
#include <ctime>

using namespace KvStoreServer;

void testSkipList()
{
    const int N = 20;
    const int R = 5000;

    LSMTree db;
    Random rnd(1000);

    for(int i = 0; i < N; i++)
    {
        KeyType key = rnd.Next() % R;
        ValueType value = (std::to_string(key.key)).c_str();
        db.Put(key, value);
    }

    db.Put(5555, "5555");
    db.ShowData();
    db.Remove(3471);
    db.ShowData();
   
    int key = 5555;
    ValueType value;
    if(db.Get(key, value))
    {
        std::cout << "find the entry " << key << ":" << value.str << std::endl;
    }
    else
    { 
        std::cout << "cant find the value of key:" << key << std::endl;
    }

    std::vector<Entry> vec;
    //vec = db.PopAllEntries();

    for(auto i : vec)
    {
        std::cout << i.internalKey.key << ":" << i.value.str << " ";
    }
    std::cout << std::endl;

    db.FlushInDisk();
    std::cout << "ShowData: ";
    db.ShowData();

    std::cout << "LoadData: ";
    db.LoadFromDisk();  
}

void testSSTable()
{
    const int N = 10;
    const int R = 5000;

    LSMTree db;
    Random rnd(1000);

    for(int i = 0; i < N; i++)
    {
        KeyType key = rnd.Next() % R;
        ValueType value = (std::to_string(key.key)).c_str();
        db.Put(key, value);
    }

    db.Put(5555, "5555");
    db.Remove(5555);
    db.Put(5555, "555555");
    ValueType value;
    if(db.Get(5555, value))
    {
        std::cout << "Get 5555:" << value.str << std::endl; 
    }
    
    db.ShowData();
    db.FlushInDisk();
}

void testSSTableLoad()
{
    LSMTree db;
    db.LoadFromDisk();
}

int main()
{
    //testSSTable();
    testSSTableLoad();

    return 0;
}
