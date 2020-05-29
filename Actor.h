#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
#include <cmath>
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp


//GraphObject(
//            int imageID,
//            double startX,
//            double startY,
//            Direction dir = 0,
//            int depth = 0,
//            double size = 1.0)

class Actor: public GraphObject{
public:
    virtual void playSoundHurt(){};
    Actor(StudentWorld* w,
          int initHp,
          int imageID,
          double startX,
          double startY,
          Direction dir = 0,
          int depth = 0,
          double size = 1.0):GraphObject(imageID,startX,startY,dir,depth,size),hp(initHp),sw(w){}
    virtual ~Actor(){}
    virtual void doSomething() = 0;
    bool isAlive()const {return hp>0;}
    virtual bool isDamageable()const=0;
    virtual string type()const=0;
    void setHp(int hpInput){hp = hpInput;}
    int getHp() const {return hp;}
    double distance(const Actor* a2){
        return sqrt(pow(getX()-a2->getX(),2) + pow(getY()-a2->getY(),2));
    }
    double distance(double x,double y){
        return sqrt(pow(getX()-x,2) + pow(getY()-y,2));
    }
    bool isOverlap(const Actor* a2){
        return pow(getX()-a2->getX(),2) + pow(getY()-a2->getY(),2) <= pow(SPRITE_WIDTH,2);
    }
    bool isOverlap(double x,double y){
        return pow(getX()-x,2) + pow(getY()-y,2) <= pow(SPRITE_WIDTH,2);
    }
    StudentWorld* getWorld(){return sw;}
private:
    int hp;
    StudentWorld* sw;
};
class Projectiles: public Actor{
public:
    Projectiles(StudentWorld* w,int IID,int travel,int x, int y , int dir,int do_dmg):Actor(w,1,IID,x,y,dir,1),traveled(0),max_travel(travel),sw(w),dmg(do_dmg){}
    virtual bool isDamageable() const{return false;};
    virtual void playSoundHurt(){};
    virtual void doSomething(){
        //act(modify data in StudentWorld) based on StudentWorld
        if (!isAlive()) return;
        
        for (int i = 0; i< sw->getActors()->size();i++){
            if ((sw->getActors())->at(i)->isDamageable()
                &&(sw->getActors())->at(i)->type()!="Socrates"){
                if (isOverlap((sw->getActors())->at(i))){
                    setHp(0);
                    (sw->getActors())->at(i)->setHp((sw->getActors())->at(i)->getHp()-dmg);
                    if ((sw->getActors())->at(i)->isAlive()) (sw->getActors())->at(i)->playSoundHurt();
                    return;
                }
            }
            
            
        }
        
        //3
        if (traveled >= max_travel){
            setHp(0);
            return;
        }
        moveAngle(getDirection(),SPRITE_RADIUS*2);
        traveled +=SPRITE_RADIUS*2;
    }
    
   
private:
    double traveled;
    double max_travel;
    int dmg;
    StudentWorld* sw;
};
class Flame: public Projectiles{
public:
    virtual string type()const{return "Flame";}
    Flame(StudentWorld* sw,int x, int y , int dir):Projectiles(sw,IID_FLAME,32,x,y,dir,5){}
};
class Spray: public Projectiles{
public:
    virtual string type()const{return "Spray";}
    Spray(StudentWorld* sw,int x, int y , int dir):Projectiles(sw,IID_SPRAY,112,x,y,dir,2){}
};
class Socrates: public Actor{
public:
    virtual string type()const{return "Socrates";}
    Socrates(StudentWorld* stuW):
    Actor(stuW,100,IID_PLAYER,0,128,0,0),spray(20),flame(5){
    }
    virtual bool isDamageable() const{return true;}
    virtual void playSoundHurt(){
        getWorld()->playSound(SOUND_PLAYER_HURT);
    };
    int getSpray()const{return spray;}
    int getFlame()const{return flame;}
    void setFlame(int x){flame = x;};
    virtual ~Socrates(){
        getWorld()->playSound(SOUND_PLAYER_DIE);
    }
    
