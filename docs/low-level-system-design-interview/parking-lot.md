A parking lot system manages vehicle parking across multiple spots. When a vehicle enters, the system assigns an available spot matching the vehicle type and issues a ticket. When the vehicle exits, the system calculates the parking fee based on time spent and frees up the spot for the next customer.

## Requirements:
1. System supports three vehicle types: Motorcycle, Car, Large Vehicle
2. When a vehicle enters, system automatically assigns an available compatible spot
3. System issues a ticket at entry.
4. When a vehicle exits, user provides ticket ID
   - System validates the ticket
   - Calculates fee based on time spent (hourly, rounded up)
   - Frees the spot for next use
5. Pricing is hourly with same rate for all vehicles
6. System rejects entry if no compatible spot is available
7. System rejects exit if ticket is invalid or already used

Out of scope:
- Payment processing
- Physical gate hardware
- Security cameras or monitoring
- UI/display systems
- Reservations or pre-booking


For each class, we'll ask two questions:

1. What does this class need to remember to enforce the requirements (its state)?
2. What operations does this class need to support (its methods)?


## Core Entities and Relationships

### **Vehicle**
We only need enum for this as we are not tracking and it is outside the system.

### **ParkingSpot**
It has clear entity. Spot has ID, a type to match vehicle types and needs to track whether it is occupied.

### Ticket
When a vehicle enters, we issue a ticket. The ticket holds the record of the parking session. It holds the ticket ID, which spot was assigned and type of vehicle.

---
### ParkingLot
Something needs to orchestrate the whole system. When a vehicle enter, something needs to find a available spot, generate a ticket and mark the spot occupied.

After filtering, we're left with three entities:

| Entity          | Responsibility                                                                                                                                                                                                                                    |
| --------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **ParkingLot**  | The orchestrator. Owns all spots, tracks active tickets, assigns spots at entry, validates tickets and calculates fees at exit. This is the only public API for the system.                                                                       |
| **ParkingSpot** | Represents one parking space. Has an ID, a type (motorcycle spot, car spot, large spot), and an occupied flag. Provides methods to check if it's free and to mark it occupied or free. Doesn't know about tickets or pricing, just its own state. |
| **Ticket**      | A record of a parking session. Holds ticket ID, which spot was assigned, vehicle type, and entry time. Read-only after creation. No business logic here, just data that ParkingLot needs to calculate fees and validate exits.                    |

#### Asking Questions and with the answer

| Requirement                                                 | What ParkingLot must track         |
| ----------------------------------------------------------- | ---------------------------------- |
| "System automatically assigns an available compatible spot" | All parking spots in the lot       |
| "System automatically assigns an available compatible spot" | Which spots are currently occupied |
| "System issues a ticket at entry"                           | Active tickets to validate on exit |
| "Calculates fee based on time spent (hourly)"               | The hourly rate for pricing        |
#### Good Solution: Store occupied flag on the spot

**Approach**

One option is to add an `occupied` boolean field to `ParkingSpot` Mark it true when someone parks there and false when they leave.

```
findAvailableSpot(vehicleType)
    requiredType = mapVehicleTypeToSpotType(vehicleType)
    for spot in spots
        if spot.isFree() and spot.spotType == requiredType
            return spot
    return null
```

- **Denormalization risk** - The consideration with this approach is denormalisation. Now occupation lives in two places `flag on the spot` and `activeTickets` map. 
- **Synchronization burden**: If you forget to update the flag somewhere, the two sources of truth diverge, leading to bugs


#### Great Solution : Derive occupancy from tickets

**Approach**

Skipping storing occupancy entirely. A spot is occupied if and only if an active ticket references it.
```
findAvailableSpot(vehicleType)
    requiredType = mapVehicleTypeToSpotType(vehicleType)
    occupiedSpotIds = { ticket.spotId for ticket in activeTickets.values() }
    for spot in spots
        if spot.spotType == requiredType and spot.id not in occupiedSpotIds
            return spot
    return null
```

**Challenges**
- For a small parking lot ( 200 , 100 ) , the computation is cheap.
- Concurrency issue, need to look the entire `activeTickets` map during reads and writes

#### Great Solution:  Maintain an occupancy index

**Approach**

To build the previous approach, `ParkingSpot` can still stay a pure data holder and `ParkingLot` can track while spots are occupied using a seperate Set.

```
class ParkingLot:
    - spots: List<ParkingSpot>
    - occupiedSpotIds: Set<String>      // Index for occupancy
    - activeTickets: Map<String, Ticket>
    - hourlyRateCents: long

class ParkingSpot:
    - id: String       // Intrinsic
    - spotType: SpotType  // Intrinsic
```

```
findAvailableSpot(vehicleType)
    requiredType = mapVehicleTypeToSpotType(vehicleType)
    for spot in spots
        if spot.spotType == requiredType and spot.id not in occupiedSpotIds
            return spot
    return null
```

