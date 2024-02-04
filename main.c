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
#define DEFAULT_PLAYER_POS_X 100
#define DEFAULT_PLAYER_POS_Y 100

// 函数宏
// 处理SDL错误用宏封装简化
#define CHECK_ERROR(name)                                   \
    if (!(name)) {                                          \
        SDL_Log("Failed because of: %s\n", SDL_GetError()); \
        return 1;                                           \
    }

// 枚举
// 几个游戏状态: 菜单栏 / 游戏中, 通过枚举类型实现
enum status {
    start_menu = 0,
    launching = 1,
    gaming = 2,
    pause_menu = 3,
    lose = 4,
};
// 几个玩家状态: 正常 / 蓄力 / 跳跃 / 跳上 / 失败, 通过枚举类型实现
enum motions {
    common = 0,
    gaining_momentum = 1,
    jumping = 2,
};

// 结构
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
    bool end_game;
} GAME;

PLAYER player = {
        .MaxJumpDistance = DEFAULT_MAXIMUM_JUMP_DISTANCE,
        .CurJumpDistance = 0,
        .GainMomentumSpeed = DEFAULT_GAIN_MOMENTUM_SPEED,
        .motion = common,
        .cur_x = DEFAULT_PLAYER_POS_X,
        .cur_y = DEFAULT_PLAYER_POS_Y
};

GAME game = {
        .history_high = 0,
        .score = 0,
        .mode = start_menu,
        .end_game = false
};


// SDL相关对象
SDL_Window *win;
SDL_Event MainEvent;
SDL_Surface *start_menu_img[2] = {NULL};
SDL_Surface *pause_menu_img = NULL;
SDL_Texture *start_menu_texture[2] = {NULL};
SDL_Texture *pause_menu_texture = NULL;
SDL_Texture *cur_texture[8] = {NULL}; // 下一帧的贴图, 其中[0]默认留给玩家棋子, [1]留给当前平台, [2]留给下一个平台
SDL_Renderer *Default_rdr = NULL;
SDL_Rect StartButton = {150, 656, 280, 90};
SDL_Rect QuitButton;
SDL_Rect AgainButton;
SDL_Rect PicPosition[8];
int texture_cnt = 0;
// TODO: 创建若干个rect类型对象, 用于指示贴图位置

// 函数
void draw() {
    // 更新画面
    // 先显示背景, 顺便清除上一帧
    SDL_SetRenderDrawColor(Default_rdr, 144, 144, 152, 255);
    SDL_RenderClear(Default_rdr);
    // 在指定位置加载所有贴图
    for (int i = 0; i < texture_cnt; ++i) {
        SDL_RenderCopy(Default_rdr, cur_texture[i], NULL, &PicPosition[i]);
    }
    SDL_RenderPresent(Default_rdr);
}

int initSDL() {
    // 初始化SDL子系统
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        printf("Can not init SDL\n");
        SDL_GetError();
        return 1;
    }
    // 建立窗口
    win = SDL_CreateWindow("跳一跳",
                           SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                           WINDOW_WIDTH, WINDOW_HEIGHT,
                           SDL_WINDOW_ALLOW_HIGHDPI);
    CHECK_ERROR(win);
    // 初始化win窗口的渲染器(画笔), 自动选择, 硬件加速
    Default_rdr = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    CHECK_ERROR(Default_rdr);

    return 0;
}

int InitPic(SDL_Surface **surf, SDL_Texture **text, char *file_name) {
    *surf = IMG_Load(file_name);
    CHECK_ERROR(surf);
    *text = SDL_CreateTextureFromSurface(Default_rdr, *surf);
    CHECK_ERROR(text);
    return 0;
}

int InitAllPics() {
    // 为所有图片资源分配surface和texture
    // 开始菜单贴图
    InitPic(&start_menu_img[0], &start_menu_texture[0], "start_menu_common.jpeg");
    InitPic(&start_menu_img[1], &start_menu_texture[1], "start_menu_pressed.jpeg");
    SDL_SetWindowSize(win, start_menu_img[0]->w / 2, start_menu_img[0]->h / 2);

    // 暂停菜单贴图

    // 计分板贴图

    // 平台贴图

    // 玩家贴图

    // bonus(彩蛋)贴图

    //蓄力条贴图

    return 0;
}

