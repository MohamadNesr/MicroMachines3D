#include "mat.h"
#include "draw.h"
#include "texture.h"
#include "app.h"

// utilitaire. creation d'une grille / repere.
class Png : public App
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    Png( ) : 
      App(1024, 640),
      quad_(GL_TRIANGLES)
    {}
    
    int init( )
    {
        std::cout 
          << quad_.default_color().r
          << " "
          << quad_.default_color().g
          << " "
          << quad_.default_color().b 
          << std::endl ;


        quad_.texcoord(0.f, 0.f) ;
        quad_.vertex(-0.95f, -0.95f, 0.f) ;

        quad_.texcoord(1.f, 0.f) ;
        quad_.vertex(0.95f, -0.95f, 0.f) ;

        quad_.texcoord(1.f, 1.f) ;
        quad_.vertex(0.95f, 0.95f, 0.f) ;

        quad_.texcoord(0.f, 1.f) ;
        quad_.vertex(-0.95f, 0.95f, 0.f) ;

        quad_.triangle(0, 1, 2) ;
        quad_.triangle(0, 2, 3) ;

        texture_ = read_texture(0, "/tmp/transparent.png") ;

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
        quad_.release() ;
        return 0;
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        DrawParam param ;
        param.texture(texture_) ;
        param.alpha(0.1f) ;
        param.draw(quad_) ;

        return 1;
    }

protected:
    Mesh quad_;
    GLuint texture_ ;
};


int main( int argc, char **argv )
{
    Png app;
    app.run();
    
    return 0;
}
