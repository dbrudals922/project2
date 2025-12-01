#include <SDL2/SDL.h>
#include <ctime>
#include <string>
#include "png_utils.hpp"

enum Part { HAIR, TOP, BOTTOM, SHOES, DONE };

struct Outfit {
    int hair, top, bottom, shoes;
};

int main()
{
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* win = SDL_CreateWindow("Dressup Compare Game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1500, 1200, 0);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    // PNG 로드
    PNGImage basePNG, hairPNG, topPNG, bottomPNG, shoesPNG;
    LoadPNG("assets/base.png", basePNG);
    // LoadPNG("assets/hair/hair0.png", hairPNG);
    LoadPNG("assets/top/top0.png", topPNG);
    LoadPNG("assets/bottom/bottom0.png", bottomPNG);
    LoadPNG("assets/shoes/shoes0.png", shoesPNG);

    SDL_Texture* baseTex = PNGToTexture(ren, basePNG);

    // ------------------------
    // 랜덤 정답 생성
    // ------------------------
    Outfit answer = { rand()%5, rand()%5, rand()%5, rand()%5 };

    // 정답용 PNG
    PNGImage hairAns, topAns=topPNG, bottomAns=bottomPNG, shoesAns=shoesPNG;
    LoadPNG("assets/hair/hair"+std::to_string(answer.hair)+".png", hairAns);
    ApplyFixedColorStyle(topAns, answer.top);
    ApplyFixedColorStyle(bottomAns, answer.bottom);
    ApplyFixedColorStyle(shoesAns, answer.shoes);

    SDL_Rect dstLeft = {0,100,basePNG.width, basePNG.height};
    SDL_Rect dstRight = {500,100,basePNG.width, basePNG.height};

    // 정답 2초 보여주기
    SDL_Texture* hairTex   = PNGToTexture(ren, hairAns);
    SDL_Texture* topTex    = PNGToTexture(ren, topAns);
    SDL_Texture* bottomTex = PNGToTexture(ren, bottomAns);
    SDL_Texture* shoesTex  = PNGToTexture(ren, shoesAns);

    SDL_SetRenderDrawColor(ren,255,255,255,255);
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, baseTex, NULL, &dstLeft);
    SDL_RenderCopy(ren, hairTex, NULL, &dstLeft);
    SDL_RenderCopy(ren, bottomTex, NULL, &dstLeft);
    SDL_RenderCopy(ren, topTex, NULL, &dstLeft);
    SDL_RenderCopy(ren, shoesTex, NULL, &dstLeft);
    SDL_RenderPresent(ren);
    SDL_Delay(2000);

    SDL_DestroyTexture(hairTex);
    SDL_DestroyTexture(topTex);
    SDL_DestroyTexture(bottomTex);
    SDL_DestroyTexture(shoesTex);

    // ------------------------
    // 플레이어 초기화
    // ------------------------
    Outfit player = {0,0,0,0};
    Part curPart = HAIR;
    Uint32 lastChange = SDL_GetTicks();
    bool spaceDown = false;
    bool running = true;

    // base만 먼저 보여주기
    SDL_SetRenderDrawColor(ren,255,255,255,255);
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, baseTex, NULL, &dstLeft);
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
        if(curPart!=DONE && now-lastChange>500)
        {
            lastChange = now;
            int style = rand()%5;

            if(curPart==HAIR){ LoadPNG("assets/hair/hair"+std::to_string(style)+".png", hairPNG); player.hair=style; }
            if(curPart==TOP){ LoadPNG("assets/top/top0.png", topPNG); ApplyFixedColorStyle(topPNG, style); player.top=style; }
            if(curPart==BOTTOM){ LoadPNG("assets/bottom/bottom0.png", bottomPNG); ApplyFixedColorStyle(bottomPNG, style); player.bottom=style; }
            if(curPart==SHOES){ LoadPNG("assets/shoes/shoes0.png", shoesPNG); ApplyFixedColorStyle(shoesPNG, style); player.shoes=style; }
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

        // ------------------------
        // DONE 단계 → 비교 화면 + 배경색
        // ------------------------
        if(curPart==DONE)
        {
            SDL_Delay(200);

            // 결과 판단
            bool success = (player.hair==answer.hair && player.top==answer.top &&
                            player.bottom==answer.bottom && player.shoes==answer.shoes);

            Uint8 r = success ? 144 : 255;
            Uint8 g = success ? 238 : 128; 
            Uint8 b = success ? 144 : 128; 


            SDL_SetRenderDrawColor(ren,r,g,b,50);
            SDL_RenderClear(ren);

            // 정답 텍스처
            hairTex   = PNGToTexture(ren, hairAns);
            topTex    = PNGToTexture(ren, topAns);
            bottomTex = PNGToTexture(ren, bottomAns);
            shoesTex  = PNGToTexture(ren, shoesAns);

            // 플레이어 텍스처
            hairTexP   = PNGToTexture(ren, hairPNG);
            topTexP    = PNGToTexture(ren, topPNG);
            bottomTexP = PNGToTexture(ren, bottomPNG);
            shoesTexP  = PNGToTexture(ren, shoesPNG);

            // 오른쪽: 정답
            SDL_RenderCopy(ren, baseTex, NULL, &dstRight);
            SDL_RenderCopy(ren, hairTex, NULL, &dstRight);
            SDL_RenderCopy(ren, bottomTex, NULL, &dstRight);
            SDL_RenderCopy(ren, topTex, NULL, &dstRight);
            SDL_RenderCopy(ren, shoesTex, NULL, &dstRight);

            // 왼쪽: 플레이어 선택
            SDL_RenderCopy(ren, baseTex, NULL, &dstLeft);
            SDL_RenderCopy(ren, hairTexP, NULL, &dstLeft);
            SDL_RenderCopy(ren, bottomTexP, NULL, &dstLeft);
            SDL_RenderCopy(ren, topTexP, NULL, &dstLeft);
            SDL_RenderCopy(ren, shoesTexP, NULL, &dstLeft);

            SDL_RenderPresent(ren);
            SDL_Delay(3000);
            running=false;

            SDL_DestroyTexture(hairTex);
            SDL_DestroyTexture(topTex);
            SDL_DestroyTexture(bottomTex);
            SDL_DestroyTexture(shoesTex);
            SDL_DestroyTexture(hairTexP);
            SDL_DestroyTexture(topTexP);
            SDL_DestroyTexture(bottomTexP);
            SDL_DestroyTexture(shoesTexP);
        }
    }

    SDL_DestroyTexture(baseTex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
