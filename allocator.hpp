#pragma once

#include <cstddef>
#include <cstdlib>

class ArenaAllocator {
    private:
        size_t size;
        std::byte* buffer;
        std::byte* offset;

    public:
        ArenaAllocator(size_t bytes) : size(bytes){
            buffer = static_cast<std::byte*>(malloc(size));
            offset = buffer;
        };

        ~ArenaAllocator(){
            free(buffer);
        }

        template<typename T>
        T* alloc(){
            void* temp = offset;
            offset += sizeof(T);
            return temp;
        };

};
