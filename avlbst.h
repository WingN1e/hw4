#ifndef RBBST_H
#define RBBST_H
#define LEFT 0
#define RIGHT 1
#define LINE 0
#define CROSS 1

#include <iostream>
#include <exception>
#include <cstdlib>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    signed char getBalance () const;
    void setBalance (signed char balance);
    void updateBalance(signed char diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    signed char balance_;
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
signed char AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(signed char balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(signed char diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    Node<Key, Value>* grow(Node<Key, Value>* current, Node<Key, Value>* parent, AVLNode<Key, Value>** location, const std::pair<const Key, Value>& keyValuePair);
    AVLNode<Key, Value>* predecessor(AVLNode<Key, Value>* current);
    void insertFix(bool position, AVLNode<Key, Value>* parent, AVLNode<Key, Value>* grandp, bool recursed);
    bool rotateRight(AVLNode<Key, Value>* node, bool pattern); 
    bool rotateLeft(AVLNode<Key, Value>* node, bool pattern);
    void setBalances(AVLNode<Key, Value>* grandp, AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child, signed char b1, signed char b2, signed char b3);
    void removeFix(AVLNode<Key, Value>* parent, signed char diff);
    void del(const Key& key, signed char& diff, AVLNode<Key, Value>* current, AVLNode<Key, Value>** parent);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */

template<class Key, class Value> 
void AVLTree<Key, Value>::setBalances(
    AVLNode<Key, Value>* n1, 
    AVLNode<Key, Value>* n2, 
    AVLNode<Key, Value>* n3, 
    signed char g, signed char p, signed char c) 
{
    if(n1) {
        n1->setBalance(g);
        // n1->setValue((int) n1->getBalance()); 
    }
    if(n2) {
        n2->setBalance(p);
        // n2->setValue((int) n2->getBalance()); 
    }
    if(n3) {
        n3->setBalance(c);
        // n3->setValue((int) n3->getBalance()); 
    }
}

template<class Key, class Value> 
bool AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node, bool pattern) {
    bool is_root = false;
    AVLNode<Key, Value>* left_child = node->getLeft(); 
    AVLNode<Key, Value>* child_right_tree = left_child->getRight();
    AVLNode<Key, Value>* grandp = node->getParent();

    bool direction = (grandp != nullptr && grandp->getRight() == node) ? RIGHT : LEFT;
    left_child->setRight(node);
    node->setParent(left_child);
    if(node == this->root_) {
        this->root_ = left_child;
        this->root_->setParent(nullptr);
        is_root = true;
    } else {
        left_child->setParent(grandp);
        if((pattern == CROSS && direction == RIGHT) || (pattern == LINE && direction == RIGHT)) grandp->setRight(left_child);
        else grandp->setLeft(left_child);
    }
    node->setLeft(child_right_tree);
    if(child_right_tree != nullptr) child_right_tree->setParent(node); 
    return is_root;
}   

template<class Key, class Value> 
bool AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node, bool pattern) {
    bool is_root = false;
    AVLNode<Key, Value>* right_child = node->getRight(); 
    AVLNode<Key, Value>* child_left_tree = right_child->getLeft();
    AVLNode<Key, Value>* grandp = node->getParent();

    bool direction = (grandp != nullptr && grandp->getRight() == node) ? RIGHT : LEFT;
    right_child->setLeft(node);
    node->setParent(right_child);
    if(node == this->root_) {
        this->root_ = right_child; 
        this->root_->setParent(nullptr);
        is_root = true;
    } else {
        right_child->setParent(grandp);
        if((pattern == CROSS && direction == LEFT) || (pattern == LINE && direction == LEFT)) grandp->setLeft(right_child);
        else grandp->setRight(right_child);
    }
    node->setRight(child_left_tree);
    if(child_left_tree != nullptr) child_left_tree->setParent(node);
    return is_root;
}


template<class Key, class Value>
Node<Key, Value>* AVLTree<Key, Value>::grow(Node<Key, Value>* current, Node<Key, Value>* parent, AVLNode<Key, Value>** location, const std::pair<const Key, Value>& keyValuePair)
{
    if(current == nullptr) {
        *location = new AVLNode<Key, Value>(keyValuePair.first, keyValuePair.second, nullptr); 
        (*location)->setParent(parent);
        return *location;
    } 
    else if(current->getKey() < keyValuePair.first) current->setRight(grow(current->getRight(), current, location, keyValuePair));
    else if(current->getKey() > keyValuePair.first) current->setLeft(grow(current->getLeft(), current, location, keyValuePair));
    else current->setValue(keyValuePair.second);
    return current;
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(bool childpos, AVLNode<Key, Value>* parent, AVLNode<Key, Value>* grandp, bool recursed)
{
    if(parent == nullptr || grandp == nullptr) return;
    AVLNode<Key, Value>* child; 
    bool position = (grandp->getLeft() == parent) ? LEFT : RIGHT;
    position == LEFT ? grandp->updateBalance(-1) : grandp->updateBalance(1);
    if(grandp->getBalance() == 0) return; 
    if(abs((int) grandp->getBalance()) > 1) {
        if(position == LEFT && childpos == RIGHT) {
            child = parent->getRight();
            rotateLeft(parent, CROSS);
            rotateRight(grandp, LINE);
            if(child->getBalance() == -1) setBalances(grandp, parent, child, 1, 0, 0);
            else if(child->getBalance() == 1) setBalances(grandp, parent, child, 0, -1, 0);
            else setBalances(grandp, parent, child, 0, 0, 0);
        } else if(position == LEFT && childpos == LEFT) {
            child = parent->getLeft();
            rotateRight(grandp, LINE);
            recursed ? setBalances(grandp, parent, nullptr, 0, 0, 0) : setBalances(grandp, parent, child, 0, 0, 0);
        } else if(position == RIGHT && childpos == LEFT) {
            child = parent->getLeft();
            rotateRight(parent, CROSS);
            rotateLeft(grandp, LINE);
            if(child->getBalance() == 1) setBalances(grandp, parent, child, -1, 0, 0);
            else if(child->getBalance() == -1) setBalances(grandp, parent, child, 0, 1, 0);
            else setBalances(grandp, parent, child, 0, 0, 0);     
        } else {
            child = parent->getRight();
            rotateLeft(grandp, LINE);
            recursed ? setBalances(grandp, parent, nullptr, 0, 0, 0) : setBalances(grandp, parent, child, 0, 0, 0);
        } 
    } else insertFix(position, grandp, grandp->getParent(), true);
}
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if(this->root_ == nullptr) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr); return;
    }
    AVLNode<Key, Value>* inserted_at = nullptr;
    grow(this->root_, nullptr, &inserted_at, new_item); 
    if(inserted_at != nullptr) {
        AVLNode<Key, Value>* parent = inserted_at->getParent();
        if(parent->getBalance() == 0) {
            bool position = parent->getLeft() == inserted_at ? LEFT : RIGHT;
            (position == LEFT) ? parent->updateBalance(-1) : parent->updateBalance(1);
            insertFix(position, parent, parent->getParent(), false);
        } else parent->setBalance(0);
    } 
    
}   

