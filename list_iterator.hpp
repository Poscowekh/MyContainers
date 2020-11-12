#ifndef LIST_ITERATOR_HPP
#define LIST_ITERATOR_HPP
#include <iterator>
#include "Node.hpp"

/// Basic Iterator for List using Node*
template<typename value, typename category = std::random_access_iterator_tag>
class ListIterator : public std::iterator<category, value, std::size_t> {
public:
    typedef value value_type;
    typedef value& reference;
    typedef const value& const_reference;
    typedef value* pointer;
    typedef std::random_access_iterator_tag iterator_category;
    typedef std::size_t difference_type;

private:
    using node = Node<value>*;
    using const_node = const Node<value>*;
    enum Direction {
        Right,
        Left,
        None
    };

    node _node;

    std::pair<difference_type, Direction> _distance_to_non_end(const ListIterator& other) const {
        difference_type result = 0;
        ListIterator left = other, right = other;

        while(left._node != _node && right._node != _node) {
            if(left._node != 0)
                ++left;
            if(right._node != 0)
                ++right;
            ++result;
        };

        if(left._node == _node)
            return std::make_pair(result, Direction::Left);
        else
            return std::make_pair(result, Direction::Right);
    };
    difference_type _distance_to_end(const ListIterator& other) const {
        difference_type result = 0;
        ListIterator copy = other;

        while(copy._node != 0) {
            ++copy;
            ++result;
        };

        return result;
    };

    std::pair<difference_type, Direction> distance_direction(const ListIterator& other) const {
        if((_node == 0 && other._node == 0) || _node == other._node)
            return std::make_pair(0, Direction::None);
        if(other._node == 0 && _node != 0) {
            auto tmp =  other.distance_direction(*this);
            tmp.second = Direction::Right;
            return tmp;
        } if(_node != 0)
            return _distance_to_non_end(other);
        return std::make_pair(_distance_to_end(other), Direction::Right);
    };

public:
    ListIterator() = delete;
    ListIterator(node other_node) : _node(other_node) {};
    ListIterator(const_node other_node) : _node(const_cast<node>(other_node)) {};
    ListIterator(const ListIterator* other) : _node(other->_node) {};
    ListIterator(const ListIterator& other) : _node(other._node) {};
    ListIterator(ListIterator&& other) : _node(other._node) {
        other._node = 0;
    };

    const ListIterator& operator=(const ListIterator& other) {
        _node = other._node;
        return *this;
    };
    const ListIterator& operator=(ListIterator&& other) {
        _node = other._node;
        other._node = 0;
        return *this;
    };

    ListIterator& operator++(){
        if(_node == 0)
            throw std::runtime_error("\nList iterator exception: incrementing end iterator");
        _node = _node->next;
        return *this;
    };
    ListIterator operator++(int){
        if(_node == 0)
            throw std::runtime_error("\nList iterator exception: incrementing end iterator");
        ListIterator copy = ListIterator(this);
        _node = _node->next;
        return copy;
    };

    ListIterator& operator--(){
        if(_node == 0)
            throw std::runtime_error("\nList iterator exception: decrementing end iterator");
        _node = _node->prev;
        return *this;
    };
    ListIterator operator--(int){
        if(_node == 0)
            throw std::runtime_error("\nList iterator exception: decrementing end iterator");
        ListIterator copy = ListIterator(this);
        _node = _node->prev;
        return copy;
    };

    ListIterator copy() const{
        return ListIterator(this);
    };

    reference operator*(){
        if(_node == 0)
            throw std::runtime_error("\nList iterator exception: dereferencing end iterator");
        return *(_node->data);
    };
    const_reference operator*() const{
        if(_node == 0)
            throw std::runtime_error("\nList iterator exception: dereferencing end iterator");
        return *(_node->data);
    };

    bool operator==(const ListIterator& other) const {
        return _node == other._node;
    };
    bool operator!=(const ListIterator& other) const {
        return _node != other._node;
    };

    bool operator>(const ListIterator& other) const {
        if(distance_direction(other).second == Direction::Right)
            return true;
        return false;
    };
    bool operator>=(const ListIterator& other) const {
        auto direction = distance_direction(other).second;
        if(direction == Direction::Right || direction == Direction::None)
            return true;
        return false;
    };

