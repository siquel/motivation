#pragma once

namespace moti {
    namespace memory {
        struct Block {

            Block();

            Block(void* _ptr, size_t _length);

            Block(Block&& b);

            Block& operator=(Block&& b);

            Block(const Block&) = default;
            Block& operator=(const Block&) = default;
            
            ~Block();

            void reset();

            operator bool() const;

            bool operator==(const Block& rhs) const;

            // pointer to start address of the block
            void* m_ptr;
            // length of the reserved bytes
            size_t m_length;
        };
    }

    inline size_t roundToAlignment(size_t basis, size_t n) {
        return n + ((n % basis == 0) ? 0 : (basis - n % basis));
    }
}
