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
};
struct TILE{
    int type; // Type of tile
};

void PRINT(vector<AGENT> agent){
    for(auto i : agent){
        cerr<<i.id<<":\n";
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

void reduceCooldown(vector<AGENT> &agent){
    for(auto &i : agent){
        if(i.cooldownLeft != 0){
            i.cooldownLeft--;
        }
    }
}

int main()
{
    GAME game;
    vector<AGENT> agent;
    
    cin >> game.my_id; cin.ignore();
    cin >> game.agent_count; cin.ignore();
    for (int i = 0; i < game.agent_count; i++) {
        AGENT temp;
        cin >> temp.id >> temp.player >> temp.cooldown >> temp.range >> temp.power >> temp.bombs; cin.ignore();
        agent.push_back(temp);
    }
    cin >> game.width >> game.height; cin.ignore();
    
    TILE map[game.width][game.height];
    
    for (int i = 0; i < game.height; i++) {
        for (int j = 0; j < game.width; j++) {
            int x, y, type;
            cin >> x >> y >> type; cin.ignore();
            map[x][y].type = type;
        }
    }

    while (1) {
        cin >> game.agent_count; cin.ignore();
        for (int i = 0; i < game.agent_count; i++) {
           cin >> agent[i].id >> agent[i].c.x >> agent[i].c.y >> agent[i].cooldown >> agent[i].bombs >> agent[i].wetness; cin.ignore();
        }
        cin >> game.my_agent_count; cin.ignore();

        //-------------- Logic Starts Here
        
        // Reduce cooldownLeft
        reduceCooldown(agent);

        // Find first enemy
        int enemyI;
        for(int i = 0; i<game.agent_count; i++){
            if(agent[i].player != game.my_id){
                enemyI = i;
                break;
            }
        }

        // Moves for each agent
        for(int i = 0; i<game.agent_count; i++){
            if(agent[i].player == game.my_id){
                // My agent
                if(agent[i].range*2 >= distance(agent[i].c, agent[enemyI].c)){
                    cout<<agent[i].id<<";SHOOT "<<agent[enemyI].id<<endl;
                    continue;
                }else{
                    cout<<agent[i].id<<";MOVE "<<agent[enemyI].c.x<<" "<<agent[enemyI].c.y<<";HUNKER_DOWN"<<endl;
                    continue;
                }
            }
        }
    }
}