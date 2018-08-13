/*
 * Copyright (C) 2002-2008 The Warp Rogue Team
 * Part of the Warp Rogue Project
 *
 * This software is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License.
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY.
 *
 * See the license.txt file for more details.
 */

/*
 * SDL platform code
 */

/*
 * The SDL platform is an incomplete platform that must be
 * combined with another platform which provides the missing
 * functionality. 
 */


#include "wrogue.h"


#include <SDL/SDL.h>




/*
 * image files
 */
#define FILE_BACKGROUND_IMAGE   "back.bmp"
#define FILE_ICON_IMAGE         "icon.bmp"
#define FILE_FONT_IMAGE         "font.bmp"
#define FILE_CURSOR_IMAGE       "cursor.bmp"


/* 
 * constants
 */
#define FONT_WIDTH              8
#define FONT_HEIGHT             16

#define PADDING_V               1
#define PADDING_H               2

#define SCREEN_WIDTH            800
#define SCREEN_HEIGHT           600
#define COLOUR_DEPTH            16

typedef float                   GAMMA_VALUE;



static void                     sdl_init(void);
static void                     sdl_window_init(void);
static void                     sdl_screen_init(void);
static void                     sdl_input_init(void);
static void                     sdl_clean_up(void);

static void                     colours_init(void);
static void                     colours_clean_up(void);

static void                     font_init(void);
static void                     font_clean_up(void);

static void                     cursor_init(void);
static void                     cursor_clean_up(void);

static void                     background_init(void);
static void                     background_clean_up(void);

static void                     load_bmp_error(void);


static Uint32                   VideoFlags = SDL_SWSURFACE;

static SDL_Surface *            Screen = NULL;
static Uint32                   ScreenBlack;

static SDL_Surface *            Font = NULL;

static SDL_Colour               Colour[MAX_COLOURS];
static N_COLOURS                N_Colours;

static SCREEN_COORD             CursorX = 0;
static SCREEN_COORD             CursorY = 0;
static SDL_Surface *            Cursor = NULL;

static SDL_Surface *            Background = NULL;



/*
 * platform UI init
 */
void ui_init(void)
{

        sdl_init();

        colours_init();
        
        font_init();
        
        cursor_init();
        
        background_init();
}



/*
 * platform UI clean up
 */
void ui_clean_up(void)
{

        background_clean_up();

        cursor_clean_up();

        font_clean_up();
        
        colours_clean_up();

        sdl_clean_up();
}



/* 
 * returns true if the fullscreen mode is activated
 */
bool fullscreen_mode(void)
{

        if ((VideoFlags & SDL_FULLSCREEN) == 0) return false;

        return true;
}



/*
 * changes the screen mode 
 */
void change_screen_mode(void)
{

        /* fullscreen mode activated .. */
        if (fullscreen_mode()) {
                
                /* switch to windowed */
                VideoFlags &= ~SDL_FULLSCREEN;
        
        /* windowed mode activated .. */
        } else {
                
                /* switch to fullscreen mode */
                VideoFlags |= SDL_FULLSCREEN;
        }

        /* switch mode */
        Screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT,
                COLOUR_DEPTH, VideoFlags
        );
}



/*
 * updates the screen
 */
void update_screen(void)
{

        SDL_Flip(Screen);
}



/*
 * clears the screen
 */
void clear_screen(void)
{

        SDL_FillRect(Screen, NULL, ScreenBlack);
}



/*
 * places the cursor at the specified location
 */
void place_cursor_at(SCREEN_COORD y, SCREEN_COORD x)
{

        CursorY = y;
        CursorX = x;
}



/*
 * returns the current Y coordinate of the cursor
 */
SCREEN_COORD cursor_y(void)
{

        return CursorY;
}



/*
 * returns the current X coordinate of the cursor
 */
SCREEN_COORD cursor_x(void)
{

        return CursorX;
}



/*
 * renders the cursor at its current position
 */
void render_cursor(COLOUR colour)
{
        SDL_Rect dest;

        SDL_SetColors(Cursor, &Colour[colour], 1, 1);

        dest.x = CursorX * FONT_WIDTH;
        dest.y = CursorY * FONT_HEIGHT;

        SDL_BlitSurface(Cursor, NULL, Screen, &dest);
}



/*
 * renders a character at the current cursor position
 */
