#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H
#include "array_iterator.hpp"

/// Dynamic Array class with two buffers
template<typename T>
class Array {
public:
    using InitializerFunction = T (*)(const int& index);
    using iterator = ArrayIterator<T>;
    using const_iterator = ConstArrayIterator<T>;

private:
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using value = T;

    /// current amount of allocated memory
    int allocated_ = 0;
    /// current size of left buffer (is part of allocated)
    int left_buffer_ = 0;
    /// current size of right buffer (is part of allocated)
    int right_buffer_ = 0;
    /// preset amount of buffer by which the array is_extended all of the time
    int buffer_size_ = 10;
    /// data storage location
    pointer data_ = 0;

    inline pointer _alloc(const int& size) const {
        return pointer(calloc(size, el_size()));
    };

    /// Reallocates all data to new memory of new_alloc size.
    /// If it is less than previous memory than excess elements will be cleared going backwards
    void _realloc_right(const int& new_alloc = 1){
        _check_except_size(new_alloc);
        pointer tmp = _alloc(new_alloc);

        if(new_alloc >= allocated_) {
            // copy contents and increase buffer size aka extend array
            int memory_diff = new_alloc  - allocated_;
            memcpy(tmp + left_buffer_, data_ + left_buffer_, el_size() * size());
            right_buffer_ += memory_diff;
        } else {
            // copy any contents and decrease buffer size aka cut left side of array
            int memory_diff = allocated_ - new_alloc;
            //_check_except_size(memory_diff);

            if(memory_diff <= right_buffer_) {
                // only right buffer decreases
                memcpy(tmp + left_buffer_, data_ + left_buffer_, el_size() * size());
                right_buffer_ -= memory_diff;
            } else if(new_alloc >= left_buffer_) {
                // right buffer is gone and some elements are also deleted
                memcpy(tmp + left_buffer_, data_ + left_buffer_, el_size() * (new_alloc - left_buffer_));
                right_buffer_ = 0;
            } else { //(new_alloc < left_buffer_)
                    // left buffer also decreases
                    right_buffer_ = 0;
                    left_buffer_ = new_alloc;
            };
        };

        free(data_);
        data_ = tmp;
        allocated_ = new_alloc;
    };

    /// Reallocates all data to new memory of new_alloc size.
    /// If it is less than previous memory than excess elements will be cleared
    void _realloc_left(const int& new_alloc = 1) {
        _check_except_size(new_alloc);
        pointer tmp = _alloc(new_alloc);

        if(new_alloc >= allocated_) {
            // copy contents and increase buffer size aka extend array
            int memory_diff = new_alloc  - allocated_;
            memcpy(tmp + left_buffer_ + memory_diff, data_ + left_buffer_, el_size() * size());
            left_buffer_ += memory_diff;
        } else {
            // copy any contents and decrease buffer size aka cut right side of array
            int memory_diff = allocated_ - new_alloc;
            //_check_except_size(memory_diff);

            if(memory_diff <= left_buffer_) {
                // only left buffer decreases
                memcpy(tmp + memory_diff, data_ + left_buffer_, el_size() * size());
                right_buffer_ -= memory_diff;
            } else if(new_alloc >= right_buffer_) {
                // left buffer is gone and some elements are also deleted
                memcpy(tmp, data_ + memory_diff, el_size() * (new_alloc - right_buffer_));
                left_buffer_ = 0;
            } else { //(new_alloc < left_buffer_)
                    // right buffer also decreases
                    left_buffer_ = 0;
                    right_buffer_ = new_alloc;
            };
        };

        free(data_);
        data_ = tmp;
        allocated_ = new_alloc;
    };

    /// Reallocates memory to new location.
    /// Copies elements from old memory starting from index leaving left_buffer__ elements empty at left.
    /// Requires you to set buffers sizes yourself
    void _realloc_both(const int& new_alloc, const int& left_buffer__ = 10, const int& index = 0) {
        T* tmp = _alloc(new_alloc);
        memcpy(tmp + left_buffer__, data_ + left_buffer_ + index, el_size() * (size() - index));

        free(data_);
        data_ = tmp;
        allocated_ = new_alloc;
        left_buffer_ = left_buffer__;
    };

