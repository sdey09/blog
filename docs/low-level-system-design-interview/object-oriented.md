## OOP Concepts
Where the design principles page taught you how to think about clean code, OOP concepts are the mechanisms your language gives you to actually implement those ideas.

### Encapsulation

Encapsulation means keeping an object's data private and letting the object control how that data is used.

### Abstraction

Abstraction means exposing only what's essential and hiding implementation details behind clear interfaces. You define what something can do without revealing how it does it.

The benefit is simplification. An abstraction hides complexity. When your payment processing code depends on a `PaymentMethod` interface instead of concrete classes like `CreditCardProcessor` or `PayPalProcessor`, you can swap implementations without touching the code that uses them. 

The caller doesn't need to know whether you're hitting Stripe's API or storing payment tokens in a database. It just calls process() and gets a result.

### Polymorphism

Polymorphism naturally follows from abstraction. Once you define an interface each can provide its own behaviour

### Inheritance
Inheritance lets one class be a more specific version of another, automatically getting the parent's data and behavior. 
It's a tool for sharing implementation, but it comes with a big cost: tight coupling.