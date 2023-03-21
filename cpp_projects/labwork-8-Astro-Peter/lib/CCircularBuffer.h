//
// Created by Vanst on 05.03.2023.
//

#ifndef LABWORK_8_ASTRO_PETER_BUFFER_CCIRCULARBUFFER_H_
#define LABWORK_8_ASTRO_PETER_BUFFER_CCIRCULARBUFFER_H_

#include <memory>
#include <utility>


namespace CircBuff {

using bounds = size_t;

template<class T, class Allocator = std::allocator<T>>
class CCircularBuffer {
private:
    size_t capacity_;
    T* memory_start;
    bounds tail_ = 0;
    bounds head_ = 0;
    Allocator alloc;
    using alloc_traits = std::allocator_traits<Allocator>;

    virtual void IncrementTail() {
        if ((tail_ + reserved) % capacity_ == head_) {
            IncrementHead();
        }
        tail_++;
        tail_ %= capacity_;
    };

    void DecreaseTail(size_t n) {
        if (n < tail_) {
            tail_ -= n;
        } else {
            tail_ = capacity_ - (n - tail_);
        }
    }

    void IncrementHead() {
        head_++;
        head_ %= capacity_;
    };

public:

    class BaseIterator {
    private:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        CCircularBuffer<value_type>* buffer_;
        size_t position;
        pointer ptr;
    public:
        BaseIterator(size_t pos, CCircularBuffer* buffer) {
            buffer_ = buffer;
            position = pos;
            bounds tmp = buffer->GetHead();
//            std::cout << "h:" << buffer->GetHead() << "\tpos:" << pos << '\n';
            ptr = buffer_->GetMemoryStart() + (tmp + pos) % buffer->GetBufferCapacity();
        }

        pointer& GetPtr() {
            return ptr;
        }

        pointer GetPtrConst() const {
            return ptr;
        }

        CCircularBuffer*& GetBuff() {
            return buffer_;
        }

        CCircularBuffer* GetBuffConst() const {
            return buffer_;
        }

        size_t& GetPos() {
            return position;
        }

        size_t GetPosConst() const {
            return position;
        }
    };

    class CircIterator : public BaseIterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        CircIterator(size_t pos, CCircularBuffer* buffer) : BaseIterator(pos, buffer) {}

        CircIterator(CircIterator const& b) : BaseIterator(b.GetPosConst(), b.GetBuffConst()) {}

        CircIterator& operator=(CircIterator b) {
            BaseIterator::GetPos() = b.GetPosConst();
            BaseIterator::GetPtr() = b.GetPtrConst();
            BaseIterator::GetBuff() = b.GetBuffConst();
        }

        CircIterator& operator-=(difference_type n) {
//            std::cout << BaseIterator::GetPosConst() << '\t' << *BaseIterator::GetPtrConst() << '\n';
            if ((n <= BaseIterator::GetPos() && n > 0)
                || (n < 0 && BaseIterator::GetBuffConst()->GetBufferCapacity() - BaseIterator::GetPos() > abs(n))) {
                BaseIterator::GetPos() -= n;
                BaseIterator::GetPtr() = BaseIterator::GetBuffConst()->GetMemoryStart()
                    + (BaseIterator::GetBuffConst()->GetHead() + BaseIterator::GetPos())
                        % BaseIterator::GetBuffConst()->GetBufferCapacity();
            }
            return *this;
        }

        void swap(CircIterator& b) {
            auto tmp = b;
            b = *this;
            *this = tmp;
        }

        bool operator<(const CircIterator b) const {
            return BaseIterator::GetPosConst() < b.GetPosConst();
        }

        bool operator<=(const CircIterator b) const {
            return BaseIterator::GetPosConst() <= b.GetPosConst();
        }

        bool operator>=(const CircIterator b) const {
            return BaseIterator::GetPosConst() >= b.GetPosConst();
        }

        bool operator>(const CircIterator b) const {
            return BaseIterator::GetPosConst() > b.GetPosConst();
        }

        bool operator==(const CircIterator b) const {
            return BaseIterator::GetPosConst() == b.GetPosConst();
        }

        bool operator!=(const CircIterator b) const {
            return BaseIterator::GetPosConst() != b.GetPosConst();
        }

        CircIterator& operator--() {
            this->operator-=(1);
            return *this;
        }

        const CircIterator operator--(int) {
            CircIterator tmp = *this;
            this->operator-=(1);
            return tmp;
        }

        CircIterator& operator++() {
            this->operator-=(-1);
            return *this;
        }

        const CircIterator operator++(int) {
            CircIterator tmp = *this;
            this->operator+=(1);
            return tmp;
        }