    /// unsafe (does not throw exceptions) set (used internally)
    inline void __set(const int& index, const_reference data) {
        *(data_ + index) = data;
    };

    /// automatic check for deallocation of excess memory
    void _dealloc_check(const int& factor = 2) {
        if(left_buffer_ > factor * buffer_size_) {
            if(right_buffer_ > factor * buffer_size_)
                /// deallocs both sides
                __alloc_checks_helper();
            else
                /// dealloc only left
                _realloc_left(allocated_ - left_buffer_ + buffer_size_);
        } else if(right_buffer_ > factor * buffer_size_) {
            if(left_buffer_> factor * buffer_size_)
                /// deallocs both sides
                __alloc_checks_helper();
            else
                /// dealloc only right
                _realloc_right(allocated_ - right_buffer_ + buffer_size_);
        };
    };
    /// automatic check for reallocation for more memory
    void _realloc_check() {
        if(left_buffer_ == 0) {
            if(right_buffer_ < buffer_size_ / 2)
                /// also extends right
                __alloc_checks_helper();
            else
                /// extends only left
                _realloc_left(allocated_ - left_buffer_ + buffer_size_);
        } else if(right_buffer_ == 0) {
            if(left_buffer_ < buffer_size_ / 2)
                /// also extends left
                __alloc_checks_helper();
            else
                /// extends only right
                _realloc_right(allocated_ - right_buffer_ + buffer_size_);
        };
    };
    /// regulates reallocating both directions
    void __alloc_checks_helper() {
        int tmp = size() + buffer_size_ * 2;
        _realloc_both(tmp, buffer_size_);
        right_buffer_ = buffer_size_;
    };

    /// also can throw exception according to index
    pointer _get_ptr(const int& index) const{
        _check_except_index(index);
        return data_ + left_buffer_ + index;
    };

    /// checks whether size is <= 0
    inline void _check_except_size(const int& size_) const {
        if(size_ <= 0)
            throw std::runtime_error("Array exception: negative or zero size");
    };
    /// checks whether index is in range
    inline void _check_except_index(const int& index) const {
        if(index < 0 || index >= size())
            throw std::runtime_error("Array exception: index out of range");
    };
    /// checks whether index is in range
    inline void _check_except_index_insert(const int& index) const {
        if(index < 0 || index > size())
            throw std::runtime_error("Array exception: index out of range");
    };
    /// checks whether pointer is not null
    inline void _check_except_null_pointer(pointer ptr, const int& right_border = 0) const {
        if(ptr == 0)
            throw std::runtime_error("Array exception: null pointer");
        if(right_border != 0)
            _check_except_null_pointer(ptr + right_border);
    };
    inline void _check_except_null_pointer(const_pointer ptr, const int& right_border = 0) const {
        if(ptr == 0)
            throw std::runtime_error("Array exception: null pointer");
        if(right_border != 0)
            _check_except_null_pointer(ptr + right_border);
    };
    inline void _check_except_null_pointer(const Array* ptr) {
        if(ptr == 0)
            throw std::runtime_error("Array exception: null pointer");
    };

    /// Creates an array with allocate_ allocated memory
    Array(const int& allocate_)
        : allocated_(allocate_ + 20), left_buffer_(10), right_buffer_(10),
          buffer_size_(10), data_(0) {
        _check_except_size(allocate_ + 1);
        data_ = _alloc(allocated_);
    };

public:
    /// Creates an ampty array with buffer_size_ set to 10
    Array() : Array(0) {};
    /// Creates an array with size_ elements equal to default_member
    Array(const int& size, const_reference default_member) : Array(size) {
        auto iter = begin(),
             last = end();

        while(iter != last) {
            *iter = default_member;
            ++iter;
        };
    };
    /// Creates an array with size_ elements equal to default_member
    Array(const int& size, const_pointer default_member) : Array(size, *default_member) {};

