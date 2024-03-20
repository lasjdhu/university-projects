#include <stdlib.h>
#include <stdio.h>

#include "cnf.h"

struct Literal {
    int var;
    struct Literal *next_literal;
};

struct Clause {
    Literal* first_literal;
    Literal* last_literal;

    struct Clause* next_clause;

    unsigned num_of_crossroads;
};

struct CNF {
    Clause* first_clause;
    Clause* last_clause;

    unsigned num_of_clauses;
    unsigned num_of_crossroads;
    unsigned num_of_streets;
};

void add_clause_to_formula(Clause *clause, CNF *formula) {
    assert(clause != NULL);
    assert(formula != NULL);

    if (formula->last_clause == NULL) {
        assert(formula->first_clause == NULL);
        formula->first_clause = clause;
    } else {
        formula->last_clause->next_clause = clause;
    }
    formula->last_clause = clause;
    clause->num_of_crossroads = formula->num_of_crossroads;

    ++formula->num_of_clauses;
}

Clause* create_new_clause(CNF* formula) {
    Clause *new_clause = malloc(sizeof(Clause));
    new_clause->first_literal = NULL;
    new_clause->last_literal = NULL;
    new_clause->next_clause = NULL;
    add_clause_to_formula(new_clause, formula);
    return new_clause;
}

void add_literal_to_clause(Clause *clause, bool is_positive, unsigned step, unsigned crossroad_from, unsigned crossroad_to) {
    assert(clause != NULL);

    Literal *new_literal = malloc(sizeof(Literal));

    unsigned num_of_crossroads = clause->num_of_crossroads;
    int lit_num = step*num_of_crossroads*num_of_crossroads + crossroad_from*num_of_crossroads + crossroad_to + 1;

    if (!is_positive) {
        lit_num = -lit_num;
    }
    new_literal->var = lit_num;
    new_literal->next_literal = NULL;

    if (clause->last_literal == NULL) {
        assert(clause->first_literal == NULL);
        clause->first_literal = new_literal;
    } else {
        clause->last_literal->next_literal = new_literal;
    }
    clause->last_literal = new_literal;
}


unsigned get_num_of_variables(CNF* formula) {
    assert(formula != NULL);
    return formula->num_of_crossroads * formula->num_of_crossroads * formula->num_of_streets;
}

unsigned get_num_of_clauses(CNF* formula) {
    assert(formula != NULL);
    return formula->num_of_clauses;
}

void clear_clause(Clause* cl) {
    assert(cl != NULL);
    while(cl->first_literal != NULL) {
        Literal *cur_lit = cl->first_literal;
        cl->first_literal = cl->first_literal->next_literal;
        free(cur_lit);
    }
    cl->last_literal = NULL;
}

void clear_cnf(CNF* formula) {
    assert(formula != NULL);
    while(formula->first_clause != NULL) {
        Clause *this_cl = formula->first_clause;
        formula->first_clause = formula->first_clause->next_clause;
        clear_clause(this_cl);
        free(this_cl);
    }
    formula->last_clause = NULL;
    formula->num_of_clauses = 0;
}

void error(char* error_msg) {
    fprintf(stderr, "%s\n", error_msg);
    exit(-1);
}

void print_formula(CNF* formula) {
    assert(formula != NULL);
    
    printf("p cnf %u %u\n", get_num_of_variables(formula), get_num_of_clauses(formula));
    Clause *next_cl = formula->first_clause;
    while (next_cl != 0) {
        Literal *next_lit = next_cl->first_literal;
        while (next_lit != 0) {
            printf("%d ", next_lit->var);
            next_lit = next_lit->next_literal;
        }
        next_cl = next_cl->next_clause;
        printf("0\n");
    }
}

int main (int argc, char** argv) {
    if (argc != 2) {
        error("Program ocekava presne jeden argument: soubor obsahujici seznam ulic");
    }

    FILE *input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        error("Vstupni soubor nelze otevrit");
    }

    printf("c Vstupni soubor:\n");
    
    unsigned num_of_crossroads, num_of_streets;
    if (fscanf(input_file, "%u %u", &num_of_crossroads, &num_of_streets) != 2) {
        fclose(input_file);
        error("Hlavicka souboru by mela obsahovat dve cisla: pocet krizovatek a pocet ulic v souboru");
    }
    printf("c %u %u\n", num_of_crossroads, num_of_streets);
    if (num_of_crossroads == 0 || num_of_streets == 0) {
        fclose(input_file);
        error("Je treba stanovit kladny pocet krizovatek a pocet ulic");
    }

    Street* streets = malloc(num_of_streets * sizeof(Street));

    for (unsigned i = 0; i < num_of_streets; ++i) {
        Street e;
        if (fscanf(input_file, "%u %u", &e.crossroad_from, &e.crossroad_to) != 2) {
            free(streets);
            fclose(input_file);
            error("Ulice je dana dvema cisly - krizovatkou, ze ktere vychazi a do ktere vede");
        }
        printf("c %u %u\n", e.crossroad_from, e.crossroad_to);
        if (e.crossroad_from >= num_of_crossroads || e.crossroad_to >= num_of_crossroads) {
            free(streets);
            fclose(input_file);
            error("Krizovatka pro nejakou ulici ma vyssi hodnotu nez maximalni povolena hodnota");
        }
        streets[i] = e;
    }
    fclose(input_file);

    printf("c\nc Mapovani promennych na krok a ulici:\n");
    for (unsigned var = 1; var <= num_of_crossroads * num_of_crossroads * num_of_streets; ++var) {
        unsigned step = (var-1) / (num_of_crossroads * num_of_crossroads);
        unsigned rest = (var-1) % (num_of_crossroads * num_of_crossroads);
        printf("c %u - v kroku %u se pouzije ulice od krizovatky %u ke krizovatce %u\n", var, step, rest / num_of_crossroads, rest % num_of_crossroads);
    }

    printf("c\nc\n");

    CNF f = { .first_clause = NULL, .last_clause = NULL, .num_of_clauses = 0, .num_of_crossroads = num_of_crossroads, .num_of_streets = num_of_streets };
    at_least_one_valid_street_for_each_step(&f, num_of_crossroads, num_of_streets, streets);
    free(streets);
    at_most_one_street_for_each_step(&f, num_of_crossroads, num_of_streets);
    streets_connected(&f, num_of_crossroads, num_of_streets);
    streets_do_not_repeat(&f, num_of_crossroads, num_of_streets);

    printf("c Formula:\n");
    print_formula(&f);

    clear_cnf(&f);

    return 0;
} 
