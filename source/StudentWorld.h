#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <vector>

class Actor;

class GhostRacer;

class StudentWorld : public GameWorld {
public:
    StudentWorld(std::string assetDir);

    virtual ~StudentWorld();

    int init() final;

    int move() final;

    void cleanUp() final;

    // Return a pointer to the world's GhostRacer.
    GhostRacer *getGhostRacer();

    // Add an actor to the world.
    void addActor(Actor *a);

    // Record that a soul was saved.
    void recordSoulSaved();

    // If actor a overlaps some live actor that is affected by a holy water
    // projectile, inflict a holy water spray on that actor and return true;
    // otherwise, return false.  (See Actor::beSprayedIfAppropriate.)
    bool sprayFirstAppropriateActor(Actor *a);

    // Return true if actor a1 overlaps actor a2, otherwise false.
    bool overlaps(const Actor *a1, const Actor *a2) const;

    // If actor a overlaps this world's GhostRacer, return true
    // otherwise return false
    bool overlapsGhostRacer(Actor *a) const;

    bool outOfScreen(Actor *caller) const;

    Actor *findSameLaneAppropriateActor(double x, double y, bool isInFront, Actor *self = nullptr);

private:
    GhostRacer *m_GhostRacer;
    std::vector<Actor *> m_Actors;
    int m_last_border_Y;
    int m_SoulSaved;
    int m_bonus;

    void constructYellowBorderLines();

    void constructWhiteBorderLines();

    void constructBorderLines() {
        constructWhiteBorderLines();
        constructYellowBorderLines();
    }

    void addEverything();

    void addNewBorderLines();

    void addNewZombieCabs();

    void addNewOilSlicks();

    void addNewZombiePed();

    void addNewHumanPed();

    void addNewWaterRefill();

    void addNewLostSoul();

    int findLane(double x, double y);
};

#endif // STUDENTWORLD_H_
