#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

// Globals
#define HEIGHT 6
#define WIDTH 7
int moves[7] = {4,3,5,2,6,1,7};

uint64_t boardMask;
uint64_t colMask;
uint64_t fin;


struct BitBoard {
    uint64_t player1;
    uint64_t player2;
    uint64_t board;
    int playerTurn;
};

// Forward decs
void bboard_initMask();
void bboard_init(struct BitBoard*, char*);
void bboard_resetBoard(struct BitBoard*, uint64_t, uint64_t, int);
void bboard_reinitFromString(struct BitBoard*, char*);
void bboard_computeBoard(struct BitBoard*);
void bboard_print(struct BitBoard*);
int bboard_done(struct BitBoard*);
int bboard_canPlay(struct BitBoard*, int);
int bboard_play(struct BitBoard*, int);
int bboard_checkForWin(struct BitBoard*);

uint64_t bboard_find_winning_pos(struct BitBoard*, int);
int* bboard_get_moves(struct BitBoard*);
int* bboard_get_moves_scored(struct BitBoard*);
int bboard_movescore(struct BitBoard*, int);
int popcount(uint64_t);
void bboard_sort_moves(int*, int*);


// Actual code lul
void bboard_initMask() {
    // boardMask
    for(int i = 0; i < HEIGHT; i++) {
        boardMask = boardMask << 3;
        
        for(int j = 0; j < WIDTH - 3; j++) {
            boardMask = boardMask << 1;
            boardMask++;
        }
    }

    // colMask
    for(int i = 0; i < HEIGHT; i++) {
        colMask = colMask << WIDTH;
        colMask = colMask + 1;
    }

    fin = ((uint64_t) 1 << (HEIGHT * WIDTH)) - 1;
}

void bboard_init(struct BitBoard* board, char* gameString) {
    board->player1 = 0;
    board->player2 = 0;
    board->board = 0;
    board->playerTurn = 0;

    int i = 0;
    while(gameString[i] != '\0') {
        bboard_play(board, gameString[i] - 48);
        i++;
    }
}

void bboard_resetBoard(struct BitBoard* board, uint64_t p1, uint64_t p2, int t) {
    board->player1 = p1;
    board->player2 = p2;
    board->playerTurn = t;
    board->board = p1 | p2;
    //bboard_computeBoard(board);
}

void bboard_reinitFromString(struct BitBoard* board, char* string) {
    board->player1 = 0;
    board->player2 = 0;
    board->playerTurn = 0;
    bboard_computeBoard(board);

    int i = 0;
    while(string[i] != '\0') {
        bboard_play(board, string[i] - 48);
        i++;
    }
}

void bboard_computeBoard(struct BitBoard* board) {
    board->board = board->player1 | board->player2;
}

void bboard_print(struct BitBoard* board) {
    char s[WIDTH * 2] = "";
    uint64_t p1;
    uint64_t p2;
    uint64_t mask;

    for(int i = HEIGHT - 1; i >= 0; i--) {
        for(int j = 0; j < WIDTH; j++) {
            mask = (uint64_t) 1 << i*WIDTH + j;
            p1 = (board->player1 & mask);
            p2 = (board->player2 & mask);

            if(p1) {
                s[2*j] = 'x';
                s[2*j + 1] = ' ';
            }
            else if(p2) {
                s[2*j] = 'o';
                s[2*j + 1] = ' ';
            }
            else {
                s[2*j] = '.';
                s[2*j + 1] = ' ';
            }
        }
        printf("%s\n", s);
    }
    printf("\n");
}

int bboard_done(struct BitBoard* board) {
    //uint64_t fin = ((uint64_t) 1 << (mult)) - 1;
    if(board->board == fin) return 1;
    else return 0;
}

int bboard_canPlay(struct BitBoard* board, int col) {
    uint64_t mask = (uint64_t) 1 << ((HEIGHT-1) * WIDTH + col - 1);
    if(board->board & mask) return 0;
    else return 1;
}