    /// Creates an array with copy of data
    Array(const_pointer data, const int& count) : Array(count) {
        _check_except_null_pointer(data, count - 1);
        memcpy(data_, data, el_size() * count);
    };
    /// Pointer copy constructor
    Array(const Array* other) : Array(other->data_, other->size()) {};
    /// Reference copy constructor
    Array(const Array& other) : Array(&other) {};
    /// Move constructor
    Array(Array&& other) {
        std::swap(allocated_, other.allocated_);
        std::swap(left_buffer_, other.left_buffer_);
        std::swap(right_buffer_, other.right_buffer_);
        std::swap(buffer_size_, other.buffer_size_);
        std::swap(data_, other.data_);
    };

    /// Copies elements of init_list into array
    Array(const std::initializer_list<T>& list) : Array(list.begin(), list.end()) {};
    /*/// Creates array of size elements each of them initialized by func (like lambda)
    template<typename Function = int (&)(), typename ...Arguments,
             typename = typename std::enable_if<!std::is_same<Function, pointer>::value &&
                                                !std::is_same<Function, const_pointer>::value &&
                                                !std::is_same<Function, Array*>::value > >
    Array(Function function, const int& size, Arguments... function_args) : Array(size) {
        auto iter = begin(),
             last = end();
        int index = 0;

        while(iter != last) {
            *iter = function(function_args...);
            ++index;
            ++iter;
        };
    };*/

    /// Range constructor.
    /// ForwardIterator must allow prefix increment and dereferencing.
    /// Count is used if evaluating distance between iterators is slow else -1 is defaulted
    template<typename ForwardIterator>
    Array(const ForwardIterator& from, const ForwardIterator& to, const int& count = -1) : Array() {
        int count_ = 0;
        if(count == -1) {
            count_ = to - from;
        } else {
            count_ = count;
            _check_except_size(count_);
        };

        allocated_ = count_ + 2 * buffer_size_;
        data_ = _alloc(allocated_);

        auto iter = from,
             last = to;
        auto input_iter = begin();

        while(iter != last){
            *input_iter = *iter;
            ++iter;
            ++input_iter;
        };
    };

    /// Double Range constructor
    /// ForwardIterator1 and ForwardIterator2 must allow prefix increment and dereferencing.
    /// Count is used if evaluating distance between iterators is slow else -1 is defaulted
    template<typename ForwardIterator1, typename ForwardIterator2 = ForwardIterator1>
    Array(const ForwardIterator1& from1, const ForwardIterator1& to1,
          const ForwardIterator2& from2, const ForwardIterator2& to2, const int& count = -1)
        : Array() {
        int count_ = 0;
        if(count == -1) {
            count_ = to1 - from1;
            count_ += to2 - from2;
        } else {
            count_ = count;
            _check_except_size(count_);
        };

        allocated_ = count_ + 2 * buffer_size_;
        data_ = _alloc(allocated_);

        auto iter1 = from1,
             last1 = to1;
        auto iter2 = from2,
             last2 = to2;
        auto input = begin();

        while(iter1 != last1){
            *input = *iter1;
            ++iter1;
            ++input;
        };

        while(iter2 != last2){
            *input = *iter2;
            ++iter2;
            ++input;
        };
    };

    /// returns current amount of elements
    int size() const {
        return allocated_ - left_buffer_ - right_buffer_;
    };
    /// size of element in bytes
    inline int el_size() const noexcept {
        return sizeof(T);
    };

    /// returns amount of elements that can be pushed front without reallocation
    const int& left_buffer() const {
        return left_buffer_;
    };
    /// returns amount of elements that can be pushed back without reallocation
    const int& right_buffer() const {
        return right_buffer_;
    };
    /// returns amount of elements array is increased by each extension of memory
    const int& buffer_size() const{
        return buffer_size_;
    };
    /// returns amount of memory array occupies at the moment
    const int& current_memory() const {
        return allocated_;
    };

