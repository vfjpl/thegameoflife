#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif

#include <SDL2/SDL.h>

bool done = false;
bool pause = true;
bool usecopyedges=true;
bool newmouse=true;

int width = 640;
int height = 480;
unsigned int fullscreen=0;

int factor = 19;
unsigned int msdelay = 10;

int mouse_old_position_x;
int mouse_old_position_y;
int mouse_new_position_x;
int mouse_new_position_y;

bool arrayused;
unsigned int howmuchpixels;
unsigned long long int oldgetperformancecounter;
unsigned long long int newgetperformancecounter;

bool **firstarray;
bool **secondarray;

SDL_Rect pixels[ 1920 * 1080 ];
SDL_mutex *mutex;

void copyedges(bool **array)
{
    array[0][0] = array[height][width];

    for(int i=1; i<=width; i++)
        array[0][i] = array[height][i];

    array[0][width+1] = array[height][1];

    for(int i=1; i<=height; i++)
        array[i][width+1] = array[i][1];

    array[height+1][width+1] = array[1][1];

    for(int i=width; i>=1; i--)
        array[height+1][i] = array[1][i];

    array[height+1][0] = array[1][width];

    for(int i=height; i>=1; i--)
        array[i][0] = array[i][width];
}

void life( bool **inputarray, bool **outputarray )
{
    SDL_LockMutex(mutex);

    unsigned char count;
    howmuchpixels = 0;

    if(usecopyedges)
        copyedges( inputarray );

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
    arrayused=!arrayused;

    SDL_UnlockMutex(mutex);
}

void arraytopixels( bool **array )
{
    SDL_LockMutex(mutex);

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

    SDL_UnlockMutex(mutex);
}

void glider_array(bool **array)
{
    array[(height/2)-1][(width/2)]=1;
    array[(height/2)][(width/2)+1]=1;
    array[(height/2)+1][(width/2)-1]=1;
    array[(height/2)+1][(width/2)]=1;
    array[(height/2)+1][(width/2)+1]=1;
}

void diehard_array(bool **array)
{
    array[(height/2)-1][(width/2)+2]=1;
    array[(height/2)][(width/2)-4]=1;
    array[(height/2)][(width/2)-3]=1;
    array[(height/2)+1][(width/2)-3]=1;
    array[(height/2)+1][(width/2)+1]=1;
    array[(height/2)+1][(width/2)+2]=1;
    array[(height/2)+1][(width/2)+3]=1;
}

void dakota(bool **array)
{
    array[(height/2)-2][(width/2)-1]=1;
    array[(height/2)-2][(width/2)+2]=1;
    array[(height/2)-1][(width/2)-2]=1;
    array[(height/2)][(width/2)-2]=1;
    array[(height/2)][(width/2)+2]=1;
    array[(height/2)+1][(width/2)-2]=1;
    array[(height/2)+1][(width/2)-1]=1;
    array[(height/2)+1][(width/2)]=1;
    array[(height/2)+1][(width/2)+1]=1;
}

void linetoarray(bool **array)
{
    for(int i=1; i<=width; i++)
        array[height/2][i] = 1;
}

void randomtoarray( bool **array )
{
    SDL_LockMutex(mutex);

    for(int j = 1; j <= height; j++)
    {
        for(int i = 1; i <= width; i++)
        {
            if((rand() % factor) == 0)
                array[j][i] = 1;
        }
    }

    SDL_UnlockMutex(mutex);
}

void cleararray( bool **array )
{
    SDL_LockMutex(mutex);

    for(int j = 1; j <= height; j++)
    {
        for(int i = 1; i <= width; i++)
        {
            array[j][i] = 0;
        }
    }

    SDL_UnlockMutex(mutex);
}

void line(int x0, int y0, int x1, int y1, bool **array)
{
    int x;
    int y;

    x = (x0 + x1) / 2;
    y = (y0 + y1) / 2;

    if( x!=x0 || y!=y0) // x1 and y1 crash
    {
        line(x,y,x0,y0,array);
        line(x,y,x1,y1,array);
    }
    else
    {
        array[y1+1][x1+1] = 1;
    }

}

