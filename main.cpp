#include <iostream>
#include <fstream>
#include <list>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <sstream>

using namespace std;
// #include "kernels/cuda.cuh"

struct Vertex{
  string id;
  string name;
  vector<string> references;
};

class Graph {
  map<Vertex*, list<Vertex*>> adjLists;
  map<Vertex*, bool> visited;

public:
  Graph();
  void addEdge(Vertex* src, Vertex* dest);
  map<Vertex*, Vertex*> BFS(Vertex* startVertex, Vertex* finalVertex);
};

// Create a graph with given vertices,
// and maintain an adjacency list
Graph::Graph() {
}

// Add edges to the graph
void Graph::addEdge(Vertex* src, Vertex* dest) {
  adjLists[src].push_back(dest);
  adjLists[dest].push_back(src);
}

// BFS algorithm
map<Vertex*, Vertex*> Graph::BFS(Vertex* startVertex, Vertex* finalVertex) {
	map<Vertex*, Vertex*> previous;

  list<Vertex*> queue;

  visited[startVertex] = true;
  queue.push_back(startVertex);

  list<Vertex*>::iterator i;

  while (!queue.empty()) {
    Vertex* currVertex = queue.front();
    queue.pop_front();

    // iterates through the adjacency list of the current vertex
    for (auto const& adjVertex : adjLists[currVertex]) {
      if (!visited[adjVertex]) {
				previous[adjVertex] = currVertex;
				if(adjVertex == finalVertex)
					return previous;
        visited[adjVertex] = true;
        queue.push_back(adjVertex);
      }
    }
  }
	return previous;
}

vector<string> split(string str, char delimiter) {
  stringstream ss(str);
  string segment;
  vector<string> segments;
  while(getline(ss, segment, delimiter)){
    segments.push_back(segment);
  }
  return segments;
}

ifstream loadFile(string fileName) {
  cout << "Loading file " << fileName << endl;
  std::ifstream ifs(fileName);
  return ifs;
}

bool IsActor (string s) {
  return s == "actor" || s == "actress";
}


int main(void) {
	// printf("Hello CPP!\n");
  // int i = wrapper(4);
	// printf("%d\n", i);

  string line;
  vector<Vertex*> actors;
  map<string, Vertex*> movies;

  //parse title.basics.tsv
  ifstream title_basics = loadFile("title.basics.tsv");
  int max = 100000;
  while(getline(title_basics, line)){
    vector<string> segments = split(line, '\t');
    if(segments[1] == "movie"){
      Vertex* movie = new Vertex{
        .id = segments[0],
        .name = segments[2]
      };
      movies[movie->id] = movie;
    }
    max--;
  }
  cout << "Finished parsing title.basics.tsv" << endl;

  //parse name.basics.tsv
  ifstream name_basics = loadFile("name.basics.tsv");
  max = 100000;
  while(getline(name_basics, line)){
    vector<string> segments = split(line, '\t');
    vector<string> v = split(segments[4], ',');
    if(segments.size() >= 6 && any_of(v.begin(), v.end(), IsActor)){
      Vertex* actor = new Vertex{
        .id = segments[0],
        .name = segments[1],
        .references = split(segments[5], ',')
      };
      actors.push_back(actor);
    }
    max--;
  }
  cout << "Finished parsing name.basics.tsv" << endl;

  //create graph
  Graph graph;

  cout << "Creating graph" << endl;
  for(Vertex* actor : actors){
    for(string movieId : actor->references){
      if(movies.count(movieId) == 1)
        graph.addEdge(actor, movies[movieId]);
    }
  }
  cout << "Finished creating graph" << endl;

  cout << "BFS" << endl;
  map<Vertex*, Vertex *> previous = graph.BFS(actors[111], actors[114]);
  cout << "Finished BFS" << endl;

  cout << "Printing results" << endl;
	list<Vertex*> path;
  path.push_back(actors[114]);
  while(path.front() != actors[111]) {
    path.push_front(previous[path.front()]);
  }
  for(Vertex* v : path) {
    cout << v->name << endl;
  }
  return 0;
}