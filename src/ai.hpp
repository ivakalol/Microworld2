#pragma once
#include<algorithm>
#include<string>
#include<random>
#include <vector>
#include <stack>
#include<map>
#include<cstdlib>
#include<iostream>
#include<fstream>
#include"percepts.hpp"
#include"comm.hpp"

using namespace std;

struct Cell {
    string symbol;
    bool visited = false;
    bool explored = false;
};

struct PathResult {
    bool found = false;
    int cost = 0;
    stack<string> commands;
};

struct TeleporterInfo {
    int chunkId;
    pair<int, int> position;
};

struct TeleportLink {
    string destinationSymbol;
    bool destinationKnown = false;
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
                                   // every agent will start with local north. It will be ok if they don't share info one to another.
  string last_move;
  //pair<int, int> goal;
  PathResult goal_path;
  
public:
  AI();
  AI(unsigned id,  unsigned agent_speed, mt19937_64* rng, Symbols symbols, Costs costs, int max_turn);
  void PrintPercepts(const Percepts & percepts);
  vector<string> Run( Percepts & percepts, AgentComm * comms);
  void UpdateMap(Percepts & percepts);
  void PrintKnownMap();
  string bombNextToMe(string last_move, Percepts & percepts);
  PathResult FindPath(pair<int, int> target, bool explore);
  string TreasureAround(Percepts & percepts, Symbols & symbols);
};