Yes, the Set is technically redundant with the ticket data. Like a database index, it must stay in sync with the source data, but the trade-off is worth it.

#### The Final Class Design
```
class ParkingLot:
    - spots: List<ParkingSpot>
    - occupiedSpotIds: Set<String>
    - activeTickets: List<Ticket>
    - hourlyRateCents: long
```

Now for operations. What actions does the outside world need to perform?

| Need from requirements                                         | Method on ParkingLot                |
| -------------------------------------------------------------- | ----------------------------------- |
| "When a vehicle enters, system assigns spot and issues ticket" | enter(vehicleType) returns a Ticket |
| "When vehicle exits, validates ticket and calculates fee"      | exit(ticketId) returns fee amount   |
```
class ParkingLot:
    - spots: List<ParkingSpot>
    - occupiedSpotIds: Set<String>
    - activeTickets: List<Ticket>  // We'll refine this to a Map during implementation
    - hourlyRateCents: long

    + ParkingLot(spots, hourlyRateCents)
    + enter(vehicleType) -> Ticket
    + exit(ticketId) -> long
```

`enter` - throws an error if no spot is found
`exit` - takes a ticket ID and returns the fee in cents and throws error if the ticket is invalid

---
### ParkingSpot

ParkingSpot represents one physical parking space. From requirements:

|Requirement|What ParkingSpot must track|
|---|---|
|"System assigns compatible spot"|Spot type (motorcycle, car, large) to match with vehicle type|
|"When a vehicle exits, user provides ticket ID"|Unique ID for the spot|

```
class ParkingSpot:
    - id: String
    - spotType: SpotType
```

For operations:

|Need from requirements|Method on ParkingSpot|
|---|---|
|"System automatically assigns an available compatible spot"|getSpotType() returns type|
|"System issues a ticket at entry"|getId() returns spot ID|
```
class ParkingSpot:
    - id: String
    - spotType: SpotType

    + ParkingSpot(id, spotType)
    + getSpotType() -> SpotType
    + getId() -> String
      

enum SpotType:
    MOTORCYCLE
    CAR
    LARGE

enum VehicleType:
    MOTORCYCLE
    CAR
    LARGE
```


### Ticket

Ticket is a record of a parking session. From requirements:

|Requirement|What Ticket must track|
|---|---|
|"When a vehicle exits, user provides ticket ID"|Ticket ID string|
|"Frees the spot for next use"|Which spot the vehicle is in|
|"System supports three vehicle types"|Type of vehicle (not used in base pricing, but stored for per-type pricing extension)|
|"Calculates fee based on time spent"|When they entered (needed for fee calculation)|
```
class Ticket:
    - id: String
    - spotId: String
    - vehicleType: VehicleType
    - entryTimeMs: long
```

**Why spotId as a string, not a reference to ParkingSpot?** 
Tickets are records, not navigational objects. They shouldn't reach into the domain model. Storing just the ID keeps them simple and prevents tickets from accidentally calling methods on spots.

#### Bad Solution : calculateFee() method on Ticket

**Approach**
Adding `calculateFee` in the Ticket

```
class Ticket:
    - id: String
    - spotId: String
    - vehicleType: VehicleType
    - entryTime: long
    - hourlyRateCents: long  // Store rate in ticket

  long calculateFee(long exitTime) -> long
```

This seems to be reasonable on the surface. The ticket owns the entry time, so shouldn't it own the calculation that uses it?

**Challenges**

This violates `Single Responsibility Principle`. The Ticket is now both a record of a parking session and a pricing calculator. It has two reasons to change, if we change what data a ticket stores or if we change how fees are calculated.

More importantly pricing is a business policy not Ticket.
#### Good Solution: calculate computeFee() in ParkingLot

**Approach**
Keep Ticket as a pure data holder with no behaviour. Put the fee calculation in ParkingLot, where all the other business rules live.

```
class Ticket:
    - id: String
    - spotId: String
    - vehicleType: VehicleType
    - entryTime: long

class ParkingLot:
    - hourlyRateCents: long
```

```
computeFee(entryTime, exitTime)
    durationMillis = exitTime - entryTime
    durationHours = durationMillis / (1000 * 60 * 60)
    if durationMillis % (1000 * 60 * 60) > 0
        durationHours++  // Any partial hour rounds up, so 5 minutes becomes 1 hour
    return durationHours * hourlyRateCents
```

Tickets stays immutable. Its a simple record with no business logic. ParkingLot orchestrates everything, including pricing. When you call `exit`, ParkingLot gets the entry time from the ticket and calculates the fee

**Benefits**

This follows `Separation Of Concerns`. Business rules live in the orchestrator, data records stay simple. All pricing logic is in one making it easy to test modify and understand.

