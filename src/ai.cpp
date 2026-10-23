#include "ai.hpp"
#include <tuple>
#include <vector>
#include <queue>
#include <ranges>
using namespace std;

/***************************************************************
  AI CLASS DEFINITION
  */
AI::AI() {}
AI::AI(unsigned id, unsigned agent_speed, mt19937_64 *rng, Symbols symbols, Costs costs,int max_turn)
    : id(id), agent_speed(agent_speed), rng(rng), symbols(symbols), costs(costs),max_turn(max_turn) {}

//PROFESSORS PRINT PERCEPTS FUNCTION
void AI::PrintPercepts(const Percepts &percepts) {
  cout << "DISTANCE: " << percepts.detector << endl;
  cout << "CURRENT:  " << percepts.current[0] << endl;
  cout << "FORWARD:  ";
  for (vector<string>::const_iterator it = percepts.forward.begin(); it != percepts.forward.end();
       it++)
    cout << *it << " ";
  cout << endl;
  cout << "LEFT:     ";
  for (vector<string>::const_iterator it = percepts.left.begin(); it != percepts.left.end(); it++)
    cout << *it << " ";
  cout << endl;
  cout << "BACKWARD: ";
  for (vector<string>::const_iterator it = percepts.backward.begin(); it != percepts.backward.end();
       it++)
    cout << *it << " ";
  cout << endl;
  cout << "RIGHT:    ";
  for (vector<string>::const_iterator it = percepts.right.begin(); it != percepts.right.end(); it++)
    cout << *it << " ";
  cout << endl;
  cout << "Others:\n";
  for (int i = 0; i < percepts.others.size(); i++) {
    cout << "   " << i << ": " << percepts.others[i].to_string() << endl;
  }
}
void AI::PrintKnownMap() {
  cout << "KNOWN MAP:\n";
  for (int i = 10; i >= -10; i--) {
    for (int j = -10; j <= 10; j++) {
      if (j == x && i == y) {
        cout << "X ";
        continue;
      }

      auto it = knownMap.find({j, i});
      if (it != knownMap.end() && it->second.explored) {
        cout << it->second.symbol << " ";
      } else {
        cout << "  ";
      }
    }
    cout << endl;
  }
}

