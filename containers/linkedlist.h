#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <iostream>
#include <cstddef> // size_t
#include <string>
#include <sstream>
#include <shared_mutex> // shared_mutex
#include "general_iterator.h"
#include "util.h"
#include "../types.h"
#include <mutex>
using namespace std;

// Forward iterator
template <typename Container>
class LinkedListForwardIterator : public general_iterator<Container, LinkedListForwardIterator<Container>>{
    using MySelf = LinkedListForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
    // TODO: Completar el operator++
    public:
    MySelf operator++() { 
        if(this->m_pNode){
            this->m_pNode = this->m_pNode->getNext();
        }
        return *this; 
     }
};

// Linked List Node
template <typename T>
class LLNode{
    using Node = LLNode<T>;
private:
    T   m_data;
    Ref m_ref;
    Node *m_next;
public:
    LLNode() : m_data(T()), m_ref(Ref()), m_next(nullptr) {}
    LLNode(T data) : m_data(data), m_ref(Ref()), m_next(nullptr) {}
    LLNode(T data, Ref ref, Node *next) : m_data(data), m_ref(ref), m_next(next) {}
    virtual ~LLNode() {}

    T      getData() const { return m_data; }
    T&     getDataRef()    { return m_data; }
    void   setData(T data) { m_data = data; }
    Ref    getRef() const { return m_ref; }
    void   setRef(Ref ref) { m_ref = ref; }
    Node*  getNext() const { return m_next; }
    Node*& getNextRef()    { return m_next; }
    void   setNext(Node *next) { m_next = next; }
};

template <typename T>
struct AscendingLinkedListTrait{
    using value_type = T;
    using Node = LLNode<T>;
    using Comp = less<T>;
};

template <typename T>
struct DescendingLinkedListTrait{
    using value_type = T;
    using Node = LLNode<T>;
    using Comp = greater<T>;
};

template <typename Trait>
class LinkedList{
public:
    using value_type = typename Trait::value_type;
    using Node       = typename Trait::Node;
    using Comp       = typename Trait::Comp;
    using MySelf     = LinkedList<Trait>;

    using forward_iterator = LinkedListForwardIterator<MySelf>;
    // friend forward_iterator;

private:
    Node *m_pRoot = nullptr;
    Node *m_tail = nullptr;
    size_t m_size = 0;
    Comp   m_comp;
    mutable shared_mutex m_mtx;
public:
    LinkedList() {}
    LinkedList(const LinkedList &other): m_pRoot(nullptr), m_tail(nullptr), m_size(0), m_comp(other.m_comp){ // TODO: Copy constructor
        Node *current = other.m_pRoot; // nodo actual del linked list original
        Node **insertPosition = &m_pRoot; // posición del nuevo nodo del linked list copia
        while(current){
            *insertPosition = new Node(current->getData(), current->getRef(), nullptr);
            m_tail = *insertPosition;
            insertPosition = &((*insertPosition)->getNextRef());
            current = current->getNext();
            ++m_size;
        }
    }
    LinkedList(LinkedList &&other) noexcept { // TODO: Move constructor
        // transfiriendo los datos del linked list original
        m_pRoot = other.m_pRoot;
        m_tail = other.m_tail;
        m_size = other.m_size;
        m_comp = move(other.m_comp);
        // vaciando los datos del linked list original
        other.m_pRoot = nullptr;
        other.m_tail = nullptr;
        other.m_size = 0;
    }
    LinkedList& operator=(const LinkedList &other){ // TODO: Copy assignment operator
        if(this != &other){
            // Vaciando linked list actual
            Node* current = m_pRoot;
            while(current){
                Node* next = current->getNext();
                delete current;
                current = next;
            }
            m_pRoot = nullptr;
            m_tail = nullptr;
            m_size = 0;

            // Copiando nodos del otro linked list
            m_comp = other.m_comp;
            current = other.m_pRoot;
            Node **insertPosition = &m_pRoot;
            while(current){
                *insertPosition = new Node(current->getData(), current->getRef(), nullptr);
                m_tail = *insertPosition;
                insertPosition = &((*insertPosition)->getNextRef());
                current = current->getNext();
                ++m_size;
            }
        }
        return *this;
    }
    LinkedList& operator=(LinkedList &&other) noexcept{ // TODO: Move assignment operator
        if(this != &other){
            // Vaciando el linked list actual
            Node* current = m_pRoot;
            while(current){
                Node* next = current->getNext();
                delete current;
                current = next;
            }
            // Transfiriendo los datos del linked list original
            m_pRoot = other.m_pRoot;
            m_tail = other.m_tail;
            m_size = other.m_size;
            m_comp = move(other.m_comp);
            // Vaciando el linked list original
            other.m_pRoot = nullptr;
            other.m_tail = nullptr;
            other.m_size = 0;
        }
        return *this;
    }


    
    virtual ~LinkedList() {
        // TODO: Destructor seguro
        Node* current = m_pRoot;
        while(current){
            Node* next = current->getNext();
            delete current;
            current = next;
        }
    }
    
    virtual void    push_front(value_type value, Ref ref);
    virtual void    pop_front();
    virtual void    push_back(value_type value, Ref ref);
    virtual void    pop_back(); 
private:
            void    internal_insert(Node* &pPrev, const value_type &value, Ref ref);
public:
    virtual void    insert(const value_type &value, Ref ref);
    
