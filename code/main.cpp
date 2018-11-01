#include "obst.h"

#include <cstdlib>
#include <vector>

using std::vector;

int main(int argc, char** argv) {
    // Initialize keys, probabilities of keys occuring 
    // and probabilities of values that are not keys occuring
    vector<int>   k{    0,    1,    2,    3,    4,    5 };
    vector<float> p{ 0.00, 0.15, 0.10, 0.05, 0.10, 0.20 };
    vector<float> q{ 0.05, 0.10, 0.05, 0.05, 0.05, 0.10 };
    
    // Calculate the obst and write to a file (visualization with webgraphviz.com/)
    // Total cost: 2.75 (see right top of z table)
    // Also possible to have 2 as root (also total cost 2.75)
    Obst<int> obst(k, p, q);
    obst.visualize("OptimalBinarySearchTree.txt");
    
    return 0;
}