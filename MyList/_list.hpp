#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include <iostream>
#include <algorithm>
#include "list_iterator.hpp"

/// Double-linked list implementation
template<typename T> class List{
private:
    /// a function that sets element's value by its index
    using InitializerFunction = T (*)(const int& index);
    /// rai iterator
    using iterator = ListIterator<T>;
    /// read-only rai iterator
    using const_iterator = ConstListIterator<T>;;

    using Node = Node<T>;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using value = T;

    Node* _head = 0;
    Node* _tail = 0;
    int _size = 0;

    /// also controls throwing exception
    Node* _get_node(const int& index) const{
        _check_except_index(index);
        if(index > _size / 2)
            return _from__tail(index);
        return _from__head(index);
    };
    Node* _from__head(const int& index) const{
        Node* tmp = _head;
        for(int i = 0; i < index; ++i)
            tmp = tmp->next;
        return tmp;
    };
    Node* _from__tail(int index) const{
        Node* tmp = _tail;
        for(int i = _size; i > index + 1; --i)
            tmp = tmp->prev;
        return tmp;
    };

    reference _get_data(const int& index){
        return *(_get_node(index)->data);
    };
    value _get_data(const int& index) const{
        return *(_get_node(index)->data);
    };

    /// concates a tmp list by pointer exchange
    void _concate_to_copy(List&& list) {
        if(_size == 0) {
            *this = List(list);
            return;
        };

        _tail->next = list._head;
        _tail->next->prev = _tail;
        _tail = list._tail;
        _size += list.size();

        list._tail = list._head = 0;
        list._size = 0;
    };

    /// inserts a tmp list by pointer exchange
    void _insert_to_copy(List&& list, const int& index) {
        _check_except_index_insert(index);

        if(_size == 0) {
            // exchange lists
            *this = list;
            return;
        };
        if(index == _size) {
            _concate_to_copy(std::move(list));
            return;
        };


        Node* at = _get_node(index);

        if(at->prev == 0) {
            // inserting to the _head
            at->prev = list._tail;
            at->prev->next = _head;
            _head = list._head;
        } else if(at == 0) {
            // inserting to the tail
            _tail->next = list._head;
            _tail->next->prev = _tail;
            _tail = list._tail;
        } else {
            // inserting in between
            Node* prev = at->prev;
            prev->next = list._head;
            prev->next->prev = prev;
            at->prev = list._tail;
            at->prev->next = at;
            prev = 0;
        };

        _size += list.size();

        at = 0;
        list._head = list._tail = 0;
        list._size = 0;
    };

    /// check index for exception
    void _check_except_index(const int& index) const {
        if(index < 0 || index >= _size)
            throw std::runtime_error("List exception: index out of range");
    };
    void _check_except_index_insert(const int& index) const {
        if(index < 0 || index > _size)
            throw std::runtime_error("List exception: index out of range");
    };
    /// check size for exception
    void _check_except_size(const int& size) const {
        if(size <= 0)
            throw std::logic_error("List exception: negative or zero size");
    };

public:
    List() : _size(0), _head(0), _tail(0) {};
    List(const List* other) : List() {
        Node* tmp = other->_head;
        while(tmp != 0){
            push_back(tmp->data);
            tmp = tmp->next;
        };
    };
    List(const List& other) : List(&other) {};
    List(List&& other) {
        std::swap(_head, other._head);
        std::swap(_tail, other._tail);
        std::swap(_size, other._size);
    };

    List(const_pointer data, const int& count, const int& from = 0) : List() {
        _check_except_size(count);
        if(data != 0)
            for(int i = from; i < count + from; ++i)
                push_back(data + from + i);
    };
    List(const int& size, const_reference default_member = T()) : List() {
        _check_except_size(size);
        for(int i = 0; i < size; ++i)
            push_back(&default_member);
    };
    List(const int& size, const_pointer default_member) : List(size, *default_member) {};

    List(const int& size, InitializerFunction func) : List() {
        _check_except_size(size);
        for(int i = 0; i < size; ++i)
            push_back(func(i));
    };
    List(const std::initializer_list<T>& list) : List(list.begin(), list.end()) {};
    /// Range constructor.
    /// ForwardIterator must support dereferencing and prefix increment,
    /// 'to' is not dereferenced!
    template<typename ForwardIterator>
    List(const ForwardIterator& from, const ForwardIterator& to) : List() {
        auto iter = from;
        while(iter != to){
            push_back(*iter);
            ++iter;
        };
    };
    /// Two range constructor.
    /// ForwardIterator1 & ForwardIterator2 must support dereferencing and prefix increment,
    /// 'to' is not dereferenced!
    template<typename ForwardIterator1, typename ForwardIterator2 = ForwardIterator1>
    List(const ForwardIterator1& from1, const ForwardIterator1& to1,
         const ForwardIterator2& from2, const ForwardIterator2& to2) : List(from1, to1) {
        auto iter = from2;
        while(iter != to2){
            push_back(*iter);
            ++iter;
        };
    };

    /// adds data to the list in the defined order
    void push_front(const_pointer data, const int& count = 1){
        _check_except_size(count);
        for(int i = 0; i < count; ++i){
            Node* tmp = new Node(data + i, 0, _head);
            if(_size == 0)
                _tail = tmp;
            else
                _head->prev = tmp;
            _head = tmp;
            ++_size;
        };
    };
    void push_front(const_reference data){
        push_front(&data);
    };

    /// adds data to the list in the defined order
    void push_back(const_pointer data, const int& count = 1){
        _check_except_size(count);

        for(int i = 0; i < count; ++i){
            Node* tmp = new Node(data + i, _tail, 0);
            if(_size == 0)
                _head = tmp;
            else
                _tail->next = tmp;
            _tail = tmp;
            ++_size;
        };
    };
    void push_back(const_reference data){
        push_back(&data);
    };

    /// returns reference to the first element
    reference front(){
        return _get_data(0);
    };
    const_reference front() const{
        return _get_data(0);
    };

    /// returns reference to the element with the 'index'
    reference get(const int& index){
        return _get_data(index);
    };
    const_reference get(const int& index) const{
        return _get_data(index);
    };

    /// returns reference to the last element
    reference back(){
        return _get_data(_size - 1);
    };
    const_reference back() const{
        return _get_data(_size - 1);
    };

    /// sets the value at 'index' to 'data'
    void set(const int& index, const_pointer data){
        _check_except_index(index);
        memcpy(_get_node(index)->data, data, sizeof(T));
    };
    void set(const int& index, const_reference data){
        set(index, &data);
    };

    /// expands list by 'count' with empty constructed elements
    void expand(const int& count, const_reference value_to_insert = T()){
        _check_except_size(count);
        for(int i = 0; i < count; ++i)
            push_back(value_to_insert);
    };

    const int& size() const{
        return _size;
    };
    /// true if the list is empty
    bool empty() const{
        if(_size > 0)
            return false;
        return true;
    };
    void clear(){
        if(_size > 0)
            pop_back(_size);
        _head = _tail = 0;
        _size = 0;
    };
    /// prints the list in []-brackets
    void print() const {
        std::cout << *this;
    };

    /// removes the last element returning its copy
    value pop_back(const int& count = 1){
        _check_except_index(count - 1);
        value result;

        for(int i = 0; i < count; ++i){
            if(i == count - 1)
                result = T(back());
            if(_size > 1){
                _tail->prev->next = 0;
                free(_tail->data);
                _tail = _tail->prev;
            } else {
                free(_tail);
                _head = _tail = 0;
            };
            --_size;
        };

        return result;
    };

    /// removes the first element returning its copy
    value pop_front(const int& count = 1){
        _check_except_index(count - 1);
        value result;

        for(int i = 0; i < count; ++i){
            if(i == count - 1)
                result = T(back());
            if(_size > 1){
                _head->next->prev = 0;
                free(_head->data);
                _head = _head->next;
            } else {
                free(_head);
                _head = _tail = 0;
            };
            --_size;
        };

        return result;
    };

    /// inserts 'data' at 'index'
    void insert(const_pointer data, const int& index){
        _check_except_index_insert(index);

        if(index == 0)
            push_front(data);
        else if(index == _size)
            push_back(data);
        else if(index < _size){
            Node* new_node = new Node(data, 0, _get_node(index));
            new_node->prev = new_node->next->prev;
            new_node->next->prev->next = new_node;
            new_node->next->prev = new_node;
            ++_size;
        };
    };
    void insert(const_reference data, const int& index){
        insert(&data, index);
    };
    /// inserts the range 'from'->'to' excluding 'to' at index 'at'
    template<typename ForwardIterator, typename EndIterator = ForwardIterator>
    void insert(const ForwardIterator& from, const EndIterator& to, const int& at) {
        _insert_to_copy(List(from, to), at);
    };
    /// inserts contents of initializer list
    void insert(const std::initializer_list<T>& list, const int& index) {
        insert(list.begin(), list.end(), index);
    };
    /// inserts contents of list
    void insert(const List& list, const int& index) {
        _insert_to_copy(List(list), index);
    };
    void insert(List&& list, const int& index) {
        _insert_to_copy(std::move(list), index);
    };

    /// removes data at 'index' returning its copy
    value remove(const int& index){
        _check_except_index(index);

        if(index == 0)
            return pop_front();
        else if(index == _size - 1)
            return pop_back();
        else{
            Node* tmp = _get_node(index);
            value result = *tmp->data;
            tmp->next->prev = tmp->prev;
            tmp->prev->next = tmp->next;
            delete tmp;
            --_size;

            return result;
        };
    };
    /// Removes every element returning true when used in predicate.
    /// Returns true if at least one element was removed
    template<typename Predicate>
    bool remove_if(Predicate predicate) {
        auto iter = begin();
        bool flag = false;
        int index = 0;

        while (iter != end()) {
            if(predicate(*iter)) {
                ++iter;
                remove(index);
                flag = true;
            } else {
                ++iter;
                ++index;
            };
        };

        return flag;
    };

    /// returns a sublist from defined range excluding 'to'
    List sublist(const int& from, const int& to) const{
        _check_except_index(from);
        _check_except_index(to);
        _check_except_size(to - from);
        return sublist(at(from), at(to));
    };
    List sublist(const const_iterator& from, const const_iterator& to) const{
        return List(from, to);
    };

    /// concates the range to the list excluding 'to'
    template<typename ForwardIterator>
    void concate(const ForwardIterator& from, const ForwardIterator& to) {
        _concate_to_copy(List(from, to));
    };
    /// concates 'other' list to the end
    void concate(const List* other){
        _concate_to_copy(List(other));
    };
    void concate(const List& other){
        concate(&other);
    };
    void concate(List&& other){
        concate(std::move(other));
    };

    /// returns a new list which is the result of concatenation to range
    template<typename ForwardIterator>
    List get_concated(const ForwardIterator& from, const ForwardIterator& to) const {
        List result = List(this);
        result.concate(from, to);
        return result;
    };
    /// returns a new list which is the result of concatenation
    List get_concated(const List* other){
        return get_concated(other->begin(), other->end());
    };
    List get_concated(const List& other){
        return get_concated(&other);
    };

    /// returns a copy of the list
    List get_copy() const {
        return List(this);
    };

    reference operator[](const int& index) {
        return get(index);
    };
    const_reference operator[](const int& index) const {
        return get(index);
    };

    /// returns iterator at position
    iterator operator()(const int& index) {
        _check_except_index(index);
        return at(index);
    };
    /// returns const_iterator at position
    const_iterator operator()(const int& index) const{
        _check_except_index(index);
        return cat(index);
    };

    /// returns a new list copy with 'other' concated to it
    List operator+(const List& other) const{
        List result = List(this);
        result.concate(other.begin(), other.end());

        return result;
    };
    /// concates 'other' to the list
    void operator+=(const List& other) {
        concate(other.begin(), other.end());
    };

    /// returns a new list copy with data added to its end
    List operator+(const_reference data) const {
        List result = List(*this);
        result.push_back(&data);

        return result;
    };
    /// adds element to the end
    void operator+=(const_reference data){
        push_back(&data);
    };

    /// returns this list concated ti init_list
    List operator+(const std::initializer_list<T>& list) const {
        return List(begin(), end(), list.begin(), list.end());
    };
    /// returns this list concated ti init_list
    void operator+=(const std::initializer_list<T>& list) {
        concate(list.begin(), list.end());
    };

    /// adds an empty constructed element to the end of list
    void operator++(int count) {
        push_back(T(), count);
    };
    void operator++() {
        push_back(T());
    };
    /// removes last element
    value operator--(int count) {
        return pop_back(count);
    };
    value operator--() {
        return pop_back();
    };

    bool operator==(const List& list) const{
        if(_size == 0 && list._size == 0)
            return true;
        if(_size != list._size)
            return false;
        Node* tmp1 = _head;
        Node* tmp2 = list._head;
        for(int i = 0; i < _size; ++i){
            if(*(tmp1->data) != *(tmp2->data))
                return false;
            tmp1 = tmp1->next;
            tmp2 = tmp2->next;
        };
        return true;
    };
    bool operator!=(const List& list) const{
        return !(*this == list);
    };

    const List& operator=(const List& other){
        if(this != &other){
            clear();
            auto iter = other.begin(),
                 last = other.end();

            while (iter != last) {
                push_back(*iter);
                ++iter;
            }
        };

        return *this;
    };
    const List& operator=(List&& other){
        using namespace std;

        clear();
        swap(_size, other._size);
        swap(_head, other._head);
        swap(_tail, other._tail);

        return *this;
    };
    const List& operator=(const std::initializer_list<T>& list) {
        clear();

        auto iter = list.begin(),
             last = list.end;
        while (iter != last) {
            push_back(*iter);
            ++iter;
        };

        return *this;
    };

    /*std::string to_string(const int& members_in_line = -1, bool show_indexes = false, bool add_ws = false) const{
        if(members_in_line == -1)
            return to_string(_size, show_indexes, add_ws);
        std::string result = " [ ";
        if(_size == 0)
            return result + ']';
        else {
            for(int i = 0; i < _size - 1; ++i){
                std::string tmp;
                if(show_indexes)
                    tmp += std::to_string(i) + ':';
                tmp += std::to_string(get(i)) + ", ";
                result += tmp;
                if(members_in_line != 1){
                    if(i != 0 && i % members_in_line == members_in_line - 1)
                        result += "\n   ";
                } else
                    result += "\n   ";
            };
            if(show_indexes)
                result += std::to_string(_size - 1) + ':';
            result += std::to_string(back()) + " ]";
            if(add_ws)
                result += '\n';
        };
        return result;
    };
    explicit operator std::string() const{
        return to_string(1, true);
    };*/

    /// list supports using output streams with it like std::cout<<
    friend std::ostream& operator<<(std::ostream& out, const List& list){
        out << " [ ";
        if(list.size() > 0) {
            auto iter = list.begin();
            for(int i = 0; i < list.size() - 1; ++i, ++iter)
                out << *iter << ", ";
            out << *iter << ' ';
        };
        out << "] ";
        return out;
    };

    /// list can be added to a value resulting in a new list
    friend List operator+(const_reference value, const List& list){
        List result = List(list);
        result.push_front(value);
        return result;
    };

    /*
    abandoned due to complications in use
    friend std::istream& operator>>(std::istream& in, List& list){
        list = List();
        int size = 0;
        in >> size;
        for(int i = 0; i < size; i++){
            T tmp = T();
            in >> tmp;
            list.push_back(tmp);
        };
        return in;
    };*/

    /// returns index of data in list or -1 if it is not present
    int find_index(const_reference data){
        auto iter = begin(),
             last = end();
        int index = 0;

        while(iter != last){
            ++index;
            if(data == *iter)
                return index;
        };
        return -1;
    };
    /// returns true if data is present in list
    bool find(const_reference data){
        if(find_index(data) >= 0)
            return true;
        return false;
    };

    iterator begin(){
        return iterator(_head);
    };
    const_iterator begin() const{
        return cbegin();
    };
    const_iterator cbegin() const{
        return const_iterator(_head);
    };

    iterator at(const int& index){
        return iterator(_get_node(index));
    };
    const_iterator at(const int& index) const{
        return cat(index);
    };
    const_iterator cat(const int& index) const{
        return const_iterator(_get_node(index));
    };

    iterator end(){
        return iterator(_tail->next);
    };
    const_iterator end() const{
        return cend();
    };
    const_iterator cend() const{
        return const_iterator(_tail->next);
    };

    /// sort using std::sort
    void sort() {
        std::sort(begin(), end());
    };

    ~List(){
        clear();
    };
};

#endif // LINKEDLIST_H
