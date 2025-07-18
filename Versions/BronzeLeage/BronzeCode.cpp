#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

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
    COORDS bestCover; // Every agent gets his own best cover and its job is to try to dominate that area
    COORDS nextMove = {-1, -1};
    bool dead = true;
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

// Returnes the coordinates of the best cover for that agent
COORDS findBestCover(GAME& game, vector<vector<TILE>> map, vector<COVER>& covers, unordered_map<int, AGENT>& agent, int id){
    COORDS bestCover = {-1, -1};
    int minPoints = INT32_MAX;
    for(auto i : covers){
        if(agent[id].c.x < game.width/2){
            i.c.x--;
        }else{
            i.c.x++;
        }
        if(map[i.c.x][i.c.y].type != 0){continue;}
        
        bool coverIsTakenAlready = false;
        for(auto &[j_id, j] : agent){
            if(j.mine){
                if(j.bestCover.x != -1 && i.c.x == j.bestCover.x && i.c.y == j.bestCover.y){
                    coverIsTakenAlready = true;
                    break;
                }
            }
        }
        if(coverIsTakenAlready){continue;}

        int points = distance(i.c, {game.width/2, agent[id].c.y});
        if(points < minPoints && abs(agent[id].c.x - i.c.x) < game.width/2-1){
            minPoints = points;
            bestCover = i.c;
        }
    }
    return bestCover;
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
    for(auto &[id, i] : agent){
        if(i.mine){
            i.bestCover = findBestCover(game, map, covers, agent, id);
        }
    }

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
        
        for(auto &[id, i] : agent){
            if(i.mine && !i.dead){
                if(i.bestCover.x != -1){
                    cout<<id<<";MOVE "<<i.bestCover.x<<" "<<i.bestCover.y<<endl;
                }else{
                    cout<<id<<";MOVE 1 1"<<endl;
                }
            }
        }

        PRINT(agent);
        PRINT(covers);
    }
}