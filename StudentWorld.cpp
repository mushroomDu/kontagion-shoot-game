#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include "Actor.h"
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    
}

int StudentWorld::init()
{
    
    numbacteria = 0;
    player = new Socrates(this);
    actors.push_back(player);
    for (int i  = 0; i< max(180-20 * getLevel(), 20);i++){
        
        double r = 120*((double)rand() / (double)RAND_MAX);
        double dir = 8*atan(1)*((double)rand() / (double)RAND_MAX);
        
        Dirt* d = new Dirt(this,VIEW_WIDTH/2 +r*cos(dir) ,VIEW_HEIGHT/2 + r*sin(dir));
        actors.push_back(d);
    }
    int pitNum = 0;
    while (pitNum<getLevel()){
        double r = randInt(0, 120);
        double d = 4*atan(1)*randInt(0, 359)/180;
        bool overlapped = false;
        for (int i = 0; i< actors.size();i++){
            if (actors[i]->isOverlap(VIEW_WIDTH/2 +r*cos(d),VIEW_HEIGHT/2 +r*sin(d))){
                overlapped = true;
                break;
            }
        }
        if (overlapped) continue;
        Pits* p = new Pits(this, VIEW_WIDTH/2 +r*cos(d), VIEW_HEIGHT/2 +r*sin(d));
        actors.push_back(p);
        pitNum++;
    }
    
    
    int foodNum = 0;
    while(foodNum<min(5 * getLevel(), 25)){
        double r = randInt(0, 120);
        double d = 4*atan(1)*randInt(0, 359)/180;
        bool overlapped = false;
        for (int i = 0; i< actors.size();i++){
            if (actors[i]->isOverlap(VIEW_WIDTH/2 +r*cos(d),VIEW_HEIGHT/2 +r*sin(d))){
                overlapped = true;
                break;
            }
        }
        if (overlapped) continue;
        Food* f = new Food(this, VIEW_WIDTH/2 +r*cos(d), VIEW_HEIGHT/2 +r*sin(d));
        actors.push_back(f);
        foodNum++;
    }
    
   
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    player->doSomething();
    
    for (int i = 0; i< actors.size(); i++){
        actors[i]->doSomething();
    }
    string scoreText = to_string(getScore());
    while (scoreText.length()<6){scoreText = "0"+scoreText;}
    setGameStatText("Score: "+scoreText+
                    " Level: "+to_string(getLevel())+
                    " Lives: "+to_string(getLives())+
                    " health: "+to_string(player->getHp())+
                    " Sprays: "+to_string(player->getSpray())+
                    " Flames: "+to_string(player->getFlame()));
    
    if (!player->isAlive()){
        decLives();
        cleanUp();
        return GWSTATUS_PLAYER_DIED;
    }
    
    
    
    vector<Actor*>::iterator it = actors.begin();
    while (it != actors.end()){
        if (!(*it)->isAlive()) {
            delete *it;
            it = actors.erase(it);
        }else it++;
    }
    
    int ChanceFungus = max(510-(getLevel() * 10), 200);
    int f = randInt(0, ChanceFungus-1);
    if (f==0){
        double r = VIEW_RADIUS;
        double d = 4*atan(1)*randInt(0, 359)/180;
        actors.push_back(new Fungus(this, VIEW_WIDTH/2 +r*cos(d), VIEW_HEIGHT/2 +r*sin(d),getLevel()));
    }
    
    int ChanceGoodie = max(510-(getLevel() * 10), 250);
    f = randInt(0, ChanceFungus-1);
    if (f==0){
        double r = VIEW_RADIUS;
        double d = 4*atan(1)*randInt(0, 359)/180;
        f = randInt(0, 100);
        if (f<=60){
            actors.push_back(new Extra_health(this, VIEW_WIDTH/2 +r*cos(d), VIEW_HEIGHT/2 +r*sin(d),getLevel()));
        }else if (f>=90){
            actors.push_back(new Extra_life(this, VIEW_WIDTH/2 +r*cos(d), VIEW_HEIGHT/2 +r*sin(d),getLevel()));
        }else{
            actors.push_back(new FlameThrower(this, VIEW_WIDTH/2 +r*cos(d), VIEW_HEIGHT/2 +r*sin(d),getLevel()));
        }
        
    }
    
    if (numbacteria<=0){
        cleanUp();
        playSound(SOUND_FINISHED_LEVEL);
        
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}


void StudentWorld::create(Actor* a){
    actors.push_back(a);
}
void StudentWorld::cleanUp()
{
//    if (player!=nullptr)delete player;
    for (int i = 0; i< actors.size(); i++){
        delete actors[i];
        actors[i] = nullptr;
    }
    actors.clear();
}