void render_char(COLOUR colour, SYMBOL ch)
{
        SDL_Rect dest;
        SDL_Rect symbol_rect = {0, 0, FONT_WIDTH, FONT_HEIGHT};

        ch -= SYMBOL_OFFSET; 

        symbol_rect.x = ch * FONT_WIDTH;

        dest.x = CursorX * FONT_WIDTH;
        dest.y = CursorY * FONT_HEIGHT;

        SDL_SetColors(Font, &Colour[colour], 1, 1);

        SDL_BlitSurface(Font, &symbol_rect, Screen, &dest);

        ++CursorX;
}



/*
 * renders a character at the specified position
 */
void render_char_at(COLOUR colour, SCREEN_COORD y, SCREEN_COORD x, SYMBOL ch)
{

        CursorY = y;
        CursorX = x;

        render_char(colour, ch);
}



/*
 * get key
 */
KEY_CODE lowlevel_get_key(void)
{
        SDL_Event event;
        SDLKey key;

        do {

                do {

                        SDL_WaitEvent(&event);

                        if (event.type == SDL_QUIT) {

                                die("abnormal program "\
                                        "termination (SDL_QUIT)"
                                );
                        }

                } while (event.type != SDL_KEYDOWN);

                key = event.key.keysym.sym;

        } while (
                key == SDLK_CAPSLOCK ||
                key == SDLK_RSHIFT ||
                key == SDLK_LSHIFT ||
                key == SDLK_RCTRL ||
                key == SDLK_LCTRL ||
                key == SDLK_RALT ||
                key == SDLK_LALT ||
                key == SDLK_RMETA ||
                key == SDLK_LMETA ||
                key == SDLK_MODE
        );

        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE: return KEY_ESC;
        case SDLK_RETURN: /* FALLTHROUGH */
        case SDLK_KP_ENTER: return KEY_ENTER;
        case SDLK_BACKSPACE: return KEY_BKSP;
        case SDLK_TAB: return KEY_TAB;
        case SDLK_SPACE: return KEY_SPACE;
        case SDLK_DELETE: return KEY_DELETE;
        case SDLK_UP: return KEY_UP;
        case SDLK_DOWN: return KEY_DOWN;
        case SDLK_RIGHT: return KEY_RIGHT;
        case SDLK_LEFT: return KEY_LEFT;
        case SDLK_F1: return KEY_F1;
        case SDLK_F2: return KEY_F2;
        case SDLK_F3: return KEY_F3;
        case SDLK_F4: return KEY_F4;
        case SDLK_F5: return KEY_F5;
        case SDLK_F6: return KEY_F6;
        case SDLK_F7: return KEY_F7;
        case SDLK_F8: return KEY_F8;
        case SDLK_F9: return KEY_F9;
        case SDLK_F10: return KEY_F10;
        case SDLK_F11: return KEY_F11;
        case SDLK_F12: return KEY_F12;
        case SDLK_KP0: return '0';
        case SDLK_KP1: return '1';
        case SDLK_KP2: return '2';
        case SDLK_KP3: return '3';
        case SDLK_KP4: return '4';
        case SDLK_KP5: return '5';
        case SDLK_KP6: return '6';
        case SDLK_KP7: return '7';
        case SDLK_KP8: return '8';
        case SDLK_KP9: return '9';
        case SDLK_PAGEUP: return KEY_PAGE_UP;
        case SDLK_PAGEDOWN: return KEY_PAGE_DOWN;
        default: return event.key.keysym.unicode;
        }
}



/*
 * clears a section of the screen for text display
 */
void clear_text_window(SCREEN_COORD y1, SCREEN_COORD x1,
        SCREEN_COORD y2, SCREEN_COORD x2
)
{
        SDL_Rect rect;

        rect.x = x1 * FONT_WIDTH - PADDING_H;
        rect.y = y1 * FONT_HEIGHT - PADDING_V;
        rect.w = FONT_WIDTH * (x2 - x1) + FONT_WIDTH + (PADDING_H * 2);
        rect.h = FONT_HEIGHT * (y2 - y1) + FONT_HEIGHT + (PADDING_V * 2);

        SDL_FillRect(Screen, &rect, ScreenBlack);
}



/*
 * renders the background
 */