void AI::UpdateMap(Percepts &percepts, AgentComm *comms) {
  if (last_move == "F" || last_move == "B") {
    int nextX = x;
    int nextY = y;

    int step = 0;
    if (last_move == "F") {
      step = 1;
    } else if (last_move == "B") {
      step = -1;
    }

    switch (facing) {
      case Direction::N:
        nextY += step;
        break;
      case Direction::E:
        nextX += step;
        break;
      case Direction::S:
        nextY -= step;
        break;
      case Direction::W:
        nextX -= step;
        break;
    }

    auto destination = knownMap.find({nextX, nextY});

    if (destination != knownMap.end() && destination->second.symbol != symbols.wall) {
      x = nextX;
      y = nextY;
    } else {
      cout << "LAST MOVE WAS INVALID!!!" << endl;
      for (int i = 0; i < 10; i++) {
        goal_path.commands.push("L"); //Punish agent ! (actually for testing)
      }
      last_move = ""; // Clear last_move to avoid repeating the same action.
      return;
    }
  } else if (last_move == "L") {
    switch (facing) {
      case Direction::N:
        facing = Direction::W;
        break;
      case Direction::E:
        facing = Direction::N;
        break;
      case Direction::S:
        facing = Direction::E;
        break;
      case Direction::W:
        facing = Direction::S;
        break;
    }
  } else if (last_move == "R") {
    switch (facing) {
      case Direction::N:
        facing = Direction::E;
        break;
      case Direction::E:
        facing = Direction::S;
        break;
      case Direction::S:
        facing = Direction::W;
        break;
      case Direction::W:
        facing = Direction::N;
        break;
    }
  }

  // Register the current teleporter even if this cell was visited before.
  string& currentSymbol = percepts.current[0];

  if (matchesTeleporter(currentSymbol, symbols.teleporters)) {
    TeleporterInfo& here = teleporters[currentSymbol];
    here.chunkId = currentChunkId;
    here.Tx = x;
    here.Ty = y;
  }

  //cell under agent
  Cell &currentCell = knownMap[{x, y}];
  if (!currentCell.visited) {
    currentCell.symbol = percepts.current[0];
    currentCell.visited = true;
    currentCell.explored = true;
  }

  //cells in front of agent
  for (int i = 0; i < 4; i++) {
    int changeX = 0;
    int changeY = 0;
    switch (facing) {
      case Direction::N:
        changeY = 1;
        break;
      case Direction::E:
        changeX = 1;
        break;
      case Direction::S:
        changeY = -1;
        break;
      case Direction::W:
        changeX = -1;
        break;
    }
    Cell &cellInFront = knownMap[{x + (1 + i) * changeX, y + (1 + i) * changeY}];
    if (!cellInFront.visited && !cellInFront.explored) {
      cellInFront.explored = true;
      string cellSymbol = percepts.forward[i];
      cellInFront.symbol = cellSymbol;
      if (matchesTeleporter(cellSymbol, symbols.teleporters)) {
        teleporters[cellSymbol].chunkId = currentChunkId;
        teleporters[cellSymbol].Tx = x + (1 + i) * changeX;
        teleporters[cellSymbol].Ty = y + (1 + i) * changeY;
      }
    }

    if (percepts.forward[i] == symbols.wall) {
      break;
    }
  }

  //cells to the left of agent
  for (int i = 0; i < 2; i++) {
    int changeX = 0;
    int changeY = 0;
    switch (facing) {
      case Direction::N:
        changeX = -1;
        break;
      case Direction::E:
        changeY = 1;
        break;
      case Direction::S:
        changeX = 1;
        break;
      case Direction::W:
        changeY = -1;
        break;
    }

    Cell &cellToLeft = knownMap[{x + (1 + i) * changeX, y + (1 + i) * changeY}];
    if (!cellToLeft.visited && !cellToLeft.explored) {
      cellToLeft.explored = true;
      string cellSymbol = percepts.left[i];
      cellToLeft.symbol = cellSymbol;
      if (matchesTeleporter(cellSymbol, symbols.teleporters)) {
        teleporters[cellSymbol].chunkId = currentChunkId;
        teleporters[cellSymbol].Tx = x + (1 + i) * changeX;
        teleporters[cellSymbol].Ty = y + (1 + i) * changeY;
      }
    }
    if (percepts.left[i] == symbols.wall) {
      break;
    }
  }

  //cells to the right of agent
  for (int i = 0; i < 2; i++) {
    int changeX = 0;
    int changeY = 0;
    switch (facing) {
      case Direction::N:
        changeX = 1;
        break;
      case Direction::E:
        changeY = -1;
        break;
      case Direction::S:
        changeX = -1;
        break;
      case Direction::W:
        changeY = 1;
        break;
    }

    Cell &cellToRight = knownMap[{x + (1 + i) * changeX, y + (1 + i) * changeY}];
    if (!cellToRight.visited && !cellToRight.explored) {
      cellToRight.explored = true;
      string cellSymbol = percepts.right[i];
      cellToRight.symbol = cellSymbol;
      if (matchesTeleporter(cellSymbol, symbols.teleporters)) {
        teleporters[cellSymbol].chunkId = currentChunkId;
        teleporters[cellSymbol].Tx = x + (1 + i) * changeX;
        teleporters[cellSymbol].Ty = y + (1 + i) * changeY;
      }
    }
    if (percepts.right[i] == symbols.wall) {
      break;
    }
  }

  //cells of other agents
  for ( Vec2 &other : percepts.others) {
    // The list includes this agent.
    if (other.x == 0 && other.y == 0)
      continue;

    int dx = 0;
    int dy = 0;

    switch (facing) {
      case Direction::N:
        dx = other.x;
        dy = other.y;
        break;
      case Direction::E:
        dx = other.y;
        dy = -other.x;
        break;
      case Direction::S:
        dx = -other.x;
        dy = -other.y;
        break;
      case Direction::W:
        dx = -other.y;
        dy = other.x;
        break;
    }

    Cell &cellOfOther = knownMap[{x + dx, y + dy}];
    if (!cellOfOther.visited && !cellOfOther.explored) {
      cellOfOther.explored = true;
      if (comms->GetSignal(1) != 1){
        cellOfOther.visited = true;
      }
      cellOfOther.symbol = "A"; // where agent was observed
    }
  }



}

