// 头文件
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <stdio.h>
#include <stdbool.h>

// 宏
// 初始游戏窗口大小设定
#define WINDOW_WIDTH 620
#define WINDOW_HEIGHT 822
// 初始数值设定
#define DEFAULT_MAXIMUM_JUMP_DISTANCE
#define DEFAULT_PLATFORM_SIZE

// 函数宏
// 处理SDL错误用宏封装简化
#define CHECK_ERROR(name)                              \
    if (!(name)) {                                     \
        printf("Failed when creating %s!\n", (#name)); \
        SDL_GetError();                                \
        return 1;                                      \
    }

// 枚举
// 几个游戏状态: 菜单栏 / 游戏中, 通过枚举类型实现
enum status {
    start_menu = 0,
    gaming = 1,
    pause_menu = 2
};
// 几个玩家状态: 正常 / 蓄力 / 跳跃, 通过枚举类型实现
enum motions {
    common = 0,
    gaining_momentum = 1,
    jumping = 2
};
// 玩家数值


// SDL相关对象
SDL_Window *win;
SDL_Event MainEvent;
SDL_Surface *bg_img = NULL;
SDL_Texture *bg_texture;
SDL_Renderer *Default_rdr;
// TODO: 创建若干个rect类型对象, 用于指示贴图位置

// 游戏状态, 玩家状态指示器
int game_mode = 0;
int player_motion = 0;

// 函数
// TODO: 封装一个draw()函数, 用于使用渲染器更新所有texture的画面
// TODO: 封装一个DestroyAll函数, 用于一次性退出所有打开的对象
void draw() {
    // 更新画面
}

void initSDL() {
    // 初始化SDL子系统
}

void initPics() {
    // 为所有图片资源分配surface和texture
}

void DestroyAll() {
    // 关闭所有子系统
}

void GainMomentum() {
    // 实现蓄力, 其中同时需要实现读条
}

void Jump() {
    // 实现跳跃
}

void PauseMenu() {
    // 实现暂停菜单
}


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
    // 背景图片
    bg_img = IMG_Load("start_menu.jpg");
    CHECK_ERROR(bg_img);
    SDL_SetWindowSize(win, bg_img->w / 2, bg_img->h / 2);
    bg_texture = SDL_CreateTextureFromSurface(Default_rdr, bg_img);
    CHECK_ERROR(bg_texture);
    // 加载背景图片
    SDL_RenderCopy(Default_rdr, bg_texture, NULL, NULL);
    SDL_RenderPresent(Default_rdr);

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
                case SDL_MOUSEMOTION:
                    SDL_Log("Current Mouse Position: x = %d, y = %d", MainEvent.motion.x, MainEvent.motion.y);
                    break;
                case SDL_KEYDOWN:
                    if (MainEvent.key.keysym.sym == SDLK_SPACE) {
                        // 游戏中(非跳跃过程中)按下空格 -> 蓄力
                        // TODO: 处理蓄力
                        if (game_mode == gaming && player_motion == common) {
                            player_motion = gaining_momentum;
                            // GainMomentum();
                        }
                    } else if (MainEvent.key.keysym.sym == SDLK_ESCAPE) {
                        // 游戏中任何时候按下Esc -> 退出游戏, 进入暂停菜单
                        // TODO: 建立暂停菜单
                        if (game_mode == gaming) {
                            game_mode = pause_menu;
                            // PauseMenu();
                        }
                    }
                    break;
                case SDL_KEYUP:
                    if (MainEvent.key.keysym.sym == SDLK_SPACE) {
                        // 放开空格 -> 跳跃
                        // TODO: 处理跳跃
                        if (game_mode == gaming && player_motion == gaining_momentum) {
                            player_motion = jumping;
                            // Jump();
                        }
                    }
                    break;
                default:
                    continue;
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