    virtual value_type& operator[](size_t index);
    virtual size_t  size() const;
    virtual string  toString() const;

    forward_iterator begin() { return forward_iterator(this, m_pRoot); }
    forward_iterator end()   { return forward_iterator(this, nullptr); }

    // TODO: Agregar Foreach
    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&...  args){
        unique_lock<shared_mutex> lock(m_mtx);
        ::ForEach(begin(), end(), func, std::forward<Args>(args)... );
    }
};

template <typename T>
void LinkedList<T>::internal_insert(Node* &pPrev, const value_type &value, Ref ref){
    if(!pPrev || m_comp(value, pPrev->getDataRef())){
        pPrev = new Node(value, ref, pPrev);
        m_size++;
        if(pPrev->getNext() == nullptr)
            m_tail = pPrev;
        return;
    }
    internal_insert(pPrev->getNextRef(), value, ref);
}

template <typename T>
void LinkedList<T>::insert(const value_type &value, Ref ref){
    internal_insert(m_pRoot, value, ref);
}


template <typename T>
typename LinkedList<T>::value_type& LinkedList<T>::operator[](size_t index){
    unique_lock<shared_mutex> lock(m_mtx);
    if(index >= m_size){
        throw runtime_error("indice fuera de rango :(");
    }

    Node* current = m_pRoot;
    for(size_t i = 0; i < index; ++i)
        current = current->getNext();
    return current->getDataRef();
}

template <typename T>
size_t LinkedList<T>::size() const{
    shared_lock<shared_mutex> lock(m_mtx);
    return m_size;
}

template <typename T>
string LinkedList<T>::toString() const{
    ostringstream oss;
    oss << "[";
    Node* current = m_pRoot;    
    bool first = true;
    while(current){
        if(!first)
            oss << ",";
        first = false;
        oss << "(" << current->getData() << ", " << current->getRef() << ")";
        current = current->getNext();
    }
    oss << "]";
    return oss.str();
}

// TODO: push front
template<typename T>
void LinkedList<T>::push_front(value_type value, Ref ref){
    unique_lock<shared_mutex> lock(m_mtx);
    Node* newNode = new Node(value, ref, m_pRoot);
    m_pRoot = newNode;
    if(m_size == 0)
        m_tail = newNode;
    m_size++;
}

// TODO: pop front
template<typename T>
void LinkedList<T>::pop_front(){
    unique_lock<shared_mutex> lock(m_mtx);
    if(m_size == 0){
        cout<<"Linked list vacio :("<<endl;
        return;
    }
    Node* tmp = m_pRoot;
    m_pRoot = m_pRoot->getNext();
    delete tmp;
    m_size--;
    if(m_size == 0)
        m_tail = nullptr;
}

//TODO: push back
template<typename T>
void LinkedList<T>::push_back(value_type value, Ref ref){
    unique_lock<shared_mutex> lock(m_mtx);
    Node* newNode = new Node(value, ref, nullptr);
    if(m_size == 0){
        m_pRoot = newNode;
        m_tail = newNode;
    } else {
        m_tail->setNext(newNode);
        m_tail = newNode;
    }
    m_size++;
}

//TODO: pop back
template<typename T>
void LinkedList<T>::pop_back(){
    unique_lock<shared_mutex> lock(m_mtx);
    if(m_size == 0){
        cout<<"Linked list vacio :("<<endl;
        return;
    }
    if(m_size == 1){
        delete m_pRoot;
        m_pRoot = nullptr;
        m_tail = nullptr;
    } else {
        Node* current = m_pRoot;
        while(current->getNext() != m_tail){
            current = current->getNext();
        }
        delete m_tail;
        current->setNext(nullptr);
        m_tail = current;
    }
    m_size--;
}

//TODO: operator<<
template<typename T>
ostream& operator<<(ostream& os, const LinkedList<T>& list){
    return os << list.toString();
}

//TODO: operator>>
template<typename T>
istream& operator>>(istream& is, LinkedList<T>& list){
    char ch;
    // Se valida que el input comience con "["
    if(!(is >> ch) || ch != '['){
        is.setstate(ios::failbit);
        return is;
    }

    // Se valida que el input termine con "]"
    is >> ws;
    if(is.peek() == ']'){
        is.get();
        return is;
    }

    typename T::value_type data{};
    Ref ref{};
    char open, sep, close;

    while(true){
        // Se valida que cada elemento inicie con "("
        if(!(is >> open) || open != '('){
            is.setstate(ios::failbit);
            return is;
        }

        // Se valida que el elemento tenga el formato "(data, ref)"
        if(!(is >> data >> sep >> ref >> close) || sep != ',' || close != ')'){
            is.setstate(ios::failbit);
            return is;
        }

        // Se agrega el elemento a la lista enlazada
        list.insert(data, ref);

        is >> ws;

        // Se intenta leer el siguiente separador o el final del vector "," o "]"
        if(!(is >> ch))
            return is;
        if(ch == ']')
            break;
        if(ch != ','){
            is.setstate(ios::failbit);
            return is;
        }
    }
    return is;
}

void LinkedListDemo();
void ListsDemo();

#endif // __LINKEDLIST_H__