//MATCHES TELEPORTER FUNCTION
bool AI::matchesTeleporter(string cell, vector<string> &teleporters) {
  for (int i = 0; i < teleporters.size(); i++) {
    if (cell == teleporters[i])
      return true;
  }
  return false;
}

//TREASURE AROUND FUNCTION
string AI::TreasureAround(Percepts &percepts, Symbols &symbols) {
  // for (string cell : percepts.forward) {
  //     if (cell == symbols.treasure) return "F";
  // }

  // for (string cell : percepts.left) {
  //   if (cell == symbols.treasure) return "L";
  // }

  // for (string cell : percepts.right) {
  //   if (cell == symbols.treasure) return "R";
  // }

  int sizeF = percepts.forward.size();
  int sizeL = percepts.left.size();
  int sizeR = percepts.right.size();
  for (int i = 0; i < sizeF; i++) {
    if (percepts.forward[i] == symbols.treasure)
      return "F";
  }
  for (int i = 0; i < sizeL; i++) {
    if (percepts.left[i] == symbols.treasure)
      return "L";
  }
  for (int i = 0; i < sizeR; i++) {
    if (percepts.right[i] == symbols.treasure)
      return "R";
  }

  return "NO TREASURE INFRONT";
}

//BOMB NEXT TO ME FUNCTION
string AI::bombNextToMe(string last_move, Percepts &percepts) {
  if (last_move == "D") {
    if (goal_path.found == true) {
      goal_path.commands.push("R");
    }
    return "L";
  } else {
    return "D";
  }
}

// https://www.youtube.com/watch?v=icZj67PTFhc
PathResult AI::FindPath(pair<int, int> target, bool explore) {
  struct SearchState { //possible situation + commands to get there
    int x;
    int y;
    Direction facing;
    int g;
    int h;
    vector<string> commands;
  };

  PathResult result;

  // Position AND facing identify a search state.
  using StateKey = tuple<int, int, Direction>;
  //using this like a dictionary from C#
  map<StateKey, int> bestCost;

  int startHeauristic = abs(x - target.first) + abs(y - target.second);
  SearchState start{x, y, facing, 0, startHeauristic, {}};

  vector<SearchState> queue;
  queue.push_back(start);
  bestCost[StateKey{x, y, facing}] = 0;

  while (!queue.empty()) {
    // Select the state with the smallest g + h.
    int bestIndex = 0;

    switch (explore) {
      case true:
        for (int i = 1; i < queue.size(); i++) {
          if (queue[i].g < queue[bestIndex].g) {
            bestIndex = i;
          }
        }
        break;
      case false:
        vector<int> chance = {0, 1};
        shuffle(chance.begin(), chance.end(), *rng);
        if (chance[0] == 0) {
          for (int i = 1; i < queue.size(); i++) {
            if ((queue[i].g + queue[i].h) < (queue[bestIndex].g + queue[bestIndex].h)) {
              bestIndex = i;
            }
          }
        } else {
          for (int i = queue.size() - 1; i > 0; i--) {
            if ((queue[i].g + queue[i].h) < (queue[bestIndex].g + queue[bestIndex].h)) {
              bestIndex = i;
            }
          }
        }
        break;
    }

    SearchState current = queue[bestIndex];
    queue.erase(queue.begin() + bestIndex);

    StateKey currentKey{current.x, current.y, current.facing};

    // A cheaper version of this state may have been added.
    if (current.g > bestCost.at(currentKey)) {
      continue;
    }

    // The stored commands already contain the full route.
    bool reachedGoal;

    if (explore) {
      reachedGoal = !knownMap.at({current.x, current.y}).visited;
    } else {
      reachedGoal = current.x == target.first && current.y == target.second;
    }

    if (reachedGoal) {
      result.found = true;
      result.cost = current.g;

      for (auto it = current.commands.rbegin(); it != current.commands.rend(); ++it) {
        result.commands.push(*it);
      }
      return result;
    }

    // Calculate forward movement for this state's facing.
    int dx = 0;
    int dy = 0;
    switch (current.facing) {
      case Direction::N:
        dy = 1;
        break;
      case Direction::E:
        dx = 1;
        break;
      case Direction::S:
        dy = -1;
        break;
      case Direction::W:
        dx = -1;
        break;
    }

    //find children for all actions.
    // this is like foreach loop in C#
    vector<string> actions = {"F", "L", "R"};
    shuffle(actions.begin(), actions.end(), *rng);
    for (const string &action : actions) {
      SearchState neighbour = current;

      if (action == "F") {
        neighbour.x += dx;
        neighbour.y += dy;
      } else if (action == "B") {
        neighbour.x -= dx;
        neighbour.y -= dy;
      } else if (action == "L") {
        // Direction enum order: N, E, S, W.
        neighbour.facing = static_cast<Direction>((static_cast<int>(current.facing) + 3) % 4);
      } else if (action == "R") {
        neighbour.facing = static_cast<Direction>((static_cast<int>(current.facing) + 1) % 4);
      }

      // Movement must lead to a known, non-wall cell.
      if (action == "F" || action == "B") {
        auto cell = knownMap.find({neighbour.x, neighbour.y});

        if (cell == knownMap.end() || cell->second.symbol == symbols.wall) {
          continue;
        }
      }

      neighbour.g = current.g + 1;
      neighbour.h = abs(neighbour.x - target.first) + abs(neighbour.y - target.second);

      StateKey neighbourKey{neighbour.x, neighbour.y, neighbour.facing};

      auto previous = bestCost.find(neighbourKey);

      // Only keep routes that improve the known cost.
      if (previous != bestCost.end() && neighbour.g >= previous->second) {
        continue;
      }

      bestCost[neighbourKey] = neighbour.g;
      neighbour.commands.push_back(action);
      // if(action == "B"){ //didn't work so i simply removed "B" from list of actions... This will never be used
      //   neighbour.commands.insert(neighbour.commands.begin(), "L");
      //   neighbour.commands.insert(neighbour.commands.begin(), "L");
      // }
      // else{

      // }
      queue.push_back(neighbour);
    }
  }
  // No route through the known map.
  return result;
}

