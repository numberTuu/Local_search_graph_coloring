#include "graph.hpp"
#include "readDimacs.hpp"
#include <vector>
#include <set>
#include <numeric>
#include <algorithm>
#include <limits.h>
#include <map>

Graph::Graph(int k){
  //Setting up a vector of colors
  std::vector<int> t(k);
  std::iota(t.begin(),t.end(),0);
  colors = t;

  adjacent_matrix = ReadFile("../inputs/games120.col", &vertices_number);
  for(int i=0;i<vertices_number;i++){
    Vertex temp(i,0,-1,false);
    vertices.push_back(temp);
  }

  //Counting the degree of each vertex by going per row, then per column
  for(int i=0;i<vertices_number;i++){
    for(int j=0;j<vertices_number;j++){
      if(adjacent_matrix[i][j] != 0){
        vertices[i].degree++;
      }
    }
  }
}

int Graph::preprocessing(){
  for(auto it = vertices.begin(); it != vertices.end();){
    if((*it).degree > colors.size()){
      removeEdge((*it).id);                   //Remove edge from adjacency matrix and decrease all the neighbor nodes degree by 1
      vertices.erase(it);
    } else {
      ++it;
    }
  }
  return vertices.size();
}

//Remember that the id of a vertex is the same as an index in the adjacency table
void Graph::removeEdge(int id){
  for(int j = 0; j < vertices_number; ++j){
    if (adjacent_matrix[id][j] == 1){
      for(auto& elem: vertices){
        if(elem.id == j){
          elem.degree -= 1;                  //Decrease the degree of the neighbor nodes
        }
      }
      adjacent_matrix[id][j] = 0;                   //Zero out in the matrix so that there is no edge
      adjacent_matrix[j][id] = 0;
    }
  }
}


int Graph::get_ID(int index){
  for(auto& elem: vertices){
    if(elem.id == index){
      return elem.id;
    }
  }
  return -1;
}

int Graph::get_color(int index){
  for(auto& elem: vertices){
    if(elem.id == index){
      return elem.color;
    }
  }
  return -1;
}

int Graph::get_color_new(int index, std::vector<Vertex> new_solution){
  for(auto& elem: new_solution){
    if(elem.id == index){
      return elem.color;
    }
  }
  return -1;
}

//Ordering the vertices, we're checking to see which vertex has the most edges
void Graph::order_vertices(int vertices_count){
    int a;
    Vertex temp;
    for (int i = 0; i < vertices_count; i++)
    {
        a = i;
        for (int j = i + 1; j < vertices_count; j++)
        {
            if (vertices[j].degree > vertices[a].degree)
                a = j;
        }
        //Note that the assignment operator is overloaded here
        temp = vertices[i];
        vertices[i] = vertices[a];
        vertices[a] = temp;
    }
}

void Graph::ColoringLocal(int flips, int reps){
  srand(time(NULL));
  //Setting up the graph, cleaning up any vertex with degree larger than k-color
  int actual_vcount = preprocessing();

  order_vertices(actual_vcount); //There is room to improve here but more on this later


  //Randomly assign color to each vertex
  for(auto& elem: vertices){
    int color = (rand() % colors.size()) + 0;
    elem.color = color;
  }

  int fcounter = 0;
  std::map<int,int> stash_conflicts; //Map the conflict count to each perspective flip
  std::map<int, std::pair<int,int>> changed_node; //Map

  //Begin of the algorithm
  while (fcounter < flips) {

    std::set<int> node_to_move; //Picked a set to avoid duplicates
    int conflict = 0;

    //Going through the adjancy matrix to find any conflict that we have
    for(int i = 0; i < vertices_number; ++i){
      for(int j = i + 1; j < vertices_number; ++j){
        if(adjacent_matrix[i][j] > 0){ //If there is an edge
          if(get_color(i) == get_color(j)){
            node_to_move.insert(i);                           //Insert the ID of possible node to move into the set
            node_to_move.insert(j);
            conflict+=1;
          }
        }
      }
    }

    //Stash the 'original' conflict count here after the first run
    stash_conflicts.insert(std::pair<int,int>(fcounter,conflict));

    //If there is no conflict, break 
    if(conflict == 0){
      break;
    }

    //Copying the set of ID of nodes that need to be recolor to a vector for ease of use
    std::vector<int> node_to_recolor{node_to_move.begin(), node_to_move.end()};

    //We will recolor a random node in the new_solution vector
    //And storing that change in a map
    changed_node.insert(std::make_pair(fcounter, std::make_pair(-1,-1))); //Set up the stash of changed node
    std::vector<Vertex> new_solution(vertices); //Create a neighor solution 

    for(int i = 0; i < reps; ++i){

      //Picking a node's ID to recolor at random
      int node_recolor_id = node_to_recolor[(rand() % node_to_recolor.size())];

      //Generate a new color base on the colors vector
      int new_color = colors[rand() % colors.size()];

      //Check if the new color isn't the same as the current color of the node
      while(new_color == get_color(node_recolor_id))
        new_color = colors[rand() % colors.size()];

      //Then we recolor that node that we've picked to be recolor in the new solution
      for(auto& elem: new_solution){
        if(elem.id == node_recolor_id){
          elem.color = new_color;
        }
      }

      //Checking for conflicts after changing the color of the chosen node
      int new_conflict = 0;       
      for(int i = 0; i < vertices_number; ++i){
        for(int j = i+1; j < vertices_number; ++j){
          if(adjacent_matrix[i][j] > 0){
            if(get_color_new(i,new_solution) == get_color_new(j,new_solution)){
              new_conflict += 1;
            }
          }
        }
      }

      //If the new solution has less conflict then the previous solution, then we have found an improved solution
      //we may want to stash the color and the node's id that changed
      //The current way ensure that we can find the best possible solution for this flip
      if(new_conflict < stash_conflicts.at(fcounter)){
        stash_conflicts.at(fcounter) = new_conflict; //update the conflict count at current flip

        //Stashing the most optimal node and color for the current flip
        changed_node.at(fcounter).first = node_recolor_id;
        changed_node.at(fcounter).second = new_color;
      }
    }

    //Change a node based on the new solution
    for(auto& elem: vertices){
      if(elem.id == changed_node.at(fcounter).first){
        elem.color = changed_node.at(fcounter).second;
      }
    }
    fcounter +=1;
  }

  if(fcounter < flips){
    std::cout << "We got a solution" << std::endl;
  }
}

