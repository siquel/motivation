#pragma once

namespace moti {
    namespace memory {
        struct Block {

            Block() noexcept;

            Block(void* _ptr, size_t _length) noexcept;

            Block(Block&& b) noexcept;

            Block& operator=(Block&& b) noexcept;

            Block(const Block&) noexcept = default;
            Block& operator=(const Block&) noexcept = default;
            
            ~Block();

            void reset() noexcept;

            operator bool() const;

            bool operator==(const Block& rhs) const;

            // pointer to start address of the block
            void* m_ptr;
            // length of the reserved bytes
            size_t m_length;
        };
    }
}