void AI::FindTeleportBracket() {
  vector<int> chance = {1, 2};
  shuffle(chance.begin(), chance.end(), *rng);

  if (chance[0] == 1) {
    // Forward loop: first element to last
    for (const auto &[position, cell] : knownMap) {
      if (!matchesTeleporter(cell.symbol, symbols.teleporters)) {
        continue;
      }

      if (teleporters[cell.symbol].destinationKnown) {
        continue;
      }

      PathResult path = FindPath(position, false);

      if (!path.found) {
        continue;
      }

      plannedTeleporter = cell.symbol;
      goal_path = std::move(path);
      teleporters[cell.symbol].chunkId = currentChunkId;
      //currentlyDoing = "Walking to teleport chance1" + plannedTeleporter;
      break;
    }
  } else {
    // Reverse loop: last element to first
    for (auto it = knownMap.rbegin(); it != knownMap.rend(); ++it) {
      const auto &[position, cell] = *it;

      if (!matchesTeleporter(cell.symbol, symbols.teleporters)) {
        continue;
      }

      if (teleporters[cell.symbol].destinationKnown) {
        continue;
      }

      PathResult path = FindPath(position, false);

      if (!path.found) {
        continue;
      }

      plannedTeleporter = cell.symbol;
      teleporters[cell.symbol].chunkId = currentChunkId;
      goal_path = std::move(path);
      //currentlyDoing = "Walking to teleport chance2" + plannedTeleporter;
      break;
    }
  }
}

