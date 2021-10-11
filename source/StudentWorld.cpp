#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <sstream>

using namespace std;

GameWorld *createStudentWorld(string assetPath) {
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
        : GameWorld(assetPath), m_last_border_Y(0), m_SoulSaved(0) {
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::init() {
    m_SoulSaved = 0;
    m_last_border_Y = 0;
    m_bonus = 5000;
    m_GhostRacer = new GhostRacer(this, 128, 32);
    m_Actors.push_back(m_GhostRacer);
    constructBorderLines();
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() {
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    int m_Actors_size = m_Actors.size();
    for (int i = 0; i < m_Actors_size; i++) {
        m_Actors[i]->doSomething();
        if (m_GhostRacer->isDead()) {
            decLives();
            // std::cout << "Player died" << std::endl;
            return GWSTATUS_PLAYER_DIED;
        }
    }
    vector<Actor *>::iterator it = m_Actors.begin();
    while (it != m_Actors.end()) {
        if ((*it)->isDead()) {
            // std::cout << (*it)->getY() << std::endl;
            delete (*it);
            it = m_Actors.erase(it);
        } else
            it++;
    }
    m_last_border_Y += -4 - m_GhostRacer->getVerticalSpeed();
    addEverything();
    ostringstream oss;
    oss << "Score: " << getScore() << "  Lvl: " << getLevel() << "  Souls2Save: " << 2 * getLevel() + 5 - m_SoulSaved
        << "  Lives: " << getLives()
        << "  Health: " << getGhostRacer()->getHP() << "  Sprarys: " << getGhostRacer()->getNumSprays() << "  Bonus: "
        << m_bonus;
    setGameStatText(oss.str());
    if (m_SoulSaved >= 2 * getLevel() + 5) {
        increaseScore(m_bonus);
        return GWSTATUS_FINISHED_LEVEL;
    }
    m_bonus = max(0, m_bonus - 1);
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() {
    vector<Actor *>::iterator it = m_Actors.begin();
    while (it != m_Actors.end()) {
        delete (*it);
        it = m_Actors.erase(it);
    }
}

GhostRacer *StudentWorld::getGhostRacer() {
    return m_GhostRacer;
}

void StudentWorld::addActor(Actor *a) {
    if (a != nullptr)
        m_Actors.push_back(a);
}

bool StudentWorld::sprayFirstAppropriateActor(Actor *a) {
    for (vector<Actor *>::iterator it = m_Actors.begin(); it != m_Actors.end(); it++)
        if (overlaps(*it, a) && (*it)->beSprayedIfAppropriate())
            return true;
    return false;
}

void StudentWorld::recordSoulSaved() {
    m_SoulSaved++;
}

bool StudentWorld::overlaps(const Actor *a1, const Actor *a2) const {
    if (a1 == nullptr || a2 == nullptr)
        return false;
    double delta_x = a1->getX() - a2->getX();
    double delta_y = a1->getY() - a2->getY();
    double radius_sum = a1->getRadius() + a2->getRadius();
    if (delta_x < 0)
        delta_x = -delta_x;
    if (delta_y < 0)
        delta_y = -delta_y;
    return (delta_x < radius_sum * 0.25 && delta_y < radius_sum * 0.6);
}

bool StudentWorld::overlapsGhostRacer(Actor *a) const {
    return overlaps(a, m_GhostRacer);
}

bool StudentWorld::outOfScreen(Actor *caller) const {
    double x = caller->getX();
    double y = caller->getY();
    return (x < 0 || x > VIEW_WIDTH || y < 0 || y > VIEW_HEIGHT);
}

void StudentWorld::constructYellowBorderLines() {
    int N = VIEW_HEIGHT / SPRITE_HEIGHT;
    int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
    int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
    for (int i = 0; i < N; i++) {
        BorderLine *newLeftBorderLine = new BorderLine(this, LEFT_EDGE, i * SPRITE_HEIGHT, true);
        BorderLine *newRightBorderLine = new BorderLine(this, RIGHT_EDGE, i * SPRITE_HEIGHT, true);
        m_Actors.push_back(newLeftBorderLine);
        m_Actors.push_back(newRightBorderLine);
    }
}

void StudentWorld::constructWhiteBorderLines() {
    int M = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);
    int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
    int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
    int LEFT_LINE = LEFT_EDGE + ROAD_WIDTH / 3;
    int RIGHT_LINE = RIGHT_EDGE - ROAD_WIDTH / 3;
    for (int i = 0; i < M; i++) {
        BorderLine *newLeftBorderLine = new BorderLine(this, LEFT_LINE, i * (4 * SPRITE_HEIGHT), false);
        BorderLine *newRightBorderLine = new BorderLine(this, RIGHT_LINE, i * (4 * SPRITE_HEIGHT), false);
        m_Actors.push_back(newLeftBorderLine);
        m_Actors.push_back(newRightBorderLine);
    }
}

void StudentWorld::addEverything() {
    addNewBorderLines();
    addNewZombieCabs();
    addNewOilSlicks();
    addNewZombiePed();
    addNewHumanPed();
    addNewWaterRefill();
    addNewLostSoul();
}

void StudentWorld::addNewBorderLines() {
    int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    int delta_y = new_border_y - m_last_border_Y;
    if (delta_y >= SPRITE_HEIGHT) {
        Actor *newYellowLeftBorderLine =
                new BorderLine(this, ROAD_CENTER - ROAD_WIDTH / 2.0, new_border_y, true);
        Actor *newYellowRightBorderLine =
                new BorderLine(this, ROAD_CENTER + ROAD_WIDTH / 2.0, new_border_y, true);

        m_Actors.push_back(newYellowLeftBorderLine);
        m_Actors.push_back(newYellowRightBorderLine);
    }
    if (delta_y >= 4 * SPRITE_HEIGHT) {
        Actor *newWhiteLeftBorderLine =
                new BorderLine(this, ROAD_CENTER - ROAD_WIDTH / 2.0 + ROAD_WIDTH / 3.0, new_border_y, false);
        Actor *newWhiteRightBorderLine =
                new BorderLine(this, ROAD_CENTER + ROAD_WIDTH / 2.0 - ROAD_WIDTH / 3.0, new_border_y, false);
        m_last_border_Y = new_border_y;
        m_Actors.push_back(newWhiteLeftBorderLine);
        m_Actors.push_back(newWhiteRightBorderLine);
    }
}

void StudentWorld::addNewZombieCabs() {
    int ChanceVehicle = max(100 - getLevel() * 10, 20);

    if (randInt(0, ChanceVehicle - 1) == 0) {
        int lane_num = randInt(0, 2);
        bool found_lane = false;
        double startX = 0;
        double startY = 0;
        double init_vert_speed = 0;
        for (int i = 0; i < 3; i++) {
            double coord_x = ROAD_CENTER + (lane_num - 1) * (ROAD_WIDTH / 3.0);
            double coord_y = 0;
            Actor *closestToBottom = findSameLaneAppropriateActor(coord_x, coord_y, true);
            if (closestToBottom == nullptr || closestToBottom->getY() > (VIEW_HEIGHT / 3.0)) {
                found_lane = true;
                startX = coord_x;
                startY = SPRITE_HEIGHT / 2.0;
                init_vert_speed = getGhostRacer()->getVerticalSpeed() + randInt(2, 4);
                break;
            }
            coord_y = VIEW_HEIGHT;
            Actor *closestToTop = findSameLaneAppropriateActor(coord_x, coord_y, false);
            if (closestToTop == nullptr || closestToTop->getY() < (VIEW_HEIGHT * 2 / 3.0)) {
                found_lane = true;
                startX = coord_x;
                startY = VIEW_HEIGHT - SPRITE_HEIGHT / 2.0;
                init_vert_speed = getGhostRacer()->getVerticalSpeed() - randInt(2, 4);
                break;
            }
            lane_num = (lane_num + 1) % 3;
        }
        if (found_lane) {
            Actor *new_ZombieCab = new ZombieCab(this, startX, startY);
            new_ZombieCab->setVerticalSpeed(init_vert_speed);
            addActor(new_ZombieCab);
        }
    }
}

void StudentWorld::addNewOilSlicks() {
    int ChanceOilSlick = max(150 - getLevel() * 10, 40);
    if (randInt(0, ChanceOilSlick - 1) == 0) {
        Actor *new_OilSlick = new
                OilSlick(this, randInt(ROAD_CENTER - ROAD_WIDTH / 2.0, ROAD_CENTER + ROAD_WIDTH / 2.0), VIEW_HEIGHT);
        addActor(new_OilSlick);
    }
}

void StudentWorld::addNewZombiePed() {
    int ChanceZombiePed = max(100 - getLevel() * 10, 20);
    if (randInt(0, ChanceZombiePed - 1) == 0) {
        Actor *new_ZombiePed = new
                ZombiePedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT);
        addActor(new_ZombiePed);
    }
}

void StudentWorld::addNewHumanPed() {
    int ChanceHumanPed = max(200 - getLevel() * 10, 30);
    if (randInt(0, ChanceHumanPed - 1) == 0) {
        Actor *new_HumanPed = new
                HumanPedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT);
        addActor(new_HumanPed);
    }
}

void StudentWorld::addNewWaterRefill() {
    int ChanceOfWaterRefill = 100 + 10 * getLevel();
    if (randInt(0, ChanceOfWaterRefill - 1) == 0) {
        Actor *new_WaterRefill = new
                HolyWaterGoodie(this, randInt(ROAD_CENTER - ROAD_WIDTH / 2.0, ROAD_CENTER + ROAD_WIDTH / 2.0),
                                VIEW_HEIGHT);
        addActor(new_WaterRefill);
    }
}

void StudentWorld::addNewLostSoul() {
    int ChanceOFLostSoul = 100;
    if (randInt(0, ChanceOFLostSoul - 1) == 0) {
        Actor *new_LostSoul = new
                SoulGoodie(this, randInt(ROAD_CENTER - ROAD_WIDTH / 2.0, ROAD_CENTER + ROAD_WIDTH / 2.0),
                           VIEW_HEIGHT);
        addActor(new_LostSoul);
    }
}

int StudentWorld::findLane(double x, double y) {
    if (x < ROAD_CENTER - ROAD_WIDTH || x > ROAD_CENTER + ROAD_WIDTH || y < 0 || y > VIEW_HEIGHT)
        return -1;
    if (x >= ROAD_CENTER - ROAD_WIDTH && x < ROAD_CENTER - ROAD_WIDTH / 2.0 + ROAD_WIDTH / 3.0)
        return 0;
    if (x >= ROAD_CENTER - ROAD_WIDTH / 2.0 + ROAD_WIDTH / 3.0 && x < ROAD_CENTER + ROAD_WIDTH / 2.0 - ROAD_WIDTH / 3.0)
        return 1;
    if (x >= ROAD_CENTER + ROAD_WIDTH / 2.0 - ROAD_WIDTH / 3.0 && x < ROAD_CENTER + ROAD_WIDTH / 2.0)
        return 2;
    return -1;
}

Actor *StudentWorld::findSameLaneAppropriateActor(double x, double y, bool isInFront, Actor *self) {
    int lane_num = findLane(x, y);
    if (lane_num == -1)
        return nullptr;
    Actor *closestInFront = nullptr;
    Actor *closestBehind = nullptr;
    for (vector<Actor *>::iterator it = m_Actors.begin(); it != m_Actors.end(); it++)
        if ((*it)->isCollisionAvoidanceWorthy() && (*it) != self)
            if (findLane((*it)->getX(), (*it)->getY()) == lane_num) {
                if (isInFront && (*it)->getY() >= y &&
                    (closestInFront == nullptr || (*it)->getY() < closestInFront->getY()))
                    closestInFront = *it;
                else if ((*it)->getY() <= y && (closestBehind == nullptr || (*it)->getY() > closestBehind->getY()))
                    closestBehind = *it;
            }
    if (isInFront)
        return closestInFront;
    else
        return closestBehind;
}