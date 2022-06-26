#include "terrain.hpp"
#include "draw.h"
#include "image_io.h"

//Flat Terrain

FlatTerrain::FlatTerrain(const Point& pmin, const Point& pmax) : mesh_(GL_TRIANGLES) {
  //terrain mesh
  unsigned int a = mesh_.vertex(pmin.x, pmin.y, 0.f) ;
  unsigned int b = mesh_.vertex(pmax.x, pmin.y, 0.f) ;
  unsigned int c = mesh_.vertex(pmax.x, pmax.y, 0.f) ;
  unsigned int d = mesh_.vertex(pmin.x, pmax.y, 0.f) ;

  mesh_.triangle(a, b, c) ;
  mesh_.triangle(a, c, d) ;
}

void FlatTerrain::project(const Point& from, Point& to, Vector& n) const {
  to.z = 0.f ;
  n = Vector(0.f, 0.f, 1.f) ;
}

void FlatTerrain::draw(const Transform& v, const Transform& p) {
  ::draw(mesh_, Identity(), v, p) ;
}

//Image Terrain

ImgTerrain::ImgTerrain(const Point& pmin, const Point& pmax, const char* filename) : mesh_(GL_TRIANGLES) {
  image_ = read_image(filename) ;

  origin_ = pmin ;

  scale_.x = (pmax.x - pmin.x) / image_.width() ;
  scale_.y = (pmax.y - pmin.y) / image_.height() ;
  scale_.z = (pmax.z - pmin.z) ;

  float w = image_.width() ;
  float h = image_.height() ;

  for(int i = 0; i < h; ++i) {
    for(int j = 0; j < w; ++j) {
      Color c = image_(i,j) ;
      mesh_.vertex(pmin.x + i * scale_.x, pmin.y + j * scale_.y, pmin.z + scale_.z * c.r) ;
    }
  }

  for(int i = 0; i < h-1; ++i) {
    for(int j = 0; j < w-1; ++j) {
      unsigned int p0 = i * w + j ;
      unsigned int p1 = i * w + j + 1 ;
      unsigned int p2 = (i+1) * w + j ;
      unsigned int p3 = (i+1) * w + j + 1 ;

      mesh_.triangle(p0, p2, p1) ;
      mesh_.triangle(p2, p3, p1) ;
    }
  }
}

Color ImgTerrain::pixel(float x, float y) const {
  float s_x = (x - origin_.x) / scale_.x ;
  float s_y = (y - origin_.y) / scale_.y ;

  s_x = s_x < image_.width() - 1 ? s_x : image_.width() - 1.5 ;
  s_x = s_x > 0 ? s_x : 0 ;
  s_y = s_y < image_.height() - 1 ? s_y : image_.height() - 1.5 ;
  s_y = s_y > 0 ? s_y : 0 ;

  int p_x = (int) s_x ;
  int p_y = (int) s_y ;

  float w_x = s_x - p_x ;
  float w_y = s_y - p_y ;

  Color c_0 = image_(p_x, p_y) * (1-w_x) * (1-w_y) ;
  Color c_1 = image_(p_x+1, p_y) * w_x * (1-w_y) ;
  Color c_2 = image_(p_x+1, p_y+1) * w_x * w_y ;
  Color c_3 = image_(p_x, p_y+1) * (1-w_x) * w_y ;

  return c_0 + c_1 + c_2 + c_3 ;
}

void ImgTerrain::project(const Point& from, Point& to, Vector& n) const {

  Color pix = pixel(to.x, to.y) ;
  float init_length = length(to - from) ;
  to.z = (origin_.z + pix.r * scale_.z) ;
  if(init_length > 0 && to.z > from.z && length(to - from) / init_length > 2.) {
    to = from ;
  }
  n = Vector(0.f, 0.f, 1.f) ;

}

void ImgTerrain::draw(const Transform& v, const Transform& p) {
  ::draw(mesh_, Identity(), v, p) ;
}

void ImgTerrain::setCheckpoints(){
  checkpoints_.push_back(Point(0.6, 0, 0));
  checkpoints_.push_back(Point(2.0, 3.0, 0));
  checkpoints_.push_back(Point(5.5, 3.0, 0));
  checkpoints_.push_back(Point(8.3, 2.36, 0));
  checkpoints_.push_back(Point(13, 0.0, 0));
  checkpoints_.push_back(Point(15.4, 4.3, 0));
  checkpoints_.push_back(Point(14.35, 13.33, 0));
  checkpoints_.push_back(Point(9.77, 15.34, 0));
  checkpoints_.push_back(Point(3.5, 12.35, 0));
  checkpoints_.push_back(Point(-1.3, 10.2, 0));
  checkpoints_.push_back(Point(-4.3, 2.66, 0));
  checkpoints_.push_back(Point(-3.3, -4.5, 0));
  checkpoints_.push_back(Point(-5.2, -10.2, 0));
  checkpoints_.push_back(Point(-10.8, -7, 0));
  checkpoints_.push_back(Point(-12, 0, 0));
  checkpoints_.push_back(Point(-10.5, 7.42, 0));
  checkpoints_.push_back(Point(-15.1, 5, 0));
  checkpoints_.push_back(Point(-15.9, -3.12, 0));
  checkpoints_.push_back(Point(-15.11, -12.2, 0));
  checkpoints_.push_back(Point(-10.5, -14.95, 0));
  checkpoints_.push_back(Point(-5.75, -15.2, 0));
  checkpoints_.push_back(Point(6.44, -14.37, 0));
  checkpoints_.push_back(Point(8.29, -9.17, 0));
  checkpoints_.push_back(Point(5.2, -5.5, 0));
  checkpoints_.push_back(Point(0.5, 0.5, 0));

}

std::vector<Point> ImgTerrain::getCheckpoints() const {
  return checkpoints_;
}