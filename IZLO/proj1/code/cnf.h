#ifndef __CNF_H
#define __CNF_H

#include <stdbool.h>
#include <assert.h>

typedef struct Literal Literal;

typedef struct Clause Clause;

typedef struct CNF CNF;

// Struktura reprezentujici ulici z křižovatky crossroad_from do křižovatky crossroad_to
typedef struct {
    unsigned crossroad_from;
    unsigned crossroad_to;
} Street;

// Vytvoreni nove klauzule v danej formuli
Clause* create_new_clause(CNF *formula);
// Prida (pripadne negovanou) promennou do klauzule reprezentujici, že v kroku step je zvolena ulice z křižovatky crossroad_from do křižovatky crossroad_to
void add_literal_to_clause(Clause *clause, bool is_positive, unsigned step, unsigned crossroad_from, unsigned crossroad_to);

void at_least_one_valid_street_for_each_step(CNF* formula, unsigned num_of_crossroads, unsigned num_of_streets, const Street* streets);
void at_most_one_street_for_each_step(CNF* formula, unsigned num_of_crossroads, unsigned num_of_streets);
void streets_connected(CNF* formula, unsigned num_of_crossroads, unsigned num_of_streets);
void streets_do_not_repeat(CNF* formula, unsigned num_of_crossroads, unsigned num_of_streets);

#endif