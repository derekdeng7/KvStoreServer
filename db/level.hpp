#ifndef _KVSTORESERVER_DB_LEVEL_HPP_
#define _KVSTORESERVER_DB_LEVEL_HPP_

#include "ssTable.hpp"

namespace KvStoreServer{


    class Level
    {
    public:
        Level(size_t levelNo)
          : levelNo_(levelNo)
        {
            ReadLevelMeta();
        }

        bool WriteLevelMeta();
        bool ReadLevelMeta();
        bool Search(const KeyType& key, ValueType& value);

    private:
        size_t levelNo_;
        LevelMeta meta_;
    };

}

#endif //_KVSTORESERVER_DB_LEVEL_HPP_
