#pragma once

#include "moti/memory/allocator.h"

namespace moti {
    template <typename T>
    struct Array {
        memory::Allocator* m_allocator;
        uint32_t m_capacity;
        uint32_t m_size;
        memory::Block m_data;

        Array(memory::Allocator& _alloc);
        Array(memory::Allocator& _alloc, uint32_t _capacity);
        ~Array();
        T& operator[](uint32_t _index);
        const T& operator[](uint32_t index) const;

        Array<T>(const Array<T>& _other) = delete;
        Array<T>& operator=(const Array<T>& _other) = delete;

        void resize(uint32_t _size);
        void reserve(uint32_t _capacity);
        uint32_t push_back(const T& _item);
        uint32_t pop_back();

        T* begin();
        const T* begin() const;

        T* end();
        const T* end() const;

        T& front();
        const T& front() const;

        T& back();
        const T& back() const;

        
    };
}

namespace moti {
    template <typename T>
    inline Array<T>::Array(memory::Allocator& _alloc) 
        : m_allocator(&_alloc), m_capacity(0u), m_size(0u) {}

    template <typename T>
    inline Array<T>::Array(memory::Allocator& _alloc, uint32_t _capacity)
        : m_allocator(&_alloc), m_capacity(0u), m_size(0u) {
        reserve(_capacity);
    }

    template <typename T>
    inline Array<T>::~Array() {
        m_allocator->deallocate(m_data);
    }

    template <typename T>
    inline T& Array<T>::operator[](uint32_t _index) {
        return const_cast<T&>(const_cast<const Array<T>&>(*this)[_index]);
    }

    template <typename T>
    inline const T& Array<T>::operator[](uint32_t _index) const {
        MOTI_ASSERT(_index < m_size, "Index out of bounds");
        return *reinterpret_cast<T*>(
            static_cast<char*>(m_data.m_ptr) + _index * sizeof(T)
        );
    }

    template <typename T>
    void setCapacity(Array<T>& _a, uint32_t _capacity) {
        if (_capacity == _a.m_capacity)
            return;

        if (_capacity < _a.m_size)
            _a.resize(_capacity);

        if (_capacity > 0) {
            _a.m_capacity = _capacity;
            assert(_a.m_allocator->reallocate(_a.m_data, _capacity * sizeof(T)));
        }
    }

    template <typename T>
    void grow(Array<T>& _array, uint32_t _minCapacity) {
        uint32_t newCapacity = _array.m_capacity * 2 + 1;
        if (newCapacity < _minCapacity) {
            newCapacity = _minCapacity;
        }

        setCapacity(_array, newCapacity);
    }

    template <typename T>
    inline void Array<T>::resize(uint32_t _size) {
        if (_size > m_capacity) {
            grow(*this, _size);
        }
        m_size = _size;
    }

    template <typename T>
    inline void Array<T>::reserve(uint32_t _capasity) {
        if (_capasity > m_capacity) {
            grow(*this, _capasity);
        }
    }

    template <typename T>
    inline uint32_t Array<T>::push_back(const T& _item) {
        if (m_capacity == m_size) {
            grow(*this, 0);
        }
        
        memcpy((static_cast<char*>(m_data.m_ptr) + sizeof(T) * m_size), static_cast<const void*>(&_item), sizeof(T));
        return m_size++;
    }

    template <typename T>
    inline uint32_t Array<T>::pop_back() {
        MOTI_ASSERT(m_size > 0, "Array is empty");
        --m_size;
    }

    template <typename T>
    inline T* Array<T>::begin() {
        return reinterpret_cast<T*>(
            m_data.m_ptr
        );
    }

    template <typename T>
    inline const T* Array<T>::begin() const {
        return reinterpret_cast<const T*>(
            m_data.m_ptr
        );
    }

    template <typename T>
    inline T* Array<T>::end() {
        return reinterpret_cast<T*>(
            static_cast<char*>(m_data.m_ptr) + (m_size - 1) * sizeof(T)
        );
    }

    template <typename T>
    inline const T* Array<T>::end() const {
        return reinterpret_cast<const T*>(
            static_cast<char*>(m_data.m_ptr) + (m_size - 1) * sizeof(T)
        );
    }

    template <typename T>
    inline T& Array<T>::front() {
        MOTI_ASSERT(m_size > 0, "The array is empty");
        return *reinterpret_cast<T*>(
            m_data.m_ptr
        );
    }

    template <typename T>
    inline const T& Array<T>::front() const {
        MOTI_ASSERT(m_size > 0, "The array is empty");
        return *reinterpret_cast<const T*>(
            m_data.m_ptr
        );
    }

    template <typename T>
    inline T& Array<T>::back() {
        MOTI_ASSERT(m_size > 0, "The array is empty");
        return *reinterpret_cast<T*>(
            static_cast<char*>(m_data.m_ptr) + (m_size - 1) * sizeof(T)
        );
    }

    template <typename T>
    inline const T& Array<T>::back() const {
        MOTI_ASSERT(m_size > 0, "The array is empty");
        return *reinterpret_cast<const T*>(
            static_cast<char*>(m_data.m_ptr) + (m_size - 1) * sizeof(T)
        );
    }
}