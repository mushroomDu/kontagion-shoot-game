#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>

class Socrates;
class Actor;
class Dirt;
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld(){cleanUp();}
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void create(Actor* a);
    void decNumbacteria(){numbacteria--;};
    void incNumbacteria(int i){numbacteria+=i;};
    Socrates* getSocrates(){return player;}
    vector<Actor*>* getActors(){
        return &actors;
    }
private:
    Socrates* player;
    vector<Actor*> actors;
    int numbacteria;
    
};

#endif // STUDENTWORLD_H_
