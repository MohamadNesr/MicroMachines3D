#include "terrain.hpp"
#include "controller.hpp"
#include "player.hpp"
#include "obstacle.hpp"
#include "texture.h"
#include "mat.h"
#include "mat.h"
#include "wavefront.h"
#include "orbiter.h"
#include "draw.h"
#include "app.h"
#include "camera.hpp"
#include "text.h"

bool END_GAME = false;
int ZFAR = 18;
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
  
  if(distance(p1.position_, p2.position_) <=0.99){
    //std::cout<<" collision !"<< std::endl;
    p1.speed_ =Vector(0,0,0);
    p2.speed_ =Vector(0,0,0);
    Vector rebound = Vector(0.25,0.25,0);
    if((p2.direction_.x - p1.direction_.x == 0 && p1.direction_.x != 0 )|| (p2.direction_.y - p1.direction_.y == 0 && p1.direction_.y != 0)){
      p1.position_ = p1.position_ - rebound;
      p2.position_ = p2.position_ + rebound;
    }else{
      p1.position_ = p1.position_ + rebound;
      p2.position_ = p2.position_ - rebound;
    }
    p1.forward_ = false;
    p2.forward_ = false;

  }
}

void playerObstacleCollision(Player &p, Obstacle &o){
  
  if(distance(p.position_, o.position_) <=0.8){
    //std::cout<<" collision !"<< std::endl;
    p.speed_ =Vector(0,0,0);
    if(p.controller_-> down()){
        p.position_ = p.position_ + p.direction_*0.5;
     }else{
              p.position_ = p.position_ - p.direction_*0.5;

     }
    p.forward_ = false;
  }
}

