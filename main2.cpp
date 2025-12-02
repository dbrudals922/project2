#include <SDL2/SDL.h>
#include <ctime>
#include <string>
#include "png_utils.hpp"

enum Part { HAIR, TOP, BOTTOM, SHOES, DONE };

struct Outfit {
    int hair, top, bottom, shoes, topNum, bottomNum, shoesNum;
};

void renderOutfit(SDL_Renderer* renderer, SDL_Rect& dst, SDL_Texture* base,
                  SDL_Texture* hair, SDL_Texture* top,
                  SDL_Texture* bottom, SDL_Texture* shoes) {
    // SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, base, NULL, &dst);
    if (hair) SDL_RenderCopy(renderer, hair, NULL, &dst);
    if (bottom) SDL_RenderCopy(renderer, bottom, NULL, &dst);
    if (top) SDL_RenderCopy(renderer, top, NULL, &dst);
    if (shoes) SDL_RenderCopy(renderer, shoes, NULL, &dst);
    SDL_RenderPresent(renderer);
}

int main()
{
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* win = SDL_CreateWindow("Dressup Compare Game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1500, 1200, 0);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    Outfit answer = { rand()%4, rand()%5, rand()%5, rand()%5, rand()%4, rand()%4, rand()%2 };

    PNGImage basePNG, hairPNG, topPNG, bottomPNG, shoesPNG;
    LoadPNG("assets/base.png", basePNG);
    LoadPNG("assets/hair/hair0.png", hairPNG);
    LoadPNG("assets/top/top"+std::to_string(answer.topNum)+".png", topPNG);
    LoadPNG("assets/bottom/bottom"+std::to_string(answer.bottomNum)+".png", bottomPNG);
    LoadPNG("assets/shoes/shoes"+std::to_string(answer.shoesNum)+".png", shoesPNG);
    // LoadPNG("assets/top/top0.png", topPNG);
    // LoadPNG("assets/bottom/bottom0.png", bottomPNG);
    // LoadPNG("assets/shoes/shoes0.png", shoesPNG);

    SDL_Texture* baseTex = PNGToTexture(ren, basePNG);


    PNGImage hairAns, topAns=topPNG, bottomAns=bottomPNG, shoesAns=shoesPNG;
    LoadPNG("assets/hair/hair"+std::to_string(answer.hair)+".png", hairAns);
    ApplyFixedColorStyle(topAns, answer.top);
    ApplyFixedColorStyle(bottomAns, answer.bottom);
    ApplyFixedColorStyle(shoesAns, answer.shoes);

    SDL_Rect dstLeft = {0, 100, basePNG.width, basePNG.height};
    SDL_Rect dstRight = {500, 100, basePNG.width, basePNG.height};

    SDL_SetRenderDrawColor(ren,255,255,255,255);
    SDL_RenderClear(ren);

    // 정답 보여주기
    renderOutfit(ren, dstLeft, baseTex,
                  PNGToTexture(ren, hairAns),
                  PNGToTexture(ren, topAns),
                  PNGToTexture(ren, bottomAns),
                  PNGToTexture(ren, shoesAns));
    SDL_Delay(2000);

    Outfit player = {0,0,0,0,0,0,0};
    Part curPart = HAIR;
    Uint32 lastChange = SDL_GetTicks();
    bool spaceDown = false;
    bool running = true;

    SDL_Texture* hairTex   = PNGToTexture(ren, hairPNG);

    SDL_SetRenderDrawColor(ren,255,255,255,255);
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, baseTex, NULL, &dstLeft);
    SDL_RenderCopy(ren, hairTex, NULL, &dstLeft);
    SDL_RenderPresent(ren);

    while(running)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            if(e.type==SDL_QUIT) running=false;
            if(e.type==SDL_KEYDOWN && e.key.keysym.sym==SDLK_SPACE && !spaceDown)
            {
                spaceDown=true;
                if(curPart==HAIR) curPart=TOP;
                else if(curPart==TOP) curPart=BOTTOM;
                else if(curPart==BOTTOM) curPart=SHOES;
                else if(curPart==SHOES) curPart=DONE;
            }
            if(e.type==SDL_KEYUP && e.key.keysym.sym==SDLK_SPACE)
                spaceDown=false;
        }

        Uint32 now = SDL_GetTicks();
        if(curPart!=DONE && now-lastChange>350)
        {
            lastChange = now;
            int style = rand()%5;

            if(curPart==HAIR){ LoadPNG("assets/hair/hair"+std::to_string(rand()%4)+".png", hairPNG); player.hair=style; continue; }
            if(curPart==TOP){ LoadPNG("assets/top/top"+std::to_string(rand()%4)+".png", topPNG); ApplyFixedColorStyle(topPNG, style); player.top=style; continue; }
            if(curPart==BOTTOM){ LoadPNG("assets/bottom/bottom"+std::to_string(rand()%4)+".png", bottomPNG); ApplyFixedColorStyle(bottomPNG, style); player.bottom=style; continue; }
            if(curPart==SHOES){ LoadPNG("assets/shoes/shoes"+std::to_string(rand()%2)+".png", shoesPNG); ApplyFixedColorStyle(shoesPNG, style); player.shoes=style; continue; }
        }

        // 텍스처 생성
        SDL_Texture* hairTexP   = PNGToTexture(ren, hairPNG);
        SDL_Texture* topTexP    = PNGToTexture(ren, topPNG);
        SDL_Texture* bottomTexP = PNGToTexture(ren, bottomPNG);
        SDL_Texture* shoesTexP  = PNGToTexture(ren, shoesPNG);

        SDL_SetRenderDrawColor(ren,255,255,255,255);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, baseTex, NULL, &dstLeft);

        // 단계별로 나타나기
        if(curPart>=HAIR)   SDL_RenderCopy(ren, hairTexP, NULL, &dstLeft);
        if(curPart>=BOTTOM) SDL_RenderCopy(ren, bottomTexP, NULL, &dstLeft);
        if(curPart>=TOP)    SDL_RenderCopy(ren, topTexP, NULL, &dstLeft);
        if(curPart>=SHOES)  SDL_RenderCopy(ren, shoesTexP, NULL, &dstLeft);

        SDL_RenderPresent(ren);

        SDL_DestroyTexture(hairTexP);
        SDL_DestroyTexture(topTexP);
        SDL_DestroyTexture(bottomTexP);
        SDL_DestroyTexture(shoesTexP);

        if(curPart==DONE)
        {
            SDL_Delay(200);

            bool success = (player.hair==answer.hair && player.top==answer.top &&
                            player.bottom==answer.bottom && player.shoes==answer.shoes &&
                            player.topNum==answer.topNum && player.bottomNum==answer.bottomNum &&
                            player.shoesNum==answer.shoesNum);

            Uint8 r = success ? 144 : 255;
            Uint8 g = success ? 238 : 128; 
            Uint8 b = success ? 144 : 128; 

            SDL_SetRenderDrawColor(ren,r,g,b,50);
            SDL_RenderClear(ren);

            // 플레이어 선택
            renderOutfit(ren, dstLeft, baseTex,
                PNGToTexture(ren, hairPNG),
                PNGToTexture(ren, topPNG),
                PNGToTexture(ren, bottomPNG),
                PNGToTexture(ren, shoesPNG));

            // 정답
            renderOutfit(ren, dstRight, baseTex,
                          PNGToTexture(ren, hairAns),
                          PNGToTexture(ren, topAns),
                          PNGToTexture(ren, bottomAns),
                          PNGToTexture(ren, shoesAns));

            SDL_RenderPresent(ren);
            SDL_Delay(3000);
            running=false;
        }
    }

    SDL_DestroyTexture(baseTex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
