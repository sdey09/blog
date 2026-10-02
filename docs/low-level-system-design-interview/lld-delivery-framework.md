# LLD Delivery Framework

<div class="excalidraw-embed" data-src="assets/excalidraw/low-level-system-design-interview/lld-delivery-framework.json"></div>

### Approach

![file:/Users/subhojitdey/Documents/cses/Screenshot%202026-10-02%20at%2010.37.29%20AM.png](file:///Users/subhojitdey/Documents/cses/Screenshot%202026-10-02%20at%2010.37.29%20AM.png)
### 1. Requirements (~ 5 minutes)

Every low-level design interview begins with a prompt.
We have to make the design around it.

- Primary capabilities
- Rules and Completion
- Error handling
- Scope Boundaries

For example, for Designing with Tik Tac Toe

```
Requirements:
1. Two players alternate placing X and O on a 3x3 grid.
2. A player wins by completing a row, column, or diagonal.
3. The game ends in a draw if all nine cells are filled with no winner.
4. Invalid moves should be rejected (placing on an occupied cell, acting after the game is over).
5. The system should provide a way to query current game state and reset the game.

Out of Scope:
- UI/rendering layer
- AI opponent or move suggestions
- Networked multiplayer
- Variable board sizes (NxN grids)
- Undo/redo functionality
```

### 2. Entities and Relationships ( ~ 3 minutes )

Every low-level design interview begins with a prompt.
We have to make the design around it.

- Primary capabilities
- Rules and Completion
- Error handling
- Scope Boundaries

- Identify Entities
    It's helpful to apply a simple filter
    - If something maintains changing state or enforces rules, its probably a field on another class
    - If its just information attached to something else, its probably just a field on another class

- Define Relationships
    - Which entity is the orchestrator
    - Which entities own the durable state
    - How do they depend on each other
    - Specific logical live

For Tic Tac Toe, whiteboard might show

```
Entities:
- Game
- Board
- Player

Relationships:
- Game -> Board
- Game -> Player (2x)
```

## 3. Class Design

Entity
- What does this class need to remember to enforce this requirements
- What does this class need to do in terms of operation or queries

- Deriving State from Requirements
- Deriving Behaviour from Requirements

```
class Game:
  - board: Board
  - playerX: Player
  - playerO: Player
  - currentPlayer: Player
  - state: GameState (IN_PROGRESS, WON, DRAW)
  - winner: Player? (null if no winner)

  + makeMove(player, row, col) -> bool
  + getCurrentPlayer() -> Player
  + getGameState() -> GameState
  + getWinner() -> Player?
  + getBoard() -> Board
```

### 4. Verification

### 5. Extensibility