void DestroyAll() {
    SDL_FreeSurface(start_menu_img[0]);
    SDL_FreeSurface(start_menu_img[1]);
    SDL_FreeSurface(pause_menu_img);
    SDL_DestroyWindowSurface(win);
    SDL_DestroyTexture(start_menu_texture[0]);
    SDL_DestroyTexture(start_menu_texture[1]);
    SDL_DestroyTexture(pause_menu_texture);
    for (int i = 0; i < 7; ++i) {
        SDL_DestroyTexture(cur_texture[i]);
    }
    SDL_DestroyRenderer(Default_rdr);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

void RefreshGame() {
    // 重开
    // 重置游戏状态

    // 重置玩家数值

}

void MoveMap() {
    static int cnt = 0;
    // 让地图动起来, 即完成两项工作: 移动视角, 加载新平台
    // 移动视角
    for (int i = 0; i < 100; ++i) {
        if (cnt % 2 == 0) {
            // 移动旧平台与玩家棋子

            // 生成新平台

        } else {

        }
    }
}

void GainMomentum() {
    // 实现蓄力, 其中同时需要实现读条
    // 蓄力方式 -> 更改CurJumpDistance
}

void EnterFailMenu() {
    // 修改游戏状态
    game.mode = lose;
    // 展示失败菜单

    // 等待事件, 决定是重新开始还是退出游戏

}

bool Landed() {
    // 判断是否成功着陆

    return true;
}

void Jump() {
    // 实现跳跃
    // 跳跃方式 -> 向目的地做出跳跃的过渡动画, 期间除了暂停不允许其他操作
    // 修改状态
    player.motion = jumping;
    // 跳跃动画

    // 判断是否落到平台上
    if (Landed()) {
        // 修改棋子和平台坐标->视角移动(使用循环和延迟不断更新画面)

        // 加载下一个平台

        // 最终回到common状态
        player.motion = common;
        return;
    } else {
        // 着陆失败, 显示结束菜单
        EnterFailMenu();
    }
}

// 为判断点击按钮而造的轮子
bool MyPointInRect(SDL_Rect *rect, int x, int y) {
    if (rect->x <= x && x <= rect->x + rect->w
        && rect->y <= y && y <= rect->y + rect->h) {
        return true;
    } else {
        return false;
    }
}

void StartMenuPressed() {
    // 准备启动
    // 修改游戏状态
    game.mode = launching;
    // 显示按下按钮之后的图片
    SDL_RenderClear(Default_rdr);
    SDL_RenderCopy(Default_rdr, start_menu_texture[1], NULL, NULL);
    SDL_RenderPresent(Default_rdr);
}

void StartGame() {
    // 切换游戏模式
    game.mode = gaming;
    // 显示一段时间的原始菜单(模拟按钮回弹效果)
    SDL_RenderClear(Default_rdr);
    SDL_RenderCopy(Default_rdr, start_menu_texture[0], NULL, NULL);
    SDL_RenderPresent(Default_rdr);
    SDL_Delay(200);
    // 正式启动游戏
    // 显示背景
    SDL_SetRenderDrawColor(Default_rdr, 144, 144, 152, 255);
    SDL_RenderClear(Default_rdr);
    SDL_RenderPresent(Default_rdr);
    // 初始化两个平台一个棋子
}

void WaitForStart() {
    // 展示开始界面
    SDL_RenderClear(Default_rdr);
    SDL_RenderCopy(Default_rdr, start_menu_texture[0], NULL, NULL);
    SDL_RenderPresent(Default_rdr);

    // 接收开始信号
    SDL_Event StartEvent;
    while (game.mode == start_menu || game.mode == launching) {
        while (SDL_PollEvent(&StartEvent)) {
            switch (StartEvent.type) {
                case SDL_MOUSEBUTTONDOWN: // 鼠标点击事件, 仅用于处理点击开始菜单中的按钮
                    SDL_Log("Mouse Click Position: x = %d, y = %d", StartEvent.button.x, StartEvent.button.y);
                    // 在开始菜单中点击"开始游戏"按钮
                    if (game.mode == start_menu &&
                        MyPointInRect(&StartButton, StartEvent.button.x, StartEvent.button.y)) {
                        // 开始菜单点击
                        StartMenuPressed();
                    }
                    // 借鉴中断例程, 暂停菜单和失败菜单都由函数本身来轮询(反正暂停了)
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (game.mode == launching) {
                        StartGame();
                    }
                    break;
            }
        }
    }
}

void PauseMenu() {
    // 实现暂停菜单
    // 修改游戏状态
    game.mode = pause_menu;
    // 展示暂停菜单
    SDL_RenderClear(Default_rdr);
    SDL_RenderCopy(Default_rdr, pause_menu_texture, NULL, NULL);
    SDL_RenderPresent(Default_rdr);
    // 开始子例程轮询
    SDL_Event PauseMenuEvent;
    while (game.mode == pause_menu) {
        while (SDL_PollEvent(&PauseMenuEvent)) {
            switch (PauseMenuEvent.type) {
                case SDL_MOUSEBUTTONDOWN: // 鼠标点击事件, 仅用于处理点击开始菜单中的按钮
                    SDL_Log("Mouse Click Position: x = %d, y = %d", PauseMenuEvent.button.x, PauseMenuEvent.button.y);
                    int cur_x = PauseMenuEvent.button.x, cur_y = PauseMenuEvent.button.y;
                    // 在开始菜单中点击"开始游戏"按钮
                    if (MyPointInRect(&QuitButton, cur_x, cur_y)) {
                        // 退出
                        game.end_game = true;
                        return;

                    } else if (MyPointInRect(&AgainButton, cur_x, cur_y)) {
                        // 重开, 保留历史最高分, 清除现有得分,游戏时间以及难度系数
                        RefreshGame();
                    }
                    // 借鉴中断例程, 暂停菜单和失败菜单都由函数本身来轮询(反正暂停了)
                    break;
                default:;
            }
        }
    }
}

void DuringGame() {
    // 处理可能发生的玩家行为
    // 需要实现异步, 必须通过轮询(PollEvent)而不能等待(WaitEvent)
    while (SDL_PollEvent(&MainEvent) && game.end_game == false) {
        // 选择事件类型
        // 综合几个因素, 修改下一帧画面显示对象的种类及位置
        switch (MainEvent.type) {
            case SDL_QUIT:
                // 处理退出事件
                SDL_Log("QUIT GAME!\n");
                game.end_game = true;
                break;
            case SDL_KEYDOWN: // 键盘点击事件, 处理玩家操作
                if (MainEvent.key.keysym.sym == SDLK_SPACE) {
                    // 空格键
                    // 游戏中(非跳跃过程中)按下空格 -> 蓄力
                    SDL_Log("蓄力ing\n");
                    if (game.mode == gaming && player.motion == common) {
                        player.motion = gaining_momentum;
                        GainMomentum();
                    }
                } else if (MainEvent.key.keysym.sym == SDLK_ESCAPE) {
                    // Esc键
                    // 游戏中任何时候按下Esc -> 退出游戏, 进入暂停菜单
                    if (game.mode == gaming) {
                        PauseMenu();
                    }
                }
                break;
            case SDL_KEYUP:
                // 放开空格键跳跃的部分, 是否可以作为Jump函数的一部分, 而不是放在轮询过程中?
                if (MainEvent.key.keysym.sym == SDLK_SPACE) {
                    // 放开空格 -> 跳跃
                    // TODO: 处理跳跃
                    if (game.mode == gaming && player.motion == gaining_momentum) {
                        Jump();
                    }
                }
                break;
            default:;
        }
    }
}

int main() {
    // ①初始化SDL子系统
    initSDL();

    // 加载所有图片, 显示开始菜单
    InitAllPics();

    // 等待开始
    WaitForStart();

    // 游戏主体: 两个阶段(处理事件->渲染下一帧)
    while (game.end_game == false) {
        // 玩家操作
        DuringGame();
        // 更新下一帧画面
        draw();
    }
    // 有开就有关
    DestroyAll();
    return 0;
}