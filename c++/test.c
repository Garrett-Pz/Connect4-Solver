#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "BitBoard.c"
#include "table.c"


int minmaxx(struct BitBoard* board, struct Entry** T, int alpha, int beta) {
    uint64_t p1tmp = board->player1;
    uint64_t p2tmp = board->player2;
    int turntmp = board->playerTurn;
    struct Entry* entry = NULL;
    uint64_t p1, p2;
    int best, result, i, hash;

    int* move_arr = bboard_get_moves_scored(board);

    if(board->playerTurn == 0) {
        best = -1;
        for(int j = 0; j < WIDTH; j++) {
            if(bboard_done(board)) return 0;

            // FIND MOVES AND CREATE NEW BOARD
            i = move_arr[j];
            if(i == 0) break;

            bboard_play(board, i);
            p1 = board->player1;
            p2 = board->player2;

            // RECURSE TO FIND SOLUTION TO NEW BOARD.
            // UPDATE TABLE
            hash = table_hash(p1, p2);
            if(table_contains(T, p1, p2, hash)) {
                entry = T[hash];
            }

            if(entry != NULL && entry->bound == 0) {
                result = entry->result;
            }
            else if(entry != NULL && entry->bound == -1 && entry->result >= beta) {
                result = entry->result;
            }
            else if(entry != NULL && entry->bound == 1 && entry->result <= alpha) {
                result = entry->result;
            }
            else {
                result = minmaxx(board, T, alpha, beta);

                if(result <= alpha) {
                    table_put(T, hash, p1, p2, result, 1);
                }
                else if(result >= beta) {
                    table_put(T, hash, p1, p2, result, -1);
                }
                else {
                    table_put(T, hash, p1, p2, result, 0);
                }
            }

            // ALPHA-BETA PRUNING FROM RESULT
            if(result > best) best = result;
            if(result > alpha) alpha = result;
            if(alpha >= beta) break;

            bboard_resetBoard(board, p1tmp, p2tmp, turntmp);
            entry = NULL;
        }
        free(move_arr);
        return best;
    }
    else {
        best = 1;
        for(int j = 0; j < WIDTH; j++) {
            if(bboard_done(board)) return 0;

            // FIND MOVES AND CREATE NEW BOARD
            i = move_arr[j];
            if(i == 0) break;
            
            bboard_play(board, i);

            // RECURSE TO FIND SOLUTION TO NEW BOARD.
            // UPDATE TABLE
            result = minmaxx(board, T, alpha, beta);

            // ALPHA-BETA PRUNING FROM RESULT
            if(result < best) best = result;
            if(result < beta) beta = result;
            if(beta <= alpha) break;

            bboard_resetBoard(board, p1tmp, p2tmp, turntmp);
        }
        free(move_arr);
        return best;
    }
}

void eval(struct BitBoard* board, struct Entry** T) {
    uint64_t p1tmp = board->player1;
    uint64_t p2tmp = board->player2;
    int ttmp = board->playerTurn;

    for(int i = 1; i <= WIDTH; i++) {
        if(bboard_canPlay(board, i)) {
            bboard_play(board, i);
            printf("Pos: %d, Eval: %d\n", i, minmaxx(board, T, -1, 1));
            table_reset(T);

            bboard_resetBoard(board, p1tmp, p2tmp, ttmp);
        }
    }
}


int main() {
    bboard_initMask();
    table_init();

    struct BitBoard* board = (struct BitBoard*) calloc(1, sizeof(struct BitBoard));
    struct Entry** T = (struct Entry**) calloc(TABLE_SIZE, sizeof(struct Entry*));
    
    
    bboard_reinitFromString(board, "4444443");
    bboard_print(board);
    eval(board, T);
    exit(1);
    
    /*
    bboard_reinitFromString(board, "444");
    bboard_print(board);
    printf("%d\n", minmaxx(board, T, -1, 1));
    exit(1);
    */


    FILE *f = fopen("../Test_L1_R2", "r");
    char line[128], token[64], split0[64];
    int split1, win, result;
    int total = 1;

    double start = (double) clock();
    
    while(fgets(line, 256, f) != NULL) {
        strcpy(token, line);
        strcpy(split0, strtok(token, " "));
        split1 = atoi(strtok(NULL, " "));

        bboard_reinitFromString(board, split0);
        
        if(!board->playerTurn) win = 1;
        else win = -1;
        result = minmaxx(board, T, -1, 1);
        //result = minmaxab(board, -1, 1);

        if((!result && split1) || (result && !split1)) {
            printf("error %s, %d\n", split0, split1);
            exit(1);
        }
        else if(win * result > 0 && split1 < 0) {
            printf("error %s, %d\n", split0, split1);
            exit(1);
        }
        else if(win * result < 0 && split1 > 0) {
            printf("error %s, %d\n", split0, split1);
            exit(1);
        }
        else 
            printf("good %s, %d, avg: %lf\n", split0, split1, (((double) clock() - start) / total) / CLOCKS_PER_SEC);

        total++;
        //table_reset(T);
    }
    printf("Time: %lf\n", ((double) clock() - start) / CLOCKS_PER_SEC);
    

    fclose(f);
    free(board);
    table_free(T);
}