    /// clears all memory
    void clear() {
        _realloc_both(20);
        allocated_;
        left_buffer_ = right_buffer_ = 10;
    };
    bool empty() const {
        if(size() > 0)
            return false;
        return true;
    };
    /// Adds empty constructed elements to the back of array
    void resize(const int& size_){
        _check_except_size(size_);

        if(size_ >= allocated_)
            _realloc_right(size_ + buffer_size_);

        if(size_ >= size()) {
            T element = T();
            push_back(element, size_);
        } else
            right_buffer_ += size_;
    };
    /// sets buffer size to a new value so that next reallocation will extend the buffer limit
    void set_buffer_size(const int& buffer_size__) {
        _check_except_size(buffer_size__);
        buffer_size_ = buffer_size__;
        _dealloc_check(1);
    };

    /// prints array in [ ] brackets
    void print() const{
        std::cout << *this;
    };

    const_reference front() const{
        return *_get_ptr(0);
    };
    reference front(){
        return *_get_ptr(0);
    };
    const_reference get(const int& index) const{
        return *_get_ptr(index);
    };
    reference get(const int& index){
        return *_get_ptr(index);
    };
    const_reference back() const{
        return *_get_ptr(size() - 1);
    };
    reference back(){
        return *_get_ptr(size() - 1);
    };

    /// copies count data elements starting from from_index at at_index one by one
    /// (by default at_index = -1 means in the end of array)
    void copy_from(const_pointer data, const int& count = 1, int at_index = -1, const int& from_index = 0) {
        if(at_index == -1) {
            push_back(data, count, from_index);
            return;
        };

        _check_except_index_insert(at_index);
        _check_except_size(count);
        _check_except_size(from_index + 1);
        _check_except_null_pointer(data + from_index, count - 1);

        allocated_ += - right_buffer_ + buffer_size_ + count;
        pointer tmp = _alloc(allocated_);

        memcpy(tmp + left_buffer_, data_ + left_buffer_, el_size() * at_index);
        memcpy(tmp + left_buffer_ + at_index, data + from_index, el_size() * count);
        memcpy(tmp + left_buffer_ + at_index + count, data_ + left_buffer_ + at_index, el_size() * (size() - at_index));

        free(data_);
        data_ = tmp;
        right_buffer_ = buffer_size_;
    };

    /// copies count data elements starting from from_index at index position in array
    void set(const int& index, const_pointer data, const int& count = 1, const int& from_index = 0){
        _check_except_size(from_index + 1);
        _check_except_index(index);
        _check_except_index(index + count);
        _check_except_size(count);
        _check_except_null_pointer(data + from_index, count - 1);

        memcpy(data_ + left_buffer_ + index, data + from_index, el_size() * count);
    };
    void set(const int& index, const_reference data){
        set(index, &data);
    };
    /// copies init_list elements in list starting from at_index
    void set(const int& index, const std::initializer_list<T>& list) {
        set(index, list.begin(), list.end());
    };
    /// copies range of elements into array starting from index position in array,
    /// count may be used to evade evaluating distance between iterators
    template<typename ForwardIterator>
    void set(const int& index, const ForwardIterator& from, const ForwardIterator& to, const int& count = -1) {
        _check_except_index(index);
        int count_ = 0;
        if(count == -1)
            count_ = to - from;
        else {
            count_ = count;
            _check_except_size(count_);
        };
        _check_except_index(index + count_);

        auto iter = from,
             last = to;

        count_ += index + left_buffer_;
        for(int i = index + left_buffer_; i < count_; ++i, ++iter)
            __set(i, *iter);
    };

    /// removes last element returning its copy
    value pop_back() {
        _check_except_index(0);

        T result = back();
        right_buffer_ += 1;

        return result;
    };
    /// removes count last elements
    void pop_back(const int& count) {
        _check_except_size(size() - count);

        right_buffer_ = count;
        _dealloc_check();
    };