        CircIterator& operator+=(difference_type num) {
            this->operator-=(-num);
            return *this;
        };

        reference operator[](size_t n) const {
            auto tmp = *this;
            tmp += n;
            return *tmp.GetPtr();
        }

        reference operator*() const {
            return *(BaseIterator::GetPtrConst());
        }

        friend difference_type operator-(CircIterator a, CircIterator b) {
            return a.GetPosConst() - b.GetPosConst();
        }

        friend CircIterator operator-(CircIterator a, difference_type n) {
            return a.operator-=(n);
        }

        pointer operator->() {
            return BaseIterator::GetPtrConst();
        }

        friend CircIterator operator+(CircIterator a, difference_type n) {
            return a.operator+=(n);
        }

        friend CircIterator operator+(difference_type n, CircIterator a) {
            return a.operator+=(n);
        }
    };

    class ConstIterator : public BaseIterator {

    public:

        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using const_pointer = const T*;
        using const_reference = const T&;

        ConstIterator(size_t pos, CCircularBuffer* buffer) : BaseIterator(pos, buffer) {}

        ConstIterator& operator-=(difference_type n) {
            if ((n < BaseIterator::GetPos() && n > 0)
                || (n < 0 && BaseIterator::GetBuffConst()->GetBufferCapacity() - BaseIterator::GetPos() > abs(n))) {
                BaseIterator::GetPos() -= n;
                BaseIterator::GetPtr() = BaseIterator::GetBuffConst()->GetMemoryStart()
                    + (BaseIterator::GetBuffConst()->GetHead() + BaseIterator::GetPos())
                        % BaseIterator::GetBuffConst()->GetBufferCapacity();
            }
            return *this;
        }

        bool operator<(const ConstIterator b) const {
            return BaseIterator::GetPosConst() < b.GetPosConst();
        }

        bool operator<=(const ConstIterator b) const {
            return BaseIterator::GetPosConst() <= b.GetPosConst();
        }

        bool operator>=(const ConstIterator b) const {
            return BaseIterator::GetPosConst() >= b.GetPosConst();
        }

        bool operator>(const ConstIterator b) const {
            return BaseIterator::GetPosConst() > b.GetPosConst();
        }

        bool operator==(const ConstIterator b) const {
            return BaseIterator::GetPosConst() == b.GetPosConst();
        }

        bool operator!=(const ConstIterator b) const {
            return BaseIterator::GetPosConst() != b.GetPosConst();
        }

        ConstIterator& operator--() {
            return *(this->operator-=(1));
        }

        ConstIterator operator--(int) {
            ConstIterator tmp = *this;
            this->operator-=(1);
            return tmp;
        }

        ConstIterator& operator++() {
            this->operator+=(1);
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator tmp = *this;
            this->operator+=(1);
            return tmp;
        }

        ConstIterator& operator+=(difference_type num) {
            this->operator-=(-num);
            return *this;
        };

        const_reference operator[](size_t n) const {
            auto tmp = *this;
            return *(tmp.operator+=(n).GetPtr());
        }

        const_reference operator*() const {
            return *(BaseIterator::GetPtrConst());
        }

        friend difference_type operator-(ConstIterator const& a, ConstIterator const& b) {
            return a.GetPosConst() - b.GetPosConst();
        }

        friend ConstIterator operator-(ConstIterator const& a, difference_type n) {
            return a + (-n);
        }

        friend ConstIterator operator+(ConstIterator const& a, difference_type n) {
            return a + n;
        }

        friend ConstIterator operator+(difference_type n, ConstIterator const& a) {
            return a + n;
        }
    };

    Allocator& GetAlloc() {
        return alloc;
    }

    bounds GetHead() {
        return head_;
    }

    bounds& GetHeadRef() {
        return head_;
    }

    bounds GetTail() {
        return tail_;
    }

    bounds& GetTailRef() {
        return tail_;
    }

    T*& GetMemoryStart() {
        return memory_start;
    }

    size_t& GetBufferCapacity() {
        return capacity_;
    }

    CircIterator begin() {
        return CircIterator(0, this);
    };

    CircIterator end() {
        return CircIterator(size(), this);
    };

    const ConstIterator& cbegin() {
        return ConstIterator(0, *this);
    }

    const ConstIterator& cend() {
        return ConstIterator(size(), *this);
    }

    CircIterator erase(CircIterator pos) {
        MoveObjects(pos + 1, end(), pos);
        return pos;
    }

    CircIterator erase(CircIterator start, CircIterator finish) {
        MoveObjects(finish, end(), start);
        return start;
    }

