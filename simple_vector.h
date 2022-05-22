#pragma once

#include <cassert>
#include "array_ptr.h"
#include <initializer_list>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <memory>

using namespace std::literals;

struct ReserveProxyObj
{
    size_t capacity_;
    ReserveProxyObj() = default;
    explicit ReserveProxyObj(size_t capacity) : capacity_(capacity)  {}



};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
private:
    size_t size_ = 0;
    size_t capacity_ = 0;
    ArrayPtr<Type> data_;

public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size) : size_(size), capacity_(size) , data_(new Type[size_]) {
        std::fill(data_.Get(), data_.Get() + size_, Type{});

    }
    explicit SimpleVector(ReserveProxyObj obj) : size_(0), capacity_( obj.capacity_), data_(new Type [capacity_])
    {

    }

    SimpleVector ( SimpleVector &&other) {
      swap(std::move(other));
    }

    SimpleVector(size_t size, const Type& value) : size_(size), capacity_(size), data_(new Type [capacity_]) {
        std::fill(data_.Get(), data_.Get() + size_, Type{value});
    }

    SimpleVector(std::initializer_list<Type> init) : size_(init.size()), capacity_(init.size()), data_(new Type[capacity_]) {
        std::copy( init.begin(), init.end(), begin());

    }

    SimpleVector(const SimpleVector& other) : size_(other.size_), capacity_(other.capacity_), data_(new Type [capacity_]){
        if( *this != other)
        {
            for(size_t i = 0; i < capacity_;i++)
            {
                data_[i] = other.data_[i];
            }
        }
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if(*this != rhs)
        {
            SimpleVector<Type> tmp(rhs);
            this->swap(tmp);
        }
        return *this;
    }

    SimpleVector& operator=(SimpleVector &&rhs)
    {
        swap(std::move(rhs));
        return *this;
    }

    ~SimpleVector()
    {

    }

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return data_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return data_[index];
    }

    Type& At(size_t index) {
        // Напишите тело самостоятельно
        if(index >= size_)
            throw std::out_of_range ("invalid index"s);
        return data_[index];
    }


    const Type& At(size_t index) const {
        if(index >= size_)
            throw std::out_of_range ("invalid index"s);
        return data_[index];
    }

    void PushBack(const Type& item) {
        resize_capacity();
        data_[size_++] = item;
    }

    void PushBack(Type&& item) {
        resize_capacity();
        data_[size_++] = std::move(item);
    }


    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= begin() and pos <= end());
        size_t tmpPos = pos - begin();
        if( size_ +1 <= capacity_)
        {
            std::copy_backward(std::make_move_iterator(begin() + tmpPos), std::make_move_iterator(end()), end() + 1);
            data_[tmpPos] = std::move(value);
            ++size_;
        }
        else
        {
            SimpleVector<Type> tmp (*this);
            tmp.Resize(size_ + 1);
            std::copy_backward(std::make_move_iterator(tmp.arr_ + tmpPos),std::make_move_iterator(tmp.end() - 1), tmp.end());
            tmp[tmpPos] = std::move(value);
            this->swap(tmp);
        }

        return begin()+ tmpPos;
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos >= begin() and pos <= end());
        size_t tmpPos = pos - begin();
        if( size_ +1 <= capacity_)
        {
            std::copy_backward(std::make_move_iterator(begin()+ tmpPos), std::make_move_iterator(end()), end() + 1);
            data_[tmpPos] = std::move(value);
            ++size_;
        }
        else
        {
            SimpleVector<Type> tmp (std::move(*this));
            tmp.Resize(size_ + 1);
            std::copy_backward(std::make_move_iterator(tmp.begin() + tmpPos),std::make_move_iterator(tmp.end() - 1), tmp.end());
            tmp[tmpPos] = std::move(value);
            this->swap(tmp);
        }

        return begin()+ tmpPos;
    }

    void PopBack() noexcept {
        if(data_)
            --size_;
    }

    Iterator Erase(ConstIterator pos) {
        assert(pos >= begin() and pos <= end());
        for( size_t i = pos - begin(); i != size_; ++i)
        {
            std::swap (data_[i],data_[i + 1]);
        }
        PopBack();
        return begin() + (pos - begin());
    }

    void swap(SimpleVector& other) noexcept {
        if(this != &other)
        {
            data_.swap(other.data_);
            std::swap(size_,other.size_);
            std::swap(capacity_,other.capacity_);
        }
    }

    void swap(SimpleVector &&other) noexcept {
           size_ = std::exchange(other.size_, 0);
           capacity_ = std::exchange(other.capacity_, 0);
           data_.swap(other.data_);
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Resize(size_t new_size) {

        if(new_size <=capacity_)
        {
            if(new_size > size_)
            {
                for(size_t i = 0; i < (capacity_ - size_); ++i)
                {
                    PushBack(std::move(Type{}));
                }
            }
            size_ = new_size;
        }
        else
        {
            ArrayPtr<Type> tmp(std::max(capacity_ *2, new_size));
            std::copy( std::make_move_iterator(begin()), std::make_move_iterator(end()), tmp.Get());
            for(size_t i = size_; i < std::max(capacity_ *2, new_size) *2;i++)
            {
                tmp[i] = std::move(Type{});
            }
            capacity_ = std::max(capacity_ *2, new_size);
            size_ = new_size;
            data_.swap(tmp);
        }
    }

    void Reserve(size_t new_capacity)
    {
        if( new_capacity >= capacity_)
        {
            SimpleVector<Type> tmp(new_capacity);
            std::copy( std::make_move_iterator(begin()), std::make_move_iterator(end()), tmp.begin());
            tmp.size_ = size_;
            this->swap(tmp);

        }
    }
	
    Iterator begin() noexcept {
        return data_.Get();
    }

    Iterator end() noexcept {
        return data_.Get() + size_;
    }

    ConstIterator begin() const noexcept {
        return data_.Get();
    }

    ConstIterator end() const noexcept {
        return data_.Get() + size_;
    }

    ConstIterator cbegin() const noexcept {
        return data_.Get();
    }

    ConstIterator cend() const noexcept {
        return data_.Get() + size_;
    }
private:
    void resize_capacity()
    {
        if (data_.Get() == nullptr)
        {
            ArrayPtr<Type> tmp(1);
            data_.swap(tmp);
            capacity_ = 1;

        }

        else if (size_ == capacity_)
        {
            ArrayPtr<Type> tmp(capacity_ * 2);
            std::copy( std::make_move_iterator(begin()), std::make_move_iterator(end()), tmp.Get());
            for(size_t i = size_; i < capacity_;i++)
            {
                tmp[i] = std::move(Type{});
            }
            data_.swap(tmp);

            capacity_ = capacity_ * 2;

        }
    }
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs) and lhs != rhs;
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return lhs < rhs or lhs == rhs;
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return lhs > rhs or rhs == lhs;
}
