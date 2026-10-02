## Elevator Low Level System Design

### Understanding the Problem

Design an elevator control system for a building. The system should handle multiple elevators, floor requests, and move elevators efficiently to service requests.

## The LLD Framework

1. Requirements
2. Entities
3. Class Design
4. Implementation
5. Extensibility

### Clarifying Questions
1. Find the number of possibilities
	1. Establishing the scale. This matters because the complexity changes dramatically with scale.
2. What is the possible working behaviour.
	1. This tells us we are modeling the usual two-button hall calls ( not destination dispatch ) and dispatch logic is in scope with flexibility in how sophisticated it needs to be
3. Once someone is inside, they can call multiple floors.
4. Hall calls specify direction. Destination buttons inside don't have a direction, just stop it.

### Final Requirements

```
Requirements
	1. System manages 3 elevators serving 10 floors
	2. Users can request and elevator from any floor. System desides which one to dispatch
	3. Once inside, users can select one or more destination floors
	4. Simulation runs in discrete time steps ( e.g. a `step()` or `tick()` call advances time )
	5. Elevator stops come in two types
	   - Hall calls: Request from a floor with direction ( UP or DOWN )
	   - Destination: Request from inside elevator ( no direction specified )
	     
	6. System handles multiple concurrent pickup requests accross floors
	7. Invalid should be rejected and return false  
	8. Requests for the current floor are treated as a no-op / already served ( doors out of scope )

Out of scope:
- Weight capacity and passenger limits
- Door open/close mechanics
- Emergency stop functionality
- Dynamic floor/elevator configuration
- UI/rendering layer
```

### Core Entities and Relationships

The trick is to scan through the requirements and then find the core-entities. These are generally nouns that have behaviour or state.

From the above requirements,
- Elevator
	- Represents one elevator in the building.
	- Maintains the current floor, direction and queue of the requests.
- Floor
- ElevatorController
	- Handles the requests
	- Decides which elevator handle each request and coordinates the overall system
	- It doesn't need the elevator to work just dispatch the elevator to move
- Request

## Class Design

### ElevatorController
- External code interacts with the controller.
- Design its `interface` then drill into the Elevator class

### Fields
| Requirement                                    | What ElevatorController must track      |
| ---------------------------------------------- | --------------------------------------- |
| "System manages 3 elevators serving 10 floors" | The collection of elevators it controls |

```
class ElevatorController:
	- elevators: List<Elevators>
```

### Methods requirement

| Need from requirements                         | Method on ElevatorController                                 |
| ---------------------------------------------- | ------------------------------------------------------------ |
| "Users can request an elevator from any floor" | `requestElevator(floor, type)` for the hall call entry point |
| "Discrete time steps"                          | `step()` to advance all elevators one tick                   |

```
class ElevatorController
	- elevators: List<Elevators>
	
	+ ElevatorController()
	+ requestElevator(floor, type) -> bool
	+ step() -> void
	  
	  
ElevatorController()
    elevators = [
        Elevator(),
        Elevator(),
        Elevator()
    ]
```

`requestElevator` - for a given floor and a request type, dispatch an elevator

`step` - how time advances in our simulation. Each call represents one unit of time passing. The controller tells each elevator to take one step. Move one floor, handle stops or update the direction

### Elevator

`Elevator` represents one elevator in the builder. From the requirements, we need to track position, movement direction and which floors to visit.

```
class Elevator:
	- currentFloor: int
	- direction: Direction // Enum - UP, DOWN, IDLE
	- requests: Set<Request>
	
	// Users can select one or more destination floors
	+ addRequest(floor, type) -> boolean
	 
	// Discrete time steps, elevator moves 
	+ step() -> void
	  
	// Controller needs to know where elevator is
	+ getCurrentFloor() -> int
	  
	// Controller need to know direction for dispatch
	+ getDirection() -> Direction
	  
	addRequest(floor, type):
		requests.add(Request(floor, type))
	
	step()
		pickupType = (direction == UP) ? PICKUP_UP : PICKUP_DOWN
		pickUpRequest = Request(currentFloor, pickupType)
		
		destinationRequest = Request(currentFloor, DESTINATION)
		
		if requests.contains(pickUpRequest) || requests.contain(destinationRequest)
			
			requests.remove(pickUpRequest)
			requests.remove(destinationRequest)

```


### Request

```
// what are the types a user can request
enum RequestType:
	PICKUP_UP
	PICKUP_DOWN
	IDLE

class Request:
	- floor: int
	- type: RequestType
	
	+ Request(floor, type)

```