int bboard_play(struct BitBoard* board, int col) {
    uint64_t mask = 1 << (col - 1);
    uint64_t p1 = board->player1;
    uint64_t p2 = board->player2;
    uint64_t b = board->board;
    int t = board->playerTurn;

    for(int i = 0; i < HEIGHT; i++) {
        if(!(b & mask)) {
            // Player 2's turn
            if(t){
                p2 = p2 | mask;
                board->player2 = p2;
            }
            // Player 1's turn
            else {
                p1 = p1 | mask;
                board->player1 = p1;
            }
            
            board->playerTurn = 1 ^ t;
            board->board = p1 | p2;
            return 1;
        }
        else {
            mask = mask << WIDTH;
        }
    }
    return 0;
}

int bboard_checkForWin(struct BitBoard* board) {
    uint64_t pieces;
    uint64_t check;

    if(board->playerTurn) pieces = board->player1;
    else pieces = board->player2;

    // Horizontal
    check = pieces & (pieces >> 1);
    if(check & (check >> 2) & boardMask) return 1;

    // Vertical
    check = pieces & (pieces >> WIDTH);
    if(check & check >> (2*WIDTH)) return 1;

    // Bottom-To-Top Diagonal
    check = pieces & (pieces >> (WIDTH+1));
    if(check & check >> (2*(WIDTH+1)) & boardMask) return 1;

    // Top-To-Bottom Diagonal
    check = pieces & (pieces << (WIDTH-1));
    if(check & (check << (2*(WIDTH-1)) & boardMask)) return 1;

    return 0;
}


uint64_t bboard_find_winning_pos(struct BitBoard* board, int player) {
    // Find positions that could be played now or in the future where the opponent wins
    uint64_t pieces;
    uint64_t pos;
    uint64_t tmp;
    
    if(player) pieces = board->player2;
    else pieces = board->player1;

    // Vertical
    pos = (pieces << WIDTH) & (pieces << (WIDTH*2)) & (pieces << (WIDTH*3));

    // Horizontal
    tmp = pieces & (pieces >> 1);
    pos |= (tmp >> 1) & (pieces >> 3) & boardMask;
    pos |= ((pieces & tmp >> 2) & boardMask) << 1;
    pos |= ((tmp & pieces >> 3) & boardMask) << 2;
    pos |= ((tmp & pieces >> 2) & boardMask) << 3;

    // Bottom-To-Top Diagonal
    tmp = pieces & (pieces >> (WIDTH+1));
    pos |= (tmp >> (WIDTH+1)) & (pieces >> 3*(WIDTH+1)) & boardMask;
    pos |= ((tmp >> (WIDTH+2)) & (pieces << WIDTH) & boardMask) << 1;
    pos |= ((tmp << 2*WIDTH) & (pieces >> (WIDTH+3)) & boardMask) << 2;
    pos |= ((tmp << 2*WIDTH-1) & (pieces << 3*WIDTH) & boardMask) << 3;

    // Top-To-Bottom Diagonal
    tmp = pieces & (pieces >> (WIDTH-1));
    pos |= (tmp << 2*(WIDTH-1)) & (pieces << 3* (WIDTH-1)) & boardMask;
    pos |= ((tmp << 2*WIDTH-3) & (pieces >> WIDTH) & boardMask) << 1;
    pos |= ((tmp >> WIDTH+1) & (pieces << WIDTH-3) & boardMask) << 2;
    pos |= ((tmp >> WIDTH+2) & (pieces >> 3*WIDTH) & boardMask) << 3;

    return pos;
}

