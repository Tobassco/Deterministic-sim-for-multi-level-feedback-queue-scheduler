#define _POSIX_C_SOURCE 200809L
#include "parse.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// parse -f, -n, -q, -b arguments in any order 
int parse_args(int argc, char *argv[], Args *out){
    out->filename = NULL;
    out -> num_levels = 0;
    out -> quantums = NULL;
    out -> boost_interval = 0;
    
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-f") == 0 && i + 1 < argc){
            out -> filename = argv[++i];

        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc){
            out->num_levels = (int)strtol(argv[++i], NULL, 10);
        
        } else if(strcmp(argv[i], "-q") == 0 && i + 1 < argc){
            i++;
            // count comma seperated values 
            int n = 1;
            for (const char *c = argv[i]; *c; c++){
                if(*c == ','){
                    n++;
                }
            }
            out -> quantums = malloc(n * sizeof(uint32_t));
            char *copy = strdup(argv[i]);
            char *tok = strtok(copy, ",");
            int j = 0;
            while (tok){
                out -> quantums[j++] = (uint32_t)strtoul(tok, NULL, 10);
                tok = strtok(NULL, ",");

            }
            free(copy);

        } else if (strcmp(argv[i], "-b") == 0 && i + 1 < argc){
            out->boost_interval= (uint32_t)strtoul(argv[++i], NULL, 10);

        }

    }
    return (out->filename && out->num_levels > 0 && out ->quantums) ? 1 : 0;

}

// load all process from input file and free process and array 
Process **load_processes(const char *filename, int *count_out){
    FILE *f = fopen(filename, "r");
    if (!f){
        perror("fopen");
        return NULL;
    }
    int cap = 16;
    int n = 0;
    Process **procs = malloc(cap * sizeof(Process *));

    char line[4096];
    while(fgets(line, sizeof(line), f)){
        line[strcspn(line,"\n")] = '\0';
        if (line[0] == '\0'){
            continue;
        }
        char *p = line;

        // arrival time 
        uint32_t arrival = (uint32_t)strtoul(p, &p, 10);
        while(*p == ' '){
            p++;
        }

        // PID
        char pid[9];
        int j = 0;
        while(*p && *p != ' ' && j < 8){
            pid[j++] = *p++;
        }
        pid[j] = '\0';


        //bursts
        int burst_cap = 8;
        int burst_count = 0;
        uint32_t *bursts = malloc(burst_cap * sizeof(uint32_t));
        while(*p){
            while(*p == ' '){
                p++;
            }
            if(*p == '\0'){
                break;
            }
            if(burst_count >= burst_cap){
                burst_cap *= 2;
                bursts = realloc(bursts, burst_cap * sizeof(uint32_t));

            }
            bursts[burst_count++] = (uint32_t)strtoul(p, &p, 10);

        }

        if (n >= cap){
            cap *= 2;
            procs = realloc(procs, cap * sizeof(Process *));
        }

        procs[n++] = process_create(pid, arrival, bursts, burst_count);
        free(bursts);

    }

    fclose(f);
    *count_out = n;
    return procs;

}


