#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <new>

template <typename T>
class RawMemory {
public:
    T* data;
    size_t capacity = 0;

    RawMemory(const size_t& n) {
        capacity = n;
        data = static_cast<T*>(::operator new(capacity * sizeof(T)));
    }

    auto begin() {
        return data;
    }

    void clear(int sz) {
        std::destroy(data, data + sz);
        ::operator delete(data);
    }

    ~RawMemory() {
    }
};

template <typename T>
class Vector {
private:
    RawMemory<T> Raw;
    size_t sz = 0;

public:
    Vector() : sz(0), Raw(0) {
    }

    Vector(const size_t& n) : sz(n), Raw(n) {
        std::uninitialized_default_construct_n(begin(), sz);
    }

    Vector(const Vector& other) : sz(other.size()), Raw(other.size()) {
        std::uninitialized_copy_n(other.begin(), sz, begin());
    }

    Vector& operator=(const Vector& other) {
        Raw.clear();
        sz = other.sz;
        std::uninitialized_copy_n(other.begin(), sz, begin());
    }

    void reallocate() {
        if (sz == Raw.capacity) {
            Raw.capacity *= 2;
            size_t add = Raw.capacity - sz;
            if (Raw.capacity == 0) Raw.capacity = 1;
            T* ToRaw = static_cast<T*>(::operator new(Raw.capacity * sizeof(T)));
            std::uninitialized_copy_n(Raw.begin(), sz, ToRaw);
            std::uninitialized_default_construct_n(ToRaw + sz, add);
            Raw.clear(Raw.capacity / 2);
            Raw.data = ToRaw;
        }
    }

    const int capacity() const {
        return Raw.capacity;
    }

    void reserve(const size_t& n) {
        if (n > Raw.capacity) {
            Raw.capacity = n;
            T* ToRaw = static_cast<T*>(::operator new(Raw.capacity * sizeof(T)));
            std::uninitialized_copy_n(Raw.begin(), sz, ToRaw);
            Raw.clear(sz);
            Raw.data = ToRaw;
        }
    }

    void resize(const size_t& n) {
        if (n < sz) {
            std::destroy(Raw.data + n, Raw.data + sz);
        } else {
            if (n < Raw.capacity) {
                std::uninitialized_default_construct_n(Raw.begin() + sz, n - sz);
            } else {
                T* ToRaw = static_cast<T*>(::operator new(n * sizeof(T)));
                std::uninitialized_copy_n(Raw.begin(), sz, ToRaw);
                std::uninitialized_default_construct_n(ToRaw + sz, n - sz);
                Raw.clear(sz);
                Raw.data = ToRaw;
            }
        }
        sz = n;
        Raw.capacity = std::max(Raw.capacity, n);
    }

    void swap(Vector& other) {
        std::swap(Raw, other.Raw);
        std::swap(sz, other.sz);
    }

    auto begin() {
        return Raw.data;
    }

    const auto begin() const {
        return Raw.data;
    }

    auto end() {
        return Raw.data + sz;
    }

    const auto end() const {
        return Raw.data + sz;
    }

    void push_back(const T& x) {
        reallocate();
        new (Raw.data + sz) T(x);
        sz++;
    }

    void push_back(T&& x) {
        reallocate();
        new (Raw.data + sz) T(std::move(x));
        sz++;
    }

    void pop_back() {
        std::destroy(Raw.data + sz - 1, Raw.data + sz);
        sz--;
    }

    const size_t size() const {
        return sz;
    }

    T& operator[](const size_t& i) const {
        return Raw.data[i];
    }

    T& operator[](const size_t& i) {
        return Raw.data[i];
    }

    ~Vector() {
        Raw.clear(sz);
    }
};
