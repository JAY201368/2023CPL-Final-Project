#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <stdio.h>
#include <stdbool.h>

#define WINDOW_WIDTH 620
#define WINDOW_HEIGHT 822

#define CHECK_ERROR(name)                              \
    if (!(name)) {                                     \
        printf("Failed when creating %s!\n", (#name)); \
        SDL_GetError();                                \
        return 1;                                      \
    }

// 几个游戏状态: 菜单栏 / 游戏中, 通过枚举类型实现
enum status {
    start_menu = 0,
    gaming = 1,
    pause_menu = 2
};
// 几个玩家状态: 正常 / 蓄力 / 跳跃, 通过枚举类型实现
enum motion {
    common = 0,
    gaining_momentum = 1,
    jumping = 2
};

SDL_Window *win;
SDL_Event MainEvent;
SDL_Surface *bg_img = NULL;
SDL_Texture *bg_texture;
SDL_Renderer *Default_rdr;
int game_mode = 0;
int player_motion = 0;

// TODO: 封装一个draw()函数, 用于使用渲染器更新所有texture的画面

int main() {
    // ①初始化SDL子系统
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        printf("Can not init SDL\n");
        SDL_GetError();
        return 1;
    }
    // ②建立窗口和渲染器
    // 建立窗口
    win = SDL_CreateWindow("跳一跳", SDL_WINDOWPOS_CENTERED,
                           SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                           SDL_WINDOW_ALLOW_HIGHDPI);
    CHECK_ERROR(win);
    // 初始化win窗口的渲染器(画笔), 自动选择, 硬件加速
    Default_rdr = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    CHECK_ERROR(Default_rdr);

    // ③加载所有图片文件到相应画布
    bg_img = IMG_Load("start_menu.jpg");
    CHECK_ERROR(bg_img);
    SDL_SetWindowSize(win, bg_img->w / 2, bg_img->h / 2);
    bg_texture = SDL_CreateTextureFromSurface(Default_rdr, bg_img);
    CHECK_ERROR(bg_texture);
    SDL_RenderCopy(Default_rdr, bg_texture, NULL, NULL);
    SDL_RenderPresent(Default_rdr);
    // 背景图片加载完毕

    // 初始化游戏状态
    game_mode = start_menu;
    player_motion = common;
    // 游戏主体: 两个阶段(处理事件->渲染下一帧)
    while (true) {
        // 选择事件类型

        // 综合以上几个因素, 决定下一帧画面的显示内容, 用函数来实现对帧内容的编辑
        while (SDL_PollEvent(&MainEvent)) {
            switch (MainEvent.type) {
                case SDL_QUIT:
                    // 处理退出事件
                    printf("QUIT GAME!\n");
                    return 0;
                    // 处理按键操作, 分不同按键做不同处理
                case SDL_KEYDOWN:
                    if (MainEvent.key.keysym.sym == SDLK_SPACE) {
                        // 按下空格 -> 蓄力
                        // TODO: 处理蓄力
                        player_motion = gaining_momentum;
                    } else if (MainEvent.key.keysym.sym == SDLK_ESCAPE) {
                        // 按下Esc -> 退出游戏, 进入暂停菜单
                        // TODO: 建立暂停菜单
                        game_mode = pause_menu;
                    }
                case SDL_KEYUP:
                    if (MainEvent.key.keysym.sym == SDLK_SPACE) {
                        // 放开空格 -> 跳跃
                        // TODO: 处理跳跃
                        player_motion = jumping;
                    }

            }
        }
    }

    // 有开就有关
    SDL_DestroyTexture(bg_texture);
    SDL_DestroyWindowSurface(win);
    SDL_DestroyRenderer(Default_rdr);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}