    /// removes first element returning its copy
    value pop_front() {
        _check_except_index(size() - 1);

        T result = front();
        left_buffer_+= 1;

        return result;
    };
    /// removes first last elements
    void pop_front(const int& count) {
        _check_except_size(size() - count);

        left_buffer_ += count;
        _dealloc_check();
    };

    /// adds count data elements starting from from_index to the end of array
    void push_back(const_pointer data, const int& count = 1, const int& from_index = 0) {
        _check_except_size(from_index + 1);
        _check_except_size(count);
        _check_except_null_pointer(data + from_index, count - 1);

        if(count > right_buffer_) {
            _realloc_right(allocated_ - right_buffer_ + count + buffer_size_);
            memcpy(data_ + allocated_ - right_buffer_, data + from_index, el_size() * count);
            right_buffer_ = buffer_size_;
        } else {
            memcpy(data_ + allocated_ - right_buffer_, data + from_index, el_size() * count);
            --right_buffer_;
        }
    };
    void push_back(const_reference data){
        push_back(&data);
    };
    /// Adds init_list contents to the end of array
    void push_back(const std::initializer_list<T>& list) {
        push_back(list.begin(), list.end());
    };
    /// add range to the end of array, count is used optinally if distance is known
    template<typename ForwardIterator>
    void push_back(ForwardIterator from, ForwardIterator to, const int& count = -1) {
        int count_ = 0;
        if(count == -1)
            count_ = to - from;
        else {
            count_ = count;
            _check_except_size(count_);
        };

        if(count_ > right_buffer_) {
            _realloc_right(allocated_ - right_buffer_ + count_ + buffer_size_);
            right_buffer_ = buffer_size_;
        } else
            right_buffer_ -= count_;

        auto iter = from,
             last = to;
        auto input_iter = at(size() - count_);

        while (iter != last) {
            *input_iter = *iter;
            ++iter;
            ++input_iter;
        };
    };

    /// adds count data elements starting from from_index to the beginning of array
    void push_front(const_pointer data, const int& count = 1, const int& from_index = 0){
        _check_except_size(from_index + 1);
        _check_except_size(count);
        _check_except_null_pointer(data + from_index, count - 1);

        if(count > left_buffer_) {
            _realloc_left(buffer_size_ + count + allocated_ - left_buffer_);
            memcpy(data_ + buffer_size_, data + from_index, el_size() * count);
            left_buffer_ = buffer_size_;
        } else {
            memcpy(data_ + left_buffer_ - 1, data + from_index, el_size() * count);
            --left_buffer_;
        };
    };
    void push_front(const_reference data){
        push_front(&data);
    };
    /// Adds init_list contents to array
    void push_front(const std::initializer_list<T>& list) {
        push_front(list.begin(), list.end());
    };
    /// add range to the end of array, count is used optinally if distance is known
    template<typename ForwardIterator>
    void push_front(ForwardIterator from, ForwardIterator to, const int& count = -1) {
        int count_ = 0;
        if(count == -1)
            count_ = to - from;
        else {
            count_ = count;
            _check_except_size(count_);
        };

        if(count_ > left_buffer_) {
            _realloc_left(buffer_size_ + count_ + allocated_ - left_buffer_);
            left_buffer_ = buffer_size_;
        } else
            left_buffer_ -= count_;

        auto iter = from,
             last = to;
        auto input_iter = begin();

        while (iter != to) {
            *input_iter = *iter;
            ++iter;
            ++input_iter;
        };
    };

    /// return index of data if it is found in array or -1
    int find_index(const_pointer data) const{
        _check_except_null_pointer(data);

        auto iter = begin(),
             last = end();
        int index = 0;

        while(iter != last){
            if(*iter == *data)
                return index;

            ++index;
            ++iter;
        };

        return -1;
    };
    int find_index(const_reference data) const{
        return find_index(&data);
    };

    /// true if found data in array
    bool find(const_pointer data) const{
        _check_except_null_pointer(data);

        if(find_index(data) == -1)
            return false;
        return true;
    };
    bool find(const_reference data) const{
        return find(&data);
    };