/*
void line_alt(float x0, float y0, float x1, float y1, bool **array)
{
    float x;
    float y;

    const float error = 0.5;

    x = (x0 + x1) / 2;
    y = (y0 + y1) / 2;

    if( x<=x-error || y<=y-error || x>=x+error || y>=y+error )
    {
        line_alt(x,y,x0,y0,array);
        line_alt(x,y,x1,y1,array);
    }
    else
    {

    }
}
*/

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

                case SDLK_EQUALS:
                {
                    msdelay++;
                    break;
                }

                case SDLK_MINUS:
                {
                    if(msdelay>0)
                        msdelay--;
                    break;
                }

                case SDLK_SPACE:
                {
                    pause = !pause;
                    SDL_ShowCursor(!SDL_ShowCursor(-1));
                    break;
                }

                case SDLK_r:
                {
                    if(!arrayused)
                        randomtoarray( firstarray );
                    else
                        randomtoarray( secondarray );

                    break;
                }

                case SDLK_c:
                {
                    if(!arrayused)
                        cleararray( firstarray );
                    else
                        cleararray( secondarray );

                    break;
                }

                case SDLK_1:
                {
                    if(!arrayused)
                        glider_array( firstarray );
                    else
                        glider_array( secondarray );

                    break;
                }

                case SDLK_2:
                {
                    if(!arrayused)
                        diehard_array( firstarray );
                    else
                        diehard_array( secondarray );

                    break;
                }

                case SDLK_3:
                {
                    if(!arrayused)
                        dakota( firstarray );
                    else
                        dakota( secondarray );

                    break;
                }

                case SDLK_l:
                {
                    if(!arrayused)
                        linetoarray( firstarray );
                    else
                        linetoarray( secondarray );

                    break;
                }

                case SDLK_f:
                {
                    printf("%lli\n", (SDL_GetPerformanceFrequency()/
                                      (newgetperformancecounter-oldgetperformancecounter)));
                    break;
                }

                }
                break;
            }

            case SDL_MOUSEMOTION:
            {
                if(event.button.button == SDL_BUTTON_LEFT)
                {
                    if(newmouse)
                    {
                        mouse_old_position_x = mouse_new_position_x;
                        mouse_old_position_y = mouse_new_position_y;
                        mouse_new_position_x = event.motion.x;
                        mouse_new_position_y = event.motion.y;

                        if(!arrayused)
                            line(mouse_old_position_x,mouse_old_position_y,
                                 mouse_new_position_x,mouse_new_position_y, firstarray );
                        else
                            line(mouse_old_position_x,mouse_old_position_y,
                                 mouse_new_position_x,mouse_new_position_y, secondarray );
                    }
                    else
                    {
                        mouse_new_position_x = event.motion.x;
                        mouse_new_position_y = event.motion.y;

                        if(!arrayused)
                            firstarray[mouse_new_position_y+1][mouse_new_position_x+1] = 1;
                        else
                            secondarray[mouse_new_position_y+1][mouse_new_position_x+1] = 1;
                    }
                }
                break;
            }

            case SDL_MOUSEBUTTONDOWN:
            {
                if(event.button.button == SDL_BUTTON_LEFT)
                {
                    if(newmouse)
                    {
                        mouse_new_position_x = event.motion.x;
                        mouse_new_position_y = event.motion.y;

                        if(!arrayused)
                            line(mouse_new_position_x,mouse_new_position_y,
                                 mouse_new_position_x,mouse_new_position_y, firstarray );
                        else
                            line(mouse_new_position_x,mouse_new_position_y,
                                 mouse_new_position_x,mouse_new_position_y, secondarray );

                    }
                    else
                    {
                        mouse_new_position_x = event.motion.x;
                        mouse_new_position_y = event.motion.y;

                        if(!arrayused)
                            firstarray[mouse_new_position_y+1][mouse_new_position_x+1] = 1;
                        else
                            secondarray[mouse_new_position_y+1][mouse_new_position_x+1] = 1;
                    }
                }
                break;
            }

            } // end switch
        } // end of message processing
    }
    return 0;
}

void printhelp()
{
    printf("Valid options:\n");
    printf("--width <x>         horizontal resolution default:%i\n", width);
    printf("--height <x>        vertical resolution default:%i\n", height);
    printf("--fullscreen        toggle fullscreen(desktop resolution is used)\n");
    printf("--noedgecopy        toggle curved space\n");
    printf("--factor <1/x>      random factor for filling array default:%i\n", factor);
    printf("--oldmouse          use old mouse behaviour\n");
    printf("--delay <x>         rise if you have problem, lower otherwise default:%u\n", msdelay);
    printf("--newrender\n");

    printf("\n");

    printf("Keys:\n");
    printf("ESC - exit program\n");
    printf("Space - toggle pause\n");
    printf("R - random life to array\n");
    printf("C - clear array from life\n");
    printf("F - print fps value to console\n");
    printf("- - lower delay\n");
    printf("= - rise delay\n");
    printf("1 - glider\n");
    printf("2 - die hard\n");
    printf("3 - Lightweight spaceship\n");
    printf("L - line\n");

}