    bool operator<(const ListIterator& other) const {
        if(distance_direction(other).second == Direction::Left)
            return true;
        return false;
    };
    bool operator<=(const ListIterator& other) const {
        auto direction = distance_direction(other).second;
        if(direction == Direction::Left || direction == Direction::None)
            return true;
        return false;
    };

    /// unsafe
    ListIterator operator+(const difference_type& distance) const{
        ListIterator result = copy();
        result += distance;
        return result;
    };
    /// unsafe
    ListIterator operator-(const difference_type& distance) const{
         ListIterator result = copy();
         result -= distance;
         return result;
    };

    /// unsafe
    void operator+=(const difference_type& distance){
        for(auto i = 0; i < distance; ++i, ++(*this));
    };
    /// unsafe
    void operator-=(const difference_type& distance){
        for(auto i = 0; i < distance; ++i, --(*this));
    };

    difference_type operator-(const ListIterator& other) const{
        if((_node == 0 && other._node == 0) || _node == other._node)
            return 0;
        if(other._node == 0 && _node != 0)
            return other - *this;
        if(_node != 0)
            return _distance_to_non_end(other).first;
        return _distance_to_end(other);
    };
};

/// Basic const Iterator for List using Node*
template<typename value, typename category = std::random_access_iterator_tag>
class ConstListIterator : public std::iterator<category, value, std::size_t> {
public:
    typedef value value_type;
    typedef value& reference;
    typedef const value& const_reference;
    typedef value* pointer;
    typedef std::random_access_iterator_tag iterator_category;
    typedef std::size_t difference_type;

private:
    using node = Node<value>*;
    using const_node = const Node<value>*;
    using _iterator = ListIterator<value, category>;

    _iterator _iter;

public:
    ConstListIterator() = delete;
    ConstListIterator(node other_node) : _iter(other_node) {};
    ConstListIterator(const_node other_node) : _iter(other_node) {};
    ConstListIterator(const ConstListIterator* other) : _iter(other->_iter) {};
    ConstListIterator(const ConstListIterator& other) : _iter(other._iter) {};
    ConstListIterator(ConstListIterator&& other) : _iter(other._iter) {
        other._node = 0;
    };

    const ConstListIterator& operator=(const ConstListIterator& other) {
        _iter = other._iter;
        return *this;
    };
    const ConstListIterator& operator=(ConstListIterator&& other) {
        _iter = other._iter;
        return *this;
    };

    const ConstListIterator& operator++(){
        ++_iter;
        return *this;
    };
    ConstListIterator operator++(int){
        ConstListIterator copy = ConstListIterator(this);
        ++_iter;
        return copy;
    };

    const ConstListIterator& operator--(){
        --_iter;
        return *this;
    };
    ConstListIterator operator--(int){
        ConstListIterator copy = ConstListIterator(this);
        --_iter;
        return copy;
    };

    ConstListIterator copy() const{
        return ConstListIterator(this);
    };

    const_reference operator*() const{
        return *_iter;
    };

    bool operator==(const ConstListIterator& other) const {
        return _iter == other._iter;
    };
    bool operator!=(const ConstListIterator& other) const {
        return _iter != other._iter;
    };

    bool operator>(const ConstListIterator& other) const {
        return _iter > other._iter;
    };
    bool operator>=(const ConstListIterator& other) const {
        return _iter >= other._iter;
    };

    bool operator<(const ConstListIterator& other) const {
        return _iter < other._iter;
    };
    bool operator<=(const ConstListIterator& other) const {
        return _iter <= other._iter;
    };

    /// unsafe
    ConstListIterator operator+(const difference_type& distance) const{
        ConstListIterator result = copy();
        result += distance;
        return result;
    };
    /// unsafe
    ConstListIterator operator-(const difference_type& distance) const{
         ConstListIterator result = copy();
         result -= distance;
         return result;
    };

    /// unsafe
    void operator+=(const difference_type& distance){
        _iter += distance;
    };
    /// unsafe
    void operator-=(const difference_type& distance){
        _iter -= distance;
    };

    difference_type operator-(const ConstListIterator& other) const{
        return _iter - other._iter;
    };
};

#endif // LIST_ITERATOR_HPP
