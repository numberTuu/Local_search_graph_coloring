#include "graph.hpp"
#include <iostream>

int main(){
  Graph* graph = new Graph(7);
  graph -> ColoringLocal(1000,100);

  return 0;
}