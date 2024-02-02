#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <stdio.h>

enum status {
    menu,
    gaming
};
SDL_Window *win;
int mode;
const int WIDTH = 400, HEIGHT = 400;


int main() {
    // ①初始化SDL子系统
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        printf("Can not init SDL\n");
        SDL_GetError();
        return 1;
    }
    // ②建立窗口和渲染器
    // 建立窗口
    mode = menu;
    win = SDL_CreateWindow("地铁跑酷", SDL_WINDOWPOS_CENTERED,
                           SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT,
                           SDL_WINDOW_ALLOW_HIGHDPI);
    if (win == NULL) {
        printf("Can not create window");
        SDL_GetError();
        return 1;
    }
    // 初始化渲染器(画笔)
    SDL_Renderer *Default_rdr = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    // ③加载所有图片文件到相应画布
    SDL_Surface *bg_img = IMG_Load("demo1.png");
    if (!bg_img) {
        printf("Can not load background picture!\n");
        SDL_GetError();
        return 1;
    }
    SDL_Texture *bg_texture = SDL_CreateTextureFromSurface(Default_rdr, bg_img);
    SDL_RenderCopy(Default_rdr, bg_texture, NULL, NULL);
    SDL_RenderPresent(Default_rdr);
    // 背景图片加载完毕

    // 游戏主体
    SDL_Event MainEvent;
    while (SDL_WaitEvent(&MainEvent)) {
        // 创建临时texture, 画上背景后free


        // 选择事件类型
        // 几个状态: 菜单栏 / 游戏中, 通过枚举类型实现
        switch (MainEvent.type) {
            case SDL_QUIT:
                printf("SDL QUIT!\n");
                return 0;
        }
    }

    // 有开就有关
    SDL_DestroyRenderer(Default_rdr);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}