    /// Returns iterator at position of data in array or end iterator
    iterator find_iterator(const_pointer data) {
        _check_except_null_pointer(data);

        iterator
                iter = begin(),
                last = end();

        while(iter != last){
            if(*iter == *data)
                return iter;

            ++iter;
        };

        return last;
    };
    iterator find_iterator(const_reference data) {
        return find_iterator(&data);
    };

    /// Returns read-only iterator at position of data in array or end iterator
    const_iterator find_iterator(const_pointer data) const {
        _check_except_null_pointer(data);

        auto iter = cbegin(),
             last = cend();

        while(iter != last){
            if(*iter == *data)
                return iter;

            ++iter;
        };

        return last;
    };
    const_iterator find_iterator(const_reference data) const {
        return find_iterator(&data);
    };

    /// Removes data stored at position.
    /// Returns true if removed.
    /// Position iterator data will become unavailable after removal
    bool remove_iterator(const iterator& position) {
        auto iter = begin(),
             last = end();
        int index = 0;

        while (iter != position || iter == last) {
            ++index;
            ++iter;
        };

        if(iter == last)
            // element not found
            return false;

        remove_index(index);
        return true;
    };
    /// Removes data at index
    value remove_index(const int& index) {
        _check_except_index(index);

        if(index == size() - 1) {
            return pop_back();
        } else if(index == 0) {
            return pop_front();
        } else {
            value result = *_get_ptr(index);

            pointer tmp = _alloc(allocated_ - 1);
            memcpy(tmp + left_buffer_, data_ + left_buffer_, el_size() * index);
            memcpy(tmp + left_buffer_, data_ + left_buffer_ + index, el_size() * (size() - index - 1));
            allocated_ -= 1;

            free(data_);
            data_ = tmp;
            return result;
        };
    };

    /// Returns true if data was found and removed.
    /// Deletes only one copy of data
    bool remove(const_pointer data){
        _check_except_null_pointer(data);

        int index = find(data);
        if(index == -1)
            return false;

        remove_index(index);
        return true;
    };
    bool remove(const_reference data){
        return remove(&data);
    };

    /// Deletes all copies of data in array
    void remove_all(const_pointer data) {
        _check_except_null_pointer(data);
        bool flag = true;

        while (flag)
            flag = remove(data);
    };
    bool remove_all(const_reference data) {
        return remove_all(&data);
    };

    /// Inserts data at index
    void insert(const_pointer data, const int& index, const int& count = 1){
        _check_except_index_insert(index);
        _check_except_size(count);
        _check_except_null_pointer(data, count - 1);

        if(index == 0)
            push_front(data, count);
        else if(index == size())
            push_back(data, count);
        else {
            pointer tmp = _alloc(allocated_ + count);

            memcpy(tmp + left_buffer_, data_ + left_buffer_, index * el_size());
            memcpy(tmp + left_buffer_ + index, data, el_size() * count);
            memcpy(tmp + left_buffer_ + index + count, data_ + left_buffer_ + index, el_size() * (size() - index));

            free(data_);
            data_ = tmp;
            allocated_ += count;
            //right_buffer_ -= count;
        };
    };
    void insert(const_reference data, const int& index){
        insert(&data, index);
    };
    /// Inserts range at index, specufying count allows to skip evaluating distance between iterators
    template<typename ForwardIterator>
    void insert(ForwardIterator from, ForwardIterator to, const int& at, const int& count = -1) {
        int count_ = 0;
        if(count == -1)
            count_ = to - from;
        else {
            count_ = count;
            _check_except_size(count_);
        };
        if(at == size()) {
            push_back(from, to, count);
            return;
        };
        _check_except_index_insert(at);

        pointer tmp = _alloc(allocated_ + count_);
        memcpy(tmp + left_buffer_, data_ + left_buffer_, el_size() * at);

        auto iter = from,
             last = to;
        int index = at + left_buffer_;
        for(; iter != last; ++index, ++iter)
            *(tmp + index) = *iter;

        memcpy(tmp + index, data_ + index - count_, el_size() * (size() - at));
        free(data_);
        data_ = tmp;

        allocated_ += count_;
    };
    /// Inserts init_list contents into array at index
    void insert(const std::initializer_list<T>& list, const int& at) {
        insert(list.begin(), list.end(), at);
    };
    /*/// Inserts contentds of array at index
    void insert(const Array& other, const int& at) {
        insert(other.data_, at, other.size_);
    };*/

