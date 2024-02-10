/*
 * read.c
 * Work with file
 * Theme: Hamiltonian path and cycle in graph 
 * Implementation: Oleksii Fedorchenko, Dmitrii Ivanushkin, Zlata Valakhanovich, December 2022
 */
#include "../lib/graph.h"

// Handle errors with print and exit
void handleErrs (int code, char *string) {
    fprintf(stderr, string);
    exit(code);
}

// Main function made for reading file and passing vertices and edges to other functions
void readFile (int argc, char *argv[]) {
    FILE *f;

    if (argc < 2) // Check if text file exists 
        handleErrs(1, "Enter at least one text file\n");

    int i = 1; 
    while (i < argc)
    {
        int V = 0;
        char string[STR_MAX];

        int f_i = 0;
        int s_i = 0;

        int cnt = 0;
        
        f = fopen(argv[i], "r");
        // Count vertices in graph
        while (fgets(string, STR_MAX, f)) 
            V++;

        // Back to the start of the file
        rewind(f);

        // Create an object
        TGraph graph; 
        TPath path;
        initGraph(&graph, V);

        if (f == NULL)
            handleErrs(2, "There is no such file\n");
        
        // Read matrix by strings
        while (fscanf(f, "%s", &string[0]) == 1)
        {
            f_i = cnt;
            int len = strlen(string);

            for (int e_cnt = 0; e_cnt < len; e_cnt++)
            {
                if (((e_cnt % 2) == 0) && ((string[e_cnt] - 48) == 1))
                {
                    s_i = e_cnt/2;
                    insertConn(&graph, f_i, s_i);
                }
            }
            cnt++;
        }
        printGraph(&graph, V, i);
        cycle(&graph, &path, V);
        fclose(f);
        
        freeGraph(&graph, V);
        freePath(&path);
        i++;  
    }
}