#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif

#include <SDL2/SDL.h>

bool done = false;

void life(int **inputarray, int **outputarray)
{
    for(int j = 1; j < 51; j++)
 	{
 		for(int i = 1; i < 101; i++)
		{
        //The Moore neighborhood checks all 8 cells surrounding the current cell in the array.
				int count = 0;
				count = inputarray[j-1][i] +
					inputarray[j-1][i-1] +
					inputarray[j][i-1] +
					inputarray[j+1][i-1] +
					inputarray[j+1][i] +
					inputarray[j+1][i+1] +
					inputarray[j][i+1] +
					inputarray[j-1][i+1];
				//The cell dies.
            if(count < 2 || count > 3)
            {
					outputarray[j][i] = 0;
            }
				//The cell stays the same.
            if(count == 2)
            {
					outputarray[j][i] = inputarray[j][i];
            }
				//The cell either stays alive, or is "born".
            if(count == 3)
            {
					outputarray[j][i] = 1;
            }
        }
 	}
}

int inputhtreadfunction(void*)
{
    SDL_Event event;
    while(!done)
    {
        // message processing loop
        while (SDL_WaitEvent(&event))
        {
            // check for messages
            switch (event.type)
            {
                // exit if the window is closed
            case SDL_QUIT:
                done = true;
                break;

                // check for keypresses
            case SDL_KEYDOWN:
                {
                    // exit if ESCAPE is pressed
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        done = true;
                    break;
                }
            } // end switch
        } // end of message processing
    }
    return 0;
}

int main ( int argc, char** argv )
{
    // initialize SDL video
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }

    // make sure SDL cleans up before exit
    atexit(SDL_Quit);

    SDL_DisplayMode displaymode;
    SDL_GetDesktopDisplayMode(0, &displaymode);

    // create a new window
    /*
    SDL_Surface* screen = SDL_SetVideoMode(640, 480, 16,
                                           SDL_HWSURFACE|SDL_DOUBLEBUF);
    */
    SDL_Window *window = SDL_CreateWindow("The Game of Life",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          displaymode.w/2, displaymode.h/2,
                          0);
    SDL_Surface *screen = SDL_GetWindowSurface(window);
    if ( !screen )
    {
        printf("Unable to set %ix%i video: %s\n",screen->w,screen->h, SDL_GetError());
        return 1;
    }

    int **firstarray = (int**)calloc(screen->h+2,sizeof(int*));
    int **secondarray = (int**)calloc(screen->h+2,sizeof(int*));
        for(int i=0;i<screen->h+2;i++)
        {
            firstarray[i] = (int*)calloc(screen->w+2,sizeof(int));
            secondarray[i] = (int*)calloc(screen->w+2,sizeof(int));
        }

    // load an image
    SDL_Surface* bmp = SDL_LoadBMP("cb.bmp");
    if (!bmp)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
        return 1;
    }

    // centre the bitmap on screen
    SDL_Rect dstrect;
    dstrect.x = (screen->w - bmp->w) / 2;
    dstrect.y = (screen->h - bmp->h) / 2;

    SDL_Thread *inputthread = SDL_CreateThread(inputhtreadfunction,"Input Thread", (void*)NULL);

    // program main loop
    while (!done)
    {
        // DRAWING STARTS HERE

        // clear screen
        SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));

        // draw bitmap
        SDL_BlitSurface(bmp, 0, screen, &dstrect);

        // DRAWING ENDS HERE

        // finally, update the screen :)
        /*
        SDL_Flip(screen);
        */
        SDL_UpdateWindowSurface(window);
    } // end main loop

    // free loaded bitmap
    SDL_FreeSurface(bmp);

    free(firstarray);
    free(secondarray);

    // all is well ;)
    printf("Exited cleanly\n");
    return 0;
}
