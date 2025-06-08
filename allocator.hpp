#pragma once

#include <cstddef>
#include <cstdlib>
#include <iostream>

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

        // ArenaAllocator(const ArenaAllocator& other) = delete;
        // ArenaAllocator operator=(const ArenaAllocator& other) = delete;

        ~ArenaAllocator(){
            free(buffer);
        }

        template<typename T>
        T* alloc(){
            if (offset+sizeof(T) > buffer + size){
                std::cout << "Buffer overflow\n";
                exit(-1);
            };
            void* temp = offset;
            new (temp) T();
            offset += sizeof(T);
            return static_cast<T*>(temp);
        };

};
