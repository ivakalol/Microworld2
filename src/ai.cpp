#include"ai.hpp"
using namespace std;

/***************************************************************
AI CLASS DEFINITION
*/
AI::AI() {}
AI::AI(
    unsigned id, 
    unsigned agent_speed,
    mt19937_64 * rng,
    Symbols symbols,
    Costs costs,
    int max_turn
)
  : id(id), agent_speed(agent_speed), rng(rng),
    symbols(symbols), costs(costs), max_turn(max_turn)
{}

//PROFESSORS PRINT PERCEPTS FUNCTION
void AI::PrintPercepts(const Percepts & percepts) {
  cout << "DISTANCE: " << percepts.detector << endl;
  cout << "CURRENT:  " << percepts.current[0] << endl;
  cout << "FORWARD:  ";
  for(vector<string>::const_iterator it = percepts.forward.begin();
      it != percepts.forward.end(); it++) cout << *it << " ";
  cout << endl;
  cout << "LEFT:     ";
  for(vector<string>::const_iterator it = percepts.left.begin();
      it != percepts.left.end(); it++) cout << *it << " ";
  cout << endl;
  cout << "BACKWARD: ";
  for(vector<string>::const_iterator it = percepts.backward.begin();
      it != percepts.backward.end(); it++) cout << *it << " ";
  cout << endl;
  cout << "RIGHT:    ";
  for(vector<string>::const_iterator it = percepts.right.begin();
      it != percepts.right.end(); it++) cout << *it << " ";
  cout << endl;
  cout << "Others:\n";
  for(size_t i = 0; i < percepts.others.size(); i++) {
    cout << "   " << i << ": " << percepts.others[i].to_string() << endl;
  }
}
void AI::PrintKnownMap() {
  cout << "KNOWN MAP:\n";
  for(int i = -5; i <= 5; i++) {
    for(int j = -5; j <= 5; j++) {
      auto it = knownMap.find({j, i});
      if(it == knownMap.end()) {
        cout << "? ";
      } else {
        if(it->second.explored) {
          cout << it->second.symbol << " ";
        } else {
          cout << "? ";
        }
      }
    }
    cout << endl;
  }
}


//MATCHES TELEPORTER FUNCTION
bool matchesTeleporter(string cell, vector<string> & teleporters){
  for(size_t i = 0; i < teleporters.size(); i++) {
    if(cell == teleporters[i]) return true;
  }
  return false;
}

//TREASURE AROUND FUNCTION
string treasureAround(Percepts & percepts, Symbols & symbols)
{
  if(percepts.forward[0] == symbols.treasure || percepts.forward[1] == symbols.treasure || percepts.forward[2] == symbols.treasure ||percepts.forward[3] == symbols.treasure) {
    return "F";
  }
  if(percepts.left[0] == symbols.treasure || percepts.left[1] == symbols.treasure ) {
    return "L";
  }
  if(percepts.right[0] == symbols.treasure || percepts.right[1] == symbols.treasure ) {
    return "R";
  }
  return "NO TREASURE INFRONT";
}

//BOMB NEXT TO ME FUNCTION
string bombNextToMe(string last_move){
  if(last_move == "D") {
    return "L";
  } 
  else { 
    return "D";
  } 
}

void AI::UpdateMap(Percepts & percepts) {
  //cell under agent
  Cell & currentCell = knownMap[{x, y}];
  currentCell.symbol = percepts.current[0];
  currentCell.visited = true;
  currentCell.explored = true;
  
  //cells in front of agent
  for(int i = 0; i < 4; i++) {
    int changeX = 0;
    int changeY = 0;
    switch(facing) {
      case Direction::N: changeY = 1; break;
      case Direction::E: changeX = 1; break;   
      case Direction::S: changeY = -1; break;
      case Direction::W: changeX = -1; break;
    }
    Cell & cellInFront = knownMap[{ x + (1+i)*changeX, y + (1+i)*changeY }];
    cellInFront.explored = true;
    string cellSymbol = percepts.forward[i];
    cellInFront.symbol = cellSymbol;
    if(percepts.forward[i] == symbols.wall) {
      break;
    }
  }

  //cells to the left of agent
  for(int i = 0; i < 2; i++) {
    int changeX = 0;
    int changeY = 0;
    switch(facing) {
      case Direction::N: changeX = -1; break;
      case Direction::E: changeY = 1; break;   
      case Direction::S: changeX = 1; break;
      case Direction::W: changeY = -1; break;
    }

    Cell & cellToLeft = knownMap[{x + (1+i)*changeX, y + (1+i)*changeY }];
    cellToLeft.explored = true;
    string cellSymbol = percepts.left[i];
    cellToLeft.symbol = cellSymbol;
    if(percepts.left[i] == symbols.wall) {
      break;
    }
  }

  //cells to the right of agent
  for(int i = 0; i < 2; i++) {
    int changeX = 0;
    int changeY = 0;
    switch(facing) {
      case Direction::N: changeX = 1; break;
      case Direction::E: changeY = -1; break;   
      case Direction::S: changeX = -1; break;
      case Direction::W: changeY = 1; break;
    }
    
    Cell & cellToRight = knownMap[{x + (1+i)*changeX, y + (1+i)*changeY }];
    cellToRight.explored = true;
    string cellSymbol = percepts.right[i];
    cellToRight.symbol = cellSymbol;
    if(percepts.right[i] == symbols.wall) {
      break;
    }
  }
}




//MAIN THINGY
vector<string> AI::Run(
    Percepts & percepts,
    AgentComm * comms
) {
  string currentlyDoing = "nothing";

  cout << "------------------------------------------------\n";
  cout << "AGENT ID: " << id << endl;
  PrintPercepts(percepts);
  vector<string> cmds {"R", "B", "L", "F"};
  shuffle(cmds.begin(), cmds.end(), *rng);
  currentlyDoing = "Random things";

  UpdateMap(percepts);





  
  //if agent is on teleport IDK what to do now with teleporters.
  // if( ( matchesTeleporter(percepts.current[0], symbols.teleporters) ) && (last_move != "U") ) {
  //   currentlyDoing = "TELEPORTER DETECTED MODE";
  //     cmds[0] = "U";
  // }
  



  //Treasure in INFRONT mode
  string treasureAroundResult = treasureAround(percepts, symbols);
  if(treasureAroundResult != "NO TREASURE INFRONT") {
    currentlyDoing = "TREASURE INFRONT MODE";
    cmds[0] = treasureAroundResult;
  }
  





  //HIGHEST PRIORITY: LAST!
  if(percepts.detector == 1) {
    cmds[0] = bombNextToMe(last_move);
  }
  //If agent is on treasure, pick it up then think ab bombs
  if(percepts.current[0] == symbols.treasure) {
    currentlyDoing = "TREASURE UNDER ME";
    cmds[0] = "T";
  }

  cout << "Currently doing: " << currentlyDoing << endl;
  cout << "last move " << last_move << endl;
  cout << "CMD:      " << cmds[0] << endl;
  PrintKnownMap();
  
  last_move = cmds[0];
  return {cmds[0]};
}



