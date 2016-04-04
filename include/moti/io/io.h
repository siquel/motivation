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

    struct __declspec(novtable) Closer {
        virtual ~Closer() = 0;
        virtual void close() = 0;
    };

    inline Closer::~Closer() {}

    struct __declspec(novtable) ReaderOpener {
        virtual ~ReaderOpener() = 0;
        virtual bool open(const char* _filePath) = 0;
    };

    inline ReaderOpener::~ReaderOpener() {}

    struct __declspec(novtable) WriterOpener {
        virtual ~WriterOpener() = 0;
        virtual bool open(const char* _filePath, bool append) = 0;
    };

    inline WriterOpener::~WriterOpener() {}

    struct __declspec(novtable) ReaderSeeker : public Reader, public Seeker{

    };

    struct __declspec(novtable) WriterSeeker : public Writer, public Seeker {

    };

    struct __declspec(novtable) FileReader : public ReaderSeeker, public ReaderOpener, public Closer {

    };

    struct __declspec(novtable) FileWriter : public WriterSeeker, public WriterOpener, public Closer {

    };

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

    class MemoryReader : public ReaderSeeker {
    private:
        const uint8_t* m_data;
        int64_t m_pos;
        int64_t m_size;
    public:
        MemoryReader(const void* _data, uint32_t _size)
            : m_data(static_cast<const uint8_t*>(_data)), m_pos(0), m_size(0) {}
        virtual ~MemoryReader() {}

        virtual int64_t seek(int64_t _offset, Whence::Enum _whence) override {
            switch (_whence) {
            case Whence::Begin:
            case Whence::Current:
            case Whence::End: break;
            }
            return 0;
        }

        virtual int32_t read(void* _data, int32_t _size) override {
            return 0;
        }

        const uint8_t* getPointer() const { return &m_data[m_pos]; }
        int64_t position() const { return m_pos; }
        int64_t remaining() const { return m_size - m_pos; }
    };
    
    class MemoryWriter : public WriterSeeker {
    public:
        virtual ~MemoryWriter() {}
        virtual int64_t seek(int64_t _offset = 0, Whence::Enum _whence = Whence::Current) override {
            switch (_whence) {
            case Whence::Begin:
            case Whence::Current:
            case Whence::End: break;
            }
            return 0;
        }

        virtual int32_t write(const void* _data, int32_t _size) override {
            return 0;
        }
    };
}