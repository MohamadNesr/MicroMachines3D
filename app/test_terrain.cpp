#include "terrain.hpp"
#include "controller.hpp"
#include "player.hpp"
#include "obstacle.hpp"

#include "mat.h"
#include "wavefront.h"
#include "orbiter.h"
#include "draw.h"
#include "app.h"

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
  
  if(distance(p1.position_, p2.position_) <=1){
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
        vehicule1_ = read_mesh(smart_path("../assets/mmachine.obj")) ;
        vehicule1_.default_color(Color(1.0f, 0.f, 0.f)) ;
        vehicule2_ = read_mesh(smart_path("../assets/mmachine.obj")) ;
        vehicule2_.default_color(Color(0.0f, 0.f, 1.f)) ;
        obstacle_ = read_mesh(smart_path("../assets/tree.obj"));
        obstacle_.default_color(Color(0.0f, 1.f, 0.f)) ;

        joueur1_.set_terrain(&terrain_) ;
        joueur1_.set_controller(&controller1_) ;
        joueur1_.spawn_at(Point(0.60,0,0), Vector(0,1,0)) ;
        joueur1_.activate() ;

        joueur2_.set_terrain(&terrain_) ;
        joueur2_.set_controller(&controller2_) ;
        joueur2_.spawn_at(Point(-0.60,0,0), Vector(0,1,0)) ;
        joueur2_.activate() ;

        arbre_.set_terrain(&terrain_);
        arbre_.spawn_at(Point(1,4,2), Vector(0,1,0));
        arbre_.activate();

       m_camera.lookat(Point(-20.f, -20.f, 0.f), Point(20.f, 20.f, 20.f));

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

        // deplace la camera
        int mx, my;
        unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
        if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
            m_camera.rotation(mx, my);
        else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
            m_camera.move(mx);
        else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
            m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());

        Transform player1_pos = joueur1_.transform() ;
        draw(vehicule1_, player1_pos, m_camera) ;
        Transform player2_pos = joueur2_.transform() ;
        draw(vehicule2_, player2_pos, m_camera) ;
        Transform arbre_pos = arbre_.transform()* RotationX(90) * Scale(0.2,0.2,0.2);
        draw(obstacle_, arbre_pos, m_camera) ;

        Point pmin, pmax;
        pmin = Point(std::min(joueur1_.position_.x, joueur2_.position_.x),
                    std::min(joueur1_.position_.y, joueur2_.position_.y),
                    std::min(joueur1_.position_.z, joueur2_.position_.z));
        pmax = Point(std::max(joueur1_.position_.x, joueur2_.position_.x),
                    std::max(joueur1_.position_.y, joueur2_.position_.y),
                    std::max(joueur1_.position_.z, joueur2_.position_.z));

        float distJ1J2 = distance(joueur1_.position_,joueur2_.position_);
      //  if(distJ1J2> 30){
      //      m_camera.lookat(joueur1_.position_, distance(pmin,pmax)+10);
      //  } else{
            //m_camera.lookat(Point(center(joueur1_.position_ ,joueur2_.position_)), distance(pmin,pmax)+10);
      //  }
        
        playerCollision(joueur1_, joueur2_);
       // joueur1_.hasCollided();
        terrain_.draw(m_camera.view(), m_camera.projection(window_width(), window_height(), 45.f)) ;

        //reset
        if(key_state('r')) {
          joueur1_.spawn_at(Point(0.60,0,0), Vector(0,1,0)) ;
          joueur1_.activate() ;
          joueur2_.spawn_at(Point(-0.60,0,0), Vector(0,1,0)) ;
          joueur2_.activate() ;
        }

        return 1;
    }

protected:
    Mesh vehicule1_;
    Mesh vehicule2_;
    Mesh obstacle_;
    Player joueur1_;
    Player joueur2_;
    Obstacle arbre_;
    KeyboardController controller1_ ;
    KeyboardController controller2_ ;

    ImgTerrain terrain_ ;

    Orbiter m_camera;
};


int main( int argc, char **argv )
{
    Play app;
    app.run();
    
    return 0;
}