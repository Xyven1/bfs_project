#include <iostream>
#include <fstream>
#include <list>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <chrono>

using namespace std;
// #include "kernels/cuda.cuh"

struct Vertex{
  string id;
  string name;
  vector<string> references;
};

class Graph {
  map<Vertex*, list<Vertex*>> adjLists;

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
  map<Vertex*, bool> visited;
  
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


  //Globals (heavily reused)
  string line;
  vector<Vertex*> actors;
  map<string, Vertex*> movies;
  ifstream file;
  chrono::_V2::system_clock::time_point start, stop;
  std::chrono::milliseconds elapsed;
  int tabIndex, nextTabIndex;

  file = loadFile("title.basics.tsv");
  cout << "Parsing movies..." << endl;
  start = chrono::high_resolution_clock::now();
  getline(file, line); //skip first line
  TITLE_LOOP:while(getline(file, line)) {
    tabIndex = 0;
    nextTabIndex = 0;
    string segments[3];
    for(int i = 0; i < 3; i++) {
      nextTabIndex = line.find('\t', tabIndex);
      segments[i] = line.substr(tabIndex, nextTabIndex - tabIndex);
      if(i==1 && segments[i] != "movie")
        goto TITLE_LOOP;
      tabIndex = nextTabIndex + 1;
    }
    movies[segments[0]] = new Vertex{
      segments[0],
      segments[2]
    };
  }
  stop = chrono::high_resolution_clock::now();
  elapsed = chrono::duration_cast<chrono::milliseconds>(stop - start);
  cout << "Loaded " << movies.size() << " movies in " << elapsed.count() << " milliseconds" << endl;

  file = loadFile("name.basics.tsv");
  cout << "Parsing actors..." << endl;
  start = chrono::high_resolution_clock::now();
  getline(file, line); //skip first line
  NAME_LOOP:while(getline(file, line)) {
    tabIndex = 0;
    nextTabIndex = 0;
    string segments[6];
    for(int i = 0; i < 6; i++) {
      nextTabIndex = line.find('\t', tabIndex);
      if (i != 2 && i != 3) { //skip years
        segments[i] = line.substr(tabIndex, nextTabIndex - tabIndex);
        //handle roles
        if (i == 4) {
          int tabIndexRole = 0;
          int nextTabIndexRole = 0;
          string role;
          while(nextTabIndexRole != -1) {
            nextTabIndexRole = segments[i].find(',', tabIndexRole);
            role = segments[i].substr(tabIndexRole, nextTabIndexRole - tabIndexRole);
            if(role == "actor" || role == "actress")
              goto END;
            tabIndexRole = nextTabIndexRole + 1;
          }
          goto NAME_LOOP;
        }
      }
      END:tabIndex = nextTabIndex + 1;
    }
    vector<string> actorMovies;
    tabIndex = 0;
    nextTabIndex = 0;
    while (nextTabIndex != -1) {
      nextTabIndex = segments[5].find(',', tabIndex);
      actorMovies.push_back(segments[5].substr(tabIndex, nextTabIndex - tabIndex));
      tabIndex = nextTabIndex + 1;
    }
    actors.push_back(new Vertex{
      segments[0],
      segments[1],
      actorMovies
    });
  }
  stop = chrono::high_resolution_clock::now();
  elapsed = chrono::duration_cast<chrono::milliseconds>(stop - start);
  cout << "Loaded " << actors.size() << " actors in " << elapsed.count() << " milliseconds" << endl;

  //create graph
  Graph graph;
  cout << "Creating graph" << endl;
  start = chrono::high_resolution_clock::now();
  for(Vertex* actor : actors){
    for(string movieId : actor->references){
      if(movies.count(movieId) == 1)
        graph.addEdge(actor, movies[movieId]);
    }
  }
  stop = chrono::high_resolution_clock::now();
  cout << "Created graph in " << chrono::duration_cast<chrono::milliseconds>(stop - start).count() << " milliseconds" << endl;

  cout << "BFS" << endl;
  start = chrono::high_resolution_clock::now();
  map<Vertex*, Vertex *> previous = graph.BFS(actors[111], actors[114]);
  stop = chrono::high_resolution_clock::now();
  cout << "BFS took " << chrono::duration_cast<chrono::milliseconds>(stop - start).count() << " milliseconds" << endl;

  cout << "Printing results" << endl;
  cout << "----------------" << endl;
	list<Vertex*> path;
  path.push_back(actors[114]);
  while(path.front() != actors[111])
    path.push_front(previous[path.front()]);
  for(Vertex* v : path)
    cout << (v->id[0] == 't' ? "Movie" : "Actor") << "|" << v->name << endl;
  return 0;
}