#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int equalHelper(Node* root, bool& equal, int length, int temp){
    if(root == nullptr) return 0;
    if(!root->left && !root->right && temp != length && temp != 0) {
        equal = false;
        return length;
    }
    else if (!root->left && !root->right && !(temp != length && temp != 0)) {
        temp = length;
        return length;
    }
    else {
        length++;
        temp = equalHelper(root->left, equal, length, temp);
        temp = equalHelper(root->right, equal, length, temp);
    }
    return 0;
}



bool equalPaths(Node * root)
{
    // Add your code below
    bool equal = true;
    equalHelper(root, equal, 0, 0);
    return equal; 
}

