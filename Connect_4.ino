#include <stdio.h>
#include <stdlib.h> 
#include <math.h> 

int check_move(char** , int);
int horizontal_connect(char** , int, int, char, int*, int);
int vertical_connect(char** , int, int, char, int*, int);
int diagonal_connect(char** , int, int, char, int*, int);
int displayboard(char**, int*);

int connect_sum_ai(char** , char , char, int, int, int*, int*, int*, int*);
int horizontal_count(char** , int , int , char );
int vertical_count(char** , int , int, char);
int diagonal_count(char** , int , int , char);

char player_one_colour = 'a'; 
char player_two_colour = 'b'; 
int turn = 1;     // Player 1's turn - irrelevent in actual competition against robot
int inputcolumn, row, temp, result;
int player_1_score = 0; 
int player_2_score = 0;
int AI_on = 1;
int checkFF = 0;

int row_horizontal_connects[6] = {0,0,0,0,0,0};
int column_vertical_connects[7] = {0,0,0,0,0,0,0};
int diagonal_line_connects[12] = {-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10};    
// 1st 6 are up left, last 6 are up right. -10s chosen as arbitrary number that won't be mistaken for a row - column check in diagonal function

int* full_flag = (int*)calloc(8,sizeof(int));
char** array = (char**)calloc(6, sizeof(char*));
    
void setup(){   
    for (int i = 0; i < 8; i++)
    {    full_flag[i] = 0;}

    for (int i = 0; i < 6; i++)
    {    array[i] = (char*)calloc(7, sizeof(char));}

    for (int i = 0; i < 6; i++)
    {    for (int j = 0; j < 7; j++)
        {     array[i][j] = ' ';}
    }

    /* When the bot scans the game board it'll ideally be able to overwrite the arary (stored game board) as that is the more accurate one */ 
     
    displayboard(array, full_flag);    
}

// Uses a recursive DF decision game tree algorithm to assess and evaluate best weighted options from "terminal" leaves at a ...
//  ... pre-determined level down (for time consideration)
// Note: Assumes opponent makes the best decisions possibly at each turn
void loop(){
    while(full_flag[7] != 1)             
    {
        LOOP:

        checkFF = 0;            // Checks to see if the board is full
        for (int i = 0; i < 7; i++)
        {    
            if(full_flag[i] > 0)
            {    checkFF++;}
        }
        if (checkFF == 7)
        {
            full_flag[7] = 1;
            goto END;
        }
        
        if ((AI_on == 0) || (turn == 0))    // P1's turn, or P2 if AI off 
        {
            printf("Player %d's turn\nDrop the ball in which column? ", turn + 1); 
            scanf("%d", &inputcolumn);             
 
            if ((inputcolumn >= 6) && (inputcolumn <= 0)) 
            {    
                printf("%d is not a valid column\n", inputcolumn);
                goto LOOP;
            }
        }

        else 
        {    
            int moves_ahead = 3;
            printf("Player %d's turn\n", turn + 1);
            inputcolumn = connect_sum_ai(array, player_two_colour, player_one_colour, 0, moves_ahead, full_flag, row_horizontal_connects, column_vertical_connects, diagonal_line_connects);
            printf("\nI DECIDED TO GO HERE: %d\n", inputcolumn);
        }

       printf("Full flag state: ");
        for (int m = 0; m < 8; m++)
        {    printf("%d ", full_flag[m]);}
        printf("\n");
 
        result = check_move(array, inputcolumn);
 
         
        // Handles the case in which the move is invalid 
        if (result == -1) 
        {
            printf("Column is full. Try another move\n");
            full_flag[inputcolumn] = 1;
            goto LOOP;
        } 

        // Deals with points obtained from the move that just occured
        else
        {
            row = result; 
            if (turn == 0) 
            {        
                array[row][inputcolumn] = player_one_colour;
                if (row == 5) full_flag[inputcolumn] = 1;        
                temp = 4 * (horizontal_connect(array, inputcolumn, row, player_one_colour, row_horizontal_connects, -1) + vertical_connect(array, inputcolumn, row, player_one_colour, column_vertical_connects, -1) + diagonal_connect(array, inputcolumn, row, player_one_colour, diagonal_line_connects, -1));
                player_1_score += temp + 1;
        
                if (temp > 0)
                {    player_2_score -= 2 * (temp/4);}
            }
 
            else  
            {    array[row][inputcolumn] = player_two_colour;
                temp = 4 * (horizontal_connect(array, inputcolumn, row, player_two_colour, row_horizontal_connects, -1) + vertical_connect(array, inputcolumn, row, player_two_colour, column_vertical_connects, -1) + diagonal_connect(array, inputcolumn, row, player_two_colour, diagonal_line_connects, -1)); 
                player_2_score += temp + 1;
        
                if (temp > 0)
                {    player_1_score -= 2 * (temp/4);}
            } 
                 
        } 

        // To account for the point(s) case if only 1 point
        if (player_1_score != 1) printf("Player 1 has %d points\n", player_1_score);
        else printf("Player 1 has %d point\n", player_1_score);
        if (player_2_score != 1) printf("Player 2 has %d points\n", player_2_score);    
        else printf("Player 2 has %d point\n", player_2_score); 
        
        // Checking to see if the entire game board is full
        displayboard(array, full_flag);
        if (full_flag[7] > 0)    goto END;     
        turn = turn ^ 1;     
    } 
 
    END: 
    printf("The game board is full. The game is over.\n"); 
 
    if (player_1_score > player_2_score) 
    {    printf("Player 1 wins %d - %d\n", player_1_score, player_2_score);} 
     
    else if (player_1_score == player_2_score) 
    {    printf("Both players tied with %d points\n", player_1_score);} 
 
    else 
    {    printf("Player 2 wins %d - %d\n", player_2_score, player_1_score);} 

    for (int i = 0; i < 6; i++)
    {    free(array[i]);}
    free(array);
    free(full_flag);
 
    return;
} 
 
