#pragma once

namespace moti {

    template <uint16_t MaxHandles>
    class HandleManager {
    public:
        static const uint16_t Invalid = UINT16_MAX;
        HandleManager()
            : m_handleCount(0) {
            reset();
        }

        ~HandleManager() {}

        uint16_t create() {
            if (m_handleCount < MaxHandles) {
                uint16_t index = m_handleCount;
                ++m_handleCount;
                uint16_t handle = m_indices[index];
                m_handles[handle] = index;
                return handle;
            }
            return Invalid;
        }

        void destroy(uint16_t _handle) {
            uint16_t index = m_handles[_handle];
            --m_handleCount;
            uint16_t temp = m_indices[m_handleCount];
            m_indices[m_handleCount] = _handle;
            m_handles[temp] = index;
            m_indices[index] = temp;
        }

        bool isValid(uint16_t _handle) const {
            uint16_t index = m_handles[_handle];
            return index < m_handleCount && m_indices[index] == _handle;
        }

        void reset() {
            m_handleCount = 0;
            for (uint16_t i = 0; i < MaxHandles; ++i) {
                m_indices[i] = i;
            }
        }

    private:
        uint16_t m_handleCount;
        uint16_t m_indices[MaxHandles];
        uint16_t m_handles[MaxHandles];
    };
}