
#include "CommonFunc.h"
#include "PlayerObject.h"
#include "ImpTimer.h"
#include "BlockObject.h"
#include "Geometric.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 640


BaseObject gBackground;
BaseObject gGround;

TTF_Font* gFontText = NULL;
TTF_Font* gFontMenu = NULL;

Mix_Music* gMusic = NULL; 

bool InitData()
{
    BOOL bSucess = true;
    int ret = SDL_Init(SDL_INIT_VIDEO);
    if (ret < 0)
        return false;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    gWindow = SDL_CreateWindow("PlappyBird - Kim", 
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                SCREEN_WIDTH, SCREEN_HEIGHT,
                                SDL_WINDOW_OPENGL);

    if (gWindow != NULL)
    {
        gScreen = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
        if (gScreen != NULL)
        {
            SDL_SetRenderDrawColor(gScreen, RENDER_DRAW_COLOR, 
                                             RENDER_DRAW_COLOR, 
                                             RENDER_DRAW_COLOR, 
                                             RENDER_DRAW_COLOR);
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) && imgFlags))
                bSucess = false;
        }

        if (TTF_Init() == -1)
        {
            bSucess = false;
        }

        if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
        {
            bSucess = false;
        }

        gFontText = TTF_OpenFont("font//ARCADE.ttf", 38);
        if (gFontText == NULL)
        {
            return false;
        }

        gFontMenu = TTF_OpenFont("font//ARCADE.ttf", 80);
        if (gFontMenu == NULL)
        {
            return false;
        }
    }

    return bSucess;
}


bool LoadBackground()
{
    bool ret = gBackground.LoadImageFile("img//bkgn.png", gScreen);
    return ret;
}


void close()
{
    gBackground.Free();
    SDL_DestroyRenderer(gScreen);
    gScreen = NULL;

    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    if (InitData() == false)
    {
        return -1;
    }

    ImpTimer fps;
    bool quit = false;

    //Make menu game 
    int ret_menu = SDLCommonFunc::ShowMenu(gScreen, gFontMenu, "Play Game", "Exit", "img//MENU.png");
    if (ret_menu == 1)
        quit = true;

    if (!LoadBackground())
    {
        return -1;
    }

    bool ret = gGround.LoadImageFile("img//ground2.png", gScreen);
    if (ret == false)
    {
        return -1;
    }
    gGround.SetRect(0, GROUND_MAP);

        //Load music
        gMusic = Mix_LoadMUS("21_sound_effects_and_music/beat.wav");         //link nhac.
    if (Mix_PlayingMusic() == 0)
    {
        //Play the music
        Mix_PlayMusic(gMusic, -1);
    }
again_label:

    TextObject text_count_;
    text_count_.setColor(TextObject::WHITE_TEXT);

    PlayerObject player;
    ret = player.LoadImg("img//fl_bird.png", gScreen);
    player.SetRect(100, 100);
    if (ret == false)
    {
        return -1;
    }

    BlockManager manage_block;
    ret = manage_block.InitBlockList(gScreen);
    if (ret == false)
        return -1;

    while (!quit)
    {
        fps.start();
        while (SDL_PollEvent(&g_event) != 0)
        {
            if (g_event.type == SDL_QUIT)
            {
                quit = true;
            }

            player.HandleInputAction(g_event, gScreen);
        }

        SDL_SetRenderDrawColor(gScreen, RENDER_DRAW_COLOR, 
                                RENDER_DRAW_COLOR, 
                                RENDER_DRAW_COLOR, 
                                RENDER_DRAW_COLOR);

        SDL_RenderClear(gScreen);

        gBackground.Render(gScreen, NULL);

        manage_block.SetPlayerRect(player.GetRect());

        bool is_falling = player.GetFalling();
        if (is_falling == true)
        {
            manage_block.SetStopMoving(true);
        }

        manage_block.Render(gScreen);

        bool end_game = manage_block.GetColState();
        if (end_game == true)
        {
            player.SetFalling(true);
        }
        player.DoFalling(gScreen);
        player.Show(gScreen);

        //Draw Geometric
        GeometricFormat rectange_size(0, 0, SCREEN_WIDTH, 30);
        ColorData color_data(36, 36, 36);
        Gemometric::RenderRectange(rectange_size, color_data, gScreen);

        GeometricFormat outlie_size(1, 1, SCREEN_WIDTH - 1, 28);
        ColorData color_data1(255, 255, 255);
        Gemometric::RenderOutline(outlie_size, color_data1, gScreen);

        int count = manage_block.GetCount();
        std::string count_str = std::to_string(count);
        text_count_.SetText(count_str);
        text_count_.loadFromRenderedText(gFontText, gScreen);
        text_count_.RenderText(gScreen, SCREEN_WIDTH*0.5, 2);

        gGround.Render(gScreen);

        SDL_RenderPresent(gScreen);

        // Make menu game over
        bool game_over = player.GetIsDie();
        if (game_over == true)
        {
            Sleep(500);
            int ret_menu = SDLCommonFunc::ShowMenu(gScreen, gFontMenu,
                                                   "Play Again", "Exit",
                                                    "img//MENU END.png");
            if (ret_menu == 1)
            {
                quit = true;
                continue;
            }
            else
            {
                quit = false;
                manage_block.FreeBlock();
                goto again_label;
            }
        }

        //Cap the frame rate
        int val1 = fps.get_ticks();
        if (fps.get_ticks() < 1000 / FRAMES_PER_SECOND)
        {
            SDL_Delay((1000 / FRAMES_PER_SECOND) - fps.get_ticks());
        }
    }

    close();
    return 0;
}
