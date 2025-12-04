#include <shapes.h>

//drawTwitchLogo() - creates an 8x8 Twitch logo fixed to the LED matrix.
//Inputs -
    //mx - an MD_MAX72XX object properly initialized
    //start_col - the starting column of the top-left LED
    //start_row - the starting row of the top-left LED
//Output - none; updates the matrix to display shape
void drawTwitchLogo(MD_MAX72XX& mx, int start_col, int start_row)
{
    static char positions[8][8] =
    {
        {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'},
        {'X', 'X', ' ', ' ', ' ', ' ', ' ', 'X'},
        {'X', 'X', ' ', 'X', ' ', 'X', ' ', 'X'},
        {'X', 'X', ' ', 'X', ' ', 'X', ' ', 'X'},
        {'X', 'X', ' ', ' ', ' ', ' ', ' ', 'X'},
        {'X', 'X', ' ', ' ', ' ', ' ', ' ', 'X'},
        {' ', ' ', 'X', ' ', 'X', 'X', 'X', 'X'},
        {' ', ' ', 'X', 'X', ' ', ' ', ' ', ' '}
    };

    for(int row = 0; row < 8; row++)
    {
        for(int col = 0; col < 8; col++)
        {
            if(positions[row][col] == 'X' || positions[row][col] == 'x')
            {
                mx.setPoint(row + start_col, 7 - col + start_row, true);
            }
            else
            {
                mx.setPoint(row + start_col, 7 - col + start_row, false);
            }
        }
    }
}