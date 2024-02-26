#include <stddef.h>
#include "cnf.h"

//
// LOGIN: xivanu00
//

// Tato funkce by mela do formule pridat klauzule predstavujici podminku 1)
// Křižovatky jsou reprezentovany cisly 0, 1, ..., num_of_crossroads-1
// Cislo num_of_streets predstavuje pocet ulic a proto i pocet kroku cesty
// Pole streets ma velikost num_of_streets a obsahuje vsechny existujuci ulice
//    - pro 0 <= i < num_of_streets predstavuje streets[i] jednu existujici
//      ulici od krizovatky streets[i].crossroad_from ke krizovatce streets[i].crossroad_to
void at_least_one_valid_street_for_each_step(CNF* formula, unsigned num_of_crossroads, unsigned num_of_streets, const Street* streets) {
    assert(formula != NULL);
    assert(num_of_crossroads > 0);
    assert(num_of_streets > 0);
    assert(streets != NULL);

    // x_{i, streets[j].crossroad_from, streets[j].crossroad_to} pro všechny i, j
    for (unsigned i = 0; i < num_of_streets; ++i) {
        Clause* cl = create_new_clause(formula);
        for (unsigned j = 0; j < num_of_streets; ++j) {
            add_literal_to_clause(cl, true, i, streets[j].crossroad_from, streets[j].crossroad_to);
        }
    }
}

// Tato funkce by mela do formule pridat klauzule predstavujici podminku 2)
// Křižovatky jsou reprezentovany cisly 0, 1, ..., num_of_crossroads-1
// Cislo num_of_streets predstavuje pocet ulic a proto i pocet kroku cesty
void at_most_one_street_for_each_step(CNF* formula, unsigned num_of_crossroads, unsigned num_of_streets) {
    assert(formula != NULL);
    assert(num_of_crossroads > 0);
    assert(num_of_streets > 0);

    // ~x_{i, z, k} V ~x_{i, z_, k_} pro všechny i, z, k, z_, k_ takové, že z != z_ nebo k != k_
    for (unsigned i = 0; i < num_of_streets; ++i) {
        for (unsigned z = 0; z <= num_of_crossroads - 1; ++z) {
            for (unsigned k = 0; k <= num_of_crossroads - 1; ++k) {
                for (unsigned z_ = 0; z_ <= num_of_crossroads - 1; ++z_) {
                    for (unsigned k_ = 0; k_ <= num_of_crossroads - 1; ++k_) {
                        if (z != z_ || k != k_) {
                            Clause* cl = create_new_clause(formula);
                            add_literal_to_clause(cl, false, i, z, k);
                            add_literal_to_clause(cl, false, i, z_, k_);
                        }
                    }
                }
            }
        }
    }
}

// Tato funkce by mela do formule pridat klauzule predstavujici podminku 3)
// Křižovatky jsou reprezentovany cisly 0, 1, ..., num_of_crossroads-1
// Cislo num_of_streets predstavuje pocet ulic a proto i pocet kroku cesty
void streets_connected(CNF* formula, unsigned num_of_crossroads, unsigned num_of_streets) {
    assert(formula != NULL);
    assert(num_of_crossroads > 0);
    assert(num_of_streets > 0);

    // ~x_{i, z, k} V x_{i+1, k, any_cross} pro všechny i, z, k, any_cross takové, že k != any_cross
    for (unsigned i = 0; i <= num_of_streets; ++i) {
        for (unsigned z = 0; z <= num_of_crossroads - 1; ++z) {
            for (unsigned k = 0; k <= num_of_crossroads - 1; ++k) {
                Clause* cl = create_new_clause(formula);
                add_literal_to_clause(cl, false, i, z, k);
                for (unsigned any_cross = 0; any_cross < num_of_crossroads; ++any_cross) {
                    add_literal_to_clause(cl, true, i + 1, k, any_cross);
                }
            }
        }
    }
}

// Tato funkce by mela do formule pridat klauzule predstavujici podminku 4)
// Křižovatky jsou reprezentovany cisly 0, 1, ..., num_of_crossroads-1
// Cislo num_of_streets predstavuje pocet ulic a proto i pocet kroku cesty
void streets_do_not_repeat(CNF* formula, unsigned num_of_crossroads, unsigned num_of_streets) {
    assert(formula != NULL);
    assert(num_of_crossroads > 0);
    assert(num_of_streets > 0);
    
    for (unsigned i = 0; i < num_of_streets; ++i) {
        // pro kazdy krok i
        for (unsigned j = 0; j < num_of_streets; ++j) {
            if (i != j) {
                // pro kazdy jiny krok j
                for (unsigned z = 0; z < num_of_crossroads; ++z) {
                    for (unsigned k = 0; k < num_of_crossroads; ++k) {
                        // pro kazdu dvojici krizovatek (z, k)
                        Clause* cl = create_new_clause(formula);
                        add_literal_to_clause(cl, false, i, z, k);
                        add_literal_to_clause(cl, false, j, z, k);
                    }
                }
            }
        }
    }
}
