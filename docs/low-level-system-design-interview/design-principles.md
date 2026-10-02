## Design Principles

Design principles guid your decision in making to create clean, extensible and maintainable code.
There are two types of design principles
- General Software Design Principles
- Object-Oriented Design Principles


## General Software Design Principles

### KISS - Keep it Simple, Stupid

The simplest solution that works is usually the simplest one. If you can solve the problem with a simple conditional instead of a strategy pattern, do that. If a single class handles the job without getting messy, don't split it up.

The time to add complexity is when simplicity stops working. If adding a new payment method means modifying code in five places, that's when you introduce a strategy pattern. But start simple.

### DRY - Don't Repeat Yourself

When you find yourself writing the same logic in multiple places, pull it into one place. If three classes all validate email addresses the same way, create a shared validation method. If two services both need to convert timestamps, put that conversion in a utility function.

Sometimes the simplest solution is to duplicate code in two places rather than build an abstraction. There's no right answer

### YAGNI - You Aren't Gonna Need it

Build what you need not what you need later. The problem with building for future requirements is you usually guess wrong.


### Separation of Concerns

Different parts of your code should handle different responsibilities, and they shouldn't know about each other's internals. 

Your UI layer shouldn't contain business logic. 
Your business logic shouldn't know how data is stored. 
Your data access layer shouldn't format strings for display.

## Object-Oriented System Design ( SOLID )

### Single Responsibility Principle
A class should have one reason to change. If a class mixes multiple concerns, split them. This is the foundation of good class design.

### Open / Closed Principle

Classes should open for extension but closed for modification.
- Should be able to add new behaviour without changing the existing code
- This usually means using interfaces or abstract classes so you can add new implementations without touching the actual codes

### Liskov Substitution Principle

Subclasses must work wherever the base class works. The subclass can't violate the expectations set by the parent class.

Said differently if your code uses a parent class or interface, it should be able to use any subclass without knowing which specific class it is. 
The subclass can add new behavior, but it can't remove or break behavior that the parent promised.

##### Breaks LSP
```
class Bird:
    def fly(self) -> None:
        # flying logic
        pass


class Penguin(Bird):
    def fly(self) -> None:
        raise NotImplementedError("Penguins can't fly")
```

#### Follows LSP
```
from abc import ABC, abstractmethod


class Bird(ABC):
    @abstractmethod
    def eat(self) -> None:
        ...


class FlyingBird(Bird):
    @abstractmethod
    def fly(self) -> None:
        ...


class Sparrow(FlyingBird):
    def eat(self) -> None:
        pass

    def fly(self) -> None:
        pass


class Penguin(Bird):
    def eat(self) -> None:
        pass
```

### Interface Segregation Principle

Prefer small, focused interfaces over large, general-purpose ones. Don't force classes to implement methods they don't need. Make interface small

### Dependency Inversion Principle

Dependency Inversion states that your code should depends on abstractions, not concrete implementations. Instead of `NotificationService` creating an `EmailSender` directly, it should accept a `MessageSender` interface through its constructor.