    void assign(CircIterator i, CircIterator j) {
        clear();
        for (size_t cnt = 0; cnt != j - i; cnt++) {
            push_back(i[cnt]);
        }
    }

    void assign(CCircularBuffer& b) {
        assign(b.begin(), b.end());
    }

    void assign(size_t n, T obj) {
        clear();
        for (size_t i = 0; i < n; i++) {
            push_back(obj);
        }
    }

    void swap(CCircularBuffer& b) {
        CCircularBuffer<T, Allocator> tmp;
        tmp.memory_start = memory_start;
        memory_start = b.memory_start;
        b.memory_start = tmp.memory_start;
        tmp.capacity_ = capacity_;
        capacity_ = b.capacity_;
        b.capacity_ = tmp.capacity_;
        tmp.head_ = head_;
        tmp.tail_ = tail_;
        head_ = b.head_;
        tail_ = b.tail_;
        b.head_ = tmp.head_;
        b.tail_ = tmp.tail_;
    }

    bool empty() {
        return size() == 0;
    };

    size_t size() {
        return tail_ < head_ ? capacity_ - (head_ - tail_) : tail_ - head_;
    };

    CCircularBuffer& operator=(CCircularBuffer b) {
        clear();
        if (capacity_ < b.GetBufferCapacity()) {
            alloc_traits::deallocate(alloc, memory_start, capacity_);
            memory_start = alloc_traits::allocate(alloc, b.GetBufferCapacity());
            capacity_ = b.GetBufferCapacity();
        }
        b.MoveContentsTo(memory_start);
        head_ = 0;
        tail_ = b.size();
        return *this;
    }

