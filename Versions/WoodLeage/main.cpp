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
};
struct TILE{
    int type; // Type of tile
};

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
        for (int i = 0; i < game.my_agent_count; i++) {
            
        }
    }
}