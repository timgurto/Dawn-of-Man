#include <string>
#include <iostream>

#include "SDL.h"
#include "SDL_image.h"

#include "generic_structs.h"
#include "SDL_wrappers.h"

const RGB RGB_BLACK(0, 0, 0);
const RGB RGB_MAGENTA(0xff, 0, 0xff);

//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//The surfaces
SDL_Surface *image = 0, *background = 0, *man = 0;
SDL_Surface *screen = 0;

//The event structure that will be used
SDL_Event event;

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination )
{
    //Temporary rectangle to hold the offsets
    SDL_Rect offset;

    //Get the offsets
    offset.x = x;
    offset.y = y;

    //Blit the surface
    SDL_BlitSurface( source, NULL, destination, &offset );
}

bool init()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

    //Set the window caption
    SDL_WM_SetCaption( "Arche 2", NULL );

    //If everything initialized fine
    return true;
}

bool load_files()
{
    //Load the image
    image = loadImage( "Images/horse2.PNG",  RGB_MAGENTA);
    if (image==0){
       std::cerr << "File not loaded successfully" << std::endl;
       return false;
    }
    //SDL_SetAlpha( image, SDL_SRCALPHA, 128 );
    background = loadImage("Images/wall.PNG");
    if (background==0){
       std::cerr << "File not loaded successfully" << std::endl;
       return false;
    }
    man = loadImage("Images/Caveman.PNG", RGB_MAGENTA);

    //If there was an error in loading the image
    if (image==0){
       std::cerr << "File not loaded successfully" << std::endl;
       return false;
    }
    //If everything loaded fine
    return true;
}

void clean_up()
{
    //Free the surface
    SDL_FreeSurface( image );

    //Quit SDL
    SDL_Quit();
}

int main( int argc, char* args[] ){
   SDL_Rect wholeScreen = makeRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    //Make sure the program waits for a quit
    bool quit = false;

    //Initialize
    if( init() == false )
    {
        return 1;
    }

    //Load the files
    if( load_files() == false )
    {
        return 1;
    }

   SDL_FillRect(screen, &wholeScreen, SDL_MapRGB( screen->format, 0, 0, 0xff)); //0x44, 0xaa, 0));

    //Apply the surface to the screen
   apply_surface(0, 0, background, screen);
   apply_surface(512, 0, background, screen);
    apply_surface( 20, 50, image, screen );
    SDL_SetAlpha( image, SDL_SRCALPHA, 0209 );
    apply_surface(0, 250, image, screen);
    apply_surface(400, 100, man, screen);
    //Update the screen
    if( SDL_Flip( screen ) == -1 )
    {
        return 1;
    }

    //While the user hasn't quit
    while( quit == false )
    {
        //While there's an event to handle
        while( SDL_PollEvent( &event ) )
        {
            //If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
            }
        }
    }

    //Free the surface and quit SDL
    clean_up();

    return 0;
}

