#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// Actor Implementations

Actor::Actor(StudentWorld *sw, int imageID, double x, double y, double size, int dir, int depth)
        : GraphObject(imageID, x, y, dir, size, depth), m_isDead(false), m_world(sw) {
}

bool Actor::isDead() const {
    return m_isDead;
}

void Actor::setDead() {
    m_isDead = true;
}

StudentWorld *Actor::world() const {
    return m_world;
}

double Actor::getVerticalSpeed() const {
    return m_verticalSpeed;
}

void Actor::setVerticalSpeed(double speed) {
    m_verticalSpeed = speed;
}

double Actor::getHorizSpeed() const {
    return m_HorizontalSpeed;
}

void Actor::setHorizSpeed(double speed) {
    m_HorizontalSpeed = speed;
}

bool Actor::moveRelativeToGhostRacerVerticalSpeed() {
    if (isDead())
        return false;
    double vert_speed = getVerticalSpeed() - world()->getGhostRacer()->getVerticalSpeed();
    double horiz_speed = getHorizSpeed();
    double new_y = getY() + vert_speed;
    double new_x = getX() + horiz_speed;
    moveTo(new_x, new_y);
    if (world()->outOfScreen(this))
        setDead();
    return (!isDead());
}

// BorderLine Implementations

BorderLine::BorderLine(StudentWorld *sw, double x, double y, bool isYellow)
        : Actor(sw, isYellow ? IID_YELLOW_BORDER_LINE : IID_WHITE_BORDER_LINE, x, y, 2.0, 0, 2) {
    setVerticalSpeed(-4);
    setHorizSpeed(0);
}

void BorderLine::doSomething() {
    moveRelativeToGhostRacerVerticalSpeed();
}

bool BorderLine::beSprayedIfAppropriate() {
    return false;
}

bool BorderLine::isCollisionAvoidanceWorthy() const {
    return false;
}
// Agent Implementations

Agent::Agent(StudentWorld *sw, int imageID, double x, double y, double size, int dir, int hp, int depth)
        : Actor(sw, imageID, x, y, size, dir, depth), m_hp(hp) {
}

bool Agent::isCollisionAvoidanceWorthy() const {
    return true;
}

int Agent::getHP() const {
    return m_hp;
}

void Agent::getHP(int hp) {
    m_hp += hp;
    if (m_hp > 100)
        // For GhostRacer
        m_hp = 100;
}

bool Agent::takeDamageAndPossiblyDie(int hp) {
    if (hp < 0)
        return false;
    getHP(-hp);
    if (getHP() <= 0) {
        setDead();
        world()->playSound(soundWhenDie());
        world()->increaseScore(score());
        return true;
    } else {
        world()->playSound(soundWhenHurt());
        return false;
    }
    return false;
}

int Agent::score() const {
    return 0;
}

// GhostRacer Implementations
GhostRacer::GhostRacer(StudentWorld *sw, double x, double y)
        : Agent(sw, IID_GHOST_RACER, x, y, 4.0, 90, 100, 0), m_NumSprays(10) {
    setVerticalSpeed(0);
    setHorizSpeed(0);
}

void GhostRacer::doSomething() {
    if (isDead())
        return;
    bool crashedOnBorder = false;
    if (getX() <= ROAD_CENTER - ROAD_WIDTH / 2.0) {
        if (getDirection() > 90)
            takeDamageAndPossiblyDie(10);
        setDirection(82);
        world()->playSound(SOUND_VEHICLE_CRASH);
        crashedOnBorder = true;
    }
    if (getX() >= ROAD_CENTER + ROAD_WIDTH / 2.0 && !crashedOnBorder) {
        if (getDirection() < 90)
            takeDamageAndPossiblyDie(10);
        setDirection(98);
        world()->playSound(SOUND_VEHICLE_CRASH);
        crashedOnBorder = true;
    }
    if (!crashedOnBorder) {
        int keyPressed = 0;
        if (world()->getKey(keyPressed)) {
            switch (keyPressed) {
                case KEY_PRESS_SPACE:
                    sprayWater();
                    break;
                case KEY_PRESS_LEFT:
                    if (getDirection() < 114)
                        setDirection(getDirection() + 8);
                    break;
                case KEY_PRESS_RIGHT:
                    if (getDirection() > 66)
                        setDirection(getDirection() - 8);
                    break;
                case KEY_PRESS_UP:
                    if (getVerticalSpeed() < 5)
                        setVerticalSpeed(getVerticalSpeed() + 1);
                    break;
                case KEY_PRESS_DOWN:
                    if (getVerticalSpeed() > -1)
                        setVerticalSpeed(getVerticalSpeed() - 1);
                    break;
                default:
                    break;
            }
        }
    }
    // move
    double max_shift_per_tick = 4.0;
    double direction = (static_cast<double>(getDirection()) / 180.0) * M_PI;
    double delta_x = cos(direction) * max_shift_per_tick;
    double cur_x = getX();
    double cur_y = getY();
    moveTo(cur_x + delta_x, cur_y);
}

