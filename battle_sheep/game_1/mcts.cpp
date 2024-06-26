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

string Action::get_key() {
    return to_string(x) + to_string(y) + to_string(n) + to_string(dir);
}

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

GameState::GameState(int user_state[12][12], int sheep_state[12][12], char turn) : turn(turn) {
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            this->user_state[i][j] = (user_state[i][j] +1) + '0';
            this->sheep_state[i][j] = sheep_state[i][j];
        }
    }
}

GameState::GameState(const GameState* state) : turn(state->turn) {
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            this->user_state[i][j] = state->user_state[i][j];
            this->sheep_state[i][j] = state->sheep_state[i][j];
        }
    }
}

GameState::~GameState() {}

vector<Action> GameState::get_actions() {
    vector<Action> actions;
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            if (user_state[i][j] == turn && sheep_state[i][j] > 1) {
                for(int k = 0; k < 8; k++){
                    int x = i + directions8[k][0], y = j + directions8[k][1];
                    if(x >= 0 && x < 12 && y >= 0 && y < 12 && user_state[x][y]  == '1'){
                        for(int l = 1; l < sheep_state[i][j]; l++){
                            actions.push_back(Action(i, j, l, k));
                        }
                    }
                }
            }
        }
    }
    return actions;
}

string GameState::get_key() {
    /*
    This function returns the key of the state for the map
    */
    string key = "";
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            key += user_state[i][j];
            key += to_string(sheep_state[i][j]);
        }
    }
    return key;
}

void GameState::next_state(Action& action) {
    /*
    This function update the state of the game according to the action
    */
    // deal with turns that does not have further actions
    if (action.x == -1) {
        turn = (char)((int)turn + 1);
        if(turn == '6')
            turn = '2';
        return;
    }
    int x = action.x, y = action.y, n = action.n, dir = action.dir;
    sheep_state[x][y] -= n;
    x += directions8[dir][0];
    y += directions8[dir][1];
    while(1){
        int new_x = x + directions8[dir][0], new_y = y + directions8[dir][1];
        if(new_x < 0 || new_x >= 12 || new_y < 0 || new_y >= 12 || user_state[new_x][new_y] != '1'){
            break;
        }
        x = new_x;
        y = new_y;
    }

    sheep_state[x][y] += n;
    user_state[x][y] = turn;
    turn = (char)((int)turn + 1);
    if(turn == '6')
        turn = '2';
}

bool GameState::is_terminal() {
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            if (sheep_state[i][j] > 1) {
                for(int k = 0; k < 8; k++){
                    if(user_state[i + directions8[k][0]][j + directions8[k][1]] == '1'){
                        return false;
                    }
                }
            }
            
        }
    }
    return true;
}

bool GameState::is_winner(char turn) {
    double turn_score = 0, new_score = 0;
    int temp_turn_score = 0, temp_new_score = 0;
    for (int j = 0; j < 12; j++){
        for (int k = 0; k < 12; k++){
            if(user_state[j][k] == turn){
                turn_score += pow(recursive_calculate(j, k, turn), 1.25);
            }
        }
    }
    temp_turn_score = (int)(turn_score+0.5);
    for (int i = 0; i < 4; i++){
        if (i + 2 == turn - '0'){
            continue;
        }
        for (int j = 0; j < 12; j++){
            for (int k = 0; k < 12; k++){
                if(user_state[j][k] == (char)(i + 2)){
                    new_score += pow(recursive_calculate(j, k, (char)(i + 2)), 1.25);
                }
            }
        }
        temp_new_score = (int)(new_score+0.5);
        if(temp_new_score > temp_turn_score){
            return false;
        }
    }
    return true;
}

int GameState::recursive_calculate(int x, int y, char person){
    int score = 0;
    user_state[x][y] = '0';
    for(int i = 0; i < 4; i++){
        int new_x = x + directions4[i][0], new_y = y + directions4[i][1];
        if(new_x >= 0 && new_x < 12 && new_y >= 0 && new_y < 12 && user_state[new_x][new_y] == person){
            score += recursive_calculate(new_x, new_y, person);
        }
    }
    return score + 1;
}

class MCTS_agent {
    public:
        // this map stores the number of wins and the number of loses for each state
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
};