    virtual void doSomething(){
        //act(modify data in StudentWorld) based on StudentWorld and user input
        
        if (getHp()<=0|| !isAlive()){
            setHp(0);
            return;
        }
        int ch;
        if (getWorld()->getKey(ch)){
            switch (ch)
            {
                case KEY_PRESS_SPACE:
                    if (spray>0){
                        getWorld()->create(new Spray(getWorld(),getX(),getY(),getDirection()));
                        spray--;
                        getWorld()->playSound(SOUND_PLAYER_SPRAY);
                    }
                    break;
                case KEY_PRESS_ENTER:
                    if (flame > 0){
                        for (int i = 0; i< 16; i++){
                            Flame* f =new Flame(getWorld(),getX(),getY(),getDirection()+i*22.0);
                            f->moveAngle(f->getDirection());
                            getWorld()->create(f);
                        }
                        flame--;
                        getWorld()->playSound(SOUND_PLAYER_FIRE);
                    }
                    break;
                case KEY_PRESS_LEFT:
                    moveAngle(getDirection(),VIEW_RADIUS);
                    setDirection((getDirection()+5+180)%360);
                    moveAngle(getDirection(),VIEW_RADIUS);
                    setDirection((getDirection()+180)%360);
                    break;
                case KEY_PRESS_RIGHT:
                    moveAngle(getDirection(),VIEW_RADIUS);
                    setDirection((getDirection()-5+180)%360);
                    moveAngle(getDirection(),VIEW_RADIUS);
                    setDirection((getDirection()+180)%360);
                    break;
            }
        }
        else spray =20;
        if (spray<20)spray ++;
        
    }
    /*What Socrates Must Do In Other Circumstances
     • Socrates can be damaged. If a fungus object, or any type of bacteria overlap with Socrates they will cause various amounts of damage, reducing Socrates’ hit points.
        o Socrates must lower his hit points based on the amount of damage specified by the other actor.
        o If Socrates is damaged (but not killed), he must play a SOUND_PLAYER_HURT sound effect.
        o Otherwise, if Socrates hit points reach zero (or below), the Socrates object must:
            § Immediately has his status set to not-alive.
            § He must play a SOUND_PLAYER_DIE sound effect.
            § (The StudentWorld class should later detect his death and the
                current level ends)
     • Socrates does NOT block other objects from moving nearby/onto his location.*/
private:
    int spray;
    int flame;
};


class Dirt: public Actor{
public:
    Dirt(StudentWorld* w,int x,int y):Actor(w,1,IID_DIRT,x,y,0,1){}
    virtual ~Dirt(){}
    virtual void doSomething(){
        
    }
    virtual string type()const{return "Dirt";}
    virtual bool isDamageable() const{return true;}
};
/*
 What a Dirt Pile Must Do In Other Circumstances
 • Dirt piles can be damaged. A single spray or flame that hits the dirt pile will destroy it (and cause the spray/flame to dissipate once it has hit the pile).
 • Dirt piles block the movement of all bacteria (the Euclidean distance of the bacterium and of the dirt pile must not be less than SPRITE_WIDTH/2 pixels from each other).
 • Dirt piles block the movement of all spray and flames.
 */

class Food: public Actor{
public:
    Food (StudentWorld* w,int x, int y): Actor(w,1,IID_FOOD,x,y,90,1){}
    virtual ~Food(){}
    virtual void doSomething(){}
    virtual bool isDamageable() const{return false;}
    virtual string type()const{return "Food";}
};

/*
 What Food Must Do In Other Circumstances
 • Food cannot be damaged by spray or flames.
 • Food does not block the movement of bacteria.
 • Food does not block the movement of spray or flames.
 */




class Goodie: public Actor{
public:
    
    Goodie(StudentWorld* w,int IID,int L,int x, int y):Actor(w,1,IID, x,y,0,1),lifeTime(fmax(rand() % (300-10*L),50)){
    };
    virtual string type(){return "Goodie";};
    void decLifeTime(){lifeTime--;}
    int getLifeTime() const{return lifeTime;}
    virtual bool isDamageable() const{return true;}
private:
    int lifeTime;
};
class Extra_health: public Goodie{
public:
    virtual string type()const{return "Extra_health";}
    Extra_health(StudentWorld* w,int x, int y, int L): Goodie(w,IID_RESTORE_HEALTH_GOODIE,L,x,y){}
    virtual void doSomething(){
        if (!isAlive()||getLifeTime()<=0){
            setHp(0);
            return;
        }
        decLifeTime();
        if (isOverlap(getWorld()->getSocrates())){
            getWorld()->getSocrates()->setHp(100);
            getWorld()->increaseScore(250);
            setHp(0);
            getWorld()->playSound(SOUND_GOT_GOODIE);
        }
        
    }
    
