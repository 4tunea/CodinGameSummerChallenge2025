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
    int turnCount = 1; // TEMPORARY SOLUTION FOR A TEMPORARY PROBLEM
};
struct TILE{
    int type; // Type of tile
};

int distance(COORDS c1, COORDS c2){
    return abs(c1.x - c2.x) + abs(c1.y - c2.y);
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

        //Logic Starts Here
        for(int i=0;i<game.agent_count; i++){
            if(agent[i].player == game.my_id){
                if(agent[i].turnCount == 1){
                    for(int j=0;j<game.agent_count;j++){
                        if(agent[j].player == game.my_id && agent[j].id != agent[i].id){
                            if(agent[j].c.x < game.width/2 && agent[j].c.y < game.height/2){
                                agent[i].turnCount++;
                                continue;
                            }
                        }
                    }
                    if(agent[i].c.x == 5 && agent[i].c.y == 2){
                        cout<<agent[i].id<<";MOVE 5 2;THROW 2 2"<<endl;
                        agent[i].turnCount++;
                        continue;
                    }else{
                        cout<<agent[i].id<<";MOVE 5 2;"<<endl;
                    }
                }
                if(agent[i].turnCount == 2){
                    for(int j=0;j<game.agent_count;j++){
                        if(agent[j].player == game.my_id && agent[j].id != agent[i].id){
                            if(agent[j].c.x > game.width/2 && agent[j].c.y < game.height/2){
                                agent[i].turnCount++;
                                continue;
                            }
                        }
                    }
                    if(agent[i].c.x == game.width-6 && agent[i].c.y == 2){
                        cout<<agent[i].id<<";MOVE "<<game.width-6<<" 2;THROW "<<game.width-3<<" 2"<<endl;
                        agent[i].turnCount++;
                        continue;
                    }else{
                        cout<<agent[i].id<<";MOVE "<<game.width-6<<" 2;"<<endl;
                    }
                }
                if(agent[i].turnCount == 3){
                    for(int j=0;j<game.agent_count;j++){
                        if(agent[j].player == game.my_id && agent[j].id != agent[i].id){
                            if(agent[j].c.x > game.width/2 && agent[j].c.y > game.height/2){
                                agent[i].turnCount++;
                                continue;
                            }
                        }
                    }
                    if(agent[i].c.x == game.width-6 && agent[i].c.y == game.height-3){
                        cout<<agent[i].id<<";MOVE "<<game.width-6<<" "<<game.height-3<<";THROW "<<game.width-3<<" "<<game.height-3<<endl;
                        agent[i].turnCount++;
                        continue;
                    }else{
                        cout<<agent[i].id<<";MOVE "<<game.width-6<<" "<<game.height-3<<endl;
                    }
                }
                if(agent[i].turnCount == 4){
                    for(int j=0;j<game.agent_count;j++){
                        if(agent[j].player == game.my_id && agent[j].id != agent[i].id){
                            if(agent[j].c.x < game.width/2 && agent[j].c.y > game.height/2){
                                agent[i].turnCount++;
                                continue;
                            }
                        }
                    }
                    if(agent[i].c.x == 5 && agent[i].c.y == game.height-3){
                        cout<<agent[i].id<<";MOVE 5 "<<game.height-3<<";THROW 2 "<<game.height-3<<endl;
                        agent[i].turnCount++;
                        continue;
                    }else{
                        cout<<agent[i].id<<";MOVE 5 "<<game.height-3<<endl;
                    }
                }
            }
        }
    }
}