int check_move(char** array, int inputcolumn) 
{ 
    int row = 0; 
    for (row = 0; row < 6; row++)
    {
        if (array[row][inputcolumn] == ' ')
        {    break;}
    }

    if (array[5][inputcolumn] != ' ')
    {    return -1;} 
    return row; 
} 

 
// In the following three cases, moves_ahead acts to control print statements (only final iteration), and as a flag for the row/col/line connects arrays 
int horizontal_connect(char** array, int inputcolumn, int row, char colour, int* row_horizontal_connects, int moves_ahead)
{
    int count = 0;
//    if (row_horizontal_connects[row] == 1)
//    {    return 0;}

    for (int i = inputcolumn; i < 7; i++)
    {    
        if (array[row][i] != colour)
        {    break;}

        count++;
    }

    for (int i = inputcolumn - 1; i >= 0; i--)
    {    
        if (array[row][i] != colour)
        {    break;}

        count++;
    }

    if ((count == 4) && (moves_ahead == -1))
    {    row_horizontal_connects[row] = 1;}


    // To see if there was already a connect in that row
    if (row_horizontal_connects[row] == 0)
    {    return (count >= 4);}
    return (count == 4);
} 


int vertical_connect(char** array, int inputcolumn, int row, char colour, int* column_vertical_connects, int moves_ahead)
{
    int count = 0;

    for (int i = row; i < 6; i++)
    {    
        if (array[i][inputcolumn] != colour)
        {    break;}

        count++;
    }

    for (int i = row - 1; i >= 0; i--)
    {    
        if (array[i][inputcolumn] != colour)
        {    break;}

        count++;
    }

    if ((count == 4) && (moves_ahead == -1))
    {    column_vertical_connects[inputcolumn] = 1;}

    // To see if there was already a connect in that row
    if (column_vertical_connects[inputcolumn] == 0)
    {    return (count >= 4);}
    return (count == 4);
}


