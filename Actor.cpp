#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
void Salmonella:: actNormal(int isAgressive){
    //act(modify data in StudentWorld) based on StudentWorld
    if (getHp()<=0 || !isAlive()){
        setHp(0);
        return;
    }
    if (!attack(1+isAgressive)){
        double x = 0;
        double y = 0;
        
        if(eat(x,y)){
            if (!isAgressive) getWorld()->create(new Regular_salmonella(getWorld(),x,y));
            else getWorld()->create(new Aggressive_salmonella(getWorld(),x,y));
            getWorld()->incNumbacteria(1);
        }
    }
    if (getPlan()>0){
        double x = getX();
        double y = getY();
        moveAngle(getDirection(),3);
        bool wouldBeBlocked = isBlockedbyDirt()||(distance(VIEW_WIDTH/2,VIEW_HEIGHT/2)>=VIEW_RADIUS);
        
        if (wouldBeBlocked){
            moveTo(x, y);
            setPlan(10);
            setDirection(randInt(0,359));
        }else{
            setPlan(getPlan()-1);
            moveForward(3);
        }
    }else{
        for (int i = 0; i< getWorld()->getActors()->size(); i++){
            if (getWorld()->getActors()->at(i)->type()=="food"
                &&distance(getWorld()->getActors()->at(i))<= 128){
                setDirection(180*(
                                  4*atan(1)+
                                  atan2(getY()-getWorld()->getActors()->at(i)->getY(),
                                        getX()-getWorld()->getActors()->at(i)->getX()))
                             /(4*atan(1)));
                setPlan(10);
                return;
            }
        }
        
        setDirection(randInt(0,359));
        
        setPlan(10);
        return;
        
    }
    
}
