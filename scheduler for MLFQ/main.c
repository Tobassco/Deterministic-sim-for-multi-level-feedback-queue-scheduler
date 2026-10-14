#include "parse.h"
#include "scheduler.h"
#include <stdlib.h>


int main(int argc, char *argv[]) {
    Args args;
    if (!parse_args(argc, argv, &args)){
        return 1;
    }
    int num_procs = 0;
    Process **procs = load_processes(args.filename, &num_procs);
    if(!procs){
        return 1;
    }

    // empty input file to have no output
    if(num_procs > 0){
        run_simulation(procs, num_procs, args.num_levels, 
        args.quantums, args.boost_interval);
    }

    for(int i = 0; i < num_procs; i++){
        process_free(procs[i]);
    }

    free(procs);
    free(args.quantums);

    return 0;
}
