#include <iostream>
#include<fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

/**
 * The vertex class that store information about the degree (edges)
 * that the vertex (node) has.  Also store information like its color
 * and the status of whether it is colored or not
 **/
class Vertex{
public:
  int id; //The number of the vertex i.e 1,2,400,201
  int degree;
  int color;
  bool colored;
  Vertex(int id, int degree, int color, bool colored){
    this -> id = id;
    this -> degree = degree;
    this -> color = color;
    this -> colored = colored;
  };
  Vertex(){};
};


/**
 * The graph class that contain the matrix that will be worked on 
 *  
 * */
class Graph
{
    std::vector<int> colors;
    int vertices_number;
    std::vector<Vertex> vertices;
    int **adjacent_matrix;
    std::vector<int> colored_list; //List of colored node in the order of them being colored

    bool check_colorAvailability(std::vector<Vertex>); //Checking color availability
    bool search_color_adjacent(const int, const int);

    //Look ahead 
    bool search_color_adjacent(const int,const int,Vertex*);
    bool check_vertices_color(const int,const int,std::vector<int>);
    //void change_vertices_color(const int,const int,int*);

    bool vertex_left();
    int preprocessing();
    void removeEdge(int);
    void print_graph_coloring();
    int count_colors(Vertex *);
    //int count_colors_grasp(std::vector<Vertex>);
    void order_vertices(int); 
    int nextHighestDegreeVertex(int);
    bool check_if_colored(int);
    int get_ID(int);
    int get_color(int);
    int get_color_new(int, std::vector<Vertex>);
public:
    Graph(int ); //Constructor
    void dfs(int curr);
    bool attemptColoring(int);
    void ColoringDfs();
    void ColoringLocal(int,int);
};
