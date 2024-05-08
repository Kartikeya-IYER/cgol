#include "cgol.h"

// Convert a row and column address into a toroidal bit position.
// This translates a grid reference into a bit position in the Game of Life
// "bit array" which contains the viewport grid.
uint32_t row_col_to_bitpos_toroidal(int16_t row, int16_t col,
        uint8_t num_rows, uint8_t num_cols) {

    uint32_t bitpos = (uint32_t)0;

    // Bit position calculation, as determined painfully from first principles :-}
    //
    // Logic: We are basically flattening a 2-D (num_rows x num_cols)
    // array into a 1-D array, and we are compressing the array down
    // to the same number of bits as the number of cells (num_rows x num_cols).
    // Since we are storing the array in row-major format, this means that
    // each row will have num_cols cells (bits) in it.
    // So to go from Arr[row][col] to Arr[cell_bit], we basically have to
    // use a segment+offset addressing scheme (remember x86 real mode?)
    // where the segment is the zero-based row index multiplied by the total
    // number of columns (the stride), and the offset within that segment is
    // the zero-based column index for the cell in which we're interested.
    int16_t toroidal_row = row;
    int16_t toroidal_col = col;
        
    if(row != 0L || col != 0L) {
        // "To infinity and beyond!"
        if(toroidal_row < 0) toroidal_row = num_rows+row;
        if(toroidal_col < 0) toroidal_col = num_cols+col;
        if(toroidal_row >= num_rows) toroidal_row %= num_rows;
        if(toroidal_col >= num_cols) toroidal_col %= num_cols;
        
        bitpos = (uint32_t)((toroidal_row * num_cols) /*Segment*/
                + toroidal_col /*Offset*/);
    }   

// Enable the block below to see more debug info. Can get intrusive.
#if 0
    DEBUG_PRINTF("row=%"PRId16" col=%"PRId16", "
            "num_rows=%"PRIu8" num_cols=%"PRIu8", "
            "toroidal_row=%"PRId16" toroidal_col=%"PRId16","
            "bitpos=%"PRIu32"\n",
            row, col,
            num_rows, num_cols,
            toroidal_row, toroidal_col,
            bitpos);
#endif
    return bitpos;
}

// Print the 64-bit number which represents the toroidal game grid as an actual
// grid composed of n rows of "cols" columns each.
// We only need the number of columns to determine when to move to the next row,
// since we are in row-major format.
void print_grid(uint64_t num, uint8_t cols) {
    // Bit mask with 1 in the MSB position
    uint64_t mask = 1ULL << (CGOL_WORD_BITS-1);
    uint8_t ctr = 0;

    while(mask) {
        if(num & mask) {
            // Live cell
            printf(" X "); 
        } else {
            // blargh i am ded
            printf(" . "); 
        }

        // Right-shift the mask, simulating going across a row from left to right.
        mask >>= 1;

        // See if we need to move to the next row.
        ctr++;
        if(ctr >= cols) {
            ctr = 0;
            printf("\n");
        }
    }
}