int GhostRacer::soundWhenDie() const {
    return SOUND_PLAYER_DIE;
}

int GhostRacer::soundWhenHurt() const {
    // Just for pass
    return -1;
}

int GhostRacer::getNumSprays() const {
    return m_NumSprays;
}

void GhostRacer::increaseSprays(int amt) {
    m_NumSprays += amt;
}

void GhostRacer::spin() {
    int delta_dir = randInt(5, 20);
    int sign = randInt(0, 1) * 2 - 1;
    int new_dir = getDirection() + delta_dir * sign;
    new_dir = std::max(new_dir, 60);
    new_dir = std::min(new_dir, 120);
    setDirection(new_dir);
}

void GhostRacer::sprayWater() {
    if (getNumSprays() >= 1) {
        Spray *new_spray = new Spray(world(), getX(), getY(), getDirection());
        new_spray->moveForward(SPRITE_HEIGHT);
        m_NumSprays--;
        world()->playSound(SOUND_PLAYER_SPRAY);
        world()->addActor(new_spray);
    }
}

bool GhostRacer::beSprayedIfAppropriate() {
    return false;
}

// Pedestrian Implementation

Pedestrian::Pedestrian(StudentWorld *sw, int imageID, double x, double y, double size)
        : Agent(sw, imageID, x, y, size, 0, 2, 0), movement_plan_distance(0) {
    setHorizSpeed(0);
    setVerticalSpeed(-4);
}

void Pedestrian::moveAndPossiblyPickPlan() {
    if (!moveRelativeToGhostRacerVerticalSpeed())
        // became out-of-view
        return;
    // movement plan distance
    movement_plan_distance--;
    if (movement_plan_distance > 0)
        return;
    else {
        int random_sign = randInt(0, 1) * 2 - 1;
        setHorizSpeed(randInt(1, 3) * random_sign);
        movement_plan_distance = randInt(4, 32);
        setDirection(getHorizSpeed() > 0 ? 0 : 180);
    }
}

// HumanPedestrian Implementation
HumanPedestrian::HumanPedestrian(StudentWorld *sw, double x, double y) : Pedestrian(sw, IID_HUMAN_PED, x, y, 2.0) {
}

int HumanPedestrian::soundWhenHurt() const {
    return SOUND_PED_HURT;
}

int HumanPedestrian::soundWhenDie() const {
    return SOUND_PED_DIE;
}

void HumanPedestrian::doSomething() {
    if (isDead())
        return;
    if (world()->overlapsGhostRacer(this)) {
        world()->getGhostRacer()->setDead();
        return;
    }
    // human ped must move, and take action to movement plan distance
    moveAndPossiblyPickPlan();
}

bool HumanPedestrian::beSprayedIfAppropriate() {
    takeDamageAndPossiblyDie(0);
    setHorizSpeed(-getHorizSpeed());
    setDirection(180 - getDirection());
    return true;
}

// ZombiePedestrian Implementations
ZombiePedestrian::ZombiePedestrian(StudentWorld *sw, double x, double y)
        : Pedestrian(sw, IID_ZOMBIE_PED, x, y, 3.0), ticks_before_grunt(0) {
}

int ZombiePedestrian::soundWhenHurt() const {
    return SOUND_PED_HURT;
}

int ZombiePedestrian::soundWhenDie() const {
    return SOUND_PED_DIE;
}

