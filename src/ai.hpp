#pragma once
using namespace std;

#include<algorithm>
#include<string>
#include<random>
#include<map>
#include<cstdlib>
#include<iostream>
#include<fstream>
#include"percepts.hpp"
#include"comm.hpp"

struct Cell {
    string symbol;
    bool visited = false;
    bool explored = false;
};

enum class Direction {
    N, E, S, W
};

class AI {
protected:
  // Necessary, do not delete.
  unsigned id;
  unsigned agent_speed;
  mt19937_64* rng;
  Symbols symbols;
  Costs costs;
  int max_turn;
  map<pair<int, int>, Cell> knownMap;
  int x = 0; 
  int y = 0;
  Direction facing = Direction::N; //cuz we always start facing north. if -rh is enabled, then
                                   // every agent will start with local north.

  string last_move;
public:
  AI();
  AI(
     unsigned id, 
     unsigned agent_speed,
     mt19937_64* rng,
     Symbols symbols,
     Costs costs,
     int max_turn);
  void PrintPercepts(const Percepts & percepts);
  vector<string> Run(
			       Percepts & percepts,
			       AgentComm * comms);
  void UpdateMap(Percepts & percepts);
  void PrintKnownMap();
};



