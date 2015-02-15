//
//  BTree.h
//  BTree
//
//  Created by Wolfgang Kraske on 2/7/15.
//  Copyright (c) 2015 Wolfgang Kraske. All rights reserved.
//

#ifndef __BTree__BTree__
#define __BTree__BTree__

#include <stdio.h>
#include <thread>
#include <memory>
#include <vector>
#include <list>
#include <iostream>
#include <atomic>
#include <cxxabi.h>
#include <tuple>
#include <array>
#include <unordered_map>
#include <random>
#include <iterator>
#include <algorithm>
#include <functional>

#endif /* defined(__BTree__BTree__) */


template<typename T>
struct Node {
    
    Node(const Node<T>& node) :
    value(node.value),
    children(node.children) {
    }
    
    Node(T value) : value(value) {
    }
    
    Node(T value, const std::list<Node<T>>& children) : value(value), children(children) {
    }
    
    virtual ~Node() { this->children.clear(); };
    
    Node<T> operator +(const T& value) {
        
        if(this->value <= value)
            switch(this->children.size()) {
                case 0 : {  this->children.push_front(Node(this->value)); this->value = value; } break;
                case 1 : { this->children.push_back(Node(value)); } break;
                case 2 : { this->children.back() + value; } break;
            } else
                switch(this->children.size()) {
                    case 0 : { this->children.push_front(Node(value)); } break;
                    case 1 :
                    case 2 : { this->children.front()+value; } break;
                }
        
        this->totalLevels = level();
        this->totalWeight = weight();
        this->totalCardinality = cardinality();
        
        balance();
        
        return std::move(*this);
    }
    
    Node<T> operator +(const Node<T>& that) {
        if(this->value < that.value)
            switch(this->children.size()) {
                case 0 : { children.assign(that.children.begin(),that.children.end());
                    T val = this->value; this->value = that.value; *this + val; } break;
                case 1 : { children.push_back(that); } break;
                case 2 : { children.back() + that; } break;
            } else
                switch(children.size()) {
                    case 0 : { children.push_front(that); } break;
                    case 1 : { T val = this->value; children.front() + that; this->value = children.front().value;
                        children = children.front().children; *this + val;  } break;
                    case 2 : { children.front()+that; } break;
                }
        
        this->totalLevels = level();
        this->totalWeight = weight();
        this->totalCardinality = cardinality();
        
        balance();
        
        return std::move(*this);
    }
    
    bool operator <<(const Node<T>& that) {
        return this->value < that.value;
    }
    
    bool operator >>(const Node<T>& that) {
        return this->value > that.value;
    }
    
    bool operator <(const Node<T>& that) {
        return this->value < that.value;
    }
    
    bool operator >(const Node<T>& that) {
        return this->value > that.value;
    }
    
    bool operator <=(const Node<T>& that) {
        return this->value <= that.value;
    }
    
    bool operator >=(const Node<T>& that) {
        return this->value >= that.value;
    }
    
    bool operator ==(const Node<T>& that) {
        return this->value == that.value;
    }
    
    bool operator !=(const Node<T>& that) {
        return this->value != that.value;
    }
    
    int weightDiff() {
        
        return children.size() == 0 ? 0 :
        -.5 * cardinality() * children.front().weight() / children.front().cardinality() +
        (children.size() == 1 ? 0 : .5 * cardinality() * children.back().weight() / children.back().cardinality());
    }
    
    bool imbalance() {
        int thr = 1;
        return 1 < level() && ( (weightDiff() < -thr*2 && children.front().weightDiff() < -thr) ||
                               (thr*2 < weightDiff() && thr < children.back().weightDiff()));
    }
    
    void print() {
        std::cout << "value: " << value << " Weight " << totalWeight << " Weight Diff " << weightDiff() << std::endl;
    }
    
    int totalWeight = 1;
    int totalCardinality = 1;
    int totalLevels = 1;
    
protected:
    
private:
    enum { left = 0, right } child;
    T value;
    std::list<Node<T>> children{};
    
    template<typename U>
    friend std::ostream& operator<<(std::ostream&, const Node<U>&);
    
    void setVals(Node<T>& node) {
        this->value = node.value;
        this->children = node.children;
    }
    
    int min() {
        return left < children.size() ? children.front().min() : value;
    }
    
    int max() {
        return right < children.size() ? children.back().max() : value;
    }
    
    int levelDiff() {
        return children.size() == 0 ? 0 :  -children.front().level() +
        (children.size() == 1 ? 0 :  children.back().level());
    }
    
    int level() {
        int level = 1; for(Node<T> n : children) level = level <= n.level() ? n.level()+1 : level;
        
        return level;
    }
    
    int cardinality() {
        int cardinality = 1; for(Node<T> n : children) cardinality = cardinality <= n.cardinality() ? n.cardinality()*2 : cardinality;
        
        return cardinality;
    }
    
    int cpow(const int& modulus) { return modulus < 1 ? 1 : 2 * cpow(modulus-1); }
    int absimbalance() { return abs(imbalance()); }
    
    int weight() {
        int card = cardinality();
        if(children.size() == 0) return card;
        int sum = card;
        sum += .5 * children.front().weight() * card / children.front().cardinality();
        if(children.size() == 1) return sum;
        sum += card + .5 * children.back().weight() * card / children.back().cardinality();
        return sum;
    }
    
    void balance() {
        if(!imbalance()) return;
        //        if(absimbalance() <= (cardinality())/2) return;
        int ib = weightDiff();
        for(Node<T> n : this->children) n.balance();
        if(abs(ib) < abs(weightDiff()) || !imbalance()) return;
        if(ib < 0) {
            if(children.size() == 1) children.push_back(Node<T>(this->value));
            else children.back() + this->value;
            this->value = children.front().value;
            if(children.front().children.size() == 2) children.front().children.back() + children.back();
            else children.front().children.push_back(children.back());
            Node<T> nn(children.front());
            children.assign(nn.children.begin(), nn.children.end());
        } else if(0 < ib) {
            children.front() + this->value;
            this->value = children.back().value;
            children.back().children.front() + children.front();
            Node<T> nn(children.back());
            children.assign(nn.children.begin(), nn.children.end());
        }
    }
    
};

template<typename T>
bool compare(const Node<T>& a, const Node<T>& b) {
    return a < b;
}

template<typename T>
std::ostream& operator<<(std::ostream& str, const Node<T>& nd) {
    str << " { ";
    str << nd.value;
    for(Node<T> n : nd.children)
        str << n;
    
    return str << " } ";
}
