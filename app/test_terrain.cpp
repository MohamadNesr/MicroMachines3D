#include "terrain.hpp"
#include "controller.hpp"
#include "player.hpp"
#include "obstacle.hpp"

#include "mat.h"
#include "wavefront.h"
#include "orbiter.h"
#include "draw.h"
#include "app.h"
#include "camera.hpp"

bool END_GAME = false;
int ZFAR = 25;
// utilitaire. creation d'une grille / repere.
Mesh make_grid( )
{
    Mesh grid= Mesh(GL_LINES);
    
    for(int x= 0; x < 10; x++)
    {
        float px= (float) x - 5.f + 0.5f;
        grid.vertex(Point(px, 0, -4.5f)); 
        grid.vertex(Point(px, 0, 4.5f)); 
    }

    for(int z= 0; z < 10; z++)
    {
        float pz= (float) z - 5.f + 0.5f;
        grid.vertex(Point(-4.5f, 0, pz)); 
        grid.vertex(Point(4.5f, 0, pz)); 
    }
    
    return grid;
}

void playerCollision(Player &p1, Player &p2){
  
  if(distance(p1.position_, p2.position_) <=0.85){
    std::cout<<" collision !"<< std::endl;
    p1.speed_ =Vector(0,0,0);
    p2.speed_ =Vector(0,0,0);
    if((p2.direction_.x - p1.direction_.x == 0 && p1.direction_.x != 0 )|| (p2.direction_.y - p1.direction_.y == 0 && p1.direction_.y != 0)){
        p1.position_ = p1.position_ - p2.direction_*0.2;
        p2.position_ = p2.position_ + p1.direction_*0.2;
    }else{
      p1.position_ = p1.position_ + p2.direction_*0.2;
      p2.position_ = p2.position_ + p1.direction_*0.2;
    }
    p1.forward_ = false;
    p2.forward_ = false;

  }
    //std::cout<<" pas de collision"<< std::endl;
}
void playerObstacleCollision(Player &p, Obstacle &o){
  
  if(distance(p.position_, o.position_) <= 1){
    std::cout<<" collision !"<< std::endl;
    p.speed_ =Vector(0,0,0);
    p.position_ = p.position_ - p.direction_*0.2;
    p.forward_ = false;
  }
    //std::cout<<" pas de collision"<< std::endl;
}
class Play : public App
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    Play( ) : 
      App(1024, 640), 
      controller1_(SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT),
      controller2_('w', 's', 'a', 'd'),
      terrain_(Point(-20.f, -20.f, 0.f), Point(20.f, 20.f, 20.f), smart_path("../assets/circuit.png"))
    {}
    
    int init( )
    {
        terrain_.setCheckpoints();
        cp_ = terrain_.getCheckpoints();
        vehicule1_ = read_mesh(smart_path("../assets/mmachine.obj")) ;
        vehicule1_.default_color(Color(1.0f, 0.f, 0.f)) ;
        vehicule2_ = read_mesh(smart_path("../assets/mmachine.obj")) ;
        vehicule2_.default_color(Color(0.0f, 0.f, 1.f)) ;
        obstacle_ = read_mesh(smart_path("../assets/woodenCrate.obj"));
        obstacle_.default_color(Color(2.0f, 1.f, 0.5f)) ;
        chckpt_ = read_mesh(smart_path("../assets/flag.obj"));
        chckpt_.default_color(Color(2.0f, 2.0f, 0.0f)) ;

        joueur1_.set_terrain(&terrain_) ;
        joueur1_.set_controller(&controller1_) ;
        joueur1_.spawn_at(Point(0.60,0,0), Vector(0,1,0)) ;
        joueur1_.last_visited_checkpoints_ = Point(0.60,0,0);
        joueur1_.activate() ;

        joueur2_.set_terrain(&terrain_) ;
        joueur2_.set_controller(&controller2_) ;
        joueur2_.spawn_at(Point(-0.60,0,0), Vector(0,1,0)) ;
        joueur2_.last_visited_checkpoints_ = Point(0.60,0,0);
        joueur2_.activate() ;

        obs_.push_back(Point(0, 3.75, 0));
        obs_.push_back(Point(12.5, -2.0, 0));
        obs_.push_back(Point(10, 12.4, 0));
        obs_.push_back(Point(8.3, 7.36, 0));
        obs_.push_back(Point(11.77, 14.34, 0));
        obs_.push_back(Point(12.57, 1.2, 0));
        obs_.push_back(Point(-2.6, 0, 0));
        obs_.push_back(Point(-1, 9.2, 0));
        obs_.push_back(Point(-7.5, -12.25, 0));
        obs_.push_back(Point(-10.4, 10.5, 0));
        obs_.push_back(Point(-12.4, 9.4, 0));
        obs_.push_back(Point(-10.3, -10.2, 0));
        obs_.push_back(Point(-16, -10, 0));
        obs_.push_back(Point(-6, -12, 0));
        obs_.push_back(Point(-1.5, -17.3, 0));
        obs_.push_back(Point(-1.4, -13.3, 0));
        obs_.push_back(Point(6, -11.12, 0));
        obs_.push_back(Point(7.6, -4.5, 0));
        obs_.push_back(Point(9.6, -5.5, 0));
        obs_.push_back(Point(1, -1.5, 0));
        
        for(unsigned int i=0;i< obs_.size(); i++){
          boite_.set_terrain(&terrain_);
          boite_.spawn_at(obs_[i], Vector(0,1,0));
          boites_.push_back(boite_);
          boite_.activate();
        }
        for(unsigned int i=0;i< cp_.size(); i++){
          cpoint.set_terrain(&terrain_);
          std::cout<< "checkpoint initialized "<<std::endl;
          cpoint.spawn_at(cp_[i], Vector(0,1,0));
          flag_.push_back(cpoint);
          cpoint.activate();
        }

        m_camera.m_from = joueur1_.position_ - 10 * joueur1_.direction_ + Point(-10, 10, 10);
        m_camera.m_to = joueur1_.position_;
        m_camera.m_up = Vector(0, 0, 1);
        m_camera_view_matrix = Lookat(m_camera.m_from, m_camera.m_to, m_camera.m_up);
        m_camera_projection_matrix = Perspective(45, 4.f/3, 1, 40);

        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
        
        glClearDepth(1.f);                          // profondeur par defaut
        glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST);                    // activer le ztest
        
        return 0;   // ras, pas d'erreur
    }
    
    // destruction des objets de l'application
    int quit( )
    {
        vehicule1_.release();
        vehicule2_.release();
        obstacle_.release();
        return 0;
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        Transform player1_pos = joueur1_.transform() ;
        Transform player2_pos = joueur2_.transform() ;
        Transform boite_pos = boite_.transform()* RotationX(90) * Scale(0.1,0.1,0.1);
        Vector central_pos = Vector((joueur1_.position_.x+joueur2_.position_.x)/2,
                                    (joueur1_.position_.y+joueur2_.position_.y)/2,
                                    (joueur1_.position_.z+joueur2_.position_.z)/2);
        float distJ1J2 = distance(joueur1_.position_, joueur2_.position_);
        
        Vector from_to = Vector(m_camera.m_from, central_pos+Point(0,0,0));
        m_camera.m_from = m_camera.m_from + (from_to / 100);
        m_camera.m_from.z = ZFAR;
        m_camera.m_to = central_pos + Point(0,0,0);
       
        if(distJ1J2 > 9){
          if(joueur1_.nb_visited_checkpoints > joueur2_.nb_visited_checkpoints){
                m_camera.m_to = joueur1_.position_;
          }
          if(joueur1_.nb_visited_checkpoints < joueur2_.nb_visited_checkpoints){
                m_camera.m_to = joueur2_.position_;
          }
        }
        
        m_camera_view_matrix = Lookat(m_camera.m_from, m_camera.m_to, m_camera.m_up);

        draw(vehicule1_, player1_pos, m_camera_view_matrix, m_camera_projection_matrix) ;
        draw(vehicule2_, player2_pos, m_camera_view_matrix, m_camera_projection_matrix) ;
        draw(obstacle_, boite_pos, m_camera_view_matrix, m_camera_projection_matrix) ;

        for(unsigned int i=0;i< boites_.size(); i++){
          Transform boite_pos = boites_[i].transform()*RotationX(90)*Scale(0.15,0.15,0.15);
          draw(obstacle_, boite_pos, m_camera_view_matrix, m_camera_projection_matrix) ;
          playerObstacleCollision(joueur1_,boites_[i]);
          playerObstacleCollision(joueur2_,boites_[i]);
        }

        for(unsigned int i=0;i< flag_.size(); i++){
          Transform flag_pos = flag_[i].transform()*RotationX(90)*Scale(0.05,0.05,0.05);
          draw(chckpt_,flag_pos, m_camera_view_matrix, m_camera_projection_matrix) ;
        }

        playerCollision(joueur1_, joueur2_);
        if(key_state('l')){
          std::cout<<"Position "<< joueur1_.position_.x << " , "<<joueur1_.position_.y<<std::endl;

        }

        // joueur1_.hasCollided();
        terrain_.draw(m_camera_view_matrix, m_camera_projection_matrix) ;
        joueur1_.add_checkpoint(cp_);
        joueur2_.add_checkpoint(cp_);
        if(!END_GAME){
          if(key_state('p')){
            if(joueur1_.nb_visited_checkpoints >= joueur2_.nb_visited_checkpoints){
              std::cout<<"Le joueur 1 gagne! "<<std::endl;
            }
            if(joueur1_.nb_visited_checkpoints < joueur2_.nb_visited_checkpoints){
              std::cout<<"Le joueur 2 gagne! "<<std::endl;
            }
            if(joueur1_.nb_visited_checkpoints == joueur2_.nb_visited_checkpoints){
              std::cout<<"EGALITE ! "<<std::endl;
            }
          }
          if(joueur1_.nb_visited_checkpoints == cp_.size()){
              std::cout<<"Victoire du joueur 1! "<<std::endl;
              END_GAME = true;
          }
          if(joueur2_.nb_visited_checkpoints == cp_.size()){
              END_GAME = true;
              std::cout<<"Victoire du joueur 2! "<<std::endl;
          }
        }
        //reset
        if(key_state('r')) {
          joueur1_.spawn_at(Point(0.60,0,0), Vector(0,1,0)) ;
          joueur1_.last_visited_checkpoints_ = Point(0.60,0,0);
          joueur1_.nb_visited_checkpoints = 1;
          joueur1_.activate() ;
          joueur2_.spawn_at(Point(-0.60,0,0), Vector(0,1,0)) ;
          joueur2_.last_visited_checkpoints_ = Point(0.60,0,0);
          joueur2_.nb_visited_checkpoints = 1;
          joueur2_.activate() ;
          END_GAME = false;

        }
        

        return 1;
    }

protected:
    Mesh vehicule1_;
    Mesh vehicule2_;
    Mesh obstacle_;
    Mesh chckpt_;
    Player joueur1_;
    Player joueur2_;
    Obstacle boite_;
    std::vector<Obstacle> boites_;
    std::vector<Obstacle> flag_;
    Obstacle cpoint;
    KeyboardController controller1_ ;
    KeyboardController controller2_ ;
    std::vector<Point> cp_;
    std::vector<Point> obs_;
    ImgTerrain terrain_ ;

    // Orbiter m_camera;
    Camera m_camera;
    Transform m_camera_view_matrix;
    Transform m_camera_projection_matrix;
};


int main( int argc, char **argv )
{
    Play app;
    app.run();
    
    return 0;
}