void AI::afterUsingTeleporter(Percepts &percepts) {
  string arrivalTelep = percepts.current[0];
  // Learn where the departure teleporter leads.
  TeleporterInfo &departure = teleporters[departureTeleporter];
  departure.destinationSymbol = arrivalTelep;
  departure.destinationKnown = true;

  // Save the area we just left, including visited cells.
  chunks[currentChunkId].id = currentChunkId;
  chunks[currentChunkId].chunkMap = knownMap;

  // This is the DESTINATION entry, not the departure entry.
  TeleporterInfo &arrival = teleporters[arrivalTelep];

  if (arrival.chunkId != -1) {
    // We have seen this destination before.
    currentChunkId = arrival.chunkId;
    knownMap = chunks.at(currentChunkId).chunkMap;
    x = arrival.Tx;
    y = arrival.Ty;
  } else {
    // Unknown destination: start a new local map.
    currentChunkId = ++maxChunkId;
    knownMap.clear();
    x = 0;
    y = 0;

    chunks[currentChunkId].id = currentChunkId;

    // Remember where this arrival teleporter belongs.
    arrival.chunkId = currentChunkId;
    arrival.Tx = x;
    arrival.Ty = y;
  }

  cout << "Teleported from " << departureTeleporter << " to " << arrivalTelep << ", chunk "
       << currentChunkId << endl;

  // Routes from the previous location are no longer valid.
  goal_path = PathResult{};
  plannedTeleporter.clear();
  departureTeleporter.clear();
}

//MAIN THINGY
vector<string> AI::Run(Percepts &percepts, AgentComm *comms) {
  //UpdateTeleporter();
  cout << "------------------------------------------------\n";
  cout << "AGENT ID: " << id << endl;
  PrintPercepts(percepts);
  string currentlyDoing = "nothing";
  string cmd = "";
  if (last_move == "U" && !departureTeleporter.empty()) {
    afterUsingTeleporter(percepts);
  }
  UpdateMap(percepts, comms);




  //SETTING GOAL

  // If we don't have a goal, find one.
  if (goal_path.commands.empty() && plannedTeleporter.empty()) {
    currentlyDoing = "Explore";
    //goal = {0, 0};
    goal_path = FindPath({0, 0}, true);
  }

  if (!goal_path.found) {
    currentlyDoing = "No path found";
    if (last_move != "R") {
      //From the start, we can be at the end of a coridor, facing wall and on both sides walls. Only possible way is
      // the block behind us, but we can't see/map it, so we need to turn right so that we can see it
      goal_path.commands.push("R");
    } else {
      FindTeleportBracket();
    }
  }

  //Look if discovered a treasure right under us
  if (percepts.current[0] == symbols.treasure && last_move != "T") {
    currentlyDoing = "Picking up treasure";
    cmd = "T";
  }

  else {
    string treasureCmd = TreasureAround(percepts, symbols);
    if (treasureCmd != "NO TREASURE INFRONT") {
      currentlyDoing = "Going to treasure";
      goal_path = PathResult{};
      cmd = treasureCmd;
      plannedTeleporter.clear();
    }

    //Bomb around me
    if (percepts.detector == 1) {
      currentlyDoing = "BOMB NEXT TO ME";
      cmd = bombNextToMe(last_move, percepts);
    }
    
    //We have reached the planned teleporter
    if (cmd.empty() && !plannedTeleporter.empty() && percepts.current[0] == plannedTeleporter &&
        !(teleporters[plannedTeleporter].destinationKnown) && percepts.detector != 1) {
      currentlyDoing = "Using teleport " + plannedTeleporter;
      cmd = "U";
    }
  }






  //Doing Commants from goal_path
  if (cmd.empty() && !goal_path.commands.empty() && percepts.detector != 1) {
    currentlyDoing = "Following goal path";
    cmd = goal_path.commands.top();
    goal_path.commands.pop();
  }





  if (id == 0) { //reset all signals to 0
    comms->SetSignal(1, 0);
  } 
  if( matchesTeleporter(percepts.current[0], symbols.teleporters) ){
    comms->SetSignal(1, 1);
  }
  cout << "Currently doing: " << currentlyDoing << endl;
  cout << "last move " << last_move << endl;
  cout << "CMD:      " << cmd << endl;
  //PrintKnownMap();
  if (cmd == "U") {
    departureTeleporter = percepts.current[0];
    plannedTeleporter.clear();
    goal_path = PathResult{};
  }
  last_move = cmd;
  max_turn--;
  return {cmd};
}
