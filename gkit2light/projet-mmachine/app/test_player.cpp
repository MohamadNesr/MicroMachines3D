#include "terrain.hpp"
#include "controller.hpp"
#include "player.hpp"

#include "mat.h"
#include "wavefront.h"
#include "orbiter.h"
#include "draw.h"
#include "app.h"

#include "camera.hpp"
#include <iostream>

class Play : public App
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    Play( ) : 
      App(1024, 640), 
      controller1_(SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT),
      controller2_('z', 's', 'q', 'd'),
      terrain_(Point(-20.f, -20.f, 0.f), Point(20.f, 20.f, 0.f))
    {}
    
    int init( )
    {
        vehicule1_ = read_mesh(smart_path("../assets/mmachine.obj")) ;
        vehicule1_.default_color(Color(1.0f, 0.f, 0.f)) ;
        vehicule2_ = read_mesh(smart_path("../assets/mmachine.obj")) ;
        vehicule2_.default_color(Color(0.0f, 0.f, 1.f)) ;

        joueur1_.set_terrain(&terrain_) ;
        joueur1_.set_controller(&controller1_) ;
        joueur1_.spawn_at(Point(0,0,0), Vector(0,1,0)) ;
        joueur1_.activate() ;

        joueur2_.set_terrain(&terrain_) ;
        joueur2_.set_controller(&controller2_) ;
        joueur2_.spawn_at(Point(1,1,0), Vector(0,1,0)) ;
        joueur2_.activate() ;

        //m_camera.lookat(Point(-20.f, -20.f, -20.f), Point(20.f, 20.f, 20.f));
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
        return 0;
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // deplace la camera
        /*int mx, my;
        unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
        if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
            m_camera.rotation(mx, my);
        else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
            m_camera.move(mx);
        else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
            m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());*/

        Transform player1_pos = joueur1_.transform() ;

        Vector from_to = Vector(m_camera.m_from, joueur1_.position_);
        if (distance(joueur1_.position_, m_camera.m_from) > 11) {
            m_camera.m_from = m_camera.m_from + (from_to / 100);
            m_camera.m_from.z = 10;
        }
        m_camera.m_to = joueur1_.position_;
        //std::cout << joueur1_.position_ << " " << joueur1_.direction_ << std::endl;

        m_camera_view_matrix = Lookat(m_camera.m_from, m_camera.m_to, m_camera.m_up);
        draw(vehicule1_, player1_pos, m_camera_view_matrix, m_camera_projection_matrix) ;
        Transform player2_pos = joueur2_.transform() ;
        draw(vehicule2_, player2_pos, m_camera_view_matrix, m_camera_projection_matrix) ;

        terrain_.draw(m_camera_view_matrix, m_camera_projection_matrix) ;

        //reset
        if(key_state('r')) {
          joueur1_.spawn_at(Point(0,0,0), Vector(0,1,0)) ;
          joueur1_.activate() ;
        }

        return 1;
    }

protected:
    Mesh vehicule1_;
    Mesh vehicule2_;
    Player joueur1_;
    Player joueur2_;
    KeyboardController controller1_ ;
    KeyboardController controller2_ ;

    FlatTerrain terrain_ ;

    //Orbiter m_camera;
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