int diagonal_connect(char** array, int inputcolumn, int row, char colour, int* diagonal_line_connects, int moves_ahead)
{
    int count = 0;
    int count1 = 0;
    int count2 = 0;
    int check1 = 0;
    int check2 = 0;

    for (int i = row, j = inputcolumn; (i < 6)&&(j < 7); i++, j++)
    {    
        if (array[i][j] != colour)
        {    break;}

        count1++;
    }

    for (int i = row, j = inputcolumn; (i < 6)&&(j >= 0); i++, j--)
    {    
        if (array[i][j] != colour)
        {    break;}

        count2++;
    }

    for (int i = row - 1, j = inputcolumn + 1; (i >= 0)&&(j < 7); i--, j++)
    {    
        if (array[i][j] != colour)
        {    break;}

        count2++;
    }

    for (int i = row - 1, j = inputcolumn - 1; (i >= 0)&&(j >= 0); i--, j--)
    {    
        if (array[i][j] != colour)
        {    break;}

        count1++;
    }

    // For count1, the difference of row - column is the same for all numbers in an up-right diagonal
    // For count2, the sum of row + column is the same for all numbers in an up-left diagonal

    if (count1 >= 4)
    {    
        for (int i = 0; i < 6; i++)
        {    
            if (diagonal_line_connects[i] == (row - inputcolumn))
            {    break;}

            if (diagonal_line_connects[i] == -10)
            {    if (moves_ahead == -1)
                {    diagonal_line_connects[i] = (row - inputcolumn);}
                check1 = (count1 >= 4);
                break;}
        }
    }

    if (count2 >= 4)
    {
        for (int i = 6; i < 12; i++)
        {    
            if (diagonal_line_connects[i] == (row + inputcolumn))
            {    break;}

            if (diagonal_line_connects[i] == -10)
            {    if (moves_ahead == -1)
                {    diagonal_line_connects[i] = (row + inputcolumn);}
                check2 = (count2 >= 4);
                break;}
        }
    }

    count = check1 + check2;
    return count;
} 
 
 
int displayboard(char** array, int* full_flag) 
{
    int count = 0; 
    for (int i = 5; i >= 0; i--) 
    {
        printf("%d ", i); 
        for (int j = 0; j < 7; j++) 
        { 
            printf("| %c ", array[i][j]);
            if ((i == 5) && ((array[i][j] == 'b') || (array[i][j] == 'w')))
            {    full_flag[j] = 1;
                count++;} 
        } 
        printf("|\n"); 
    } 
    printf("    0   1   2   3   4   5   6\n\n");

    if (count == 7)
    {    full_flag[7] = 1;
        return 1;}
    return 0; 
}


int connect_sum_ai(char** array, char AI_colour, char opponent_colour, int moves_ahead, int total_moves, int* full_flag, int* row_horizontal_connects, int* column_vertical_connects, int* diagonal_line_connects)
{
    int points[7] = {0,0,0,0,0,0,0};
    int max = -20; 
    int max_col = 0;
    char colour;
    int factor = total_moves - moves_ahead + 1;

    if (moves_ahead % 2 == 0) colour = AI_colour;
    else colour = opponent_colour;

    for (int col = 0; col < 7; col++)
    {
        int row = 0;
        if (array[5][col] != ' ') continue;

        for (row = 0; row < 6; row++)
        {
            if (array[row][col] == ' ')
            {    break;}            
        }

        array[row][col] = colour;
        points[col] += factor*horizontal_connect(array, col, row, colour, row_horizontal_connects, moves_ahead);
        points[col] += factor*vertical_connect(array, col, row, colour, column_vertical_connects, moves_ahead);
        points[col] += factor*diagonal_connect(array, col, row, colour, diagonal_line_connects, moves_ahead);

        if (moves_ahead < total_moves)            
        {    points[col] -= connect_sum_ai(array, AI_colour, opponent_colour, moves_ahead + 1, total_moves, full_flag, row_horizontal_connects, column_vertical_connects, diagonal_line_connects);}
        
        array[row][col] = ' ';
    }


    if (moves_ahead == 0) 
    {    printf("\npoints are: ");
        for (int i = 0; i < 7; i++) {printf("%d ", points[i]);}
    }

    
    for (int col = 0; col < 7; col++)
    {
        if (full_flag[col] != 0)
        {    continue;}

        else            
        {
            if (points[col] > max)
            {    max = points[col];
                max_col = col;}

            else if (points[col] == max)        // In the case of a tie, check for which column is closest to the center and put it there
            {    
                if ((3-col)*(3-col) < (3-max_col)*(3-max_col))    // Check distance to middle column - smaller distance gets priority
                {    max_col = col;}
                else if ((3-col)*(3-col) == (3-max_col)*(3-max_col))
                {    if (col > 3) max_col = col;}
            }
        }
    }    

    if (moves_ahead > 0) 
    {    return max;}
    else 
    {    if (max > 0) printf("max points: %d col: %d\n", max, max_col);
        return max_col;
    }
}