    /// Concates array to the end of this
    void concate(const Array& other){
        concate(&other);
    };
    void concate(const Array* other) {
        copy_from(other->data_, other->size());
    };
    /// Concates init_list to the end of array
    void concate(const std::initializer_list<T>& list) {
        concate(list.begin(), list.end());
    };
    /// Concates range to the end of array, count allows to skip evaluating distance between iterators
    template<typename ForwardIterator>
    void concate(const ForwardIterator& from, const ForwardIterator& to, const int& count = -1) {
        insert(from, to, size(), count);
    };

    /// Returns result of concatenation of other to array
    Array get_concated(const Array* other) {
        _check_except_null_pointer(other);

        Array result = Array(this);
        result.concate(other);

        return result;
    };
    Array get_concated(const Array& other){
        return get_concated(&other);
    };
    /// Returns result of concatenation of init_list to array
    Array get_concated(const std::initializer_list<T>& list) {
        Array result = Array(this);
        result.concate(list);

        return result;
    };
    /// Returns result of concatenation of range to array, count allows to skip evaluating distance between iterators
    template<typename ForwardIterator>
    Array get_concated(const ForwardIterator& from, const ForwardIterator& to, const int& count = -1) {
        Array result = Array(this);
        result.concate(from, to, count);

        return result;
    };

    /// Returns subarray from from_index to to_index including to_index
    Array subarray(const int& from_index, const int& to_index){
        _check_except_size(to_index - from_index);
        _check_except_index(from_index);
        _check_except_index(to_index);

        return Array(data_ + from_index, to_index - from_index);
    };
    /// Returns subarray from from_iterator to to_iterator excluding to_iterator
    /// Works even if range is not from this array.
    /// Count allows to skip evaluating distance between iterators
    Array subarray(const const_iterator& from_iterator, const const_iterator& to_iterator, const int& count = -1) {
        return Array(from_iterator, to_iterator, count);
    };

    /// Returns result of concatenating other to this
    Array operator+(const Array& other) const {
        return Array(begin(), end(), other.begin(),
                     other.end(), size() + other.size());
    };
    /// Returns a copy of array with data added to it's back
    Array operator+(const_reference data) const {
        Array result = Array(size() + 1);
        result.set(result.size() - 1, data);

        return result;
    };
    /*/// Returns result if concatenating init_list to array
    Array operator+(const std::initializer_list<T>& list) const {
        return Array(begin(), end(), list.begin(), list.end(),
                     size() + list.size());
    };*/

    /// Concates other to array
    void operator+=(const Array& other) {
        concate(other);
    };
    /// Concates data to array
    void operator+=(const_reference data){
        push_back(data);
    };
    /// Concates init_list to array
    void operator+=(const std::initializer_list<T>& list) {
        concate(list);
    };

    /// Adds val to array's beginning
    friend Array operator+(const_reference val, const Array& array){
        Array result = Array(1 + array.allocated_);

        result.set(0, val);
        result.set(1, array.begin(), array.end(), array.size());

        return result;
    };
    /*/// Adds init_list to array's beginning
    friend Array operator+(const std::initializer_list<T>& list, const Array& array){
        return Array(list.begin(), list.end(), array.begin(), array.end(), list.size() + array.size());
    };*/

    bool operator==(const Array& other) const{
        if(this == &other)
            return true;
        if(size() == 0 && other.size() == 0)
            return true;
        if(size() != other.size())
            return false;

        auto iter1 = begin(),
             last = end(),
             iter2 = other.begin();
        while (iter1 != last) {
            if(*iter1 != *iter2)
                return false;
            ++iter1;
            ++iter2;
        };

        return true;
    };
    bool operator!=(const Array& other) const{
        return !(*this == other);
    };

