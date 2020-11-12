#ifndef NODE_HPP
#define NODE_HPP

/// Basic node for data structures
template<typename value>
struct Node {
    /// the pointer may be replaced by value
    value* data;

    Node* prev;
    Node* next;

    Node()
        : data(new value()), prev(0), next(0) {};

    Node(const value* _data, Node* _prev, Node* _next)
        : data(new value(*_data)), prev(_prev), next(_next) {};

    Node(const value& _data, Node* _prev, Node* _next)
        : Node(&_data, _prev, _next) {};

    Node(const Node* other)
        : data(new value(*other->data)), prev(0), next(0) {};

    Node(const Node& other) : Node(&other) {};

    Node(Node&& other) : Node() {
        using namespace std;

        swap(data, other.data);
        swap(next, other.next);
        swap(prev, other.prev);
    };

    const Node& operator=(const Node& other){
        if(this != &other) {
            delete data;
            data = new value(other.data);
            next = other.next;
            prev = other.prev;
        };

        return *this;
    };
    const Node& operator=(Node&& other){
        using namespace std;

        swap(data, other.data);
        swap(next, other.next);
        swap(prev, other.prev);

        return *this;
    };

    ~Node(){
        delete data;
    };
};

#endif // NODE_HPP
