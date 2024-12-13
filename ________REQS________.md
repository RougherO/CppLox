# Variables
- All resources will be tied to scopes.
- Scopes have strong ownership of the resource. Ownership can only be transferred between scopes.
- All variables own a weak reference to the resources. Weak reference are immutable borrows of the resources
- For any mutation variables will have to "up" their ownership temporarily.
- "up"ping of ownership can only be done in a scope only if that scope owns a strong reference to the resource

```
let x = 1;
{
    [x] // transfer a weak ownership to this scope
    [x]
}
```

- Scopes need to have two capture lists
    - Entry capture and exit captures
    - Entry captures will contain all variables that are captured from outer scope(strong or weak)
    - Exit captures will contain all Entry capture variables that the scope has strong ownership off(i.e. they haven't been transferred to any inner scope) or any from the current scope

{
    let x = List { data : 2, next: None, prev: None} ;
    {
        let y = x.next;
        let z = x.prev;
        let n = List { data: 3, next: y, prev: }
    }
}