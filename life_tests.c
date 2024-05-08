#ifdef DEBUG

#include "cgol.h"

// Tests and demo/tryout code for the functions in life.c
// The tests in this file do nothing when built without -DDEBUG

// Test the conversion of a row and col address to a toroidal bit position.
// TODO: extend to use asserts and to fail the build if a test fails. For now,
// we only print out the result when the project is compiled with the DEBUG define.
static void bitpos_conversion_TEST(uint8_t rows, uint8_t cols) {
    // Remember, we are zer0-indexed.

    // row=0, col=0 should give us the very first cell (index 0)
    DEBUG_PRINTF("TEST row_col_to_bitpos_toroidal() (expect 0): %"PRIu32"\n\n",
            row_col_to_bitpos_toroidal(0, 0, rows, cols));

    // The very last cell (zero-indexed)
    DEBUG_PRINTF("TEST row_col_to_bitpos_toroidal() (expect %"PRIu32"): %"PRIu32"\n\n",
            (uint32_t)((rows*cols)-1), row_col_to_bitpos_toroidal(rows-1, cols-1, rows, cols));

    // Toroid, going "before" the first row and column, should give us the last cell
    DEBUG_PRINTF("TEST row_col_to_bitpos_toroidal(): (expect %"PRIu32"): %"PRIu32"\n\n",
            (uint32_t)((rows*cols)-1), row_col_to_bitpos_toroidal(-1, -1, rows, cols));

    // First row, second column = second cell
    DEBUG_PRINTF("TEST row_col_to_bitpos_toroidal() (expect 1): %"PRIu32"\n\n",
            row_col_to_bitpos_toroidal(0, 1, rows, cols));

    // Toroid, going "after" the last row and column (i.e last cell) = back to the first cell.
    DEBUG_PRINTF("TEST row_col_to_bitpos_toroidal() (expect 0): %"PRIu32"\n\n",
            row_col_to_bitpos_toroidal(rows, cols, rows, cols));

    // TODO: Maybe Add some more cases
}

// Converts an array representing the game grid - laid out with a 0 for a dead
// cell or any positive non-zero number for a live cell, into a single 64-bit
// number. This needs CGOL_WORD_BITS to be set appropriately.
static bool convert_grid_shape_into_number_TEST(uint8_t array[8][8],
        uint8_t rows, uint8_t cols,
        uint64_t *out_number) {
    *out_number = 0ULL;

    if( (rows*cols) > CGOL_WORD_BITS) {
        return false;
    }

    uint32_t bitpos = 0;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(array[i][j] > 0) {
                bitpos = row_col_to_bitpos_toroidal(i, j, rows, cols);
                DEBUG_PRINTF("    Setting i=%d j=%d, bitpos=%"PRIu32" to 1\n", i, j, bitpos);
                SET_BIT_ULL(*out_number, bitpos);
            }
        }
    }

    return true;
}


// Main entry point into doing the debug tests.
// Compiled out entirely if we have not been compiled with -DDEBUG
void perform_life_TESTS(uint8_t rows, uint8_t cols, uint64_t lg1, uint64_t lg2) {
    DEBUG_PRINTF("%s", "***** Entered perform_life_TESTS *****\n");

    if( (rows * cols) > CGOL_WORD_BITS ) {
        DEBUG_PRINTF("rows*cols %"PRIu32" is greater than CGOL_WORD_BITS (%"PRIu64")\n",
                (rows*cols), CGOL_WORD_BITS);
        return;
    }

    DEBUG_PRINTF("Size of life_grid_one: %zu bytes (%zu bits)\n",
            (sizeof(lg1)), (sizeof(lg1) * CHAR_BIT));
    DEBUG_PRINTF("Size of life_grid_two: %zu bytes (%zu bits)\n",
            (sizeof(lg2)), (sizeof(lg2) * CHAR_BIT));

    DEBUG_PRINTF("Rows=%"PRIu8", Cols=%"PRIu8"\n", rows, cols);


    DEBUG_PRINTF("%s", "Performing array row/col to bit position tests\n...");
    bitpos_conversion_TEST(rows, cols);

    // Convert a starting pattern grid into a single number.
    // Beacon shape, laid out with LSB starting at the bottom-right of
    // the original pattern, and moving right-to-left and bottom-to-top.
    // So it is flipped vertically as well as horizontally when compared
    // to the original pattern. Without this flipping, it doesn't come
    // out correctly, because - as humans - when we start counting the
    // grid from the top-left position, we are actually starting with the MSB .
    uint8_t beacon_8x8_array[8][8] = {
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,1,1,0,0,0},
        {0,0,0,1,1,0,0,0},
        {0,1,1,0,0,0,0,0},
        {0,1,1,0,0,0,0,0},
        {0,0,0,0,0,0,0,0}
    };

    DEBUG_PRINTF("%s", "Converting beacon 8x8 grid array into number...\n");
    uint64_t beacon_number = 0;
    bool bnconv = convert_grid_shape_into_number_TEST(beacon_8x8_array,
            8/*rows*/, 8/*cols*/, &beacon_number);
    if(bnconv) {
        DEBUG_PRINTF("Beacon array converted to a number is: %"PRIu64"\n", beacon_number);
        print_grid(beacon_number, cols);
        if(beacon_number == BEACON_8x8) {
            DEBUG_PRINTF("This matches BEACON_8x8 (%"PRIu64") :-)\n", ((uint64_t)(BEACON_8x8)));
            DEBUG_PRINTF("%s", "Printing Beacon starting grid...\n");
            print_grid(beacon_number, cols);
        } else {
            DEBUG_PRINTF("ERROR CONVERTING: %"PRIu64" DOES NOT match BEACON_8x8 (%"PRIu64") :-(\n",
                    beacon_number, BEACON_8x8);
        }
    } else {
        DEBUG_PRINTF("%s", "ERROR: FAILED converting beacon 8x8 grid array into number :-(\n");
    }

    // TODO: Other tests

    DEBUG_PRINTF("%s", "***** EXIT perform_life_TESTS *****\n");
}
#endif
