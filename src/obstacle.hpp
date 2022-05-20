#ifndef _MMACHINE_OBSTACLE_HPP_
#define _MMACHINE_OBSTACLE_HPP_

#include "terrain.hpp"

#include "mat.h"
#include "vec.h"

class Obstacle {
  public :
    Obstacle() ;

    void spawn_at(const Point& position, const Vector& direction) ;

    void activate() ;

    void deactivate() ;

    Transform transform() ;

    //parameters
    void set_terrain(const Terrain* terrain) ;
    
    //environment
    const Terrain* terrain_ ;
    void project(Point& candidate) ;

    //physics
    bool active_ ;
    int last_time_ ;

    //elements
    Point position_ ;
    Vector direction_ ;
    Vector normal_ ;

} ;

#endif
