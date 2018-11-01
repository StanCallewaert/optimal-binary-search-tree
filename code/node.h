#ifndef NODE_H
#define NODE_H

#include "obst.h"

#include <memory>

using std::move;

template <class Key>
class Obst;

template <class Key>
class Node {
    friend class Obst<Key>;
    public:
        Node(const Key& key):key{key},parent(0){};
        
        Key key;
        Node<Key>* parent;
        Obst<Key> left, right;
};

#endif