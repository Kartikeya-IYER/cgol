// Compilation
//   debug mode:
//     make debug
//   release mode:
//     make

#include "cgol.h"

// TODO: Allow specifying start row and column for pattern.
// TODO: Allow user to input starting pattern cell by cell.
// TODO: Allow specifying common starting patterns.

// NOTE: The viewport grid size must be a multiple of CGOL_WORD_BITS because
// we will be using a bitfield array to represent the grid.
// This saves storage space (AKA memory).

////////////////////// Helper functions //////////////////////
void usage(char *myname) {
    fprintf(stderr, "Usage: %s [STARTING_PATTERN]\n", myname);
    fprintf(stderr, "Conway's Game of Life using a toroidal viewport.\n");
    fprintf(stderr, "The default viewport size is %"PRIu8" rows x %"PRIu8" columns (text).\n",
            GRID_MIN_ROWS, GRID_MIN_COLS);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  STARTING_PATTERN:\n");
    fprintf(stderr, "    OPTIONAL\n");
    fprintf(stderr, "    The starting pattern for the Game of Life grid.\n");
    fprintf(stderr, "    Valid options:\n");
    fprintf(stderr, "      random\n");
    fprintf(stderr, "         A random starting pattern across the viewport grid.\n");
    fprintf(stderr, "      beacon\n");
    fprintf(stderr, "         The 'beacon' standard pattern (but a mirror image of the\n");
    fprintf(stderr, "         'beacon' pattern shown on the Wikipedia page).\n");
    fprintf(stderr, "      blinker\n");
    fprintf(stderr, "         The 'blinker' standard pattern.\n");
    fprintf(stderr, "      toad\n");
    fprintf(stderr, "         The 'toad' standard pattern.\n");
    fprintf(stderr, "      \n");
    fprintf(stderr, "    Default: random\n");

    fflush(stderr);
}

// Can be extended to accept viewport rows, viewport cols, and generations
// from command line args, although we might need something like getopt at
// that point.
bool process_commandline_args(int nargs, char *args[],
        char *out_errmsg, size_t errmsg_maxlen,
        uint8_t *out_rows, uint8_t *out_cols,
        Starting_Pattern *out_pattern,
        uint64_t *out_generations) {

    char patternstr[STR_MAXLEN];
    bool convcase = false;


    // Set defaults for out params.
    // Better paranoid than sorry :-)
    snprintf(out_errmsg, errmsg_maxlen, "%s", "");
    *out_rows = (uint8_t)GRID_MIN_ROWS;
    *out_cols = (uint8_t)GRID_MIN_COLS;
    *out_pattern = RANDOM;
    *out_generations = DEFAULT_GENERATIONS;

    if(nargs == 1) {
        DEBUG_PRINTF("No command line args given, setting defaults: "
                "Viewport rows=%"PRIu8" cols=%"PRIu8", "
                "starting pattern=%s, "
                "generations=%"PRIu64"\n",
                *out_rows, *out_cols, STR(RANDOM), *out_generations);
        return true;
    }

    // If we got here, we have at least one arg.
    // Process it as the pattern.
    convcase = upper_lower_str(TO_UPPER, args[1], patternstr, STR_MAXLEN);
    if(!convcase) {
        snprintf(out_errmsg, errmsg_maxlen,
                "Failed case conversion of input argument");
        return false;
    }
    DEBUG_PRINTF("process_commandline_args: pattern='%s'\n", patternstr);

    // Now come some ugly if-elif blocks because we can't switch strings in C
    if( strncmp(patternstr, XSTR(RANDOM), STR_MAXLEN) == 0 ) {
        *out_pattern = RANDOM;
    } else if( strncmp(patternstr, XSTR(BEACON), STR_MAXLEN) == 0 ) {
        *out_pattern = BEACON;
    } else if( strncmp(patternstr, XSTR(BLINKER), STR_MAXLEN) == 0 ) {
        *out_pattern = BLINKER;
    } else if( strncmp(patternstr, XSTR(TOAD), STR_MAXLEN) == 0 ) {
        *out_pattern = TOAD;
    } else {
        snprintf(out_errmsg, errmsg_maxlen,
                "'%s' is not a valid starting pattern", args[1]);
        return false;
    }
    
    // If we got here, everything checks out
    DEBUG_PRINTF("process_commandline_args: out_rows=%"PRIu8"\n", *out_rows);
    DEBUG_PRINTF("process_commandline_args: out_cols=%"PRIu8"\n", *out_cols);
    DEBUG_PRINTF("process_commandline_args: generations=%"PRIu64"\n", *out_generations);
    DEBUG_PRINTF("process_commandline_args: pattern=%d\n", *out_pattern);
    return true;
}

////////////////////// main ////////////////////// 
int main(int argc, char *argv[]) {
    char* myname = argv[0];
    char a_string[STR_MAXLEN];
    uint8_t rows=GRID_MIN_ROWS, cols=GRID_MIN_COLS;
    Starting_Pattern pattern_type=RANDOM;
    uint64_t generations = DEFAULT_GENERATIONS;

    if(! process_commandline_args(argc, argv,
                a_string, STR_MAXLEN,
                &rows, &cols, &pattern_type, &generations)) {
        usage(myname);
        if(strnlen(a_string, STR_MAXLEN) > 0) {
            fprintf(stderr, "ERROR: %s\n", a_string);
        }
        fflush(stderr);
        return 1;
    }

    // Basic check to see if our data has the correct width
    uint16_t num_cells = rows * cols;
    if(num_cells > CGOL_WORD_BITS) {
        fprintf(stderr, "ERROR: rows x cols = %"PRIu16" which exceeds %"PRIu64", CANNOT CONTINUE,\n",
                num_cells, CGOL_WORD_BITS);
        return 1;
    }

    // If we have a pattern string arg, print it exactly as we received it.
    // If we've gotten here, we've verified that the argument is valid, so
    // let's not argue about it (heh heh).
    char *user_arg = "RANDOM";
    if(argc > 1) {
        user_arg = argv[1];
    }

    // Initialize the Game of Life and run it until the user is satisfied.
    start_life(rows, cols, pattern_type, generations, user_arg);

    return 0;
}