// Main entry point into the Game of Life.
void start_life(uint8_t rows,
        uint8_t cols,
        Starting_Pattern pattern_val,
        uint64_t generations,
        const char *user_arg) {

    DEBUG_PRINTF("starting pattern=%d word length=%"PRIu64", "
            "rows=%"PRIu8" cols=%"PRIu8", "
            "cell_count=%"PRIu16"\n",
            pattern_val, CGOL_WORD_BITS,
            rows, cols,
            (rows*cols));

    // The Bit Array representing the toroidal grid on which life plays out.
    // we have these as separate variables instead of as array members because
    // the individual variable is register size on a 64-bit machine.
    // Assuming, of course, that the compiler is kind enough to stick it in one :-P
    register uint64_t life_grid_one = 0;
    register uint64_t life_grid_two = 0;

    // Do some tests if compiled with -DDEBUG
#ifdef DEBUG
    perform_life_TESTS(rows, cols, life_grid_one, life_grid_two);
#endif

    // Initialize the primary grid variable for the given starting pattern.
    switch(pattern_val) {
        case RANDOM:
            srand((unsigned int)time(NULL));
            life_grid_one = rand();
            break;
        case BEACON:
            life_grid_one = BEACON_8x8;
            break;
        case BLINKER:
            life_grid_one = BLINKER_8x8;
            break;
        case TOAD:
            life_grid_one = TOAD_8x8;
            break;
        default:
            // oh god how did this get here i am not good with computer
            fprintf(stderr, "Unknown pattern encountered, this should not have happened\n");
            return;
    }

    printf("%s\n", user_arg);

    // Cell variables for game logic
    bool curr_cell_alive=false;
    bool cell_alive_nw=false, cell_alive_n=false, cell_alive_ne=false;
    bool cell_alive_e=false, cell_alive_w=false;
    bool cell_alive_sw=false, cell_alive_s=false, cell_alive_se=false;
    uint8_t neighbor_count = 0;

    // GAME LOGIC
    for(uint64_t gen = 0ULL; gen < generations; gen++) {
        print_grid(life_grid_one, cols);
        printf("\n\n");

        life_grid_two = 0ULL;

        // Iterate through our "bit array"
        for(int8_t r = 0; r < rows; r++) {
            for(int8_t c = 0; c < cols; c++) {
                curr_cell_alive = (TEST_BIT_ULL(life_grid_one, row_col_to_bitpos_toroidal(r, c, rows, cols)) ? true : false);

                // Get the bit positions of the neighbors
                cell_alive_nw = (TEST_BIT_ULL(life_grid_one, row_col_to_bitpos_toroidal(r-1, c-1, rows, cols)) ? true : false);
                cell_alive_n  = (TEST_BIT_ULL(life_grid_one, row_col_to_bitpos_toroidal(r-1, c,   rows, cols)) ? true : false);
                cell_alive_ne = (TEST_BIT_ULL(life_grid_one, row_col_to_bitpos_toroidal(r-1, c+1, rows, cols)) ? true : false);
                cell_alive_e  = (TEST_BIT_ULL(life_grid_one, row_col_to_bitpos_toroidal(r,   c-1, rows, cols)) ? true : false);
                cell_alive_w  = (TEST_BIT_ULL(life_grid_one, row_col_to_bitpos_toroidal(r,   c+1, rows, cols)) ? true : false);
                cell_alive_sw = (TEST_BIT_ULL(life_grid_one, row_col_to_bitpos_toroidal(r+1, c-1, rows, cols)) ? true : false);
                cell_alive_s  = (TEST_BIT_ULL(life_grid_one, row_col_to_bitpos_toroidal(r+1, c,   rows, cols)) ? true : false);
                cell_alive_se = (TEST_BIT_ULL(life_grid_one, row_col_to_bitpos_toroidal(r+1, c+1, rows, cols)) ? true : false);

                neighbor_count = cell_alive_nw + cell_alive_n + cell_alive_ne
                               + cell_alive_e  + cell_alive_w
                               + cell_alive_sw + cell_alive_s + cell_alive_se;

                // "We're no strangers to life; you know the rules, and so do I..." ;-)
                if(curr_cell_alive) {
                    // Rule 1: Any live cell with fewer than two live neighbors dies,
                    //         as if by underpopulation.
                    //
                    // Rule 3: Any live cell with more than three live neighbors dies,
                    //         as if by overpopulation.
                    //
                    // Nothing to do to life_grid_two for Rule 1 and Rule 3 because
                    // we already initialized it to 0 at the start of this generation.
                    //
                    // Rule 2: Any live cell with two or three live neighbors lives on
                    //         to the next generation.
                    if(neighbor_count == 2 || neighbor_count == 3) {
                        SET_BIT_ULL(life_grid_two, row_col_to_bitpos_toroidal(r, c, rows, cols));
                    }
                } else {
                    // Current cell is dead.
                    // Rule 4: Any dead cell with exactly three live neighbors becomes
                    //         a live cell, as if by reproduction.
                    if(neighbor_count == 3) {
                        SET_BIT_ULL(life_grid_two, row_col_to_bitpos_toroidal(r, c, rows, cols));
                    }
                } // if-else
            } // for column
        } // for row
        
        // At this point, we have completely set our game status in life_grid_two.
        // So we replace life_grid_one with life_grid_two so we can print the
        // game status at the start of the next generation processing.
        life_grid_one = life_grid_two;

    } // for gen
}
