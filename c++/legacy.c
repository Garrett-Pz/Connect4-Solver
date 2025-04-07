/*
int minmax(struct BitBoard* board) {
    uint64_t p1tmp = board->player1;
    uint64_t p2tmp = board->player2;
    int turntmp = board->playerTurn;
    int best;
    int result;

    if(board->playerTurn == 0) {
        if(bboard_checkForWin(board)) return -1;
        if(bboard_done(board)) return 0;

        best = -1;
        for(int i = 1; i <= WIDTH; i++) {
            if( !(bboard_canPlay(board, i)) ) continue;
            bboard_play(board, i);
            result = minmax(board);
            if(result > best) best = result;
            bboard_resetBoard(board, p1tmp, p2tmp, turntmp);
        }
        return best;
    }
    else {
        if(bboard_checkForWin(board)) return 1;
        if(bboard_done(board)) return 0;

        best = 1;
        for(int i = 1; i <= WIDTH; i++) {
            if( !(bboard_canPlay(board, i)) ) continue;
            bboard_play(board, i);
            result = minmax(board);
            if(result < best) best = result;
            bboard_resetBoard(board, p1tmp, p2tmp, turntmp);
        }
        return best;
    }
}

int minmaxab(struct BitBoard* board, int alpha, int beta) {
    uint64_t p1tmp = board->player1;
    uint64_t p2tmp = board->player2;
    int turntmp = board->playerTurn;
    int best;
    int result;
    int i;

    if(board->playerTurn == 0) {
        if(bboard_checkForWin(board)) return -1;
        if(bboard_done(board)) return 0;

        best = -1;
        for(int j = 0; j < WIDTH; j++) {
            i = moves[j];

            if( !(bboard_canPlay(board, i)) ) continue;
            bboard_play(board, i);
            result = minmaxab(board, alpha, beta);

            if(result > best) best = result;
            if(result > alpha) alpha = result;
            if(alpha >= beta) break;

            bboard_resetBoard(board, p1tmp, p2tmp, turntmp);
        }
        return best;
    }
    else {
        if(bboard_checkForWin(board)) return 1;
        if(bboard_done(board)) return 0;

        best = 1;
        for(int j = 0; j < WIDTH; j++) {
            i = moves[j];
            
            if( !(bboard_canPlay(board, i)) ) continue;
            bboard_play(board, i);
            result = minmaxab(board, alpha, beta);

            if(result < best) best = result;
            if(result < beta) beta = result;
            if(beta <= alpha) break;

            bboard_resetBoard(board, p1tmp, p2tmp, turntmp);
        }
        return best;
    }
}


minmaxabt(struct BitBoard* board, struct Entry** T, int alpha, int beta) {
    uint64_t p1tmp = board->player1;
    uint64_t p2tmp = board->player2;
    int turntmp = board->playerTurn;
    struct Entry* entry = NULL;
    uint64_t p1, p2;
    int best;
    int result;
    int i;

    if(board->playerTurn == 0) {
        if(bboard_checkForWin(board)) return -1;
        if(bboard_done(board)) return 0;

        best = -1;
        for(int j = 0; j < WIDTH; j++) {
            // FIND MOVES AND CREATE NEW BOARD
            i = moves[j];

            if( !(bboard_canPlay(board, i)) ) continue;
            bboard_play(board, i);
            p1 = board->player1;
            p2 = board->player2;

            // RECURSE TO FIND SOLUTION TO NEW BOARD.
            // UPDATE TABLE
            if(table_contains(T, p1, p2)) {
                entry = table_get(T, p1, p2);
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
                result = minmaxabt(board, T, alpha, beta);
                if(result <= alpha) {
                    table_put(T, p1, p2, result, 1);
                }
                else if(result >= beta) {
                    table_put(T, p1, p2, result, -1);
                }
                else {
                    table_put(T, p1, p2, result, 0);
                }
            }

            // ALPHA-BETA PRUNING FROM RESULT
            if(result > best) best = result;
            if(result > alpha) alpha = result;
            if(alpha >= beta) break;

            bboard_resetBoard(board, p1tmp, p2tmp, turntmp);
            entry = NULL;
        }
        return best;
    }
    else {
        if(bboard_checkForWin(board)) return 1;
        if(bboard_done(board)) return 0;

        best = 1;
        for(int j = 0; j < WIDTH; j++) {
            // FIND MOVES AND CREATE NEW BOARD
            i = moves[j];
            
            if( !(bboard_canPlay(board, i)) ) continue;
            bboard_play(board, i);
            p1 = board->player1;
            p2 = board->player2;

            // RECURSE TO FIND SOLUTION TO NEW BOARD.
            // UPDATE TABLE
            if(table_contains(T, p1, p2)) {
                entry = table_get(T, p1, p2);
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
                result = minmaxabt(board, T, alpha, beta);
                if(result <= alpha) {
                    table_put(T, p1, p2, result, 1);
                }
                else if(result >= beta) {
                    table_put(T, p1, p2, result, -1);
                }
                else {
                    table_put(T, p1, p2, result, 0);
                }
            }

            // ALPHA-BETA PRUNING FROM RESULT
            if(result < best) best = result;
            if(result < beta) beta = result;
            if(beta <= alpha) break;

            bboard_resetBoard(board, p1tmp, p2tmp, turntmp);
            entry = NULL;
        }
        return best;
    }
}


int minmaxabt_2(struct BitBoard* board, struct Entry** T, int alpha, int beta) {
    uint64_t p1tmp = board->player1;
    uint64_t p2tmp = board->player2;
    int turntmp = board->playerTurn;
    struct Entry* entry = NULL;
    uint64_t p1, p2;
    int best, result, i, hash;

    if(board->playerTurn == 0) {
        if(bboard_checkForWin(board)) return -1;
        if(bboard_done(board)) return 0;

        best = -1;
        for(int j = 0; j < WIDTH; j++) {
            // FIND MOVES AND CREATE NEW BOARD
            i = moves[j];

            if( !(bboard_canPlay(board, i)) ) continue;
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
                result = minmaxabt_2(board, T, alpha, beta);

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
        return best;
    }
    else {
        if(bboard_checkForWin(board)) return 1;
        if(bboard_done(board)) return 0;

        best = 1;
        for(int j = 0; j < WIDTH; j++) {
            // FIND MOVES AND CREATE NEW BOARD
            i = moves[j];
            
            if( !(bboard_canPlay(board, i)) ) continue;
            bboard_play(board, i);

            // RECURSE TO FIND SOLUTION TO NEW BOARD.
            result = minmaxabt_2(board, T, alpha, beta);

            // ALPHA-BETA PRUNING FROM RESULT
            if(result < best) best = result;
            if(result < beta) beta = result;
            if(beta <= alpha) break;

            bboard_resetBoard(board, p1tmp, p2tmp, turntmp);
        }
        return best;
    }
}


int minmaxabta(struct BitBoard* board, struct Entry** T, int alpha, int beta) {
    uint64_t p1tmp = board->player1;
    uint64_t p2tmp = board->player2;
    int turntmp = board->playerTurn;
    struct Entry* entry = NULL;
    uint64_t p1, p2;
    int best, result, i, hash;

    int* move_arr = bboard_get_moves(board);

    if(board->playerTurn == 0) {
        best = -1;
        for(int j = 0; j < WIDTH; j++) {
            if(bboard_done(board)) break;

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
                result = minmaxabta(board, T, alpha, beta);

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
            if(bboard_done(board)) break;

            // FIND MOVES AND CREATE NEW BOARD
            i = move_arr[j];
            if(i == 0) break;
            
            bboard_play(board, i);

            // RECURSE TO FIND SOLUTION TO NEW BOARD.
            // UPDATE TABLE
            result = minmaxabta(board, T, alpha, beta);

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
*/