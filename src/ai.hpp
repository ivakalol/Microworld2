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

class AI {
protected:
  // Necessary, do not delete.
  unsigned id;
  unsigned agent_speed;
  mt19937_64* rng;
  Symbols symbols;
  Costs costs;
public:
  AI();
  AI(
     unsigned id, 
     unsigned agent_speed,
     mt19937_64* rng,
     Symbols symbols,
     Costs costs);
  void PrintPercepts(const Percepts & percepts);
  vector<string> Run(
			       Percepts & percepts,
			       AgentComm * comms);
};