int main ( int argc, char** argv )
{
    for(int i=1; i<argc; i++)
    {
        if(strcmp("--width",argv[i])==0)
        {
            if((width=atoi(argv[++i]))<=0)
            {
                printf("Bad value for width…\n");
                return 0;
            }
        }
        else if(strcmp("--height",argv[i])==0)
        {
            if((height=atoi(argv[++i]))<=0)
            {
                printf("Bad value for height…\n");
                return 0;
            }
        }
        else if(strcmp("--fullscreen",argv[i])==0)
        {
            fullscreen=SDL_WINDOW_FULLSCREEN_DESKTOP;
        }
        else if(strcmp("--factor",argv[i])==0)
        {
            if((factor=atoi(argv[++i]))<=0)
            {
                printf("Bad value for factor…\n");
                return 0;
            }
        }
        else if(strcmp("--noedgecopy",argv[i])==0)
        {
            usecopyedges=0;
        }
        else if(strcmp("--newrender",argv[i])==0)
        {
            printf("Work in progress…\n");
            return 0;
        }
        else if(strcmp("--delay",argv[i])==0)
        {
            msdelay=strtoul(argv[++i],NULL,0);
        }
        else if(strcmp("--oldmouse",argv[i])==0)
        {
            newmouse=false;
        }
        else
        {
            printhelp();
            return 0;
        }
    }

    // initialize SDL video
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }

    // make sure SDL cleans up before exit
    atexit(SDL_Quit);

    // create a new window
    /*
    SDL_Surface* screen = SDL_SetVideoMode(640, 480, 16,
                                           SDL_HWSURFACE|SDL_DOUBLEBUF);
    */
    SDL_Window *window = SDL_CreateWindow("The Game of Life",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          width, height,
                                          fullscreen);
    SDL_Surface *screen = SDL_GetWindowSurface(window);

    if ( !screen )
    {
        if(fullscreen!=1)
            printf("Unable to set %ix%i video: %s\n", width, height, SDL_GetError());
        else
            printf("Unable to set fullscreen video: %s\n", SDL_GetError());

        return 1;
    }

    SDL_PixelFormat *format = screen->format;
    height = screen->h;
    width = screen->w;

    firstarray = (bool**)calloc(height+2, sizeof(bool*));
    secondarray = (bool**)calloc(height+2, sizeof(bool*));
    for(int i=0; i<height+2; i++)
    {
        firstarray[i] = (bool*)calloc(width+2, sizeof(bool));
        secondarray[i] = (bool*)calloc(width+2, sizeof(bool));
    }

    for(int i=0; i<width*height; i++)
    {
        pixels[i].h = 1;
        pixels[i].w = 1;
    }
    /*
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
    */

    newgetperformancecounter = SDL_GetPerformanceCounter();
    mutex = SDL_CreateMutex();
    SDL_Thread *inputthread = SDL_CreateThread(inputhtreadfunction, "Input Thread", nullptr);

    // program main loop
    while (!done)
    {
        SDL_Delay(msdelay);

        if(!pause)
        {
            if(!arrayused)
                life( firstarray, secondarray );
            else
                life( secondarray, firstarray );

            SDL_FillRect(screen, NULL, SDL_MapRGB(format, 0, 0, 0));
            SDL_FillRects(screen,pixels,howmuchpixels,SDL_MapRGB(format, 255, 255, 255));
            SDL_UpdateWindowSurface(window);
        }
        else
        {
            if(!arrayused)
                arraytopixels( firstarray );
            else
                arraytopixels( secondarray );

            SDL_FillRect(screen, NULL, SDL_MapRGB(format, 0, 0, 0));
            SDL_FillRects(screen,pixels,howmuchpixels,SDL_MapRGB(format, 255, 255, 255));
            SDL_UpdateWindowSurface(window);
        }
        oldgetperformancecounter = newgetperformancecounter;
        newgetperformancecounter = SDL_GetPerformanceCounter();
        /*
        // DRAWING STARTS HERE

        // clear screen
        SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));

        // draw bitmap
        SDL_BlitSurface(bmp, 0, screen, &dstrect);

        // DRAWING ENDS HERE

        // finally, update the screen :)
        SDL_Flip(screen);
        */
    } // end main loop

    // free loaded bitmap
    /*
    SDL_FreeSurface(bmp);
    */

    free( firstarray );
    free( secondarray );

    SDL_DestroyMutex(mutex);

    // all is well ;)
    printf("Exited cleanly\n");
    return 0;
}
