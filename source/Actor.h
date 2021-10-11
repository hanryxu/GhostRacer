#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject {
public:
    Actor(StudentWorld *sw, int imageID, double x, double y, double size, int dir, int depth);

    // Action to perform for each tick.
    virtual void doSomething() = 0;

    // Is this actor dead?
    bool isDead() const;

    // Mark this actor as dead.
    void setDead();

    // Get this actor's world
    StudentWorld *world() const;

    // Get this actor's vertical speed.
    double getVerticalSpeed() const;

    // Set this actor's vertical speed.
    void setVerticalSpeed(double speed);

    double getHorizSpeed() const;

    void setHorizSpeed(double speed);

    // If this actor is affected by holy water projectiles, then inflict that
    // affect on it and return true; otherwise, return false.
    virtual bool beSprayedIfAppropriate() = 0;

    // Does this object affect zombie cab placement and speed?
    virtual bool isCollisionAvoidanceWorthy() const = 0;

    // Adjust the x coordinate by dx to move to a position with a y coordinate
    // determined by this actor's vertical speed relative to GhostRacer's
    // vertical speed. Return true if the new position is within the view;
    // otherwise, return false, with the actor dead.
    bool moveRelativeToGhostRacerVerticalSpeed();

private:
    bool m_isDead;
    StudentWorld *m_world;
    double m_verticalSpeed;
    double m_HorizontalSpeed;
};

class BorderLine : public Actor {
public:
    BorderLine(StudentWorld *sw, double x, double y, bool isYellow);

    void doSomething() final;

    bool beSprayedIfAppropriate() final;

    bool isCollisionAvoidanceWorthy() const final;
};

class Agent : public Actor {
public:
    Agent(StudentWorld *sw, int imageID, double x, double y, double size, int dir, int hp, int depth);

    bool isCollisionAvoidanceWorthy() const final;

    // Get hit points.
    int getHP() const;

    // Increase hit points by hp.
    void getHP(int hp);

    // Do what the spec says happens when hp units of damage is inflicted.
    // Return true if this agent dies as a result, otherwise false.
    virtual bool takeDamageAndPossiblyDie(int hp);

    // What sound should play when this agent is damaged but does not die?
    virtual int soundWhenHurt() const = 0;

    // What sound should play when this agent is damaged and dies?
    virtual int soundWhenDie() const = 0;

    virtual int score() const;

private:
    int m_hp;
};

class GhostRacer : public Agent {
public:
    GhostRacer(StudentWorld *sw, double x, double y);

    bool beSprayedIfAppropriate() final;

    void doSomething() final;

    int soundWhenHurt() const final;

    int soundWhenDie() const final;

    // Increase the number of holy water projectiles the object has.
    void increaseSprays(int amt);

    // Spin as a result of hitting an oil slick.
    void spin();

    // How many holy water projectiles does the object have?
    int getNumSprays() const;

private:
    void sprayWater();

    int m_NumSprays;
};

class Pedestrian : public Agent {
public:
    Pedestrian(StudentWorld *sw, int imageID, double x, double y, double size);

private:
    int movement_plan_distance;

protected:
    // Move the pedestrian. If the pedestrian doesn't go off screen and
    // should pick a new movement plan, pick a new plan.
    void moveAndPossiblyPickPlan();
};

class HumanPedestrian : public Pedestrian {
public:
    HumanPedestrian(StudentWorld *sw, double x, double y);

    int soundWhenHurt() const final;

    int soundWhenDie() const final;

    void doSomething() final;

    bool beSprayedIfAppropriate() final;
};

class ZombiePedestrian : public Pedestrian {
public:
    ZombiePedestrian(StudentWorld *sw, double x, double y);

    int soundWhenHurt() const final;

    int soundWhenDie() const final;

    void doSomething() final;

    bool beSprayedIfAppropriate() final;

    bool takeDamageAndPossiblyDie(int hp);

    int score() const final;

private:
    int ticks_before_grunt;
};

class ZombieCab : public Agent {
public:
    ZombieCab(StudentWorld *sw, double x, double y);

    int soundWhenHurt() const final;

    int soundWhenDie() const final;

    bool takeDamageAndPossiblyDie(int hp);

    void doSomething() final;

    bool beSprayedIfAppropriate() final;

    int score() const final;

private:
    bool hasDamagedGhostRacer;
    int movement_plan_distance;
};

class Spray : public Actor {
public:
    Spray(StudentWorld *sw, double x, double y, int dir);

    bool isCollisionAvoidanceWorthy() const final;

    bool beSprayedIfAppropriate() final;

    void doSomething() final;

private:
    double travel_distance;
};

class GhostRacerActivatedObject : public Actor {
public:
    GhostRacerActivatedObject(StudentWorld *sw, int imageID, double x, double y, double size, int dir, int depth);

    bool isCollisionAvoidanceWorthy() const final;

    bool beSprayedIfAppropriate() final;

    void doSomething();

protected:
    // Do the object's special activity (increase health, spin GhostRacer, etc.)
    virtual void doActivity(GhostRacer *gr) = 0;

    // Return the object's increase to the score when activated.
    virtual int getScoreIncrease() const = 0;

    // Return the sound to be played when the object is activated.
    virtual int getSound() const = 0;

    // Return whether the object dies after activation.
    virtual bool selfDestructs() const = 0;

    // Return whether the object is affected by a holy water projectile.
    virtual bool isSprayable() const = 0;
};

class OilSlick : public GhostRacerActivatedObject {
public:
    OilSlick(StudentWorld *sw, double x, double y);

protected:
    void doActivity(GhostRacer *gr) final;

    int getScoreIncrease() const final;

    int getSound() const final;

    bool selfDestructs() const final;

    bool isSprayable() const final;
};

class HealingGoodie : public GhostRacerActivatedObject {
public:
    HealingGoodie(StudentWorld *sw, double x, double y);

protected:
    void doActivity(GhostRacer *gr) final;

    int getScoreIncrease() const final;

    int getSound() const final;

    bool selfDestructs() const final;

    bool isSprayable() const final;
};

class HolyWaterGoodie : public GhostRacerActivatedObject {
public:
    HolyWaterGoodie(StudentWorld *sw, double x, double y);

protected:
    void doActivity(GhostRacer *gr) final;

    int getScoreIncrease() const final;

    int getSound() const final;

    bool selfDestructs() const final;

    bool isSprayable() const final;
};

class SoulGoodie : public GhostRacerActivatedObject {
public:
    SoulGoodie(StudentWorld *sw, double x, double y);

    void doSomething();

protected:
    void doActivity(GhostRacer *gr) final;

    int getScoreIncrease() const final;

    int getSound() const final;

    bool selfDestructs() const final;

    bool isSprayable() const final;
};


#endif // ACTOR_H_
