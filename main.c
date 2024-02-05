// 头文件
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

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
    start_menu,
    launching,
    pause_menu,
    lose,
    gaming,
    common,
    gaining_momentum,
    jumping,
};

// 结构
// 玩家数值设定
typedef struct player {
    // 玩家数值
    int MaxJumpDistance;
    int GainMomentumSpeed;
    int CurJumpDistance;
    // 玩家位置
    int cur_x;
    int cur_y;
} PLAYER;

typedef struct game {
    int mode;
    int score;
    int history_high;
    int difficulty;
    bool end_game;
} GAME;

PLAYER player = {
        .MaxJumpDistance = DEFAULT_MAXIMUM_JUMP_DISTANCE,
        .CurJumpDistance = 0,
        .GainMomentumSpeed = DEFAULT_GAIN_MOMENTUM_SPEED,
        .cur_x = DEFAULT_PLAYER_POS_X,
        .cur_y = DEFAULT_PLAYER_POS_Y
};

GAME game = {
        .history_high = 0,
        .score = 0,
        .mode = start_menu,
        .end_game = false,
        .difficulty = 0
};


// SDL相关对象
SDL_Window *win;
SDL_Event MainEvent;
SDL_Renderer *Default_rdr = NULL;
SDL_Surface *start_menu_surface[2] = {NULL};
SDL_Surface *pause_menu_surface = NULL;
SDL_Surface *fail_menu_surface = NULL;
SDL_Surface *platform_surface[10];
SDL_Surface *chess_surface = NULL;
SDL_Surface *dashboard_surface = NULL;
SDL_Surface *bar_surface = NULL;
SDL_Texture *start_menu_texture[2] = {NULL};
SDL_Texture *pause_menu_texture = NULL;
SDL_Texture *fail_menu_texture = NULL;
SDL_Texture *platform_texture[10];
SDL_Texture *chess_texture = NULL;
SDL_Texture *dashboard_texture = NULL;
SDL_Rect StartButton = {150, 656, 280, 90};
SDL_Rect QuitButton;
SDL_Rect AgainButton;
SDL_Rect PlatformPos[10] = {
        {389, 489, 200, 200},
        {89, 489, 200, 200},
        {389, 489, 200, 200},
        {89, 489, 200, 200},
        {389, 489, 200, 200},
        {89, 489, 200, 200},
        {389, 489, 200, 200},
        {89, 489, 200, 200},
        {389, 489, 200, 200},
        {89, 489, 200, 200},
};
SDL_Rect ChessPos = {170, 400, 50, 180};
SDL_Rect Bar = {0, 0, 0, 2};
int plat_index = 0;
// TODO: 创建若干个rect类型对象, 用于指示贴图位置

// 函数
void DrawGame() {
    // 更新游戏画面
    // 先绘制背景, 顺便清除上一帧
    SDL_SetRenderDrawColor(Default_rdr, 144, 144, 152, 255);
    SDL_RenderClear(Default_rdr);
    // 分类讨论
    switch (game.mode) {
        case gaming:
            // 绘制平台
            for (int i = plat_index; i <= plat_index + 1; ++i) {
                SDL_RenderCopy(Default_rdr, platform_texture[i], NULL, &PlatformPos[i]);
            }
            // 绘制棋子
            SDL_RenderCopy(Default_rdr, chess_texture, NULL, &ChessPos);
            break;
        case pause_menu:
            // 绘制暂停菜单
            SDL_RenderCopy(Default_rdr, pause_menu_texture, NULL, NULL);
            break;
        case lose:
            // 绘制失败菜单
            SDL_RenderCopy(Default_rdr, fail_menu_texture, NULL, NULL);
            break;
        case jumping:
            // 在指定位置绘制两个平台
            for (int i = 0; i < 2; ++i) {
                SDL_RenderCopy(Default_rdr, platform_texture[i], NULL, &PlatformPos[i]);
            }
            // 绘制棋子
            SDL_RenderCopy(Default_rdr, chess_texture, NULL, &ChessPos);
            // 绘制计分板
            SDL_RenderCopy(Default_rdr, dashboard_texture, NULL, NULL);
            break;
        case gaining_momentum:
            // 在指定位置绘制两个平台
            for (int i = 0; i < 2; ++i) {
                SDL_RenderCopy(Default_rdr, platform_texture[i], NULL, &PlatformPos[i]);
            }
            // 绘制棋子
            SDL_RenderCopy(Default_rdr, chess_texture, NULL, &ChessPos);
            // 绘制计分板
            SDL_RenderCopy(Default_rdr, dashboard_texture, NULL, NULL);
            // 绘制蓄力条
            // TODO
            break;
        case common:
            // 在指定位置绘制两个平台
            for (int i = 0; i < 2; ++i) {
                SDL_RenderCopy(Default_rdr, platform_texture[i], NULL, &PlatformPos[i]);
            }
            // 绘制棋子
            SDL_RenderCopy(Default_rdr, chess_texture, NULL, &ChessPos);
            // 绘制计分板
            SDL_RenderCopy(Default_rdr, dashboard_texture, NULL, NULL);
            break;
        default:;
    }
    // 统一显示
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
    InitPic(&start_menu_surface[0], &start_menu_texture[0], "start_menu/start_menu_common.jpeg");
    InitPic(&start_menu_surface[1], &start_menu_texture[1], "start_menu/start_menu_pressed.jpeg");
    SDL_SetWindowSize(win, start_menu_surface[0]->w / 2, start_menu_surface[0]->h / 2);

    // 暂停菜单贴图
    InitPic(&pause_menu_surface, &pause_menu_texture, "pause_menu.jpeg");
    // 失败菜单贴图
    InitPic(&fail_menu_surface, &fail_menu_texture, "fail_menu.jpeg");
    // 计分板贴图
    InitPic(&dashboard_surface, &dashboard_texture, "dashboard.jpeg");
    // 平台贴图
    for (int i = 0; i < 7; ++i) {
        char filename[30] = {0};
        sprintf(filename, "platforms/platform_%d.png", i + 1);
        InitPic(&platform_surface[i], &platform_texture[i], filename);
    }
    // 玩家棋子贴图
    InitPic(&chess_surface, &chess_texture, "chess.png");
    //蓄力条
    bar_surface = SDL_GetWindowSurface(win);
    // bonus(彩蛋)贴图

    return 0;
}

