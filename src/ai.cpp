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
    Costs costs
)
  : id(id), agent_speed(agent_speed), rng(rng),
    symbols(symbols), costs(costs)
{}

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
bool matchesTeleporter(string cell, vector<string> & teleporters);
string treasureAround(Percepts & percepts, Symbols & symbols);
string bombNextToMe(Percepts & percepts, Symbols & symbols);

//MAIN THING
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

  

  if(percepts.current[1] == symbols.wall) {
    currentlyDoing = "WALL INFRONT";
    cmds[0] = "R";
  }




  
  //if agent is on teleport
  if( ( matchesTeleporter(percepts.current[0], symbols.teleporters) ) && (percepts.last_move != "U") ) {
    currentlyDoing = "TELEPORTER DETECTED MODE";
      cmds[0] = "U";
  }
  



  //Treasure in around agent
  string treasureAroundResult = treasureAround(percepts, symbols);
  if(treasureAroundResult != "NO TREASURE AROUND") {
    currentlyDoing = "TREASURE AROUND MODE";
    cmds[0] = treasureAroundResult;
  }
  


  //if treasure 4 blocks infront, but trap wants us to rotate once to defuse, then we will lose the treasure!!! Need to remember where treasure 



  //HIGHEST PRIORITY: LAST!
  if(percepts.detector == 1) {
    cmds[0] = bombNextToMe(percepts, symbols);
  }



  if(percepts.current[0] == symbols.treasure) {
    currentlyDoing = "TREASURE UNDER ME";
    cmds[0] = "T";
  }

  cout << "Currently doing: " << currentlyDoing << endl;
  cout << "last move " << percepts.last_move << endl;
  cout << "CMD:      " << cmds[0] << endl;
  return {cmds[0]};
}



string bombNextToMe(Percepts & percepts, Symbols & symbols){
  if(percepts.last_move == "D") {
    return "L";
  } 
  else { 
    return "D";
  } 
}


string treasureAround(Percepts & percepts, Symbols & symbols){
  if(percepts.forward[0] == symbols.treasure || percepts.forward[1] == symbols.treasure || percepts.forward[2] == symbols.treasure ||percepts.forward[3] == symbols.treasure) {
    return "F";
  }
  if(percepts.left[0] == symbols.treasure || percepts.left[1] == symbols.treasure ) {
    return "L";
  }
  if(percepts.right[0] == symbols.treasure || percepts.right[1] == symbols.treasure ) {
    return "R";
  }
  return "NO TREASURE AROUND";
}

//checks if the cell is a teleporter
bool matchesTeleporter(string cell, vector<string> & teleporters) {
  for(size_t i = 0; i < teleporters.size(); i++) {
    if(cell == teleporters[i]) return true;
  }
  return false;
}