void restart(Player& joueur1_, Player& joueur2_){
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

class Play : public App
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    Play( ) : 
      App(1024, 640), 
      quad_(GL_TRIANGLES),
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
        oil_ = read_mesh(smart_path("../assets/oil.obj"));
        oil_.default_color(Color(0.35f, 0.40f, 0.25f)) ;

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

        //-------------------------------- OBSTACLES VECTOR --------------------------------------------------------------------------
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
        obs_.push_back(Point(9.6, -5, 0));
        obs_.push_back(Point(1, -1.5, 0));
        
        for(unsigned int i=0;i< obs_.size(); i++){
          boite_.set_terrain(&terrain_);
          boite_.spawn_at(obs_[i], Vector(0,1,0));
          boites_.push_back(boite_);
          boite_.activate();
        }
        // ----------------------------------- OIL PUDDLES -------------------------------------------
        puddles_.push_back(Point(15, 3, 0));
        puddles_.push_back(Point(-12, -1.7, 0));
        puddles_.push_back(Point(8, 12, 0));
        puddles_.push_back(Point(-3, -2, 0));
        puddles_.push_back(Point(-13, 6, 0));
        puddles_.push_back(Point(-11, -16, 0));
        puddles_.push_back(Point(9, -11, 0));


        for(unsigned int i=0;i< puddles_.size(); i++){
          huile_.set_terrain(&terrain_);
          huile_.spawn_at(puddles_[i], Vector(0,1,0));
          floques_.push_back(huile_);
          huile_.activate();
        }
        huile_.set_terrain(&terrain_);
        huile_.spawn_at(Point(3,5,0), Vector(0,1,0));
        huile_.activate();

        // ----------------------------------- CHECKPOINTS --------------------------------------------
        for(unsigned int i=0;i< cp_.size(); i++){
          cpoint.set_terrain(&terrain_);
          std::cout<< "checkpoint initialized "<<std::endl;
          cpoint.spawn_at(cp_[i], Vector(0,1,0));
          flag_.push_back(cpoint);
          cpoint.activate();
        }
        
        // ------------------------------------ CAMERA -------------------------------------------
        m_camera.m_from = joueur1_.position_ - 10 * joueur1_.direction_ + Point(-10, 10, 10);
        m_camera.m_to = joueur1_.position_;
        m_camera.m_up = Vector(0, 0, 1);
        m_camera_view_matrix = Lookat(m_camera.m_from, m_camera.m_to, m_camera.m_up);
        m_camera_projection_matrix = Perspective(45, 4.f/3, 1, 40);

        // ------------------------------------ QUAD -----------------------------------------------
        std::cout 
          << quad_.default_color().r
          << " "
          << quad_.default_color().g
          << " "
          << quad_.default_color().b 
          << std::endl ;

        quad_.vertex(-0.9f, 0.99f, 0.f) ;
        quad_.vertex(-0.9f, 0.8f, 0.f) ;
        quad_.vertex(0.9f, 0.8f, 0.f) ;
        quad_.vertex(0.9f, 0.99f, 0.f) ;

        quad_.triangle(0, 1, 2) ;
        quad_.triangle(0, 2, 3) ;

        texture_ = read_texture(0, "/tmp/transparent.png") ;
        score1_ = read_texture(0, "assets/0.png");
        score2_ = read_texture(0, "assets/0.png");


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
        quad_.release();
        return 0;
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DrawParam param ;
        param.texture(texture_) ;
        param.alpha_texture(texture_,0.5f) ;
        param.draw(quad_) ;

        // ------------------------- CAMERA INIT --------------------------------------------------------------------------
        Vector central_pos = Vector((joueur1_.position_.x+joueur2_.position_.x)/2,
                                    (joueur1_.position_.y+joueur2_.position_.y)/2,
                                    (joueur1_.position_.z+joueur2_.position_.z)/2);
        float distJ1J2 = distance(joueur1_.position_, joueur2_.position_);
        
        Vector from_to = Vector(m_camera.m_from, central_pos + Point(0,0,0));
        m_camera.m_from = m_camera.m_from + (from_to / 100);
        m_camera.m_from.z = distJ1J2 + ZFAR;
        m_camera.m_to = central_pos + Point(0,0,0);
       
        //  ----------------------- CHECKING WHO IS WINNING ------------------------------------------------------------------
        if(joueur1_.nb_visited_checkpoints > joueur2_.nb_visited_checkpoints){
                joueur1_.isWinning = true;
                joueur2_.isWinning = false;
        } else if(joueur1_.nb_visited_checkpoints < joueur2_.nb_visited_checkpoints){
              joueur1_.isWinning = false;
              joueur2_.isWinning = true;
        } else if(distance(joueur1_.position_,cp_[joueur1_.nb_visited_checkpoints]) 
                      < distance(joueur2_.position_,cp_[joueur2_.nb_visited_checkpoints])){
                            joueur1_.isWinning = true;
                            joueur2_.isWinning = false;
        } else if(distance(joueur1_.position_,cp_[joueur1_.nb_visited_checkpoints]) 
                      > distance(joueur2_.position_,cp_[joueur2_.nb_visited_checkpoints])){
                            joueur1_.isWinning = false;
                            joueur2_.isWinning = true;
        }

        // -------------------------- FOCUSING ON WINNING PLAYER IF DISTANCE IS BIG --------------------------------------------
        if(distJ1J2 > ZFAR){
          if(joueur1_.isWinning){
                m_camera.m_to = joueur1_.position_;
          } else if(joueur2_.isWinning){
                m_camera.m_to = joueur2_.position_;   
          }
        }
        
        // -------------------------- DEBUG TO CHECK ACTUAL SCORE -------------------------------------------------------------
        if(key_state('p')){
          std::cout<< joueur1_.isWinning<< " , "<< joueur2_.isWinning << std::endl;
            if(joueur1_.isWinning){
              std::cout<<"Le joueur 1 gagne! "<<std::endl;
            }
            if(joueur2_.isWinning){
              std::cout<<"Le joueur 2 gagne! "<<std::endl;
            }else{
              std::cout<<"NONE "<<std::endl;
            }
        }
        
        m_camera_view_matrix = Lookat(m_camera.m_from, m_camera.m_to, m_camera.m_up);

        // -------------------------- ADDING ALL ELEMENTS AND COLLISISONS --------------------------------------------------------
        Transform player1_pos = joueur1_.transform() ;
        Transform player2_pos = joueur2_.transform() ;
        Transform boite_pos = boite_.transform() * RotationX(90) * Scale(0.1,0.1,0.1);
        draw(vehicule1_, player1_pos, m_camera_view_matrix, m_camera_projection_matrix) ;
        draw(vehicule2_, player2_pos, m_camera_view_matrix, m_camera_projection_matrix) ;
       // draw(obstacle_, boite_pos, m_camera_view_matrix, m_camera_projection_matrix) ;

        for(unsigned int i=0;i< boites_.size(); i++){
          Transform boite_pos = boites_[i].transform() * RotationX(90) * Scale(0.15,0.15,0.15);
          draw(obstacle_, boite_pos, m_camera_view_matrix, m_camera_projection_matrix) ;
          playerObstacleCollision(joueur1_,boites_[i]);
          playerObstacleCollision(joueur2_,boites_[i]);
        }

        for(unsigned int i=0;i< floques_.size(); i++){
          Transform huile_pos = floques_[i].transform()*RotationX(90);
          draw(oil_, huile_pos, m_camera_view_matrix, m_camera_projection_matrix) ;
          joueur1_.oil_trap(puddles_[i]);
          joueur2_.oil_trap(puddles_[i]);
        }

        for(unsigned int i=0;i< flag_.size(); i++){
          Transform flag_pos = flag_[i].transform() * RotationX(90) * Scale(0.05,0.05,0.05);
          draw(chckpt_,flag_pos, m_camera_view_matrix, m_camera_projection_matrix) ;
        }

        playerCollision(joueur1_, joueur2_);
        if(key_state('l')){
          std::cout<<"Position "<< joueur1_.position_.x << " , "<<joueur1_.position_.y<<std::endl;

        }

        terrain_.draw(m_camera_view_matrix, m_camera_projection_matrix) ;
        joueur1_.add_checkpoint(cp_);
        joueur2_.add_checkpoint(cp_);

        // ---------------------------- CHECK IF A PLAYER COMPLETED HIS LAP TO END ROUND --------------------------------------------
        if(!END_GAME){
          if((joueur1_.nb_visited_checkpoints == cp_.size()) 
                || (joueur1_.isWinning && distJ1J2> ZFAR)
                || (joueur1_.nb_visited_checkpoints > joueur2_.nb_visited_checkpoints+9)){
              std::cout<<"Victoire du joueur 1! "<<std::endl;
              std::cout<<"Checkpoints difference "<< joueur1_.nb_visited_checkpoints - joueur2_.nb_visited_checkpoints<<std::endl;
              std::cout<<"Distance! "<< distJ1J2<<std::endl;
              END_GAME = true;
          }
          if(joueur2_.nb_visited_checkpoints == cp_.size() 
                || (joueur2_.isWinning && distJ1J2>ZFAR)
                || (joueur2_.nb_visited_checkpoints > joueur1_.nb_visited_checkpoints+9)){
              std::cout<<"Victoire du joueur 2! "<<std::endl;
              std::cout<<"Checkpoints difference "<< joueur2_.nb_visited_checkpoints - joueur1_.nb_visited_checkpoints<<std::endl;
              std::cout<<"Distance! "<< distJ1J2<<std::endl;
              END_GAME = true;
          }
        }

        // ---------------------------- IF ROUND OVER WE GIVE THE WINNER A POINT AND WE START ANOTHER NEW ROUND ------------------------
        if(END_GAME){
          if (joueur1_.isWinning){
            p1RoundsWon++;
            END_GAME = false;
            joueur2_.spawn_at(joueur1_.position_, joueur1_.direction_);
            joueur1_.speed_ = joueur1_.speed_*0;
            joueur2_.speed_ = joueur2_.speed_*0;
            joueur2_.nb_visited_checkpoints = joueur1_.nb_visited_checkpoints;
            joueur2_.last_visited_checkpoints_ = joueur1_.last_visited_checkpoints_;
            joueur2_.activate();
           // restart(joueur1_,joueur2_);
          }
          if(joueur2_.isWinning){
            p2RoundsWon++;
            END_GAME = false;
            joueur1_.spawn_at(joueur2_.position_, joueur2_.direction_);
            joueur1_.speed_ = joueur1_.speed_*0;
            joueur2_.speed_ = joueur2_.speed_*0;
            joueur1_.nb_visited_checkpoints = joueur2_.nb_visited_checkpoints;
            joueur1_.last_visited_checkpoints_ = joueur2_.last_visited_checkpoints_;
            joueur1_.activate();
            // restart(joueur1_,joueur2_);
          }
          score[0] = p1RoundsWon;
          score[1] = p2RoundsWon;
          std::cout<<"Score : "<< score[0] << " | " << score[1] << std::endl;
        }
        
        // ---------------------------- IF A PLAYER HAS WON 3 ROUNDS THE GAME IS OVER ---------------------------------------------------
        if(p1RoundsWon == 3 || p2RoundsWon == 3){
          std::cout<< "GAME OVER !"<< std::endl;
          if (p1RoundsWon == 3){
            std::cout<<"Player 1 HAS WON"<< std::endl;
          }
          if (p2RoundsWon == 3){
            std::cout<<"Player 2 HAS WON"<< std::endl;
          }

          p1RoundsWon = 0;
          p2RoundsWon = 0;
          restart(joueur1_, joueur2_);
        }
        //reset
        if(key_state('r')) {
          p1RoundsWon = 0;
          p2RoundsWon = 0;
          restart(joueur1_, joueur2_);
        }
        
        
        return 1;
    }

protected:
    Mesh vehicule1_;
    Mesh vehicule2_;
    Mesh obstacle_;
    Mesh chckpt_;
    Mesh oil_;
    Mesh score1_;
    Mesh score2_;
    Player joueur1_;
    Player joueur2_;
    Obstacle boite_;
    Obstacle huile_;
    std::vector<Obstacle> boites_;
    std::vector<Obstacle> flag_;
    std::vector<Obstacle> floques_;
    Obstacle cpoint;
    KeyboardController controller1_ ;
    KeyboardController controller2_ ;
    std::vector<Point> cp_;
    std::vector<Point> obs_;
    std::vector<Point> puddles_;
    ImgTerrain terrain_ ;
    int p1RoundsWon = 0;
    int p2RoundsWon = 0;
    int score[2];
    // Orbiter m_camera;
    Camera m_camera;
    Transform m_camera_view_matrix;
    Transform m_camera_projection_matrix;

    Mesh quad_;
    GLuint texture_ ;
};


int main( int argc, char **argv )
{
    Play app;
    app.run();
    
    return 0;
}