template<class Key, class Value>
AVLNode<Key, Value>*
AVLTree<Key, Value>::predecessor(AVLNode<Key, Value>* current)
{
    // TODO
    if(current->getLeft() != nullptr) {
        current = current->getLeft(); 
        while(current->getRight() != nullptr) {
            current = current->getRight();
        }
    } else {
        while(current->getParent() != nullptr && current != current->getParent()->getRight()) {
            current = current->getParent();
        }
        current = current->getParent();
    }
    return current;
}

template<class Key, class Value>
void AVLTree<Key, Value>:: removeFix(AVLNode<Key, Value>* parent, signed char diff) {
    if(parent == nullptr) return;
    signed char next_diff; 
    AVLNode<Key, Value>* next_parent = parent->getParent(); 
    if(next_parent) next_diff = (next_parent->getRight() == parent) ? -1 : 1;
    if(diff == -1) {
        if((parent->getBalance() + diff) == -2) {
            AVLNode<Key, Value>* left_child = parent->getLeft();
            if(left_child->getBalance() == -1) {
                rotateRight(parent, LINE);
                setBalances(parent, left_child, nullptr, 0, 0, 0);
                removeFix(next_parent, next_diff);
            } else if(left_child->getBalance() == 1) {
                AVLNode<Key, Value>* right_child = left_child->getRight(); 
                if((!right_child->getRight() && !right_child->getLeft()) || (right_child->getLeft() && !left_child->getLeft()) || parent->getRight()->getBalance() == 0) {
                    rotateLeft(left_child, CROSS);
                    rotateRight(parent, LINE);
                } else {
                    rotateLeft(right_child, CROSS);
                    rotateRight(parent, LINE);
                }
                if(right_child->getBalance() == 1) setBalances(parent, left_child, right_child, 0, -1, 0); 
                else if(right_child->getBalance() == -1) setBalances(parent, left_child, right_child, 1, 0, 0);
                else setBalances(parent, left_child, right_child, 0, 0, 0);
                removeFix(next_parent, next_diff);
            } else {
                rotateRight(parent, LINE);
                setBalances(parent, left_child, nullptr, -1, 1, 0);
            }
        } else if(parent->getBalance() + diff == 0) {
            setBalances(parent, nullptr, nullptr, 0, 0, 0);
            removeFix(next_parent, next_diff); 
        } else setBalances(parent, nullptr, nullptr, -1, 0, 0);
    } else if(diff == 1) {
        if((parent->getBalance() + diff) == 2) {
            AVLNode<Key, Value>* right_child = parent->getRight();
            if(right_child->getBalance() == 1) {
                rotateLeft(parent, LINE); 
                setBalances(parent, right_child, nullptr, 0, 0, 0);
                removeFix(next_parent, next_diff);
            } else if(right_child->getBalance() == -1) {
                AVLNode<Key, Value>* left_child = right_child->getLeft(); 
                if((!left_child->getLeft() && !left_child->getRight()) || (!right_child->getLeft() && !right_child->getRight()) || parent->getLeft()->getBalance() == 0){
                    rotateRight(right_child, CROSS);
                    rotateLeft(parent, LINE);
                } else {
                    rotateRight(left_child, CROSS);
                    rotateLeft(parent, LINE);
                }
                if(left_child->getBalance() == -1) setBalances(parent, right_child, left_child, 0, 1, 0);
                else if(left_child->getBalance() == 1) setBalances(parent, right_child, left_child, -1, 0, 0);
                else setBalances(parent, right_child, left_child, 0, 0, 0);
                removeFix(next_parent, next_diff);
            } else {
                rotateLeft(parent, LINE); 
                setBalances(parent, right_child, nullptr, 1, -1, 0); 
            }
        } else if(parent->getBalance() + diff == 0) {
            setBalances(parent, nullptr, nullptr, 0, 0, 0);
            removeFix(next_parent, next_diff);
        } else setBalances(parent, nullptr, nullptr, 1, 0, 0);
    }
}
/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: del(const Key& key, signed char& diff, AVLNode<Key, Value>* current, AVLNode<Key, Value>** parent_loc)
{
    // TODO
    while(current != nullptr) {
        if(key > current->getKey()) current = current->getRight();
        else if(key < current->getKey()) current = current->getLeft();
        else break;
    }
    if(current) {
        if(current->getLeft() == nullptr || current->getRight() == nullptr) {
            AVLNode<Key, Value>* parent = current->getParent();
            AVLNode<Key, Value>* child = current->getLeft() ? current->getLeft() : current->getRight();
            if(current->getParent() && current) {
                bool position = (current->getParent()->getRight() == current) ? RIGHT : LEFT; 
                diff = position == LEFT ? 1 : -1;
            }
            if(child == nullptr) {
                if(current != this->root_) {
                    current == parent->getLeft() ? parent->setLeft(nullptr) : parent->setRight(nullptr);
                } else {
                    this->root_ = nullptr;
                }
            } else {
                if(current != this->root_) {
                    child->setParent(parent);
                    current == parent->getLeft() ? parent->setLeft(child) : parent->setRight(child);
                } else {
                    child->setParent(nullptr);
                    this->root_ = child;
                }
            } 
            *parent_loc = parent;
            delete current;
        } else {
            AVLNode<Key, Value>* pred = predecessor(current);
            nodeSwap(pred, current);
            del(key, diff, current, parent_loc);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    signed char diff = 0;
    AVLNode<Key, Value>* parent = nullptr;
    del(key, diff, static_cast<AVLNode<Key, Value>*>(this->root_), &parent);
    removeFix(parent, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    signed char tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
