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

vector<string> AI::Run(
    Percepts & percepts,
    AgentComm * comms
) {
  string currentlyDoing = "nothing";

  cout << "------------------------------------------------\n";
  cout << "AGENT ID: " << id << endl;
  PrintPercepts(percepts);
  vector<string> cmds {"R", "B", "L", "F", "U", "D"};
  shuffle(cmds.begin(), cmds.end(), *rng);
  currentlyDoing = "Random things";

  if(percepts.detector == 1) {
    currentlyDoing = "BOMB DETECTED MODE";
    if(percepts.last_move == "L") {
      cmds[0] = "D";
    } 
    else { 
      cmds[0] = "L";
    }
  }


  cout << "Currently doing: " << currentlyDoing << endl;
  cout << "last move " << percepts.last_move << endl;
  cout << "CMD:      " << cmds[0] << endl;
  return {cmds[0]};
}