MCTS_agent::MCTS_agent(int max_iter, int max_seconds, char player_turn) : max_iter(max_iter), max_seconds(max_seconds), player_turn(player_turn) {
    move_map = unordered_map<string, puu>();
}

MCTS_agent::~MCTS_agent() {
    move_map.clear();
}

Action MCTS_agent::decide_step(GameState& state) {
    if (state.is_terminal()) {
        return Action(-1, -1, -1, -1);
    }

    string key = state.get_key();
    string new_key;
    int action_index;
    Action now_action(-1, -1, -1, -1);
    vector<Action> actions = state.get_actions();
    if (actions.size() == 0)
        return Action(-1, -1, -1, -1);
    
    queue<string> Q;
    for(auto& action : actions){
        Q.push(key + action.get_key());
        GameState new_state(&state);
        new_state.next_state(action);

        // rollout
        while(!new_state.is_terminal()){
            vector<Action> new_actions = new_state.get_actions();
            if(new_actions.size() == 0){
                new_state.next_state(Action(-1, -1, -1, -1));
            }
            else{
                action_index = ((int)rand()) % new_actions.size();
                now_action = new_actions[action_index];
                Q.push(new_state.get_key() + now_action.get_key());
                new_state.next_state(now_action);
            }
        }

        // update move_map
        bool is_winner = new_state.is_winner(player_turn);
        while(!Q.empty()){
            new_key = Q.front();
            Q.pop();
            if(move_map.find(new_key) == move_map.end()){
                move_map[new_key] = puu(0, 0);
            }
            if(is_winner){
                move_map[new_key].first += 1;
            }
            move_map[new_key].second += 1;
        }
    }

    // calculate UCB values
    vector<int> Q_counts;
    int total_count = 0;
    for(auto action : actions){
        Q_counts.push_back(move_map[key+action.get_key()].first + move_map[key + action.get_key()].second);
        total_count += Q_counts.back();
    }
    vector<double> UCBs;
    for(int i = 0; i < Q_counts.size(); i++){
        UCBs.push_back((double)move_map[key+actions[i].get_key()].first / (double)Q_counts[i] + sqrt(2 * log(total_count) / Q_counts[i]));
    }

    // rollout
    for(int i = 0; i < max_iter; i++){
        // select max action
        GameState new_state(&state);
        auto max_iter = max_element(UCBs.begin(), UCBs.end());
        int max_index = distance(UCBs.begin(), max_iter);
        now_action = actions[max_index];
        Q.push(key + now_action.get_key());
        new_state.next_state(now_action);

        // go down the tree
        while(!new_state.is_terminal()){
            vector<Action> new_actions = new_state.get_actions();
            if(new_actions.size() == 0){
                new_state.next_state(Action(-1, -1, -1, -1));
            }
            else{
                action_index = ((int)rand()) % new_actions.size();
                now_action = new_actions[action_index];
                new_state.next_state(now_action);
            }
        }

        // update move_map and UCB values
        bool is_winner = new_state.is_winner(player_turn);
        while(!Q.empty()){
            new_key = Q.front();
            Q.pop();
            if(move_map.find(new_key) == move_map.end()){
                move_map[new_key] = pii(0, 0);
            }
            if(is_winner){
                move_map[new_key].first += 1;
            }
            move_map[new_key].second += 1;
        }

        // update UCBs
        if(is_winner){
            total_count++;
            Q_counts[max_index]++;
            for(int j = 0; j < Q_counts.size(); j++){
                UCBs[j] = (double)move_map[key+actions[j].get_key()].first / (double)Q_counts[j] + sqrt(2 * log(total_count) / Q_counts[j]);
            }
        }
    }

    // find the best action
    double max = -1;
    Action best_action;
    double ucb;
    double t = 0;
    for(auto action : actions){
        new_key = key + action.get_key();
        t += move_map[new_key].second + move_map[new_key].first;
    }
    for(auto action : actions){
        new_key = key + action.get_key();
        ucb = (double)move_map[new_key].first / ((double)move_map[new_key].first + (double)move_map[new_key].second) + sqrt(2 * log(t) / ((double)move_map[new_key].first + (double)move_map[new_key].second));
        if(ucb > max){
            max = ucb;
            best_action = action;
        }
    }



    return best_action;
}