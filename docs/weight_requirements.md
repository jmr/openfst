# FST Weight Requirements

A *semiring* is specified by two binary operations $\oplus$ and $\otimes$
and two designated elements $0$ and $1$ with the following properties:

*   $\oplus$: associative, commutative, and has $0$ as its identity.
*   $\otimes$: associative and has identity $1$, distributes w.r.t.
    $\oplus$, and has $0$ as an annihilator: $0 \otimes a = a \otimes 0 =
    0$.

A left semiring distributes on the left; a right semiring is similarly defined.

A `Weight` class must have binary functions `Plus` and `Times` and static member
functions `Zero()` and `One()` and these must form (at least) a left or right
semiring.

In addition, the following must be defined for a `Weight`:

*   `Member`: predicate on set membership.
*   `NoWeight`: static member function that returns an element that is not a set
    member; used to signal an error.
*   `>>`: reads textual representation of a weight.
*   `<<`: prints textual representation of a weight.
*   `Read(istream &)`: reads binary representation of a weight.
*   `Write(ostream &)`: writes binary representation of a weight.
*   `Hash`: maps weight to `size_t`.
*   `ApproxEqual`: approximate equality (for inexact weights)
*   `Quantize`: quantizes wrt delta (for inexact weights)
*   `Divide`: $\forall a,b,c$ s.t.
    *   *Left semiring*: $\text{Times}(a, b) = c \Rightarrow b' =
        \text{Divide}(c, a, \mathtt{DIVIDE\_LEFT})$, $b'.\text{Member}()$
    *   *Right semiring*: $\text{Times}(a, b') = c \Rightarrow a' =
        \text{Divide}(c, b, \mathtt{DIVIDE\_RIGHT})$, $a'.\text{Member}()$
    *   *Commutative semiring*: $\text{Times}(a', b) = c \Rightarrow b' =
        \text{Divide}(c, a) = \text{Divide}(c, a, \mathtt{DIVIDE\_ANY}) =
        \text{Divide}(c, a, \mathtt{DIVIDE\_LEFT}) = \text{Divide}(c, a,
        \mathtt{DIVIDE\_RIGHT})$, $\text{Times}(a,b') = \text{Times}(b', a) =
        c$.
*   `ReverseWeight`: the type of the corresponding reverse weight. Typically the
    same type as `Weight` for a (both left and right) semiring. For the left
    string semiring, it is the right string semiring.
*   `Reverse`: a mapping from `Weight` to `ReverseWeight` s.t.
    $\text{Reverse}(\text{Reverse}(a)) = a$, $\text{Reverse}(\text{Plus}(a,
    b)) = \text{Plus}(\text{Reverse}(a), \text{Reverse}(b))$,
    $\text{Reverse}(\text{Times}(a, b)) = \text{Times}(\text{Reverse}(b),
    \text{Reverse}(a))$. Typically the identity mapping in a (both left and
    right) semiring. In the left string semiring, it maps to the reverse string
    in the right string semiring.
*   `Properties`: specifies properties that hold:
    *   `LeftSemiring`: indicates weights form a left semiring
    *   `RightSemiring`: indicates weights form a right semiring
    *   `Commutative`: $\forall a,b: \text{Times}(a, b) = \text{Times}(b, a)$
    *   `Idempotent`: $\forall a: a \oplus a = a$.
    *   `Path`: $\forall a, b: a \oplus b = a$ or $a \oplus b = b.$
