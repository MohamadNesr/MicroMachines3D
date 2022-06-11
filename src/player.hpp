#ifndef _MMACHINE_PLAYER_HPP_
#define _MMACHINE_PLAYER_HPP_

#include "terrain.hpp"
#include "controller.hpp"

#include "mat.h"
#include "vec.h"

class Player {
  public :
    Player() ;

    void spawn_at(const Point& position, const Vector& direction) ;

    void activate() ;

    void deactivate() ;

    Transform transform() ;

    //parameters
    void set_terrain(const Terrain* terrain) ;
    void set_controller(const Controller* controller) ;
    void add_checkpoint(std::vector<Point> cp_) ;
    
    //environment
    const Terrain* terrain_ ;
    void project(Point& candidate) ;

    //control
    bool forward_ ;
    bool switchable_ ;
    const Controller* controller_ ;

    //physics
    bool active_ ;
    Vector speed_ ;
    int last_time_ ;
    void step() ;
    int nb_visited_checkpoints = 1;

    //elements
    Point position_ ;
    Vector direction_ ;
    Vector normal_ ;
    Point last_visited_checkpoints_ ;

    //parameters
    float acceleration_ ;
    float turning_angle_ ;
    float max_speed_ ;
    vec2 friction_ ;
    bool isWinning = false;
} ;

#endif
