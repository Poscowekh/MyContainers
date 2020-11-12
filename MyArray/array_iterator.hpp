#ifndef ARRAY_ITERATOR_HPP
#define ARRAY_ITERATOR_HPP
#include "../_libs.hpp"

/// Iterator rai for Array class
template<typename value, typename category = std::random_access_iterator_tag>
class ArrayIterator : public std::iterator<category, value, std::size_t>{
public:
    using value_type = value;
    using reference = value&;
    using const_reference = const value&;
    using pointer = value*;
    using const_pointer = const value*;
    using difference_type = std::size_t;
    using iterator_category = category;

private:
    pointer _ptr;

public:
    ArrayIterator() = delete;
    ArrayIterator(pointer data_ptr) : _ptr(data_ptr) {};
    ArrayIterator(const_pointer data_ptr) {
        _ptr = data_ptr;
    };
    ArrayIterator(const ArrayIterator* other) : _ptr(other->_ptr) {};
    ArrayIterator(const ArrayIterator& other) : _ptr(other._ptr) {};
    ArrayIterator(ArrayIterator&& other) : _ptr(other._ptr) {};

    const ArrayIterator& operator=(const ArrayIterator& other) {
        _ptr = other._ptr;
        return *this;
    };
    const ArrayIterator& operator=(ArrayIterator&& other) {
        _ptr = other._ptr;
        return *this;
    };

     ArrayIterator& operator++() {
        if(_ptr == 0)
            throw std::runtime_error("\nArray iterator exception: incrementing end iterator");
        _ptr += 1;
        return *this;
    };
    ArrayIterator operator++(int){
        if(_ptr == 0)
            throw std::runtime_error("\nArray iterator exception: incrementing end iterator");
        ArrayIterator copy = ArrayIterator(this);
        _ptr += 1;
        return copy;
    };

    ArrayIterator& operator--(){
        if(_ptr == 0)
            throw std::runtime_error("\nArray iterator exception: decrementing end iterator");
        _ptr -= 1;
        return *this;
    };
    ArrayIterator operator--(int){
        if(_ptr == 0)
            throw std::runtime_error("\nArray iterator exception: decrementing end iterator");
        ArrayIterator copy = ArrayIterator(this);
        _ptr -= 1;
        return copy;
    };

    reference operator*(){
        if(_ptr == 0)
            throw std::runtime_error("\nArray iterator exception: dereferencing end iterator");
        return *_ptr;
    };
    const_reference operator*() const{
        if(_ptr == 0)
            throw std::runtime_error("\nArray iterator exception: dereferencing end iterator");
        return *_ptr;
    };

    ArrayIterator copy() const{
        return ArrayIterator(this);
    };

    bool operator==(const ArrayIterator& other) const {
        return _ptr == other._ptr;
    };
    bool operator!=(const ArrayIterator& other) const {
        return _ptr != other._ptr;
    };

    bool operator>(const ArrayIterator& other) const {
        return _ptr > other._ptr;
    };
    bool operator>=(const ArrayIterator& other) const {
        return _ptr >= other._ptr;
    };

    bool operator<(const ArrayIterator& other) const {
        return _ptr < other._ptr;
    };
    bool operator<=(const ArrayIterator& other) const {
        return _ptr <= other._ptr;
    };

    // unsafe
    ArrayIterator operator+(const difference_type& distance) const{
        if(_ptr == 0)
            throw std::runtime_error("\nArray iterator exception: advancing end iterator");
        ArrayIterator result = ArrayIterator(this);
        result->_ptr += distance;
        return result;
    };
    // unsafe
    ArrayIterator operator-(const difference_type& distance) const{
        if(_ptr == 0)
            throw std::runtime_error("\nArray iterator exception: advancing end iterator");
        ArrayIterator result = ArrayIterator(this);
        result->_ptr -= distance;
        return result;
    };

    // unsafe
    void operator+=(const difference_type& distance){
        if(_ptr == 0)
            throw std::runtime_error("\nArray iterator exception: advancing end iterator");
        _ptr += distance;
    };
    // unsafe
    void operator-=(const difference_type& distance){
        if(_ptr == 0)
            throw std::runtime_error("\nArray iterator exception: advancing end iterator");
        _ptr -= distance;
    };

    difference_type operator-(const ArrayIterator& other) const {
        return std::abs(_ptr - other._ptr);
    };
};

/// Constant rai Iterator for Array class
template<typename value, typename category = std::random_access_iterator_tag>
class ConstArrayIterator : std::iterator<category, value, std::size_t> {
public:
    using value_type = value;
    using reference = value&;
    using const_reference = const value&;
    using pointer = value*;
    using const_pointer = const value*;
    using difference_type = std::size_t;
    using iterator_category = category;

private:
    using _iterator = ArrayIterator<value, iterator_category>;

    _iterator _iter;

public:
    ConstArrayIterator() = delete;
    ConstArrayIterator(pointer ptr) : _iter(ptr) {};
    ConstArrayIterator(const_pointer ptr) : _iter(ptr) {};
    ConstArrayIterator(ConstArrayIterator&& other) : _iter(other._iter) {};
    ConstArrayIterator(ConstArrayIterator* other) : _iter(other->_iter) {};
    ConstArrayIterator(const ConstArrayIterator& other) : _iter(other._iter) {};

    const ConstArrayIterator& operator=(const ConstArrayIterator& other) {
        _iter = other._iter;
        return *this;
    };
    const ConstArrayIterator& operator=(ConstArrayIterator&& other) {
        _iter = other._iter;
        return *this;
    };

    ConstArrayIterator& operator++() {
        ++_iter;
        return *this;
    };
    ConstArrayIterator operator++(int){
        ConstArrayIterator copy = ConstArrayIterator(this);
        ++_iter;
        return copy;
    };

    ConstArrayIterator& operator--(){
        --_iter;
        return *this;
    };
    ConstArrayIterator operator--(int){
        ConstArrayIterator copy = ConstArrayIterator(this);
        --_iter;
        return copy;
    };

    const_reference operator*() const{
        return *_iter;
    };

    ConstArrayIterator copy() const{
        return ConstArrayIterator(this);
    };

    bool operator==(const ConstArrayIterator& other) const {
        return _iter == other._iter;
    };
    bool operator!=(const ConstArrayIterator& other) const {
        return _iter != other._iter;
    };

    bool operator>(const ConstArrayIterator& other) const {
        return _iter > other._iter;
    };
    bool operator>=(const ConstArrayIterator& other) const {
        return _iter >= other._iter;
    };

    bool operator<(const ConstArrayIterator& other) const {
        return _iter < other._iter;
    };
    bool operator<=(const ConstArrayIterator& other) const {
        return _iter <= other._iter;
    };

    // unsafe
    ConstArrayIterator operator+(const difference_type& distance) const{
        ConstArrayIterator result = ConstArrayIterator(this);
        result->_iter += distance;
        return result;
    };
    // unsafe
    ConstArrayIterator operator-(const difference_type& distance) const{
        ConstArrayIterator result = ConstArrayIterator(this);
        result->_iter -= distance;
        return result;
    };

    // unsafe
    void operator+=(const difference_type& distance){
        _iter += distance;
    };
    // unsafe
    void operator-=(const difference_type& distance){
        _iter -= distance;
    };

    difference_type operator-(const ConstArrayIterator& other) const {
        return _iter - other._iter;
    };

};

#endif // ARRAY_ITERATOR_HPP