#### Great Solution: Separate Pricing interface

**Approach**
If we are building software to be used by multiple parking lots with different pricing rule, we would extract pricing into its own abstracting using `Strategy Pattern`

```
interface PricingStrategy
    calculateFee(ticket, exitTime)

class HourlyPricing implements PricingStrategy
    - hourlyRateCents
  
    calculateFee(ticket, exitTime)
        // Same logic as before

class DynamicPricing implements PricingStrategy
    calculateFee(ticket, exitTime)
        // Complex logic: surge pricing, time of day, etc.

class ParkingLot
    - pricingStrategy
  
    exit(ticketId)
        ticket = activeTickets[ticketId]
        fee = pricingStrategy.calculateFee(ticket, currentTime())
        // ... rest of exit logic
```


Final Class for the Ticket

```
class Ticket:
    - id: String
    - spotId: String
    - vehicleType: VehicleType
    - entryTime: long

    + Ticket(id, spotId, vehicleType, entryTime)
    + getId() -> String
    + getSpotId() -> String
    + getVehicleType() -> VehicleType
    + getEntryTime() -> long
```


## Final Class Design

```
class ParkingLot:
    - spots: List<ParkingSpot>
    - occupiedSpotIds: Set<String>
    - activeTickets: Map<string, Ticket>
    - hourlyRateCents: long

    + ParkingLot(spots, hourlyRateCents)
    + enter(vehicleType) -> Ticket
    + exit(ticketId) -> long
  
class ParkingSpot:
    - id: String
    - spotType: SpotType

    + ParkingSpot(id, spotType)
    + getSpotType() -> SpotType
    + getId() -> String

class Ticket:
    - id: String
    - spotId: String
    - vehicleType: VehicleType
    - entryTime: long

    + Ticket(id, spotId, vehicleType, entryTime)
    + getId() -> String
    + getSpotId() -> String
    + getVehicleType() -> VehicleType
    + getEntryTime() -> long

enum SpotType:
    MOTORCYCLE
    CAR
    LARGE

enum VehicleType:
    MOTORCYCLE
    CAR
    LARGE
```


## Implementation

### ParkingLot

Refined ParkingLot State
```
class ParkingLot:
    - spots: List<ParkingSpot>
    - occupiedSpotIds: Set<String>
    - activeTickets: Map<String, Ticket>  // Changed from List to Map
    - hourlyRateCents: long
```

Let's start with `enter`. This is where vehicles arrive and get assigned a spot.

**Core Logic**
1. Find an available spot that matches the vehicle type
2. If no spot found, throw an error
3. Add the spot ID to `occupiedSpotIds`
4. Generate a ticketId with vehicleId, spotID and currentTimestamp 
5. Add to the `activeTickets`
6. Return the ticket

**Edge Cases**
1. No available spots for this vehicle type
2. Invalid vehicle type

```
enter(vehicleType)
	spot = findAvailableSpot(vehicleType)
	if spot == null
		throw error
	
	occupiedSpotIds.add(spot.id)
	ticket = createTicket(generateId(), spot.id, vehicleType, currentTime())
	activeTickets[ticket.id] = ticket
	return ticket
```

Now `exit` This is where vehicles leave and pay

**Core Logic**
1. Look up the ticket by ID in activeTickets map
2. If not found, throw an error
3. Calculate fee based on entry time and currentTime
4. Remove the spotId from occupiedSpotIds
5. Remove the ticket from activeTickets
6. Return fee

**Edge Cases**
1. Ticket Id doesn't exist
2. Ticket Id is null or empty
3. Time calculation edge cases

```
exit(ticketId)
	if ticketId == null
		throw error
	ticket = activeTickets[ticketId]
	if ticket == null
		throw error
	
	exitTime = currentTime()
	fee = computeFee(ticket.entryTime, exitTime)
	
	occupiedSpotIds.remove(ticket.spotId)
	activeTickets.remove(ticketId)
	
	return fee
```


```
findAvailableSpot(vehicleType)
    requiredSpotType = mapVehicleTypeToSpotType(vehicleType)

    for spot in spots
        if spot.spotType == requiredSpotType and spot.id not in occupiedSpotIds
            return spot

    return null

mapVehicleTypeToSpotType(vehicleType)
    if vehicleType == MOTORCYCLE
        return MOTORCYCLE
    if vehicleType == CAR
        return CAR
    if vehicleType == LARGE
        return LARGE
    return error
```

```
computeFee(entryTime, exitTime)
    durationMillis = exitTime - entryTime
    durationHours = durationMillis / (1000 * 60 * 60)

    // Round up to nearest hour (5 minutes becomes 1 hour)
    if durationMillis % (1000 * 60 * 60) > 0
        durationHours++

    return durationHours * hourlyRateCents
```

