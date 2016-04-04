#pragma once
#include <stdint.h>

namespace moti {
    struct Whence {
        enum Enum {
            Begin, Current, End
        };
    };

    struct __declspec(novtable) Reader {
        virtual ~Reader() = 0;
        virtual int32_t read(void* _data, int32_t size) = 0;
    };
    inline Reader::~Reader() {}

    struct __declspec(novtable) Writer {
        virtual ~Writer() = 0;
        virtual int32_t write(const void* _data, int32_t _size) = 0;
    };

    inline Writer::~Writer() {}

    struct __declspec(novtable) Seeker {
        virtual ~Seeker() = 0;
        virtual int64_t seek(int64_t _offset = 0, Whence::Enum _whence = Whence::Current) = 0;
    };

    inline Seeker::~Seeker() {}

    inline int32_t read(Reader* _reader, void* _data, int32_t _size) {
        return _reader->read(_data, _size);
    }

    template <typename T>
    inline int32_t read(Reader* _reader, T& _value, int32_t _size) {
        static_assert(std::is_pod<T>::value, "T must be POD type");
        return _reader->read(&_value, sizeof(T));
    }

    inline int32_t write(Writer* _writer, void* _data, int32_t size) {
        return _writer->write(_data, size);
    }

    template <typename T>
    inline int32_t write(Writer* _writer, T& _value, int32_t size) {
        static_assert(std::is_pod<T>::value, "T must be POD type");
        return _writer->write(&_value, sizeof(T));
    }

    inline int64_t seek(Seeker* _seeker, int64_t off = 0, Whence::Enum _whence = Whence::Current) {
        return _seeker->seek(off, _whence);
    }

    inline int64_t getFileSize(Seeker* _seeker) {
        int64_t current = _seeker->seek();
        int64_t size = _seeker->seek(0, Whence::End);
        _seeker->seek(current, Whence::Begin);
        return size;
    }
}