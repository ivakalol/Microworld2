# WorldAI

Run from the repository root:

```bash
./microworld -d 1 -w WorldAI -rs
```

Omit `-rs` for fixed agent-to-start assignments. WorldAI is case-sensitive on Linux.

## Layout

37 columns by 29 rows. Four walking areas are separated by a solid central wall cross. There are four starts: two in the northwest, one in the northeast, and one in the southwest. The southeast has no start and must be reached by teleporting.

There are 12 treasures and 28 mines. Mines can be disarmed, so their presence does not make the intended routes impossible. Each area contains alternate routes and treasure in dead ends. At least one start has only a south exit, invisible to an agent initially facing north with no rear sight.

## Teleport connections

| Source | Destination | Case |
| --- | --- | --- |
| [ | ] | Same northwest walking area |
| ] | [ | Reverse connection in the same area |
| ( | ) | Southwest to southeast |
| ) | ( | Southeast to southwest |
| { | } | Northwest to northeast |
| } | b | Northeast to southeast |
| b | { | Completes a directed three-bracket cycle |
| a | a | Self-loop in northwest |
| c | d | Northwest to northeast |
| d | e | Northeast to southwest |
| e | f | Southwest to southeast |
| f | c | Completes a directed four-area cycle |

Letters a-f are additional teleporter symbols, configured exactly like the brackets. Each source symbol occurs once.

## Cases to inspect

- Wall-shortened percept vectors and corners with several walls.
- A start whose only exit is behind it.
- Treasure beside a mine: taking treasure must still produce T.
- Narrow corridors, branches, loops, and revisiting cells with a different facing.
- Multiple agents in one walking area and relative-agent observations.
- Same-area teleportation and arriving on a previously seen symbol.
- Self-teleportation without incorrectly repeating U forever.
- Directed destinations: arriving at a bracket does not reveal its outgoing destination.
- An area reachable only by teleport.
- Running out of unused brackets under a use-each-source-once policy.

Each agent keeps its own knowledge. A once-per-source policy does not guarantee that every agent can reach every remaining treasure; this world intentionally exercises that limitation. Command slipping is disabled to keep action tracking deterministic.
