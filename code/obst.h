#ifndef OBST_H
#define OBST_H

#include "node.h"

#include <vector>
#include <memory>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>

using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::move;
using std::ostream;
using std::ofstream;
using std::ostringstream;
using std::cout;
using std::endl;
using std::string;
using std::setprecision;
using std::fixed;

template <class Key>
class Node;

template <class Key>
class Obst : public unique_ptr<Node<Key>> {
    public:
        // Use unique pointer methods (default constructor of unique_ptr is used for this class)
        using unique_ptr<Node<Key>>::unique_ptr;
        
        // Use the move operator of unique_ptr<Node<Key>> as the move operator from Obst
        Obst(unique_ptr<Node<Key>>&& tree) : unique_ptr<Node<Key>> (move(tree)) { }
        
        // Create the Obst of a the probabities that a key is searched
        // and the probabilities that a value in between two keys is searched
        Obst(vector<Key>& keys, vector<float>& p, vector<float>& q);

        // Use this function to visualize the current graph
        // Just copy the textual output that is in the given file after executing the program
        // and paste it in the textarea of the website: http://webgraphviz.com
        void visualize(const char* filename) const;
    protected:
        // Construct g table
        // Initialize z table
        void construct_g_and_initialize_z_table(vector<float>& p, vector<float>& q, vector<vector<float>>* g, vector<vector<float>>* z);
        
        // Construct z and r table
        void construct_z_and_r_table(vector<vector<float>>& g, vector<vector<float>>* z, vector<vector<int>>* r);
        
        // Construct Optimal Binary Search Tree from calculated r table
        // This is done by recursively using the r table
        void construct_obst(vector<Key>& keys, vector<vector<int>>& r, int low, int high);
        
        // Methods to print a 2D float vector with a precision of 2 decimals
        // and to print a 2D int vector
        void print_2d_vec(vector<vector<float>>& vec);
        void print_2d_vec_int(vector<vector<int>>& vec);
        
        // Recursively print out the text of each node to be able to visualize the tree
        string visualizeNodesRecusively(ostream& out, int& nullCounter) const;
};

template <class Key>
Obst<Key>::Obst(vector<Key>& keys, vector<float>& p, vector<float>& q) {
    int size = q.size();
    
    // Initialize 2D vectors that will be used for dynamic programming calculations 
    vector<vector<float>> g(size, vector<float>(size));
    vector<vector<float>> z(size, vector<float>(size));
    vector<vector<int>> r(size, vector<int>(size));
    
    // Do the calculations
    this->construct_g_and_initialize_z_table(p, q, &g, &z);
    this->construct_z_and_r_table(g, &z, &r);
    
    // Print the result of the calculations
    cout << "g:" << endl;        this->print_2d_vec(g); 
    cout << endl << "z:" << endl;this->print_2d_vec(z);
    cout << endl << "g:" << endl;this->print_2d_vec_int(r);
    
    // Construct the Optimal Binary Search Tree of the resulting r 2D vector
    this->construct_obst(keys, r, 1, size-1);
}

template <class Key>
void Obst<Key>::construct_g_and_initialize_z_table(vector<float>& p, vector<float>& q, vector<vector<float>>* g, vector<vector<float>>* z) {
    int size = q.size();
    
    // Initialize diagonals of g and z 2D vectors
    for (int i=0; i<size; i++) {
        (*g)[i][i] = q[i];
        (*z)[i][i] = q[i];
    }
    
    // Initialize rest of the part above the diagonal of the g 2D vector
    for (int i=0; i<size-1; i++) {
        for (int j=i+1; j<size; j++) {
            (*g)[i][j] = (*g)[i][j-1] + p[j] + q[j];
        }
    }
}

template <class Key>
void Obst<Key>::construct_z_and_r_table(vector<vector<float>>& g, vector<vector<float>>* z, vector<vector<int>>* r) {
    int size = g.size();
    
    // Calculate z 2D vector by the formulas that are in README.md
    // r 2D vector is the root of a specific part of the tree wich has the minimal cost
    for (int i=0; i<size; i++) {
        int index = 0;
        for (int j=i+1; j<size; j++) {
            float z_i_j = std::numeric_limits<float>::max();
            int w = index;
            
            for (int k=index; k<j; k++) {
                float new_root = (*z)[index][index+k-index] + (*z)[index+k-index+1][j];
                
                if (new_root < z_i_j) {
                    z_i_j = new_root;
                    w = k;
                }
            }
            
            (*z)[index][j] = z_i_j + g[index][j];
            (*r)[index][j] = w+1;
            
            index++;
        }
    }
}

template <class Key>
void Obst<Key>::construct_obst(vector<Key>& keys, vector<vector<int>>& r, int low, int high) {
    int key_index = r[low-1][high];
    
    Obst<Key> newNode = make_unique<Node<Key>>(keys[key_index]);
    *this = move(newNode);
    
    if (low <= key_index-1)
        (*this)->left.construct_obst(keys, r, low, key_index-1);
    
    if (key_index+1 <= high)
        (*this)->right.construct_obst(keys, r, key_index+1, high);
}

template <class Key>
void Obst<Key>::print_2d_vec(vector<vector<float>>& vec) {
    int size = vec.size();
    cout << setprecision(2) << fixed;
    
    for (int i=0; i<size; i++) {
        for (int j=0; j<size; j++) {
            cout << vec[i][j] << " ";
        }
        cout << endl;
    }
}

template <class Key>
void Obst<Key>::print_2d_vec_int(vector<vector<int>>& vec) {
    int size = vec.size();
    
    for (int i=0; i<size; i++) {
        for (int j=0; j<size; j++) {
            cout << vec[i][j] << " ";
        }
        cout << endl;
    }
}

template <class Key>
void Obst<Key>::visualize(const char* filename) const {
    ofstream out(filename);
    assert(out);
    
    // Nullptr nodes need to get their own number, hence the nullcounter
    int nullCounter = 0;
    out<<"digraph {\n";
    
    // Recursively print out the text of each node to be able to visualize the tree
    this->visualizeNodesRecusively(out, nullCounter);
    
    out<<"}";
}

template <class Key>
string Obst<Key>::visualizeNodesRecusively(ostream& out, int& nullCounter) const {
    ostringstream ObstString;
    
    if (!*this){
        ObstString << "null" << ++nullCounter;
        out << ObstString.str() << " [shape=point];\n";
    }
    else{
        ObstString << '"' << (*this)->key<<'"';
        out << ObstString.str() << "[label=\"" << (*this)->key << "\"]";
        out<<";\n";
        string leftChild = (*this)->left.visualizeNodesRecusively(out, nullCounter);
        string rightChild = (*this)->right.visualizeNodesRecusively(out, nullCounter);
        out << ObstString.str() << " -> " << leftChild << ";\n";
        out << ObstString.str() << " -> " << rightChild << ";\n";
    };
    
    return ObstString.str();
}

#endif