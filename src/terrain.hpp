#ifndef _MMACHINE_TERRAIN_HPP_
#define _MMACHINE_TERRAIN_HPP_

#include "vec.h"
#include "mat.h"
#include "mesh.h"
#include "image.h"
#include "orbiter.h"

class Terrain {
  public :
    virtual void project(const Point& from, Point& to, Vector& n) const = 0 ;
    virtual void draw(const Transform& v, const Transform& p) = 0 ;
} ;


class FlatTerrain : public Terrain {
  public :
    FlatTerrain(const Point& pmin, const Point& pmax) ;
    void project(const Point& from, Point& to, Vector& n) const ;
    void draw(const Transform& v, const Transform& p) ;

  private :
    Mesh mesh_ ;
} ;

class ImgTerrain : public Terrain {
  public :
    ImgTerrain(const Point& pmin, const Point& pmax, const char* filename) ;
    void project(const Point& from, Point& to, Vector& n) const ;
    void draw(const Transform& v, const Transform& p) ;
    std::vector<Point> getCheckpoints() const;
    void setCheckpoints();

  private :
    Color pixel(float x, float y) const ;

    Mesh mesh_ ;
    Image image_ ;

    Vector scale_ ;
    Point origin_ ;
    std::vector<Point> checkpoints_ ;
} ;

#endif
