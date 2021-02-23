#include "graph.hpp"
#include "readDimacs.hpp"
#include <vector>
#include <set>
#include <numeric>
#include <algorithm>
#include <limits.h>

Graph::Graph(int k){
  std::vector<int> colors(k);
  std::iota(colors.begin(),colors.end(),0);
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


int Graph::get_ID(int index, int vertices_number){
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

/*
//Check if all vertex is colored
bool Graph::vertex_left(){
  for(int i=0;i<vertices_number;i++){
    if(vertices[i].colored == false)
      return true;
  }
  return false;
}
*/

/**
 * The two functions below are for looking toward the neighbors when coloring
 * */
/*
bool Graph::search_color_adjacent(const int id,const int color,Vertex* arr){
  for(int j=0;j<vertices_number;j++){
    if(adjacent_matrix[id][j] != 0){
      if(arr[j].color == color)
        return true;
    }
  }
  return false;
}
*/
/*
bool Graph::check_if_colored(int v_id){
  for(int i = 0; i < vertices_number; ++i){
    if(vertices[i].id == v_id){
      return vertices[i].colored;
    }
  }
}

//Counting the total color
int Graph::count_colors(Vertex* vertices){
  std::vector<int> aux;
  int colors=0;
  for(int i=0;i<vertices_number;i++){
    if(!(std::find(aux.begin(), aux.end(), vertices[i].color) != aux.end())){
      aux.push_back(vertices[i].color);
      colors++;
    }
  }
  return colors;
}

//Return the id of the vertex with highest degree, FOR LATER USE
int Graph::nextHighestDegreeVertex(int id){
  int biggestDegree = 0;
  int v = 0;
  for(int i = 0; i < vertices_number; ++i){
    if(adjacent_matrix[id][i] != 0){
      if(vertices[i].degree >= biggestDegree){
        biggestDegree = vertices[i].degree;
        v = vertices[i].id;
      }
    }
  }

  return v;
}

*/

/*
//Here is where I will try to color the vertex,
//If thing goes south, backtrack will happen
bool Graph::attemptColoring(int id){

  bool canColor;

  //Go through each color to see if we can color that node
  for(int i = 0; i < MAX_COLOR; ++i){
    if (search_color_adjacent(id, i, this -> vertices)){ //1 step look ahead
      canColor = false;
    } else {
      vertices[id].color = i;
      vertices[id].colored = true;
      colored_list.push_back(id); //Push the ID of colored node on to our list of colored node 
      canColor = true;
      break;
    }
  }

  //Backtrack here?
  if(canColor == false){
    return false;
  } else {
    return true;
  }
}

*/
//REMEBER THAT curr is the ID of the vertex
/*
void Graph::dfs(int curr){

  if(attemptColoring(curr) == false) {
    //backtrack here 
  } 

  for(int i = 0; i < vertices_number; ++i){
    if(adjacent_matrix[curr][i] == 1 && check_if_colored(i) == false){
      dfs(get_ID(i));
    }
  }
}

void Graph::ColoringDfs(){
  preprocessing();
  order_vertices();

  //Pick the vertex with the most edges
  //dfs(vertices[0].id);

}
*/

void Graph::ColoringLocal(int flips, int reps){
  srand(time(NULL));
  int actual_vcount = preprocessing();
  order_vertices(actual_vcount);

  //Randomly assign color to each vertex
  for(auto& elem: vertices){
    int color = (rand() % colors.size()) + 0;
    elem.color = color;
  }

  int fcounter = 0;
  while (fcounter < flips) {

    std::set<int> node_to_move; //Picked a set to avoid duplicates
    int conflict = 0;

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

    //If there is no conflict, break 
    if(conflict == 0){
      break;
    }

    //Copying the set of ID of nodes that need to be recolor to a vector
    std::vector<int> node_to_recolor{node_to_move.begin(), node_to_move.end()};

    //Create a neighbor solution
    //We will recolor a random node in the new_solution vector
    std::vector<Vertex> new_solution(vertices);

    for(int i = 0; i < reps; ++i){
      //Picking a node's ID to recolor
      int node_recolor_id = node_to_recolor[(rand() % node_to_recolor.size())];

      //Generate a new color 
      int new_color = colors[rand() % colors.size()];

      //Check if the new color isn't the same as the current color
      while(new_color == get_color(node_recolor_id))
        new_color = colors[rand() % colors.size()];


      //Then we recolor that node that we've picked to be recolor in the new solution
      for(auto& elem: new_solution){
        if(elem.id == node_recolor_id){
          elem.color = new_color;
        }
      }

      int new_conflict = 0;       
      for(int i = 0; i < vertices_number; ++i){
        for(int j = i+1; j < vertices_number; ++j){
          if(get_color_new(i,new_solution) == get_color_new(j,new_solution)){
            new_conflict += 1;
          }
        }
      }

      //If the new solution has less conflict then the previous solution
      //We 
      if(new_conflict < conflict){
        
      }
    }


  }
}

