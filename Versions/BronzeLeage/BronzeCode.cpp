#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <queue>

using namespace std;

struct GAME{
    int my_id; // Your player id (0 or 1)
    int agent_count; // Total number of agents in the game
    int my_agent_count; // Number of agents controlled by the player
    int width; // width
    int height; // height
};

struct COORDS{
    int x, y;
};

struct AGENT{
    COORDS c; // Coordinates
    int id; // Unique identifier for this agent
    int player; // Player id of this agent
    int cooldown; // Number of turns between each of this agent's shots
    int range; // Maximum manhattan distance for greatest damage output
    int power; // Damage output within optimal conditions
    int bombs; // Number of splash bombs this can throw this game
    int wetness; // Taken damage
    int cooldownLeft = 0; // Cooldown in turns left to wait before being able to shoot again
    bool mine; // Does the agent belong to me
    COORDS target; // The target tile to reach
    COORDS nextMove = {-1, -1}; // Next move where the agent is going to go
    bool dead = true; // If true agent is dead
    string move = ""; // Full string of move action etc, for couting
};

struct TILE{
    int type; // Type of tile
};

struct COVER{
    COORDS c;
    int type;
    int takenById = -1;
};

void PRINT(unordered_map<int, AGENT> agent){
    for(auto [id, i] : agent){
        cerr<<id<<":\n";
        cerr<<"    IsMine: "<<i.mine<<"\n";
        cerr<<"    Range: "<<i.range<<"\n";
        cerr<<"    Power: "<<i.power<<"\n";
        cerr<<"    Wetness: "<<i.wetness<<"\n";
        cerr<<"    CooldownLeft: "<<i.cooldownLeft<<"\n";
        cerr<<endl;
    }
}

void PRINT(vector<COVER> cover){
    for(auto i : cover){
        cerr<<"Cover: "<<i.c.x<<" "<<i.c.y<<"\n";
        cerr<<"    Type: "<<i.type<<"\n";
    }
    cerr<<endl;
}

// Returns manhattan distance between two coordinates
int distance(COORDS c1, COORDS c2){
    return abs(c1.x - c2.x) + abs(c1.y - c2.y);
}

// Reduces the cooldown left by 1 turn
void reduceCooldown(unordered_map<int, AGENT>& agent){
    for(auto &[id, i] : agent){
        if(i.cooldownLeft != 0){
            i.cooldownLeft--;
        }
    }
}

// Finds all covers in the map and returns them
vector<COVER> findCovers(const GAME& game, const vector<vector<TILE>>& map) {
    vector<COVER> covers;
    for(int x = 0; x < game.width; x++){
        for(int y = 0; y < game.height; y++){
            if(map[x][y].type != 0){
                covers.push_back({{x, y}, map[x][y].type});
            }
        }
    }
    return covers;
}

COORDS findTarget(GAME& game, vector<vector<TILE>>& map, vector<COVER>& covers, unordered_map<int, AGENT>& agent, int id){
    COORDS target = {game.width-1, game.height-1};
    return target;
}

void agentMineCheck(unordered_map<int, AGENT>& agent, GAME& game){
    for(auto &[id, i] : agent){
        if(i.player == game.my_id){
            i.mine = true;
        }else{
            i.mine = false;
        }
    }
}

void makeThemDead(unordered_map<int, AGENT>& agent, GAME& game){
    for(auto &[id, i] : agent){
        i.dead = true;
    }
}