void ZombiePedestrian::doSomething() {
    if (isDead())
        return;
    if (world()->overlapsGhostRacer(this)) {
        world()->getGhostRacer()->takeDamageAndPossiblyDie(5);
        takeDamageAndPossiblyDie(2);
    }
    // Possibly grunt
    double delta_x = getX() - world()->getGhostRacer()->getX();
    bool in_front_of = getY() > world()->getGhostRacer()->getY();
    if (delta_x > -30 && delta_x < 30 && in_front_of) {
        setDirection(270);
        int horiz_speed = delta_x > 0 ? 1 : -1;
        if (delta_x == 0)
            horiz_speed = 0;
        setHorizSpeed(horiz_speed);
        ticks_before_grunt--;
        if (ticks_before_grunt <= 0) {
            world()->playSound(SOUND_ZOMBIE_ATTACK);
            ticks_before_grunt = 20;
        }
    }
    moveAndPossiblyPickPlan();
}

bool ZombiePedestrian::takeDamageAndPossiblyDie(int hp) {
    if (isDead())
        return false;
    if (Agent::takeDamageAndPossiblyDie(hp) && !world()->overlapsGhostRacer(this) && randInt(1, 5) == 1) {
        HealingGoodie *new_HealingGoodie = new HealingGoodie(world(), getX(), getY());
        world()->addActor(new_HealingGoodie);
    }
    return isDead();
}

bool ZombiePedestrian::beSprayedIfAppropriate() {
    takeDamageAndPossiblyDie(1);
    return true;
}

int ZombiePedestrian::score() const {
    return 150;
}

// Zombie Cab Implementation
ZombieCab::ZombieCab(StudentWorld *sw, double x, double y)
        : Agent(sw, IID_ZOMBIE_CAB, x, y, 4.0, 90, 3, 0), hasDamagedGhostRacer(false), movement_plan_distance(0) {

}

int ZombieCab::soundWhenHurt() const {
    return SOUND_VEHICLE_HURT;
}

int ZombieCab::soundWhenDie() const {
    return SOUND_VEHICLE_DIE;
}

bool ZombieCab::takeDamageAndPossiblyDie(int hp) {
    if (Agent::takeDamageAndPossiblyDie(hp) && randInt(1, 5) == 1) {
        OilSlick *new_OilSlick = new OilSlick(world(), getX(), getY());
        world()->addActor(new_OilSlick);
    }
    return isDead();
}

void ZombieCab::doSomething() {
    if (isDead())
        return;
    // Possible collision with GhostRacer
    if (world()->overlapsGhostRacer(this) && !hasDamagedGhostRacer) {
        // std::cout << "Ready to play sound " << SOUND_VEHICLE_CRASH << std::endl;
        world()->playSound(SOUND_VEHICLE_CRASH);
        // std::cout << "sound played" << std::endl;
        world()->getGhostRacer()->takeDamageAndPossiblyDie(20);
        // std::cout << "Line 357" << std::endl;
        if (getX() <= world()->getGhostRacer()->getX()) {
            setHorizSpeed(-5);
            setDirection(120 + randInt(0, 20 - 1));
        } else {
            setHorizSpeed(5);
            setDirection(60 - randInt(0, 20 - 1));
        }
        hasDamagedGhostRacer = true;

    }
    // move Zombie cab
    if (!moveRelativeToGhostRacerVerticalSpeed())
        return;
    // change speed according to lane
    if (getVerticalSpeed() > world()->getGhostRacer()->getVerticalSpeed()) {
        Actor *closestInFront = world()->findSameLaneAppropriateActor(getX(), getY(), true, this);
        if (closestInFront != nullptr && closestInFront->getY() - getY() < 96) {
            setVerticalSpeed(getVerticalSpeed() - 0.5);
            return;
        }
    } else {
        Actor *closestBehind = world()->findSameLaneAppropriateActor(getX(), getY(), false, this);
        if (closestBehind != nullptr && getY() - closestBehind->getY() < 96) {
            setVerticalSpeed(getVerticalSpeed() + 0.5);
            return;
        }
    }
    // movement plan distance
    movement_plan_distance--;
    if (movement_plan_distance > 0)
        return;
    else {
        movement_plan_distance = randInt(4, 32);
        setVerticalSpeed(getVerticalSpeed() + randInt(-2, 2));
    }
}

bool ZombieCab::beSprayedIfAppropriate() {
    takeDamageAndPossiblyDie(1);
    return true;
}

int ZombieCab::score() const {
    return 200;
}

// Spray Implementation
Spray::Spray(StudentWorld *sw, double x, double y, int dir)
        : Actor(sw, IID_HOLY_WATER_PROJECTILE, x, y, 1.0, dir, 1), travel_distance(0) {
}