    const Array& operator=(const Array& other){
        if(this != &other){
            clear();

            buffer_size_ = other.buffer_size_;
            allocated_ = other.allocated_;
            left_buffer_ = other.left_buffer_;
            right_buffer_ = other.right_buffer_;

            data_ = _alloc(allocated_);
            memcpy(data_ + left_buffer_, other.data_ + left_buffer_, el_size() * size());
        };
        return *this;
    };
    const Array& operator=(Array&& other){
        using namespace std;
        clear();

        swap(buffer_size_, other.buffer_size_);
        swap(allocated_, other.allocated_);
        swap(data_, other.data_);
        swap(left_buffer_, other.left_buffer_);
        swap(right_buffer_, other.right_buffer_);

        return *this;
    };
    const Array& operator=(const std::initializer_list<T>& list){
        clear();

        *this = Array(list);

        return *this;
    };

    /// Returns iterator at position
    iterator operator()(const int& index) {
        return at(index);
    };
    /// Returns read-only iterator at position
    const_iterator operator()(const int& index) const {
        return cat(index);
    };

    /// Calls function for each element of array
    template<typename Function = void (&)(const T& element), typename ...Arguments,
             typename = typename std::enable_if<!std::is_integral<Function>::value> >
    void operator()(Function function, Arguments... function_args) const {
        for(auto element : *this)
            function(element, function_args...);
    };
    /// Calls function for each element of array
    /// This function changes assigns new value to each member
    template<typename Function = T (&)(const T& element), typename ...Arguments,
             typename = typename std::enable_if<!std::is_integral<Function>::value> >
    void operator()(Function function, Arguments... function_args) {
        for(auto element : *this)
            element = function(element, function_args...);
    };

    reference operator[](const int& index){
        return get(index);
    };
    const_reference operator[](const int& index) const{
        return get(index);
    };

    /*
    std::string to_string(const int& members_in_line = 1, bool show_indexes = false, bool add_ws = false) const{
        std::string result = " [ ";
        if(size_ == 0)
            return result + ']';
        else {
            for(int i = 0; i < size() - 1; ++i){
                std::string tmp;
                if(show_indexes)
                    tmp += std::to_string(i) + ':';
                tmp += std::to_string(get(i)) + ", ";
                if(i != 0 && i % members_in_line == members_in_line - 1)
                    tmp += "\n   ";
                result += tmp;
            };
            if(show_indexes)
                result += std::to_string(size_ - 1) + ':';
            result += std::to_string(back()) + " ]";
            if(add_ws)
                result += '\n';
        };
        return result;
    };
    explicit operator std::string() const{
        return to_string(1, true);
    };*/
    /// Outstream support example: std::cout
    friend std::ostream& operator<<(std::ostream& out, const Array& array){
        out << " [ ";
        if(array.size() != 0) {
            auto iter = array.begin();
            for(int i = 0; i < array.size() - 1; ++i, ++iter)
                out << *iter << ", ";
            out << array.back() << ' ';
        };
        out << "] ";

        return out;
    };

    iterator begin(){
        return iterator(_get_ptr(0));
    };
    const_iterator begin() const{
        return cbegin();
    };
    const_iterator cbegin() const{
        return const_iterator(_get_ptr(0));
    };

    iterator at(const int& index){
        return iterator(_get_ptr(index));
    };
    const_iterator at(const int& index) const{
        return cat(index);
    };
    const_iterator cat(const int& index) const{
        return const_iterator(_get_ptr(index));
    };

    iterator end(){
        return iterator(data_ + size());
    };
    const_iterator end() const{
        return cend();
    };
    const_iterator cend() const{
        return const_iterator(data_ + size());
    };

    ~Array(){
        free(data_);
    };
};

#endif // DYNAMICARRAY_H