int* bboard_get_moves(struct BitBoard* board) {
    int* arr = (int*) calloc(WIDTH+1, sizeof(int));
    // Positions that have been played
    uint64_t playedPos = board->board;
    // Positions where opponent could play and win
    uint64_t oppWinPos = bboard_find_winning_pos(board, 1 ^ board->playerTurn);
    // Positions that can be played
    uint64_t canPlay = (playedPos ^ ((playedPos << WIDTH) | (1 << WIDTH) - 1)) & fin;
    // Positions we should not play or the opponent can win
    uint64_t dontPlay = (oppWinPos >> WIDTH) & canPlay;
    
    oppWinPos = oppWinPos & canPlay;
    if(oppWinPos != 0) {
        int count = 0;

        for(int i = 0; i < WIDTH; i++) {
            if(!(oppWinPos & colMask << i)) continue;

            count++;
            if(count > 1 || dontPlay & colMask << i) {
                arr[0] = 0;
                return arr;
            }
            else {
                arr[0] = i+1;
            }
        }
    }
    else {
        int move, j = 0;

        for(int i = 0; i < WIDTH; i++) {
            move = moves[i];
            if(!(canPlay & colMask << (move-1))) continue;
            if(dontPlay & colMask << (move-1)) continue;

            arr[j] = move;
            j++;
        }
    }

    return arr;
}

int* bboard_get_moves_scored(struct BitBoard* board) {
    int* arr = (int*) calloc(WIDTH+1, sizeof(int));
    int* scores;
    // Positions that have been played
    uint64_t playedPos = board->board;
    // Positions where opponent could play and win
    uint64_t oppWinPos = bboard_find_winning_pos(board, 1 ^ board->playerTurn);
    // Positions that can be played
    uint64_t canPlay = (playedPos ^ ((playedPos << WIDTH) | (1 << WIDTH) - 1)) & fin;
    // Positions we should not play or the opponent can win
    uint64_t dontPlay = (oppWinPos >> WIDTH) & canPlay;
    
    oppWinPos = oppWinPos & canPlay;
    if(oppWinPos != 0) {
        int count = 0;

        for(int i = 0; i < WIDTH; i++) {
            if(!(oppWinPos & colMask << i)) continue;

            count++;
            if(count > 1 || dontPlay & colMask << i) {
                arr[0] = 0;
                return arr;
            }
            else {
                arr[0] = i+1;
            }
        }

        return arr;
    }
    else {
        int move, j = 0;
        scores = (int*) calloc(WIDTH+1, sizeof(int));

        for(int i = 0; i < WIDTH; i++) {
            move = moves[i];
            if(!(canPlay & colMask << (move-1))) continue;
            if(dontPlay & colMask << (move-1)) continue;

            arr[j] = move;
            scores[j] = bboard_movescore(board, move);
            j++;
        }

        bboard_sort_moves(scores, arr);
        free(scores);
        return arr;
    }
}


int bboard_movescore(struct BitBoard* board, int move) {
    uint64_t p1tmp = board->player1;
    uint64_t p2tmp = board->player2;
    int turntmp = board->playerTurn;
    uint64_t canPlay, winPos;
    int score;

    bboard_play(board, move);
    canPlay = (board->board ^ ((board->board << WIDTH) | (1 << WIDTH) - 1)) & fin;
    winPos = bboard_find_winning_pos(board, turntmp);
    winPos = winPos & canPlay;

    score = popcount(winPos);
    bboard_resetBoard(board, p1tmp, p2tmp, turntmp);

    if(score == 0) return -1;
    else return score;
}

int popcount(uint64_t x) {
    int count;
    for (count=0; x; count++)
        x &= x - 1;
    return count;
}

void bboard_sort_moves(int* scores, int* pos) {
    int i = 1;
    int tmp;
    while(scores[i] != 0) {
        for(int j = i; j > 0 && scores[j] > scores[j-1]; j--) {
            // swap
            tmp = scores[j];
            scores[j] = scores[j-1];
            scores[j-1] = tmp;

            tmp = pos[j];
            pos[j] = pos[j-1];
            pos[j-1] = tmp;
        }
    
        i++;
    }
}
