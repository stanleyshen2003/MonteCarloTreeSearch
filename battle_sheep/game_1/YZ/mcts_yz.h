#include <unordered_map>
#include <string>
#include <queue>
#include <vector>
#include <cmath>
#include <iostream>

#define pii pair<int, int>
#define puu pair<unsigned int, unsigned int>
using namespace std;


struct Action {
    int x, y, n, dir;
    Action(int x, int y, int n, int dir) : x(x), y(y), n(n), dir(dir) {}
    Action() : x(-1), y(-1), n(-1), dir(-1) {}
    string get_key();
};

class GameState {
    public:
        // the state of the game
        char user_state[12][12];
        int sheep_state[12][12];
        char turn;

        // (TODO: move dir8 & dir4 to somewhere else to make it more memory efficient)
        int directions8[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
        int directions4[4][2] = {{-1, 0}, {0, -1}, {0, 1}, {1, 0}};

        // constructors and destructor
        GameState(int user_state[12][12], int sheep_state[12][12], char turn);
        GameState(const GameState* state);
        ~GameState();

        // get all possible actions in the current state
        vector<Action> get_actions();

        // get the key of the state for the map (you can ignore this)
        string get_key();

        // update the state of the game according to the action
        void next_state(Action& action);

        // check if the game terminates
        bool is_terminal();

        // check if the player with the given turn is the winner
        bool is_winner(char turn);

        // calculate the score of the player with the given turn, used in is_winner()
        int recursive_calculate(int x, int y, char person);
};

// Define MCTSNode class representing a node in the MCTS
class MCTSNode {
public:
    GameState state;
    MCTSNode* parent;
    vector<MCTSNode*> children;
    int visits;
    double score;
    Action action; // store action

    MCTSNode(const GameState& state, MCTSNode* parent = nullptr, const Action& action = Action()) 
        : state(state), parent(parent), action(action), visits(0), score(0) {}

    MCTSNode* select_child();
    MCTSNode* add_child(const GameState&, const Action&);
    
    double calculate_ucb(MCTSNode*);
};

class MCTS_agent {
    public:
        // this map stores the number of wins and the number of loses for each state (I'm not sure how to use this)
        unordered_map<string, puu> move_map;

        // config of the agent
        int max_iter, max_seconds;

        // the player's turn
        char player_turn;

        // constructors and destructor
        MCTS_agent(int max_iter = 2000, int max_seconds = 2, char player_turn = '1');
        ~MCTS_agent();

        // decide the next step given present state (maybe adding a rollout function would be better)
        Action decide_step(GameState& state);
    
    private:
        MCTSNode* select_node(MCTSNode*);
        void expand_node(MCTSNode*);
        int rollout(GameState);
        void backpropagate(MCTSNode*, int);
        Action get_best_action(MCTSNode*);
        void delete_tree(MCTSNode*);
};