#pragma once

#include<vector>
#include<string>
#include<utility>
#include"vec2.hpp"
using namespace std;

enum class AgentType {
    AGENT,
    NONE
};

struct Percepts {
  vector<string> current;
  vector<string> forward;
  vector<string> backward;
  vector<string> left;
  vector<string> right;
  int detector;
  vector<Vec2> others;
};

struct Symbols {
  vector<string> teleporters;
  string wall;
  string open;
  string disarmed_mine;
  string exploded_mine;
  string treasure;
};

struct Costs {
  int round_cost;
  int move_cost;
  int turn_cost;
  int tele_cost;
  int crash_cost;
  int notele_cost;
  int notrap_cost;
  int death_cost;
  int disarm_cost;
  int inactive_cost;
  int treasure_cost;
  int notreasure_cost;
};