    /*
     What Restore Health Goodies Must Do In Other Circumstances
     • Restore health goodies can be damaged by flames and sprays. When damaged they will set their status to dead and disappear from the game.
     • Restore health goodies do not block other objects from moving nearby/onto them.
     */
};

class FlameThrower: public Goodie{
public:
    virtual string type()const{return "FlameThrower";}
    FlameThrower(StudentWorld* w,int x, int y, int L): Goodie(w,IID_FLAME_THROWER_GOODIE,L,x,y){}
    virtual void doSomething(){
        if (!isAlive()||getLifeTime()<=0){
             setHp(0);
            return;
        }
        decLifeTime();
        if (isOverlap(getWorld()->getSocrates())){
            getWorld()->getSocrates()->setFlame(5);
            getWorld()->increaseScore(300);
            setHp(0);
            getWorld()->playSound(SOUND_GOT_GOODIE);
        }
        /*
         2. The flame thrower goodie must determine if it overlaps5 with Socrates. If so, then
            the flame thrower goodie must:
            a. Inform the StudentWorld object that the user is to receive 300 points.
            b. Set its state to dead (so that it will be removed from the game by the
                StudentWorld object at the end of the current tick).
            c. Play a sound effect to indicate that Socrates picked up the goodie:
                SOUND_GOT_GOODIE.
            d. Tell the Socrates object to add 5 flame thrower charges to its arsenal.
            e. Return immediately...
         3. The flame thrower goodie must check to see if its lifetime has expired, and if so, set its state to dead so it disappears from the game.
         */
    }
    
    /*
     What Flame thrower Goodies Must Do In Other Circumstances
     • Flame thrower goodies can be damaged by flames and sprays. When damaged they will set their status to dead and disappear from the game.
     • Flame thrower goodies do not block other objects from moving nearby/onto them.
     */
};
class Extra_life: public Goodie{
public:
    virtual string type()const{return "Extra_life";}
    Extra_life(StudentWorld* w,int x, int y, int L): Goodie(w,IID_EXTRA_LIFE_GOODIE,L,x,y){}
    virtual void doSomething(){
        if (!isAlive()||getLifeTime()<=0){
             setHp(0);
        }
        decLifeTime();
        if (isOverlap(getWorld()->getSocrates())){
            getWorld()->incLives();
            getWorld()->increaseScore(500);
            setHp(0);
            getWorld()->playSound(SOUND_GOT_GOODIE);
        }
        /*
         2. The extra life goodie must determine if it overlaps7 with Socrates. If so, then the extra life goodie must:
            a. Inform the StudentWorld object that the user is to receive 500 points.
            b. Set its state to dead (so that it will be removed from the game by the
                StudentWorld object at the end of the current tick).
            c. Play a sound effect to indicate that Socrates picked up the goodie:
                SOUND_GOT_GOODIE.
            d. Tell the StudentWorld class that the player has earned an extra life by calling
                StudentWorld::incLives() method (this method is inherited by StudentWorld
                from GameWorld).
            e. Return immediately...
         3. The extra life goodie must check to see if its lifetime has expired, and if so, set its state to dead so it disappears from the game.
         
         */
    }
    /*
     
     
     What Extra Life Goodies Must Do In Other Circumstances
     • Extra life goodies can be damaged by flames and sprays. When damaged they will set their status to dead and disappear from the game.
     • Extra life goodies do not block other objects from moving nearby/onto them.
     */
};

class Fungus: public Goodie{
public:
    virtual string type()const{return "Fungus";}
    Fungus(StudentWorld* w,int x, int y, int L): Goodie(w,IID_FUNGUS,L,x,y){}
    virtual void doSomething(){
        if (!isAlive()||getLifeTime()<=0){
            setHp(0);
        }
        decLifeTime();
        if (isOverlap(getWorld()->getSocrates())){
            getWorld()->getSocrates()->setHp(getWorld()->getSocrates()->getHp()-20);
            getWorld()->increaseScore(-50);
            setHp(0);
        }
    }

};





