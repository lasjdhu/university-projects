# SAT solvers

## February 2024

### Description

Your Uncle Pat has asked you to help him solve his problem. As a postman, he gets up early in the morning and sets out with his cat to deliver the mail. When he was young and full of energy, he didn't worry too much about planning his route. Now, just before retirement, he called you one day with the following request:

"Listen, you understand these computers, could you help me somehow plan my route so that I can drive down every street in town to deliver mail just once and not have to keep going back and forth?"

With the knowledge gained during your studies, you decided to solve the problem by converting it to a satisfiability problem in propositional logic using the SAT solver.

### Specification

A problem instance consists of a number `K`, `K > 0`, denoting the number of intersections, and a list of streets `S` of length `U`, `U > 0`. The numbers `0, 1, ..., K - 1` represent the intersections. A street is represented by a pair of numbers `(z, k)`, where `z` is its beginning and `k` is its end (thus `0 <= z < K` and `0 <= k < K`). The given list of streets is therefore of the form `S = (z_0, k_0), ..., (z_(U-1), k_(U-1))`. For simplicity, a street is always traversable in only one direction, so it is identified by its starting and ending intersection; furthermore, there is at most one street in one direction between two intersections.
The solution to the problem is a path that passes through each given street exactly once (it can start anywhere).

Your task is to create a program that generates a formula for an instance of this problem whose models are just the solutions of that instance. You already have a skeleton (below) that takes care of processing the input and generating the formula in DIMACS format.

Within the skeleton, you already have the coding of the problem into propositional variables ready. For each triple of numbers i, z, k
where `0 <= i < U`, `0 <= z < K`, and `0 <= k < K`, we have a variable `x_(i,z,k)` with the following semantics:

`x_(i,z,k) = 1 <=>` in the `i`-th step of the path, a street starting at the intersection `z` and ending at the intersection `k` is chosen

Your task is to complete the code of the three functions in the file `code/add_conditions.c` that take care of generating the following conditions:

1. At least one existing street is selected at each step of the route. The generation of this formula needs to be added to the `at_least_one_valid_street_for_each_step(...)` function.

2. At most one street is selected in each step of the path. The generation of this formula needs to be added to the function `at_most_one_street_for_each_step(...)`.

3. Streets within a path are connected to each other. More specifically, if a street `(z, k)` is the `i`-th step of a path where `0 <= i < U -1`, then the street in the step `i + 1` starts at `k`. Add the generation of this formula to the `streets_connected(...)` function.

4. No street is visited more than once. The generation of this condition is already implemented in the `streets_do_not_repeat(...)` function as an example.

### Usage

The problem instance is described in the following text format. On the first line of the file there is a header containing exactly two natural numbers indicating, in turn, the total number of intersections and the total number of streets. This is followed by a list of streets, where each line contains one street represented as a pair of numbers (within the range defined by the header):

```
<pocet_krizovatek> <pocet_ulic>

<zacatek_ulice_1> <konec_ulice_1>
<zacatek_ulice_2> <konec_ulice_2>
...
```

A specific example of an input is the following file:

```
4 4

0 1
1 2
2 1
2 3
```

### Generating formulas

Since the DIMACS format works with variables indexed by natural numbers, the variable `x_(i,z,k)` is converted to a variable with index `n`, where `n = i * K^2 + z * K + k + 1`. However, you will not work with these variables directly, but with the functions described in the following section. However, the way the variables are represented may be useful if you want to review the generated DIMACS file (there are comments in the generated file on lines beginning with the symbol `c`, where there is a copy of the input problem and a description of the mapping of variable numbers to steps and streets).

All formulas from the problem need to be generated in conjunctive normal form (CNF), which is the standard input to SAT solvers. The necessary structures are already created in the project skeleton to represent the formulas. The following functions are available to manipulate these structures:

- `Clause *create_new_clause(CNF *f)` - Creates a new empty (disjunctive) clause in the formula `f`. **_CAUTION_**: An empty clause is equivalent to 0 (`false`).

- `void add_literal_to_clause(Clause *c, bool is_positive, unsigned i, unsigned k, unsigned z)` - Inserts a literal `x_(i,z,k)` into clause `c` (if `is_positive = true`), or `NOT x_(i,z,k)` (if `is_positive = false`).
