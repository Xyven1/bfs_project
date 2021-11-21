#include <chrono>
#include <fstream>
#include <iostream>
#include <list>
#include "robin_hood.h"

using namespace std;

struct Vertex {
  string id;
  string name;
  vector<Vertex*> references;
};

typedef robin_hood::unordered_map<string, Vertex*> VertexMap;

struct BFS_Result {
  robin_hood::unordered_map<Vertex*, Vertex*> parent;
  bool found;
};

// BFS algorithm
BFS_Result BFS(Vertex* startVertex, Vertex* finalVertex) {
  robin_hood::unordered_map<Vertex*, Vertex*> previous;
  robin_hood::unordered_map<Vertex*, bool> visited;

  list<Vertex*> queue;

  if (startVertex == finalVertex) {
    previous[startVertex] = startVertex;
    return {previous, true};
  }

  visited[startVertex] = true;
  queue.push_back(startVertex);

  list<Vertex*>::iterator i;

  while (!queue.empty()) {
    Vertex* currVertex = queue.front();
    queue.pop_front();

    // iterates through the adjacency list of the current vertex
    for (auto const& adjVertex : currVertex->references) {
      if (!visited[adjVertex]) {
        previous[adjVertex] = currVertex;
        if (adjVertex == finalVertex)
          return BFS_Result{previous, true};
        visited[adjVertex] = true;
        queue.push_back(adjVertex);
      }
    }
  }
  return BFS_Result{previous, false};
}

ifstream loadFile(string fileName) {
  ifstream ifs(fileName);
  return ifs;
}

void parseMovies(ifstream file, VertexMap& movies) {
  int tabIndex, nextTabIndex = 0;
  string line;
  getline(file, line);  // skip first line
TITLE_LOOP:
  while (getline(file, line)) {
    tabIndex = 0;
    nextTabIndex = 0;
    string segments[3];
    for (int i = 0; i < 3; i++) {
      nextTabIndex = line.find('\t', tabIndex);
      segments[i] = line.substr(tabIndex, nextTabIndex - tabIndex);
      if (i == 1 && segments[i] != "movie")
        goto TITLE_LOOP;
      tabIndex = nextTabIndex + 1;
    }
    movies[segments[0]] = new Vertex{segments[0], segments[2]};
  }
}

void parseActors(ifstream file, VertexMap& actors, VertexMap& movies) {
  int tabIndex, nextTabIndex = 0;
  string line;
  getline(file, line);  // skip first line
NAME_LOOP:
  while (getline(file, line)) {
    tabIndex = 0;
    nextTabIndex = 0;
    string segments[6];
    for (int i = 0; i < 6; i++) {
      nextTabIndex = line.find('\t', tabIndex);
      if (i != 2 && i != 3) {  // skip years
        segments[i] = line.substr(tabIndex, nextTabIndex - tabIndex);
        // handle roles
        if (i == 4) {
          int tabIndexRole = 0;
          int nextTabIndexRole = 0;
          string role;
          while (nextTabIndexRole != -1) {
            nextTabIndexRole = segments[i].find(',', tabIndexRole);
            role = segments[i].substr(tabIndexRole,
                                      nextTabIndexRole - tabIndexRole);
            if (role == "actor" || role == "actress")
              goto END;
            tabIndexRole = nextTabIndexRole + 1;
          }
          goto NAME_LOOP;
        }
      }
    END:
      tabIndex = nextTabIndex + 1;
    }
    if (segments[5] == "\\N")
      goto NAME_LOOP;
    vector<Vertex*> actorMovies;
    tabIndex = 0;
    nextTabIndex = 0;
    string movieId;
    while (nextTabIndex != -1) {
      nextTabIndex = segments[5].find(',', tabIndex);
      movieId = segments[5].substr(tabIndex, nextTabIndex - tabIndex);
      if (movies.count(movieId) > 0)
        actorMovies.push_back(movies[movieId]);
      tabIndex = nextTabIndex + 1;
    }
    if (actorMovies.size() == 0)
      goto NAME_LOOP;
    auto uniqueEnding = [](int i) {
      return i == 0 ? "" : " (" + to_string(i) + ")";
    };
    int i = 0;
    while (actors.count(segments[1] + uniqueEnding(i)) > 0)
      i++;
    segments[1] += uniqueEnding(i);
    actors[segments[1]] = new Vertex{segments[0], segments[1], actorMovies};
    for(auto const& movie : actorMovies)
      movie->references.push_back(actors[segments[1]]);
  }
}

int main(void) {
  VertexMap actors, movies;
  int tabIndex, nextTabIndex;

  chrono::_V2::system_clock::time_point start;
  auto startTime = [&start]() { start = chrono::high_resolution_clock::now(); };
  auto timeDiff = [&start]() {
    return chrono::duration_cast<chrono::milliseconds>(
               chrono::high_resolution_clock::now() - start)
        .count();
  };

  // parse movies
  cout << "Parsing movies..." << endl;
  startTime();
  parseMovies(loadFile("title.basics.tsv"), movies);
  cout << "Loaded " << movies.size() << " movies in " << timeDiff()
       << " milliseconds" << endl;

  // parse actors
  cout << "Parsing actors..." << endl;
  startTime();
  parseActors(loadFile("name.basics.tsv"), actors, movies);
  cout << "Loaded " << actors.size() << " actors in " << timeDiff()
       << " milliseconds" << endl;

  // running BFS
  bool running = true;
  while (running) {
    string actorName, actor2Name;
  PROPMT_1:
    actorName = "";
    cout << "Enter actor name: ";
    getline(cin, actorName);
    if (actors.count(actorName) == 0) {
      cout << "Actor not found" << endl;
      goto PROPMT_1;
    }
  PROPMT_2:
    actor2Name = "";
    cout << "Enter second actor name: ";
    getline(cin, actor2Name);
    if (actors.count(actor2Name) == 0) {
      cout << "Actor not found" << endl;
      goto PROPMT_2;
    }
    auto actor1 = actors[actorName];
    auto actor2 = actors[actor2Name];
    cout << "Starting BFS..." << endl;
    startTime();
    auto result = BFS(actor1, actor2);
    cout << "BFS took " << timeDiff() << " milliseconds" << endl;

    cout << "Searched " << result.parent.size() << " nodes" << endl;
    if (!result.found) {
      cout << "No path found" << endl;
      goto PROPMT_1;
    }

    cout << "Printing results" << endl;
    cout << "----------------" << endl;
    list<Vertex*> path;
    path.push_back(actor2);
    while (path.front() != actor1)
      path.push_front(result.parent[path.front()]);
    for (Vertex* v : path)
      cout << (v->id[0] == 't' ? "Movie" : "Actor") << "|" << v->name << endl;
    cout << "----------------" << endl;
  }
  return 0;
}