class Bacteria: public Actor{
public:
        Bacteria(StudentWorld* w,int IID, int x, int y,int HP):Actor(w,HP,IID,x,y,90,0),movePlan(0),foodEaten(0){
            getWorld()->playSound(SOUND_BACTERIUM_BORN);
        }
        virtual ~Bacteria(){
            getWorld()->increaseScore(100);
            getWorld()->decNumbacteria();
            if (randInt(0,1)==1){
                getWorld()->create(new Food(getWorld(),getX(),getY()));
            }
        }
    virtual bool isDamageable() const{return true;}
    int getEaten() const{return foodEaten;}
    bool isBlockedbyDirt(){
        bool wouldBeBlocked = false;
        for (int i = 0; i<getWorld()->getActors()->size(); i++){
            if (wouldBeBlocked) return true;
            if (getWorld()->getActors()->at(i)->type()!="Dirt") continue;
            wouldBeBlocked = movementOverlap(getWorld()->getActors()->at(i));
        }
        return wouldBeBlocked;
    }
    bool attack(int howmuch){
        if (isOverlap(getWorld()->getSocrates())){
            getWorld()->getSocrates()->setHp(getWorld()->getSocrates()->getHp()-howmuch);
            return true;
        }
        else return false;
    }
    bool eat(double&x, double&y){
        for (int i = 0; i<getWorld()->getActors()->size(); i++){
            if (getWorld()->getActors()->at(i)->type()=="Food"
                &&isOverlap(getWorld()->getActors()->at(i))){
                foodEaten++;
                x = -1;
                y = -1;
                getWorld()->getActors()->at(i)->setHp(0);
                if (foodEaten<3)return false;
                x = getX();
                y = getY();
                if (x<VIEW_WIDTH/2)x+=SPRITE_RADIUS;
                else if (x>VIEW_WIDTH/2)x-=SPRITE_RADIUS;
                if (y< VIEW_HEIGHT/2)y+=SPRITE_RADIUS;
                else if (y>VIEW_HEIGHT/2)y-=SPRITE_RADIUS;
                foodEaten=0;
                return true;
            }
        }
        return false;
        
    }
    int getPlan()const {return movePlan;}
    void setPlan(int newPlan) {movePlan = newPlan;}
    bool movementOverlap(Actor* DirtGrid){
        return distance(DirtGrid)<=SPRITE_WIDTH/2;
    }
private:
    int movePlan;
    int foodEaten;
};

class Salmonella: public Bacteria{
public:
    Salmonella(StudentWorld* w,int x, int y, int hp): Bacteria(w,IID_SALMONELLA,x,y,hp){}
    virtual void playSoundHurt(){
        getWorld()->playSound(SOUND_SALMONELLA_HURT);
    }
    void actNormal(int isAgressive);
    virtual ~Salmonella(){
        getWorld()->playSound(SOUND_SALMONELLA_DIE);
    }
    
};
class Regular_salmonella: public Salmonella{
public:
    virtual string type()const{
        return "Regular_salmonella";
    };
    Regular_salmonella(StudentWorld* w,int x, int y): Salmonella(w,x,y,4){}
    virtual ~Regular_salmonella(){}
    virtual void doSomething(){
        actNormal(0);
    }
};
class Aggressive_salmonella: public Salmonella{
public:
    virtual string type()const{return "Aggressive_salmonella";}
    Aggressive_salmonella(StudentWorld* w,int x,int y): Salmonella(w,x,y,10){
    }
    
    virtual ~Aggressive_salmonella(){}
    virtual void doSomething(){
        if (getHp()<=0 || !isAlive()){
            setHp(0);
            return;
        }
        if (distance(getWorld()->getSocrates())<=72){
            setDirection(180*(4*atan(1)+
                              atan2(getY()-getWorld()->getSocrates()->getY(),
                                    getX()-getWorld()->getSocrates()->getX()))
                              /(4*atan(1)));
            
            if (!attack(2)){
                double x = 0;
                double y = 0;
                if(eat(x,y)){
                    getWorld()->incNumbacteria(1);
                    getWorld()->create(new Aggressive_salmonella(getWorld(),x,y));
                }
            }
            if (!isBlockedbyDirt()) moveForward(3);
           
            
        }else{actNormal(1);}
        
        
        
        
        
    }
};
class E_coli: public Bacteria{
public:
    virtual string type()const{return "E_coli";}
    virtual void playSoundHurt(){
        getWorld()->playSound(SOUND_ECOLI_HURT);
    }
    