### Final Class Design

```
class ElevatorController:
    - elevators: List<Elevator>

    + ElevatorController()
    + requestElevator(floor, type) -> boolean
    + step() -> void

class Elevator:
    - currentFloor: int
    - direction: Direction        // UP, DOWN, IDLE
    - requests: Set<Request>

    + Elevator()
    + addRequest(request) -> boolean
    + step() -> void
    + getCurrentFloor() -> int
    + getDirection() -> Direction

class Request:
    - floor: int
    - type: RequestType

    + Request(floor, type)
    + getFloor() -> int
    + getType() -> RequestType

enum Direction:
    UP
    DOWN
    IDLE

enum RequestType:
    PICKUP_UP
    PICKUP_DOWN
    DESTINATION

```

### Implementation

When implementing each method, we'll use this approach
1. **Start with the main flow** - Happy path
2. **Handle the edge cases** - Invalid inputs, boundary conditions or unexpected states?

#### ElevatorController

**Core logic:**
1. Validate the floor number
2. Pick which elevator should handle its request
3. Tell that elevator to add the floor to its stops

**Edge Cases**
1. Floor out of bounds
2. Invalid direction

```python

# ElevatorController
requestElevator(floor, type): -> bool
	if floor < 0 || floor > 9:
		return false
	
	if type == DESTINATION:
		return false
		
	request = Request(floor, type)
	best = selectBestElevator(request);
	return best.addRequest(request)

selectBestElevator(request):
	best = findCommittedToFloor(request)
	if best != null
		return best
	
	best = findNearestIdle(request.getFloor())
	if best != null
		return best
	
	return findNearestIdle(request.getFloor())
	

findCommittedToFloor(request)
    floor = request.getFloor()
    direction = (request.getType() == PICKUP_UP) ? UP : DOWN
    nearest = null
    minDistance = Integer.MAX_VALUE

    for e in elevators
        if e.getDirection() != direction
            continue
        if (direction == UP && e.getCurrentFloor() > floor) ||
           (direction == DOWN && e.getCurrentFloor() < floor)
            continue

        // NEW: Check if elevator has stops that will take it to/past this floor
        if !e.hasRequestsAtOrBeyond(floor, direction)
            continue

        distance = abs(e.getCurrentFloor() - floor)
        if distance < minDistance
            minDistance = distance
            nearest = e

    return nearest

```


### Elevator

```cpp
hasRequestsAtOrBeyond(floor, dir)
    for request in requests
        if dir == UP && request.getFloor() >= floor
            // Has a stop at or above the requested floor
            if request.getType() == PICKUP_UP || request.getType() == DESTINATION
                return true
        if dir == DOWN && request.getFloor() <= floor
            // Has a stop at or below the requested floor
            if request.getType() == PICKUP_DOWN || request.getType() == DESTINATION
                return true
    return false
    

step()
    // Case 1: Nothing to do
    if requests.isEmpty()
        direction = IDLE
        return

    // Case 2: If idle, pick a direction based on nearest request
    if direction == IDLE
        // Find the nearest request to establish initial direction
        nearest = null
        minDistance = Integer.MAX_VALUE

        for req in requests
            distance = abs(req.getFloor() - currentFloor)
            if distance < minDistance ||
                (distance == minDistance && (nearest == null || req.getFloor() < nearest.getFloor()))
                minDistance = distance
                nearest = req

        direction = (nearest.getFloor() > currentFloor) ? UP : DOWN

    // Case 3: Check if we should stop at current floor
    // Check pickup requests matching our direction, plus any destination requests
    pickupType = (direction == UP) ? PICKUP_UP : PICKUP_DOWN
    pickupRequest = Request(currentFloor, pickupType)
    destinationRequest = Request(currentFloor, DESTINATION)

    if requests.contains(pickupRequest) || requests.contains(destinationRequest)
        requests.remove(pickupRequest)
        requests.remove(destinationRequest)
        // Note: If Request(currentFloor, PICKUP_DOWN) exists but we're going UP,
        // it survives and will be serviced on the return trip going DOWN.
        // This is correct - we only pick up passengers going our direction.

        if requests.isEmpty()
            direction = IDLE
        return  // we stopped this tick, don't move

    // Case 4: Reverse if no requests ahead
    if !hasRequestsAhead(direction)
        direction = (direction == UP) ? DOWN : UP
        return  // don't move this tick, let next tick check for stops

    // Case 5: Move one floor
    if direction == UP
        currentFloor++
    else if direction == DOWN
        currentFloor--

```

