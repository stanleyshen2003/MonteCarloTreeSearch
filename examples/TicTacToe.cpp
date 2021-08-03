#include <iostream>
#include "TicTacToe.h"
#include <ctime>


using namespace std;


TicTacToe_state::TicTacToe_state() : MCTS_state(false), turn('x'), winner(' ') {
    // initialize board as empty
    for (int i = 0 ; i < 9 ; i++) {
        board[i / 3][i % 3] = ' ';
    }
}

TicTacToe_state::TicTacToe_state(const TicTacToe_state &other)
        : MCTS_state(false), turn(other.turn), winner(other.winner) {
    // copy board
    for (int i = 0 ; i < 9 ; i++) {
        board[i / 3][i % 3] = other.board[i / 3][i % 3];
    }
}

bool TicTacToe_state::player_won(char player) {
    if (player != 'x' and player != 'o') cerr << "Warning: check winner for unknown player" << endl;
    for (int i = 0 ; i < 3 ; i++) {
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player) return true;
        if (board[0][i] == player && board[1][i] == player && board[2][i] == player) return true;
    }
    return (board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
           (board[0][2] == player && board[1][1] == player && board[2][0] == player);
}

bool TicTacToe_state::is_terminal() {
    if (player_won('x')) {
        winner = 'x';
        return true;
    } else if (player_won('o')) {
        winner = 'o';
        return true;
    }
    bool all_taken = true;
    for (int i = 0 ; i < 9 ; i++) {
        if (board[i / 3][i % 3] == ' ') {
            all_taken = false;
            break;
        }
    }
    if (all_taken) winner = 'd';   // draw
    return all_taken;
}

char TicTacToe_state::get_turn() const { return turn; }

void TicTacToe_state::change_turn() {
    turn = (turn == 'x') ? 'o' : 'x';
}

MCTS_state *TicTacToe_state::next_state(MCTS_move *move) const {
    // Note: We have to manually cast it to its correct type
    TicTacToe_move *m = (TicTacToe_move *) move;
    TicTacToe_state *new_state = new TicTacToe_state(*this);  // create new state from current
    if (new_state->board[m->x][m->y] == ' ') {
        new_state->board[m->x][m->y] = m->player;             // play move
        new_state->change_turn();
    } else {
        cerr << "Warning: Illegal move (" << m->x << ", " << m->y << ")" << endl;
        return NULL;
    }
    return new_state;
}

queue<MCTS_move *> *TicTacToe_state::actions_to_try() const {
    queue<MCTS_move *> *Q = new queue<MCTS_move *>();
    for (int i = 0 ; i < 9 ; i++) {
        if (board[i / 3][i % 3] == ' ') {
            Q->push(new TicTacToe_move(i / 3, i % 3, turn));
        }
    }
    return Q;
}

double TicTacToe_state::rollout() {
    if (is_terminal()) return (winner == 'x') ? 1.0 : (winner == 'd') ? 0.5 : 0.0;
    // Simulate a completely random game
    // Note: dequeue is not very efficient for random accesses but vector is not efficient for deletes
    deque<int> available;
    for (int i = 0 ; i < 9 ; i++){
        if (board[i / 3][i % 3] == ' ') {
            available.push_front(i);
        }
    }
    long long r;
    int a;
    TicTacToe_state *curstate = this;
    srand(time(NULL));
    do {
        if (available.empty()) {
            cerr << "Warning: Ran out of available moves and state is not terminal?";
            return 0.0;
        }
        r = rand() % available.size();
        a = available[r];
        TicTacToe_move move(a / 3, a % 3, curstate->turn);
        available.erase(available.begin() + r);    // delete from available moves
        curstate = (TicTacToe_state *) curstate->next_state(&move);
    } while (!curstate->is_terminal());
    return (curstate->winner == 'x') ? 1.0 : (curstate->winner == 'd') ? 0.5 : 0.0;
}

void TicTacToe_state::print() const {
    printf(" %c | %c | %c\n---+---+---\n %c | %c | %c\n---+---+---\n %c | %c | %c\n",
           board[0][0], board[0][1], board[0][2],
           board[1][0], board[1][1], board[1][2],
           board[2][0], board[2][1], board[2][2]);
}

char TicTacToe_state::get_winner(bool test) {
    if (test) is_terminal();
    return winner;
}

bool TicTacToe_move::operator==(const MCTS_move &other) const {
    const TicTacToe_move &o = (const TicTacToe_move &) other;        // TODO: Casting necessary
    return x == o.x && y == o.y && player == o.player;
}