void render_background(void)
{
        Sint16 y, x, y_inc, x_inc;
        SDL_Rect rect;

        for (y = 0, y_inc = Background->h;
                y <= SCREEN_HEIGHT;
                y += y_inc) {

                for (x = 0, x_inc = Background->w;
                        x <= SCREEN_WIDTH;
                        x += x_inc) {

                        rect.x = x;
                        rect.y = y;

                        SDL_BlitSurface(Background, NULL,
                                Screen, &rect
                        );
                }
        }
}



/*
 * waits n seconds
 */
void sec_sleep(int n_seconds)
{

        SDL_Delay(n_seconds * 1000);
}



/*
 * SDL init
 */
static void sdl_init(void)
{

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0 ) {

                die("*** SDL ERROR *** unable to initialize SDL (%s)",
                        SDL_GetError()
                );
        }

        sdl_window_init();
        
        sdl_screen_init();

        sdl_input_init();
}



/*
 * SDL window init
 */
static void sdl_window_init(void)
{
        SDL_Surface *window_icon;
        
        SDL_WM_SetCaption(GAME_NAME, GAME_NAME);

        set_data_path(DIR_UI, FILE_ICON_IMAGE);

        window_icon = SDL_LoadBMP(data_path());
        if (window_icon == NULL) load_bmp_error();

        SDL_WM_SetIcon(window_icon, NULL);

        SDL_FreeSurface(window_icon);
}



/*
 * SDL screen init
 */
static void sdl_screen_init(void)
{

        Screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT,
                COLOUR_DEPTH, VideoFlags
        );

        if (Screen == NULL) {

                die("*** SDL ERROR *** unable to set video mode (%s)",
                        SDL_GetError()
                );
        }
        
        ScreenBlack = SDL_MapRGB(Screen->format, 0x00, 0x00, 0x00);
}



/*
 * SDL input init(void)
 */
static void sdl_input_init(void)
{

        SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,
                SDL_DEFAULT_REPEAT_INTERVAL
        );

        SDL_EnableUNICODE(1);

        SDL_ShowCursor(SDL_DISABLE);
}



/*
 * SDL clean up
 */
static void sdl_clean_up(void)
{

        SDL_Quit();
}



/*
 * colours init
 */
static void colours_init(void)
{
        COLOUR c;
        
        N_Colours = n_colours();
        
        /* init colours */
        for (c = 0; c < N_Colours; c++) {
                const RGB_DATA *rgb;

                rgb = colour_rgb(c);

                Colour[c].r = rgb->red;
                Colour[c].g = rgb->green;
                Colour[c].b = rgb->blue;
        }
}



/*
 * colours clean up
 */
static void colours_clean_up(void)
{

        /* nothing to do */
}



/*
 * font init
 */
static void font_init(void)
{       
        set_data_path(DIR_UI, FILE_FONT_IMAGE);
        
        Font = SDL_LoadBMP(data_path());
        if (Font == NULL) load_bmp_error();
}



/*
 * font clean up
 */
static void font_clean_up(void)
{

        if (Font != NULL) {

                SDL_FreeSurface(Font);
        }
}



/*
 * cursor init
 */
static void cursor_init(void)
{

        set_data_path(DIR_UI, FILE_CURSOR_IMAGE);

        Cursor = SDL_LoadBMP(data_path());
        if (Cursor == NULL) load_bmp_error();

        SDL_SetColorKey(Cursor, SDL_SRCCOLORKEY | SDL_RLEACCEL,
                SDL_MapRGB(Cursor->format, 0, 0, 0)
        );
}



/*
 * cursor clean up
 */
static void cursor_clean_up(void)
{

        if (Cursor != NULL) {

                SDL_FreeSurface(Cursor);
        }
}



/*
 * background init
 */
static void background_init(void)
{
        SDL_Surface *background_image;
        
        set_data_path(DIR_UI, FILE_BACKGROUND_IMAGE);

        background_image = SDL_LoadBMP(data_path());
        if (background_image == NULL) load_bmp_error();

        Background = SDL_DisplayFormat(background_image);
        
        SDL_FreeSurface(background_image);
}



/*
 * background clean up
 */
static void background_clean_up(void)
{

        if (Background != NULL) {
                
                SDL_FreeSurface(Background);
        }
}



/*
 * SDL_LoadBMP error
 */
static void load_bmp_error(void)
{

        die("*** SDL ERROR *** unable to load bitmap (%s)", 
                SDL_GetError()
        );
}


