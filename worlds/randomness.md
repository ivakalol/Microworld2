# Randomness test map

Run from the repository root:

```bash
./microworld -w randomness -d 1 -t 1000 -s 42
```

Change the seed to compare runs. Keep the same seed and executable to repeat a run. Omit -rh so all five agents initially face north. Press P to pause and inspect the displayed turn count.

33 columns by 25 rows, containing 12 open 7-by-7 rooms in a 4-by-3 grid. Solid walls fully enclose each room; agents can travel between rooms only by teleporting. All five agents share the single @ at the center of room 6 (numbered left to right, top to bottom).

Each room has two portals and one treasure. There are no mines, and command slipping is disabled. Agent speed is one command per turn, which may be a rotation or teleport rather than a walking step. The simulation ends when all 12 treasures are collected or the turn limit is reached.

The rooms form a bidirectional portal ring: 1 <-> 2 <-> 3 <-> ... <-> 12 <-> 1. Each pair below links adjacent rooms:

| Portal pair | Rooms |
| --- | --- |
| [ <-> h | 1 and 2 |
| ] <-> i | 2 and 3 |
| ( <-> j | 3 and 4 |
| ) <-> k | 4 and 5 |
| { <-> l | 5 and 6 |
| } <-> m | 6 and 7 |
| a <-> n | 7 and 8 |
| b <-> p | 8 and 9 |
| c <-> q | 9 and 10 |
| d <-> r | 10 and 11 |
| e <-> s | 11 and 12 |
| f <-> g | 12 and 1 |

Teleport destinations are fixed; randomness comes from agents' action choices. Watch them spread out from the common start and use different portals. Overlapping agents are drawn on top of each other, so the existing display cannot report an exact path-divergence turn. Agents can meet again later.