bool Spray::isCollisionAvoidanceWorthy() const {
    return false;
}

bool Spray::beSprayedIfAppropriate() {
    return false;
}

void Spray::doSomething() {
    if (isDead())
        return;
    if (world()->sprayFirstAppropriateActor(this)) {
        setDead();
        return;
    }
    // move forward
    moveForward(SPRITE_HEIGHT);
    travel_distance += SPRITE_HEIGHT;
    if (world()->outOfScreen(this))
        setDead();
    if (travel_distance >= 160)
        setDead();
}

// GhostRacerActivatedObject Implementation

GhostRacerActivatedObject::GhostRacerActivatedObject(StudentWorld *sw, int imageID, double x, double y, double size,
                                                     int dir, int depth)
        : Actor(sw, imageID, x, y, size, dir, depth) {
    setVerticalSpeed(-4);
    setHorizSpeed(0);
}

bool GhostRacerActivatedObject::isCollisionAvoidanceWorthy() const {
    return false;
}

void GhostRacerActivatedObject::doSomething() {
    // move
    if (isDead())
        return;
    if (!moveRelativeToGhostRacerVerticalSpeed())
        return;
    if (world()->overlapsGhostRacer(this)) {
        doActivity(world()->getGhostRacer());
        world()->playSound(getSound());
        world()->increaseScore(getScoreIncrease());
        if (selfDestructs())
            setDead();
    }
}

bool GhostRacerActivatedObject::beSprayedIfAppropriate() {
    if (isSprayable()) {
        setDead();
        return true;
    }
    return false;
}

// Oil Slick Implementation
OilSlick::OilSlick(StudentWorld *sw, double x, double y)
        : GhostRacerActivatedObject(sw, IID_OIL_SLICK, x, y, randInt(2, 5), 0, 2) {
}

int OilSlick::getScoreIncrease() const {
    return 0;
}

int OilSlick::getSound() const {
    return SOUND_OIL_SLICK;
}

void OilSlick::doActivity(GhostRacer *gr) {
    gr->spin();
}

bool OilSlick::selfDestructs() const {
    return false;
}

bool OilSlick::isSprayable() const {
    return false;
}

// Healing Goodie Implementaion
HealingGoodie::HealingGoodie(StudentWorld *sw, double x, double y)
        : GhostRacerActivatedObject(sw, IID_HEAL_GOODIE, x, y, 1.0, 0, 2) {
}

int HealingGoodie::getScoreIncrease() const {
    return 250;
}

int HealingGoodie::getSound() const {
    return SOUND_GOT_GOODIE;
}

void HealingGoodie::doActivity(GhostRacer *gr) {
    gr->getHP(10);
}

bool HealingGoodie::isSprayable() const {
    return true;
}

bool HealingGoodie::selfDestructs() const {
    return true;
}

// HolyWaterGoodie implementation

HolyWaterGoodie::HolyWaterGoodie(StudentWorld *sw, double x, double y)
        : GhostRacerActivatedObject(sw, IID_HOLY_WATER_GOODIE, x, y, 2.0, 0, 2) {
}

void HolyWaterGoodie::doActivity(GhostRacer *gr) {
    gr->increaseSprays(10);
}

bool HolyWaterGoodie::isSprayable() const {
    return true;
}

int HolyWaterGoodie::getSound() const {
    return SOUND_GOT_GOODIE;
}

int HolyWaterGoodie::getScoreIncrease() const {
    return 50;
}

bool HolyWaterGoodie::selfDestructs() const {
    return true;
}

// SoulGoodie Implementation
SoulGoodie::SoulGoodie(StudentWorld *sw, double x, double y)
        : GhostRacerActivatedObject(sw, IID_SOUL_GOODIE, x, y, 4.0, 0, 2) {
}

void SoulGoodie::doSomething() {
    GhostRacerActivatedObject::doSomething();
    setDirection(getDirection() + 10);
}

void SoulGoodie::doActivity(GhostRacer *gr) {
    world()->recordSoulSaved();
}

bool SoulGoodie::isSprayable() const {
    return true;
}

int SoulGoodie::getScoreIncrease() const {
    return 100;
}

int SoulGoodie::getSound() const {
    return SOUND_GOT_SOUL;
}

bool SoulGoodie::selfDestructs() const {
    return true;
}