#ifndef _KVSTORESERVER_DB_FILEOPERATOR_HPP_
#define _KVSTORESERVER_DB_FILEOPERATOR_HPP_

#include "entry.hpp"

namespace KvStoreServer{

    class FileOperator
    {
    public:
        FileOperator(const char* mode, const char* filename = MANIFESTPATH)
          : filename_(filename), mode_(mode)
        { 
            fp_ = fopen(filename_, mode_);
        }

        ~FileOperator()
        {
            if(fp_ != NULL)
            {
                fclose(fp_);
            }
        }

        bool Read(void* block, off_t offset, size_t size) const
        {
            if(fp_ == NULL)
            {
                perror("fail to open file");
                return false;
            }

            fseek(fp_, offset, SEEK_SET);
            size_t result = fread(block, size, 1, fp_);

            return result;
        }

        bool Write(void* block, off_t offset, size_t size) const
        {
            if(fp_ == NULL)
            {
                perror("fail to open file");
                return false;
            }

            fseek(fp_, offset, SEEK_SET);
            size_t result = fwrite(block, size, 1, fp_);

            return result;
        }

    private:
        const char* filename_;
        const char* mode_;
        FILE* fp_;
    };

}

#endif //_KVSTORESERVER_DB_FILEOPERATOR_HPP_
