#ifndef MCTS_STATE_H
#define MCTS_STATE_H

#include <stdexcept>
#include <queue>


using namespace std;


struct MCTS_move {
    virtual ~MCTS_move() = default;
    virtual bool operator==(const MCTS_move& other) const = 0;   // implement this!
};


/** Implement all pure virtual methods. Notes:
 * - rollout() must return something in [0, 1] for UCT to work as intended
 */
class MCTS_state {
public:
    // Implement these:
    virtual ~MCTS_state() = default;
    virtual queue<MCTS_move *> *actions_to_try() const = 0;
    virtual MCTS_state *next_state(const MCTS_move *move) const = 0;
    virtual double rollout() const = 0;
    virtual bool is_terminal() const = 0;
    virtual void print() const {
        cout << "Printing not implemented" << endl;
    }
};


#endif
