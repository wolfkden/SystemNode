//
//  main.cpp
//  SNode
//
//  Created by Wolfgang Kraske on 2/14/15.
//  Copyright (c) 2015 Wolfgang Kraske. All rights reserved.
//

#include <iostream>
#include "BTree.h"

int main(int argc, const char * argv[]) {
    
    Node<int> na(4);

    std::list<int> testVals{7,6,11,8,13,17,19,23,29,2};
    for(int val : testVals) {
        na + Node<int>(val);
        std::cout << " : " << na.totalLevels << " : " << na.totalCardinality << " : " << na.totalWeight << " : " << na.weightDiff() << " TN: " << na << std::endl;
    }

//    std::cout << "Hello, World!\n";
    return 0;
}
