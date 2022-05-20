#include "obstacle.hpp"

#include <SDL2/SDL.h>

Obstacle::Obstacle() : 
  terrain_(nullptr),
  active_(false),
  position_(0.f, 0.f, 0.f),
  direction_(0.f, 0.f, 1.f),
  normal_(0.f, 0.f, 0.f)
{}

void Obstacle::spawn_at(const Point& position, const Vector& direction) {
  //reset position on terrain
  position_ = position ;
  direction_ = direction ;
  project(position_) ;
  //wait for activation
  deactivate() ;
}


void Obstacle::activate() {
  active_ = true ;
}

void Obstacle::deactivate() {
  active_ = false ;
}

Transform Obstacle::transform() {
  return Transform(direction_, cross(normal_, direction_), normal_, position_ - Point()) ;
}

void Obstacle::set_terrain(const Terrain* terrain) {
  terrain_ = terrain ;
}


void Obstacle::project(Point& candidate) {
  //project position and get normal
  terrain_->project(position_, candidate, normal_) ;

  //project direction
  direction_ = normalize(direction_ - dot(direction_, normal_)*normal_) ;
}
