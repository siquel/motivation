#pragma once
#include <stdint.h>

namespace moti {
    struct Block {

        Block();

        Block(void* _ptr, uint32_t _length);

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
        uint32_t m_length;
    };
    inline uint32_t roundToAlignment(uint32_t basis, uint32_t n) {
        return n + ((n % basis == 0) ? 0 : (basis - n % basis));
    }
}

	
