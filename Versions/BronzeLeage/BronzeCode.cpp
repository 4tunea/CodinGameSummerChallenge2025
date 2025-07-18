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
        cerr<<"    Cooldown: "<<i.cooldown<<"\n";
        cerr<<"    Target: "<<i.target.x<<" "<<i.target.y<<"\n";
        cerr<<"    NextMove: "<<i.nextMove.x<<" "<<i.nextMove.y<<"\n";
        cerr<<"    Dead: "<<i.dead<<"\n";
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

void PRINT(vector<vector<int>>& map){
    cerr <<"\nMAP:\n";
    for (size_t y = 0; y < map[0].size(); y++){
        for (size_t x = 0; x < map.size(); x++){
            cerr<<map[x][y]<<" ";
        }
        cerr<<"\n";
    }
}

// Returns manhattan distance between two coordinates
int distance(COORDS c1, COORDS c2){
    return abs(c1.x - c2.x) + abs(c1.y - c2.y);
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
    int minPoints = INT32_MAX;

    for(auto i : covers){
        COORDS pos = i.c;

        if(agent[id].c.x < game.width/2){
            if(pos.x == 0) continue;
            pos.x--;
        }else{
            if(pos.x + 1 >= game.width) continue;
            pos.x++;
        }

        if(pos.x < 0 || pos.x >= game.width || pos.y < 0 || pos.y >= game.height) continue;
        if(map[pos.x][pos.y].type != 0) continue;

        bool coverIsTakenAlready = false;
        for(auto &[j_id, j] : agent){
            if(j.mine && j.target.x != -1 && pos.x == j.target.x && pos.y == j.target.y){
                coverIsTakenAlready = true;
                break;
            }
        }
        if(coverIsTakenAlready) continue;

        int points = distance(pos, {game.width/2, agent[id].c.y});
        if(points < minPoints && abs(agent[id].c.x - pos.x) < game.width/2 - 1){
            minPoints = points;
            target = pos;
        }
    }
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
    if(start.x == end.x && start.y == end.y){
        return start;
    }
    
    COORDS selectedRoutesFirstMove = start;
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
                if(q.firstMove.x == -1){
                    selectedRoutesFirstMove = q.c;
                }else{
                    selectedRoutesFirstMove = q.firstMove;
                }
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

vector<vector<int>> mapAgents(GAME& game, unordered_map<int, AGENT>& agent){
    vector<vector<int>> mapAgent(game.width, vector<int>(game.height, 0));
    for(auto& [id, i] : agent){
        if(!i.dead){
            if(i.mine){
                mapAgent[i.c.x][i.c.y] = 1;
            }else{
                mapAgent[i.c.x][i.c.y] = 2;
            }
        }
    }
    return mapAgent;
}

vector<vector<int>> mapNextMoveAgents(GAME& game, unordered_map<int, AGENT>& agent){
    vector<vector<int>> mapNextMoveAgent(game.width, vector<int>(game.height, 0));
    for(auto& [id, i] : agent){
        if(!i.dead){
            if(i.nextMove.x >= 0 && i.nextMove.x < game.width && i.nextMove.y >= 0 && i.nextMove.y < game.height){
                if(i.mine){
                    mapNextMoveAgent[i.nextMove.x][i.nextMove.y] = 1;
                }else{
                    mapNextMoveAgent[i.nextMove.x][i.nextMove.y] = 2;
                }
            }else{
                if(i.mine){
                    mapNextMoveAgent[i.c.x][i.c.y] = 1;
                }else{
                    mapNextMoveAgent[i.c.x][i.c.y] = 2;
                }
            }
            
        }
    }
    return mapNextMoveAgent;
}

int bombDamage(vector<vector<int>>& mapAgent, GAME& game, COORDS bombDetonation){
    int bombDamage = 0;
    COORDS dir[9] = {{0, 0}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};
            for(size_t p = 0; p < 9; p++){
                if(bombDetonation.x + dir[p].x >= 0 && bombDetonation.x + dir[p].x < game.width && bombDetonation.y + dir[p].y >= 0 && bombDetonation.y + dir[p].y < game.height){
                    if(mapAgent[bombDetonation.x + dir[p].x][bombDetonation.y + dir[p].y] == 2){
                        bombDamage++;
                    }
                    if(mapAgent[bombDetonation.x + dir[p].x][bombDetonation.y + dir[p].y] == 1){
                        return -1;
                    }
                }
            }
    return bombDamage;
}

vector<vector<int>> mapBombs(vector<vector<TILE>>& map, GAME& game, unordered_map<int, AGENT>& agent, vector<vector<int>>& mapAgent){
    vector<vector<int>> mapOfBombUsage(game.width, vector<int>(game.height, 0));

    for(auto& [id, i] : agent){
        if(!i.mine && !i.dead){
            COORDS dir[9] = {{0, 0}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};
            for(size_t p = 0; p < 9; p++){
                if(i.c.x + dir[p].x >= 0 && i.c.x + dir[p].x < game.width && i.c.y + dir[p].y >= 0 && i.c.y + dir[p].y < game.height){
                    if(mapOfBombUsage[i.c.x + dir[p].x][i.c.y + dir[p].y] == 0){
                        mapOfBombUsage[i.c.x + dir[p].x][i.c.y + dir[p].y] = bombDamage(mapAgent, game, {i.c.x + dir[p].x, i.c.y + dir[p].y});
                    }
                }
            }
        }
    }

    return mapOfBombUsage;
}

COORDS canBomb(vector<vector<int>>& mapBombUsage, COORDS pos){
    COORDS toBomb = {-1, -1};
    int maxToBomb = -1;
    for (int y = 0; y < mapBombUsage[0].size(); y++){
        for (int x = 0; x < mapBombUsage.size(); x++){
            if(mapBombUsage[x][y] > 0 && maxToBomb < mapBombUsage[x][y] && distance(pos, {x, y}) <= 4){
                maxToBomb = mapBombUsage[x][y];
                toBomb = {x, y};
            }   
        }
    }
    return toBomb;
}

int canShoot(unordered_map<int, AGENT>& agent, int id){
    int toShoot = -1;
    int maxDamage = 0;
    for(auto [i_id, i] : agent){
        if(!i.mine && !i.dead){
            int damage = agent[id].power;
            agent[id].c = agent[id].nextMove;
            if(distance(agent[id].c, i.c) > agent[id].range){
                if(distance(agent[id].c, i.c) > agent[id].range*2){
                    damage = 0;
                }else{
                    damage = agent[id].power/2;
                }
            }
            if(i.wetness + damage >= 100){
                toShoot = i_id;
                break;
            }
            if(maxDamage < damage){
                maxDamage = damage;
                toShoot = i_id;
            }
        }
    }
    return toShoot;
}

void findNextMove(GAME& game, vector<vector<TILE>>& map, vector<COVER>& covers, unordered_map<int, AGENT>& agent, int id, vector<vector<int>>& mapBombUsage){
    // 1 action
    agent[id].target = findTarget(game, map, covers, agent, id);
    agent[id].nextMove = selectRoute(game, map, agent[id].c, agent[id].target);
    agent[id].move = ";MOVE " + to_string(agent[id].nextMove.x) + ' ' + to_string(agent[id].nextMove.y);
    vector<vector<int>> mapNextMoveAgent = mapNextMoveAgents(game, agent);

    // 2 action
    mapBombUsage = mapBombs(map, game, agent, mapNextMoveAgent);
    COORDS toBomb = canBomb(mapBombUsage, agent[id].c);
    if(toBomb.x != -1 && agent[id].bombs > 0){
        agent[id].move += ";THROW " + to_string(toBomb.x) + ' ' + to_string(toBomb.y);
    }else{
        int toShoot = canShoot(agent, id);
        if(toShoot != -1 && agent[id].cooldown == 0){
            agent[id].move += ";SHOOT " + to_string(toShoot);
        }
    }
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
    vector<vector<int>> mapBombUsage(game.width, vector<int>(game.height));
    vector<vector<int>> mapAgent(game.width, vector<int>(game.height));

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
        
        agentMineCheck(agent, game);
        //mapAgent = mapAgents(game, agent);
        for(auto& [id, i] : agent){
            if(i.mine && !i.dead){
                findNextMove(game, map, covers, agent, id, mapBombUsage);
            }
        }
        executeMoves(agent);

        // Debug
        PRINT(agent);
        PRINT(covers);
        PRINT(mapAgent);
        PRINT(mapBombUsage);
    }
}