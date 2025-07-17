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
};

struct TILE{
    int type; // Type of tile
};

struct COVER{
    COORDS c;
    int type;
    int takenById = -1;
};

void PRINT(vector<AGENT> agent){
    for(auto i : agent){
        cerr<<i.id<<":\n";
        cerr<<"    IsMine: "<<i.mine<<"\n";
        cerr<<"    Range: "<<i.range<<"\n";
        cerr<<"    Power: "<<i.power<<"\n";
        cerr<<"    Wetness: "<<i.wetness<<"\n";
        cerr<<"    CooldownLeft: "<<i.cooldownLeft<<"\n";
        cerr<<endl;
    }
}

// Returns manhattan distance between two coordinates
int distance(COORDS c1, COORDS c2){
    return abs(c1.x - c2.x) + abs(c1.y - c2.y);
}

// Reduces the cooldown left by 1 turn
void reduceCooldown(vector<AGENT> &agent){
    for(auto &i : agent){
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
COORDS findBestCover(GAME& game, vector<vector<TILE>> map, vector<COVER>& covers, vector<AGENT>& agent, size_t& agentI){
    COORDS bestCover;
    int minPoints = INT32_MAX;
    for(auto i : covers){
        if(agent[agentI].c.x < game.width/2){
            i.c.x--;
        }else{
            i.c.x++;
        }
        if(map[i.c.x][i.c.y].type != 0){continue;}
        
        int points = distance(i.c, {game.width/2, agent[agentI].c.y});
        if(points < minPoints && abs(agent[agentI].c.x - i.c.x) < game.width/2){
            minPoints = points;
            bestCover = i.c;
        }
    }
    return bestCover;
}

int main()
{
    GAME game;
    vector<AGENT> agent;
    
    cin >> game.my_id; cin.ignore();
    cin >> game.agent_count; cin.ignore();
    for (size_t i = 0; i < game.agent_count; i++) {
        AGENT temp;
        cin >> temp.id >> temp.player >> temp.cooldown >> temp.range >> temp.power >> temp.bombs; cin.ignore();
        if(temp.player == game.my_id){
            temp.mine = true;
        }else{
            temp.mine = false;
        }
        agent.push_back(temp);
    }
    cin >> game.width >> game.height; cin.ignore();
    

    vector<vector<TILE>> map(game.width, vector<TILE>(game.height));
    for (size_t i = 0; i < game.height; i++) {
        for (size_t j = 0; j < game.width; j++) {
            int x, y, type;
            cin >> x >> y >> type; cin.ignore();
            map[x][y].type = type;
        }
    }

    while (1) {
        cin >> game.agent_count; cin.ignore();
        for (size_t i = 0; i < game.agent_count; i++) {
           cin >> agent[i].id >> agent[i].c.x >> agent[i].c.y >> agent[i].cooldown >> agent[i].bombs >> agent[i].wetness; cin.ignore();
        }
        cin >> game.my_agent_count; cin.ignore();

        //-------------- Logic Starts Here
        
        // Reduce cooldown left
        reduceCooldown(agent);
        vector<COVER> covers = findCovers(game, map);
        for(size_t i = 0; i < game.agent_count; i++){
            if(agent[i].mine){
                COORDS bestCover = findBestCover(game, map, covers, agent, i);
                cout<<agent[i].id<<";MOVE "<<bestCover.x<<" "<<bestCover.y<<endl;
            }
        }
    }
}