    bool operator==(CCircularBuffer<T>& b) {
        if (b.size() == size()) {
            auto start = begin();
            for (auto bstart = b.begin(); bstart != b.end(); bstart++, start++) {
                if (bstart != start) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    bool operator!=(CCircularBuffer& b) {
        return !(this->operator==(b));
    }

    CCircularBuffer(CCircularBuffer const& a) {
        capacity_ = 0;
        operator=(a);
    }

    CCircularBuffer& operator=(std::initializer_list<T> list) {
        clear();
        if (capacity_ - reserved < list.size()) {
            alloc_traits::deallocate(alloc, memory_start, capacity_);
            memory_start = alloc_traits::allocate(alloc, list.size());
            capacity_ = list.size() + reserved;
        }
        for (auto i = list.begin(); i != list.end(); i++) {
            push_back(*i);
        }
        return *this;
    }

    CCircularBuffer(std::initializer_list<T> list) {
        capacity_ = list.size() + reserved;
        this->operator=(list);
    }

    void push_back(T& obj) {
        alloc_traits::construct(alloc, memory_start + tail_, obj);
        IncrementTail();
        alloc_traits::destroy(alloc, memory_start + tail_);
    }

    void push_back(const T& obj) {
        alloc_traits::construct(alloc, memory_start + tail_, obj);
        IncrementTail();
        alloc_traits::destroy(alloc, memory_start + tail_);
    }

    void pop() {
        if (tail_ != head_) {
            IncrementHead();
        }
    }

    CCircularBuffer() {
        capacity_ = 10;
        memory_start = alloc_traits::allocate(alloc, capacity_);
    }

    explicit CCircularBuffer(size_t size) {
        capacity_ = size + reserved;
        memory_start = alloc_traits::allocate(alloc, capacity_);
    }

    CCircularBuffer(size_t size, T obj) : CCircularBuffer(size) {
        for (size_t i = 0; i < size; i++) {
            push_back(obj);
        }
    }

    CCircularBuffer(CircIterator i, CircIterator j) : CCircularBuffer(j - i) {
        head_ = abs(j - i);
        for (CircIterator ind = i; ind < j; ind++) {
            memory_start[ind - i] = *ind;
        }
    }

    explicit CCircularBuffer(CCircularBuffer<T> buff) : CCircularBuffer(buff.start(), buff.end()) {}

    void MoveContentsTo(T* new_buffer, size_t offset = 0, size_t buff_capacity = 0) {
        if (buff_capacity == 0) {
            buff_capacity = offset + GetBufferCapacity();
        }
        for (auto i = begin(); i != end(); i++) {
            new_buffer[(i.GetPos() + offset) % buff_capacity] = *i;
        }
    }

    void MoveObjects(CircIterator first, CircIterator last, CircIterator destination) {
        if (first.GetBuffConst() == destination.GetBuffConst() && destination < first) {
            DecreaseTail(first - destination);
            for (CircIterator i = first; i != last; i++) {
                destination[i - first] = *i;
            }
        } else {
            for (CircIterator i = last - 1; i > first; i--) {
                destination[i - first] = *i;
            }
            destination[0] = *first;
        }
    }

    void clear() {
        head_ = 0;
        tail_ = 0;
        for (size_t i = 0; i < capacity_; i++) {
            alloc_traits::destroy(alloc, memory_start + i);
        }
    }

    ~CCircularBuffer() {
        clear();
        alloc_traits::deallocate(alloc, memory_start, capacity_);
    }
};

template<class T, class Allocator = std::allocator<T>>
class CCircularBufferExtended : public CircBuff::CCircularBuffer<T, Allocator> {
    using CircularBuffer = CircBuff::CCircularBuffer<T, Allocator>;
    using alloc_traits = std::allocator_traits<Allocator>;
public:
    explicit CCircularBufferExtended(size_t size) : CircularBuffer(size) {}

    CCircularBufferExtended(size_t size, T obj) : CircularBuffer(size) {
        for (size_t i = 0; i < size; i++) {
            CircularBuffer::push_back(obj);
        }
    }

    CCircularBufferExtended() : CircularBuffer() {}

    CCircularBufferExtended(typename CircularBuffer::CircIterator i, typename CircularBuffer::CircIterator j)
        : CircularBuffer(i, j) {}

    CCircularBufferExtended(CCircularBufferExtended& buf) : CCircularBufferExtended(buf.start(), buf.end()) {}

    void IncrementTail() override {
        CircularBuffer::GetTailRef()++;
        CircularBuffer::GetTailRef() %= CircularBuffer::GetBufferCapacity();
        if ((CircularBuffer::GetTail() + reserved) % CircularBuffer::GetBufferCapacity() == CircularBuffer::GetHead()) {
            Extend();
        }
    }

    void Extend() {
        size_t previous_capacity = CircularBuffer::GetBufferCapacity() - reserved;
        size_t previous_size = CircularBuffer::size();
        T* tmp = alloc_traits::allocate(CircularBuffer::GetAlloc(), previous_capacity * 2 + reserved);
        CircularBuffer::MoveContentsTo(tmp);
        CircularBuffer::clear();
        CircularBuffer::GetTailRef() = previous_size;
        CircularBuffer::GetHeadRef() = 0;
        alloc_traits::deallocate(CircularBuffer::GetAlloc(),
                                 CircularBuffer::GetMemoryStart(),
                                 CircularBuffer::GetBufferCapacity());
        CircularBuffer::GetMemoryStart() = tmp;
        CircularBuffer::GetBufferCapacity() = previous_capacity * 2 + reserved;
    }

    void IncreaseTail(size_t n) {
        while (CircularBuffer::size() + n > CircularBuffer::GetBufferCapacity() - reserved) {
            Extend();
        }
        CircularBuffer::GetTailRef() += n;
    }

    typename CircularBuffer::CircIterator insert(typename CircularBuffer::CircIterator it, T obj) {
        MoveObjects(it, CircularBuffer::end(), it + 1);
        *it = obj;
        return it;
    }

    typename CircularBuffer::CircIterator& insert(typename CircularBuffer::CircIterator before,
                                                  typename CircularBuffer::CircIterator iterator1,
                                                  typename CircularBuffer::CircIterator iterator2) {
        IncreaseTail(iterator2 - iterator1);
        MoveObjects(before, CircularBuffer::end(), before + (iterator2 - iterator1));
        MoveObjects(iterator1, iterator2, before);
        return before;
    }

    typename CircularBuffer::CircIterator& insert(typename CircularBuffer::CircIterator before,
                                                  CCircularBufferExtended& copy) {
        return insert(before, copy.begin(), copy.end());
    }

    typename CircularBuffer::CircIterator& insert(typename CircularBuffer::CircIterator before, size_t size, T copy) {
        CCircularBufferExtended<T, Allocator> tmp(size, copy);
        return insert(before, tmp);
    }
};

template<class T, class Allocator = std::allocator<T>>
void swap(CircBuff::CCircularBuffer<T, Allocator>& a, CircBuff::CCircularBuffer<T, Allocator>& b) {
    a.swap(b);
}

const size_t reserved = 2;
}

template<class T, class Allocator = std::allocator<T>>
void swap(typename CircBuff::CCircularBuffer<T, Allocator>::CircIterator& a,
          typename CircBuff::CCircularBuffer<T, Allocator>::CircIterator& b) {
    a.swap(b);
}
// CircBuff

#endif //LABWORK_8_ASTRO_PETER_BUFFER_CCIRCULARBUFFER_H_