    E_coli(StudentWorld* w,int x, int y): Bacteria(w,IID_ECOLI,x,y,5){}
    virtual ~E_coli(){
        getWorld()->playSound(SOUND_ECOLI_DIE);
    }
    virtual void doSomething(){
        //act(modify data in StudentWorld) based on StudentWorld
        if (getHp()<=0 || !isAlive()){
            setHp(0);
            return;
        }
        bool attacked = false;
        setDirection(180*(4*atan(1)+
                          atan2(getY()-getWorld()->getSocrates()->getY(),
                                getX()-getWorld()->getSocrates()->getX()))
                     /(4*atan(1)));
        
            
        
        
        if (attack(4)){
                double x = 0;
                double y = 0;
            if(eat(x,y)){
                
                getWorld()->create(new E_coli(getWorld(),x,y));
                getWorld()->incNumbacteria(1);
            }
            
        }
        moveAngle(getDirection(),2);
        int tried=0;
        while(isBlockedbyDirt()&&tried<10){
            setDirection((180+getDirection())%360);
            moveAngle(getDirection(),2);
            setDirection((180+getDirection())%360);
            setDirection(10+getDirection());
            moveAngle(getDirection(),2);
            tried++;
        }
            
        if (isBlockedbyDirt()){
            setDirection((180+getDirection())%360);
            moveAngle(getDirection(),2);
            setDirection((180+getDirection())%360);
        }
        
    }

};


class Pits: public Actor{
public:
    virtual string type()const{return "Pits";}
    Pits(StudentWorld* w,int x, int y): Actor(w,1,IID_PIT,x,y,0,1),regular_salmonellas(5),aggressive_salmonellas(3),E_colis(2){
        
        getWorld()->incNumbacteria(regular_salmonellas+aggressive_salmonellas+E_colis);
        
        
    }
    virtual void doSomething(){
        if (regular_salmonellas+aggressive_salmonellas+E_colis<=0) {
            setHp(0);
            return;
        }
        
            
        switch (randInt(1,3)) {
            case 1:
                if (regular_salmonellas>0&&randInt(0, 50)==0){
                    getWorld()->create(new Regular_salmonella(getWorld(), getX(), getY()));
                    regular_salmonellas--;
                }
                    
                break;
            case 2:
                if (aggressive_salmonellas>0&&randInt(0, 50)==0){
                    getWorld()->create(new Aggressive_salmonella(getWorld(), getX(), getY()));
                    aggressive_salmonellas--;
                }
                    
                break;
            case 3:
                if (E_colis>0&&randInt(0, 50)==0){
                    getWorld()->create(new E_coli(getWorld(), getX(), getY()));
                    E_colis--;
                }
                    
                break;
            default:
                break;
        }

            
            
        
        
        /*
         1. The pit must determine whether or not it has emitted all of its bacteria (all 5 regular salmonella, 3 aggressive salmonella, and 2 E. coli have been generated and added to the Petri dish). If it has emitted all of its bacteria, it must:
         a. Inform the StudentWorld that it has emitted all of its bacteria10.
         b. Set its status to not-alive so it disappears from the game at the end of the
         current tick.
         2. Otherwise, there is a 1 in 50 chance that during a tick, a given pit will emit a
         bacterium. Assuming the current tick was chosen to emit bacteria into the Petri dish:
         a. The pit must randomly pick a type of bacterium to add to the Petri dish (it can
         only choose from amongst bacteria it hasn’t already run out of)
         b. It must create a new bacterium object of the appropriate type (regular
         salmonella, aggressive salmonella or E. coli) with an (x ,y) coordinate that is
         the same as the pit’s (x ,y) coordinate.
         c. It must add that new bacterium to StudentWorld.
         d. It must decrement the count of bacteria of the type it just emitted that it has
         left in its inventory.
         e. It (or some other object) must play a sound effect to indicate that the
         bacterium was just born: SOUND_BACTERIUM_BORN.
         */
    }
    virtual bool isDamageable() const{return false;}
    /*
     What Exits Must Do In Other Circumstances
     • Pits cannot be damaged by spray or flames.
     • Pits do not block the movement of sprays or flames.
     • Pits do not block other objects from moving nearby/onto them.
     */
    
private:
    //A pit starts with an initial inventory of 5 regular salmonella,
    //3 aggressive salmonella, and 2 E. coli.
    int regular_salmonellas;
    int aggressive_salmonellas;
    int E_colis;
};

#endif // ACTOR_H_
