# SMT solvers

## April 2024

### Description

The time is 23:55. The project submission is approaching and your colleague
still hasn't finished implementing a feature that the rest of the program
depends on. You frantically refresh GitHub in your browser and finally the
`foo.c` file with the `f` function appears in the repository. At first glance,
it's not clear what the function does, and your colleague doesn't communicate
with you. Your situation is complicated by the fact that for optimal
functionality of your program, you need to call the f function with as few
inputs as possible so that it returns true. You have no choice but to use the
last few minutes until deadline to analyze the function using the SMT solver.

### Specification

Consider the following code in a C-like language. For this language, we assume
that it works with integers of arbitrary precision (so there is no need to deal
with situations like overflow).

```c
bool f(int A, int B, int C, int D, int E) {
  if (D <= 0 || E <= 0) {
    return false;
  }

  int x, y, z;

  x = A*B*2;

  if (x < E) {
    y = x + 5*B;
  } else {
    y = x - C;
  }

  if (y + 2 < D) {
    z = x*A - y*B;
  } else {
    z = x*B + y*A;
  }

  if (z < E+D) {
    return true;
  } else {
    return false;
  }
}
```

Your goal is to create a formula (in SMT format) that is satisfiable for the
given values of parameters A, B and C (they represent one instance of the
problem) if:

- there are values of the parameters `D` and `E` such that the function `f`
  returns `true`
- the sum of `D + E` is the smallest possible