COORDS selectRoute(GAME& game, vector<vector<TILE>>& map, COORDS& start, COORDS& end){
    COORDS selectedRoutesFirstMove = {-1, -1};
    struct QUE{
        COORDS c;
        int weight;
        COORDS firstMove;
    };
    int weightEnd = -1;
    int weightMap[game.width][game.height];
    for(size_t x = 0; x < game.width; x++){
        for(size_t y = 0; y < game.height; y++){
            weightMap[x][y] = -1;
        }
    }
    weightMap[start.x][start.y] = 0;
    queue<QUE> que;
    que.push({start, 0, {-1, -1}});
    while(!que.empty()){
        QUE q = que.front();
        que.pop();
        if(q.c.x == end.x && q.c.y == end.y){
            if(weightEnd == -1){
                weightEnd = q.weight;
                selectedRoutesFirstMove = q.firstMove;
            }else{
                if(weightEnd < q.weight){
                    continue;
                }
                weightEnd = q.weight;
                selectedRoutesFirstMove = q.firstMove;
            }
        }

        if(q.c.x-1 >= 0 && map[q.c.x-1][q.c.y].type == 0){
            COORDS tempFirstMove = q.firstMove;
            if(tempFirstMove.x == -1){tempFirstMove.x = q.c.x-1; tempFirstMove.y = q.c.y;}
            if(weightMap[q.c.x-1][q.c.y] == -1 || weightMap[q.c.x-1][q.c.y] > q.weight+1){
                weightMap[q.c.x-1][q.c.y] = q.weight+1;
                que.push({{q.c.x-1, q.c.y}, q.weight+1, tempFirstMove});
            }
        }
        if(q.c.x+1 < game.width && map[q.c.x+1][q.c.y].type == 0){
            COORDS tempFirstMove = q.firstMove;
            if(tempFirstMove.x == -1){tempFirstMove.x = q.c.x+1; tempFirstMove.y = q.c.y;}
            if(weightMap[q.c.x+1][q.c.y] == -1 || weightMap[q.c.x+1][q.c.y] > q.weight+1){
                weightMap[q.c.x+1][q.c.y] = q.weight+1;
                que.push({{q.c.x+1, q.c.y}, q.weight+1, tempFirstMove});
            }
        }
        if(q.c.y-1 >= 0 && map[q.c.x][q.c.y-1].type == 0){
            COORDS tempFirstMove = q.firstMove;
            if(tempFirstMove.x == -1){tempFirstMove.x = q.c.x; tempFirstMove.y = q.c.y-1;}
            if(weightMap[q.c.x][q.c.y-1] == -1 || weightMap[q.c.x][q.c.y-1] > q.weight+1){
                weightMap[q.c.x][q.c.y-1] = q.weight+1;
                que.push({{q.c.x, q.c.y-1}, q.weight+1, tempFirstMove});
            }
        }
        if(q.c.y+1 < game.height && map[q.c.x][q.c.y+1].type == 0){
            COORDS tempFirstMove = q.firstMove;
            if(tempFirstMove.x == -1){tempFirstMove.x = q.c.x; tempFirstMove.y = q.c.y+1;}
            if(weightMap[q.c.x][q.c.y+1] == -1 || weightMap[q.c.x][q.c.y+1] > q.weight+1){
                weightMap[q.c.x][q.c.y+1] = q.weight+1;
                que.push({{q.c.x, q.c.y+1}, q.weight+1, tempFirstMove});
            }
        }
    }
    return selectedRoutesFirstMove;
}

void findNextMove(GAME& game, vector<vector<TILE>>& map, vector<COVER>& covers, unordered_map<int, AGENT>& agent, int id){
    agent[id].target = findTarget(game, map, covers, agent, id);
    agent[id].nextMove = selectRoute(game, map, agent[id].c, agent[id].target);
    agent[id].move = ";MOVE " + to_string(agent[id].nextMove.x) + ' ' + to_string(agent[id].nextMove.y);
}

void executeMoves(unordered_map<int, AGENT>& agent){
    for(const auto& [id, i] : agent){
        if(i.mine && !i.dead){
            cout<<id<<i.move<<endl;
        }
    }
}

int main()
{
    GAME game;
    unordered_map<int, AGENT> agent;
    
    cin >> game.my_id; cin.ignore();
    cin >> game.agent_count; cin.ignore();
    for (size_t i = 0; i < game.agent_count; i++) {
        AGENT temp;
        cin >> temp.id >> temp.player >> temp.cooldown >> temp.range >> temp.power >> temp.bombs; cin.ignore();
        agent[temp.id] = temp;
    }
    agentMineCheck(agent, game);
    cin >> game.width >> game.height; cin.ignore();
    

    vector<vector<TILE>> map(game.width, vector<TILE>(game.height));
    for (size_t i = 0; i < game.height; i++) {
        for (size_t j = 0; j < game.width; j++) {
            int x, y, type;
            cin >> x >> y >> type; cin.ignore();
            map[x][y].type = type;
        }
    }

    vector<COVER> covers = findCovers(game, map);

    while (1) {
        cin >> game.agent_count; cin.ignore();
        makeThemDead(agent, game);
        for (size_t i = 0; i < game.agent_count; i++) {
            int id;
            cin >> id >> agent[id].c.x >> agent[id].c.y >> agent[id].cooldown >> agent[id].bombs >> agent[id].wetness; cin.ignore();
            agent[id].dead = false;
        }

        cin >> game.my_agent_count; cin.ignore();

        //-------------- Logic Starts Here
        
        // Reduce cooldown left
        reduceCooldown(agent);
        agentMineCheck(agent, game);
        for(auto& [id, i] : agent){
            if(i.mine && !i.dead){
                findNextMove(game, map, covers, agent, id);
            }
        }
        executeMoves(agent);

        // Debug
        PRINT(agent);
        PRINT(covers);
    }
}