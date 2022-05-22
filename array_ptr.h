#pragma once
#include <cassert>
#include <cstdlib>
#include <utility>


template <typename Type>
class ArrayPtr {
public:

    ArrayPtr() = default;

    explicit ArrayPtr(size_t size) {
        if(size != 0)
        raw_ptr_ = new Type[size];
    }

    explicit ArrayPtr(Type* raw_ptr) noexcept  {
        raw_ptr_ = raw_ptr;
    }

    explicit ArrayPtr(ArrayPtr<Type> && other)
    {
        raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
    }

    ArrayPtr(const ArrayPtr&) = delete;
    ~ArrayPtr() {
        delete [] raw_ptr_;
    }

    ArrayPtr& operator=(const ArrayPtr&) = delete;
    ArrayPtr& operator=(ArrayPtr&& other)
    {
        raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
    }

    [[nodiscard]] Type* Release() noexcept {
         Type* tmp = raw_ptr_;
        return raw_ptr_ = nullptr, tmp;
    }


    Type& operator[](size_t index) noexcept {
        return raw_ptr_[index];
    }


    const Type& operator[](size_t index) const noexcept {
        return raw_ptr_[index];
    }

    explicit operator bool() const {
        return raw_ptr_ != nullptr;
    }


    Type* Get() const noexcept {
        return raw_ptr_;
    }


    void swap(ArrayPtr& other) noexcept {
        if( this != &other)
        {
            std::swap(other.raw_ptr_,raw_ptr_);
        }
    }


private:
    Type* raw_ptr_ = nullptr;
};