void DestroyAll() {
    // TODO: 完善销毁步骤
    SDL_FreeSurface(start_menu_surface[0]);
    SDL_FreeSurface(start_menu_surface[1]);
    SDL_FreeSurface(pause_menu_surface);
    SDL_DestroyWindowSurface(win);
    SDL_DestroyTexture(start_menu_texture[0]);
    SDL_DestroyTexture(start_menu_texture[1]);
    SDL_DestroyTexture(pause_menu_texture);
    SDL_DestroyRenderer(Default_rdr);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

void RefreshGame() {
    // 重开
    // 重置游戏状态
    game.mode = common;
    game.end_game = false;
    game.difficulty = 0;
    game.score = 0;
    // 重置玩家数值
    player.CurJumpDistance = 0;
    player.MaxJumpDistance = DEFAULT_MAXIMUM_JUMP_DISTANCE;
    player.cur_x = DEFAULT_PLAYER_POS_X;
    player.cur_y = DEFAULT_PLAYER_POS_Y;
    player.GainMomentumSpeed = DEFAULT_GAIN_MOMENTUM_SPEED;
    // 重置平台位置
    PlatformPos[1] = (SDL_Rect) {};
    PlatformPos[2] = (SDL_Rect) {};
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

bool Landed() {
    // 判断是否成功着陆

    return true;
}

void Jump() {
    // 实现跳跃
    // 修改状态
    game.mode = jumping;
    // 跳跃动画

    // 判断是否落到平台上
    if (Landed()) {
        // 加分
        game.score++;
        game.history_high = game.score >= game.history_high ? game.score : game.history_high;
        // 修改棋子和平台坐标->视角移动(使用循环和延迟不断更新画面)
        MoveMap();
        // 加载下一个平台

        // 恢复玩家数值(跳跃距离)
        player.CurJumpDistance = 0;
        // 最终回到common状态
        game.mode = common;
        return;
    } else {
        // 着陆失败, 显示结束菜单
        game.mode = lose;
        return;
    }
}

void GainMomentum() {
    // 实现蓄力, 其中同时需要实现读条
    // 蓄力方式 -> 更改CurJumpDistance
    game.mode = gaining_momentum;
    if (player.CurJumpDistance <= player.MaxJumpDistance) {
        player.CurJumpDistance++;
        Bar.w++;
    } else {
        player.CurJumpDistance = player.MaxJumpDistance;
    }
    // 更新蓄力条
    SDL_FillRect(bar_surface, &Bar, 999999);
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
    // 初始化两个平台一个棋子
    for (int i = 0; i < 2; ++i) {
        SDL_RenderCopy(Default_rdr, platform_texture[i], NULL, &PlatformPos[i]);
    }
//    SDL_RenderCopy(Default_rdr, chess_texture, NULL, &ChessPos);
//    SDL_RenderPresent(Default_rdr);
//    SDL_Delay(5000);
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
                case SDL_QUIT:
                    SDL_Log("Quit Game!\n");
                    DestroyAll();
                    game.end_game = true;
                    return;
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
                SDL_Log("QUIT GAME\n");
                game.end_game = true;
                break;
            case SDL_KEYDOWN: // 键盘点击事件, 处理玩家操作
                if (MainEvent.key.keysym.sym == SDLK_SPACE) {
                    // 空格键
                    // 游戏中(非跳跃过程中)按下空格 -> 蓄力
                    if (game.mode == gaming) {
                        SDL_Log("蓄力ing\n");
                        GainMomentum();
                    }
                } else if (MainEvent.key.keysym.sym == SDLK_ESCAPE) {
                    // Esc键
                    // 游戏中任何时候按下Esc -> 退出游戏, 进入暂停菜单
                    if (game.mode == gaming || game.mode == jumping || game.mode == gaining_momentum) {
                        SDL_Log("Game Paused\n");
                        game.mode = pause_menu;
                    }
                }
                break;
            case SDL_KEYUP:
                if (game.mode == gaining_momentum) {
                    Jump();
                }
            case SDL_MOUSEBUTTONDOWN: // 鼠标点击事件, 用于处理点击菜单中的按钮
                SDL_Log("Mouse Click Position: x = %d, y = %d", MainEvent.button.x, MainEvent.button.y);
                int cur_x = MainEvent.button.x, cur_y = MainEvent.button.y;
                if (game.mode == lose || game.mode == pause_menu) {
                    if (MyPointInRect(&QuitButton, cur_x, cur_y)) {
                        // 退出
                        SDL_Log("Quit Game\n");
                        game.end_game = true;
                    } else if (MyPointInRect(&AgainButton, cur_x, cur_y)) {
                        // 重开, 保留历史最高分, 清除现有得分,游戏时间以及难度系数
                        SDL_Log("Game Refreshed\n");
                        RefreshGame();
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
    // 将几个菜单放在事件函数中实现, 作为分支任务
    while (game.end_game == false) {
        // 玩家操作
        DuringGame();
        // 更新动画
        DrawGame();
        // 控制帧率
        SDL_Delay(50);
    }
    // 有开就有关
    DestroyAll();
    return 0;
}
