#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif

#include <SDL2/SDL.h>

bool done = false;
bool pause = true;
int howmuchpixels = 0;

int width = 800;
int height = 600;


int factor = 18;

int mouselastposition_x = 0;
int mouselastposition_y = 0;

int **firstarray;
int **secondarray;

SDL_Rect pixels[ 1920 * 1080 ];

void life( int **inputarray, int **outputarray )
{
    int count = 0;
    howmuchpixels = 0;

    for(int j = 1; j <= height; j++)
 	{
 		for(int i = 1; i <= width; i++)
		{
        //The Moore neighborhood checks all 8 cells surrounding the current cell in the array.
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
            else if(count == 2)
            {
					if((outputarray[j][i] = inputarray[j][i]) == 1)
                    {
                        pixels[howmuchpixels].y = j-1;
                        pixels[howmuchpixels].x = i-1;
                        howmuchpixels++;
                    }
            }
				//The cell either stays alive, or is "born".
            else if(count == 3)
            {
					outputarray[j][i] = 1;
					pixels[howmuchpixels].y = j-1;
					pixels[howmuchpixels].x = i-1;
					howmuchpixels++;
            }
        }
 	}
}

void arraytopixels( int **array )
{
    howmuchpixels = 0;
    for(int j = 1; j <= height; j++)
 	{
 		for(int i = 1; i <= width; i++)
		{
		    if(array[j][i] == 1)
            {
                pixels[howmuchpixels].y = j-1;
                pixels[howmuchpixels].x = i-1;
                howmuchpixels++;
            }
        }
 	}
}

void copyedges(int **array)
{
    array[0][0]=array[height][width];

    for(int i=1;i<=width;i++)
        array[0][i]=array[height][i];

    array[0][width+1]=array[height][1];

    for(int i=1;i<=height;i++)
        array[i][width+1]=array[i][1];

    array[height+1][width+1]=array[1][1];

    for(int i=width;i>=1;i--)
        array[height+1][i]=array[1][i];

    array[height+1][0]=array[1][width];

    for(int i=height;i>=1;i--)
        array[i][0]=array[i][width];

}

void randomtoarray( int **array, int randomfactor )
{
    for(int j = 1; j <= height; j++)
 	{
 		for(int i = 1; i <= width; i++)
		{
		    if(random() % randomfactor == 0)
                array[j][i] = 1;
		}
 	}
}

void cleararray( int **array )
{
    for(int j = 1; j <= height; j++)
 	{
 		for(int i = 1; i <= width; i++)
		{
                array[j][i] = 0;
		}
 	}
}

int inputhtreadfunction(void*)
{
    SDL_Event event;
    while (!done)
    {
        // message processing loop
        while (SDL_WaitEvent(&event))
        {
            // check for messages
            switch (event.type)
            {
                // exit if the window is closed
            case SDL_QUIT:
                {
                    done = true;
                    break;
                }
                // check for keypresses
            case SDL_KEYDOWN:
                {

                    switch(event.key.keysym.sym)
                    {
                        // exit if ESCAPE is pressed
                    case SDLK_ESCAPE:
                        {
                            done = true;
                            break;
                        }

                    case SDLK_SPACE:
                        {
                            pause = !pause;
                            break;
                        }

                    case SDLK_r:
                        {
                            if(pause == 1)
                                randomtoarray( firstarray, factor );
                            break;
                        }

                    case SDLK_c:
                        {
                            if(pause == 1)
                                cleararray( firstarray );
                            break;
                        }

                    case SDLK_EQUALS:
                        {
                            if(factor>1)
                                factor--;

                            break;
                        }

                    case SDLK_MINUS:
                        {
                            factor++;

                            break;
                        }
                    }
                    break;
                }

            case SDL_MOUSEMOTION:
                {
                    mouselastposition_x=event.motion.x;
                    mouselastposition_y=event.motion.y;

                    if(event.button.button == SDL_BUTTON_LEFT && pause == 1)
                        firstarray[mouselastposition_y+1][mouselastposition_x+1] = 1;

                    break;
                }

            case SDL_MOUSEBUTTONDOWN:
                {
                    if(event.button.button == SDL_BUTTON_LEFT && pause == 1)
                        firstarray[event.motion.y+1][event.motion.x+1] = 1;

                    mouselastposition_x=event.motion.x;
                    mouselastposition_y=event.motion.y;
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
    SDL_GetDesktopDisplayMode( 0, &displaymode );

    // create a new window
    /*
    SDL_Surface* screen = SDL_SetVideoMode(640, 480, 16,
                                           SDL_HWSURFACE|SDL_DOUBLEBUF);
    */
    SDL_Window *window = SDL_CreateWindow("The Game of Life",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          width, height,
                          0);
    SDL_Surface *screen = SDL_GetWindowSurface(window);

    height = screen->h;
    width = screen->w;

    if ( !screen )
    {
        printf("Unable to set %ix%i video: %s\n",width, height, SDL_GetError());
        return 1;
    }

    firstarray = (int**)calloc(height+2, sizeof(int*));
    secondarray = (int**)calloc(height+2, sizeof(int*));
    for(int i=0;i<height+2;i++)
        {
            firstarray[i] = (int*)calloc(width+2, sizeof(int));
            secondarray[i] = (int*)calloc(width+2, sizeof(int));
        }

    for(int i=0;i<width*height;i++)
        {
            pixels[i].h = 1;
            pixels[i].w = 1;
        }
    // load an image
    /*
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
    */

    SDL_Thread *inputthread = SDL_CreateThread(inputhtreadfunction, "Input Thread", 0);

    // program main loop
    while (!done)
    {
        if(pause==0)
        {
            SDL_Delay(0);
            copyedges(firstarray);
            SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));
            life(firstarray,secondarray);
            SDL_FillRects(screen,pixels,howmuchpixels,SDL_MapRGB(screen->format, 255, 255, 255));
            SDL_UpdateWindowSurface(window);


            SDL_Delay(0);
            copyedges(secondarray);
            SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));
            life(secondarray,firstarray);
            SDL_FillRects(screen,pixels,howmuchpixels,SDL_MapRGB(screen->format, 255, 255, 255));
            SDL_UpdateWindowSurface(window);
        }
        else if(pause==1)
        {
            SDL_Delay(6);
            SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));
            arraytopixels(firstarray);
            SDL_FillRects(screen,pixels,howmuchpixels,SDL_MapRGB(screen->format, 255, 255, 255));
            SDL_UpdateWindowSurface(window);
        }
        // DRAWING STARTS HERE

        // clear screen
        //SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));

        // draw bitmap
        //SDL_BlitSurface(bmp, 0, screen, &dstrect);

        // DRAWING ENDS HERE

        // finally, update the screen :)
        /*
        SDL_Flip(screen);
        */
    } // end main loop

    // free loaded bitmap
    /*
    SDL_FreeSurface(bmp);
    */

    free(firstarray);
    free(secondarray);

    // all is well ;)
    printf("Exited cleanly\n");
    return 0;
}
