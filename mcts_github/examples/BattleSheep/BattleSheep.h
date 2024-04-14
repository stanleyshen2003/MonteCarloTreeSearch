#ifndef MCTS_TICTACTOE_H
#define MCTS_TICTACTOE_H

#include "../../mcts/include/state.h"


class BattleSheep_state : public MCTS_state {
    char board[12][12]{};
    bool player_won(char player) const;
    char calculate_winner() const;
    char turn, winner;
    void change_turn();
public:
    BattleSheep_state();
    BattleSheep_state(const BattleSheep_state &other);
    char get_turn() const;
    char get_winner() const;
    bool is_terminal() const override;
    MCTS_state *next_state(const MCTS_move *move) const override;
    queue<MCTS_move *> *actions_to_try() const override;
    double rollout() const override;                        // the rollout simulation in MCTS
    void print() const override;
    bool player1_turn() const override { return turn == 'x'; }
};


struct BattleSheep_move : public MCTS_move {
    int x, y;
    char player;
    BattleSheep_move(int x, int y, char p) : x(x), y(y), player(p) {}
    bool operator==(const MCTS_move& other) const override;
};

#endif
