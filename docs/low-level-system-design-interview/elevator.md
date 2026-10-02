### Final Requirement
```
Requirements:
1. System manages 3 elevators serving 10 floors (0-9)
2. Users can request an elevator from any floor (hall call). System decides which elevator to dispatch.
3. Once inside, users can select one or more destination floors
4. Simulation runs in discrete time steps (e.g., a `step()` or `tick()` call advances time)
5. Elevator stops come in two types:
    - Hall calls: Request from a floor with direction (UP or DOWN)
    - Destination: Request from inside elevator (no direction specified)
6. System handles multiple concurrent pickup requests across floors
7. Invalid requests should be rejected (return false)
    - Non-existent floor numbers
8. Requests for the current floor are treated as a no-op / already served (doors out of scope)

Out of scope:
- Weight capacity and passenger limits
- Door open/close mechanics
- Emergency stop functionality
- Dynamic floor/elevator configuration
- UI/rendering layer
```

### Core Entities

Separating the classes based on the requirements. Ask questions what is required.
1. Floor - Floor doesn't do anything. It is required only for the numbers.
2. Request - May be required for calling the elevator and giving the direction
3. Elevator - It will maintain state, current_floor, direction which floor will stop at
4. ElevatorController - This is the orchestrator. It owns the system-level view of all elevators and makes coordination decisions.


For this system, we land on three entities
1. **ElevatorController** - The orchestrator. Receives hall calls from people on floors, decides which elevator should handle each request, and coordinates the overall system.
2. **Elevator**  - Represents one elevator in the building. Maintains its current floor, direction, and queue of requests. Knows how to execute the movement behaviour. Move one floor at a time, stop when needed, reverse when there are no more stops ahead. Doesn't know about other elevators.
3. **Request** - Represents a stop the elevator needs to make


### Class Design

```
class ElevatorController:
    - elevators: List<Elevator>

    + ElevatorController()
    + requestElevator(floor, type) -> boolean
    + step() -> void
```

