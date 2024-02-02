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
// TODO: 完善合理的初始数值设定
#define DEFAULT_MAXIMUM_JUMP_DISTANCE 100
#define DEFAULT_PLATFORM_SIZE 100
#define DEFAULT_GAIN_MOMENTUM_SPEED 100

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
    pause_menu = 2,
    lose = 3,
};
// 几个玩家状态: 正常 / 蓄力 / 跳跃 / 跳上 / 失败, 通过枚举类型实现
enum motions {
    common = 0,
    gaining_momentum = 1,
    jumping = 2,
    hit_dest = 3,
    failed = 4
};
// 玩家数值设定
typedef struct player {
    // 玩家数值
    int MaxJumpDistance;
    int GainMomentumSpeed;
    int CurJumpDistance;
    // 玩家状态
    int motion;
    // 玩家位置
    int cur_x;
    int cur_y;
} PLAYER;
typedef struct game {
    int mode;
    int score;
    int history_high;
} GAME;


// SDL相关对象
SDL_Window *win;
SDL_Event MainEvent;
SDL_Surface *bg_img = NULL;
SDL_Texture *bg_texture;
SDL_Renderer *Default_rdr;
// TODO: 创建若干个rect类型对象, 用于指示贴图位置

// 游戏状态, 玩家状态指示器
int game_mode = 0;

// 函数
void draw() {
    // 更新画面
}

int initSDL() {
    // 初始化SDL子系统
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        printf("Can not init SDL\n");
        SDL_GetError();
        return 1;
    }
    // 建立窗口
    win = SDL_CreateWindow("跳一跳", SDL_WINDOWPOS_CENTERED,
                           SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                           SDL_WINDOW_ALLOW_HIGHDPI);
    CHECK_ERROR(win);
    // 初始化win窗口的渲染器(画笔), 自动选择, 硬件加速
    Default_rdr = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    CHECK_ERROR(Default_rdr);

    return 0;
}

int initPics() {
    // 为所有图片资源分配surface和texture
    // ③加载所有图片文件到相应画布
    // 背景图片
    bg_img = IMG_Load("start_menu.jpg");
    CHECK_ERROR(bg_img);
    SDL_SetWindowSize(win, bg_img->w / 2, bg_img->h / 2);
    bg_texture = SDL_CreateTextureFromSurface(Default_rdr, bg_img);
    CHECK_ERROR(bg_texture);

    return 0;
}

void DestroyAll() {
    // 关闭所有子系统
    SDL_DestroyTexture(bg_texture);
    SDL_DestroyWindowSurface(win);
    SDL_DestroyRenderer(Default_rdr);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

void MoveMap() {
    // 让地图动起来
}

void GainMomentum() {
    // 实现蓄力, 其中同时需要实现读条
    // 蓄力方式 -> 更改CurJumpDistance
}

void Jump() {
    // 实现跳跃
    // 跳跃方式 -> 向目的地做出跳跃的过渡动画, 期间除了暂停不允许其他操作
}

void PauseMenu() {
    // 实现暂停菜单
}


int main() {
    // ①初始化SDL子系统
    initSDL();

    // 加载背景图片
    initPics();
    SDL_RenderCopy(Default_rdr, bg_texture, NULL, NULL);
    SDL_RenderPresent(Default_rdr);

    // 初始化游戏状态
    PLAYER player = {
            .MaxJumpDistance = DEFAULT_MAXIMUM_JUMP_DISTANCE,
            .CurJumpDistance = 0,
            .GainMomentumSpeed = DEFAULT_GAIN_MOMENTUM_SPEED,
            .motion = common,
            // .cur_x = ,
            // .cur_y =
    };
    GAME game = {
            .history_high = 0,
            .score = 0,
            .mode = start_menu
    };
    // 游戏主体: 两个阶段(处理事件->渲染下一帧)
    while (true) {
        while (SDL_PollEvent(&MainEvent)) {
            // 选择事件类型
            // 综合几个因素, 决定下一帧画面的显示内容, 用函数来实现对帧内容的编辑
            switch (MainEvent.type) {
                case SDL_QUIT:
                    // 处理退出事件
                    printf("QUIT GAME!\n");
                    return 0;
                case SDL_MOUSEMOTION: // 鼠标事件, 供测试用
                    SDL_Log("Current Mouse Position: x = %d, y = %d", MainEvent.motion.x, MainEvent.motion.y);
                    break;
                case SDL_KEYDOWN: // 键盘点击事件, 处理玩家操作
                    if (MainEvent.key.keysym.sym == SDLK_SPACE) {
                        // 游戏中(非跳跃过程中)按下空格 -> 蓄力
                        // TODO: 处理蓄力
                        // 测试: SDL_Log("蓄力ing\n");
                        if (game.mode == gaming && player.motion == common) {
                            player.motion = gaining_momentum;
                            GainMomentum();
                        }
                    } else if (MainEvent.key.keysym.sym == SDLK_ESCAPE) {
                        // 游戏中任何时候按下Esc -> 退出游戏, 进入暂停菜单
                        // TODO: 建立暂停菜单
                        if (game.mode == gaming) {
                            game.mode = pause_menu;
                            PauseMenu();
                        }
                    }
                    break;
                case SDL_KEYUP:
                    if (MainEvent.key.keysym.sym == SDLK_SPACE) {
                        // 放开空格 -> 跳跃
                        // TODO: 处理跳跃
                        if (game.mode == gaming && player.motion == gaining_momentum) {
                            player.motion = jumping;
                            Jump();
                        }
                    }
                    break;
                default:
                    continue;
            }
            // 更新画布
            draw();
        }
    }
    // 有开就有关
    DestroyAll();
    return 0;
}