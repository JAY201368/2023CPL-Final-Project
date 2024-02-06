// 头文件
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include <stdio.h>
#include <stdbool.h>

// 宏
// 初始游戏窗口大小设定
#define WINDOW_WIDTH 620
#define WINDOW_HEIGHT 822
// 初始数值设定
// TODO: 完善合理的初始数值设定
#define DEFAULT_CHESS_POS_X 370
#define DEFAULT_CHESS_POS_Y 600
#define DEFAULT_GRAVITY 10
#define DEFAULT_MAX_VX 12
#define DEFAULT_VY 10
#define DEFAULT_VX_INC_SPEED 1

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
    lose_menu,
    common,
    gaining_momentum,
    jumping,
    landing,
    move_map
};

// 结构
// 玩家数值设定
typedef struct player {
    // 玩家数值
    double MaxVx;
    double gravity;
    double Vx;
    double Vy;
} PLAYER;

typedef struct game {
    int mode;
    int score;
    int history_high;
    bool end_game;
} GAME;

typedef struct img {
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect pos;
} IMG;

PLAYER player = {
        .gravity = DEFAULT_GRAVITY,
        .MaxVx = DEFAULT_MAX_VX,
        .Vy = DEFAULT_VY,
        .Vx = 0
};

GAME game = {
        .history_high = 0,
        .score = 0,
        .mode = start_menu,
        .end_game = false,
};


// SDL相关对象
SDL_Window *win;
SDL_Event MainEvent;
SDL_Renderer *Default_rdr = NULL;
IMG StartMenu[2];
IMG PauseMenu;
IMG Platform[10];
IMG Chess;
IMG Dashboard;
IMG SrcPlatform, DstPlatform;
IMG AgainButton;
IMG QuitButton;
IMG ResumeButton;
SDL_Rect StartButton = {150, 656, 280, 90};
// TODO: 建立一种机制来定位平台, 即列举出平台所有的可能位置
SDL_Rect DefaultPlatformPos[10] = {
        {289, 689, 200, 200},
        {489, 689, 200, 200},
        {489, 689, 200, 200},
        {589, 689, 200, 200},
        {689, 689, 200, 200},
        {789, 689, 200, 200},
        {789, 689, 200, 200},
};
SDL_Rect ChessPos = {370,
                     600,
                     50,
                     180};
SDL_Rect BarPos = {0, 0, 0, 10};
// plat_index指示当前使用的平台
int plat_index = 0;
int last_mode = common;

// 函数
bool MyPointInRect(SDL_Rect *ButtonPos, int x, int y) {
    // 为判断点击按钮而造的轮子
    if (ButtonPos->x <= x && x <= ButtonPos->x + ButtonPos->w
        && ButtonPos->y <= y && y <= ButtonPos->y + ButtonPos->h) {
        return true;
    } else {
        return false;
    }
}

int InitSDL() {
    // 初始化SDL子系统
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        SDL_Log("Can not init SDL: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init()) {
        SDL_Log("Can not init TTF: %s\n", SDL_GetError());
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
    CHECK_ERROR(*surf);
    *text = SDL_CreateTextureFromSurface(Default_rdr, *surf);
    CHECK_ERROR(*text);
    return 0;
}

// TODO: 设置菜单按键位置
int InitAllPics() {
    // 为所有图片资源分配surface和texture
    // 开始菜单贴图
    InitPic(&StartMenu[0].surface, &(StartMenu[0].texture), "start_menu/start_menu_common.jpeg");
    InitPic(&StartMenu[1].surface, &(StartMenu[1].texture), "start_menu/start_menu_pressed.jpeg");
    SDL_SetWindowSize(win, StartMenu[0].surface->w / 2, StartMenu[0].surface->h / 2);
    // 菜单贴图
    InitPic(&AgainButton.surface, &AgainButton.texture, "buttons/again_button.PNG");
    InitPic(&QuitButton.surface, &QuitButton.texture, "buttons/quit_button.PNG");
    InitPic(&ResumeButton.surface, &ResumeButton.texture, "buttons/resume_button.PNG");
    AgainButton.pos = (SDL_Rect) {150, 656, 280, 90};
    ResumeButton.pos = (SDL_Rect) {450, 656, 100, 90};
    QuitButton.pos = (SDL_Rect) {560, 656, 90, 90};
    // 计分板贴图
    InitPic(&Dashboard.surface, &Dashboard.texture, "dashboard.jpeg");
    // 平台贴图
    for (int i = 0; i < 7; ++i) {
        char filename[30] = {0};
        // 生成文件名
        sprintf(filename, "platforms/platform_%d.png", i + 1);
        InitPic(&Platform[i].surface, &Platform[i].texture, filename);
        Platform[i].pos = DefaultPlatformPos[i % 2];
    }
    // 玩家棋子贴图
    InitPic(&Chess.surface, &Chess.texture, "chess.png");
    // bonus(彩蛋)贴图
    return 0;
}

// TODO: 完善销毁步骤
void DestroyAll() {
    SDL_FreeSurface(StartMenu[0].surface);
    SDL_FreeSurface(StartMenu[1].surface);
    SDL_FreeSurface(PauseMenu.surface);
    SDL_DestroyWindowSurface(win);
    SDL_DestroyTexture(StartMenu[0].texture);
    SDL_DestroyTexture(StartMenu[1].texture);
    SDL_DestroyTexture(PauseMenu.texture);
    SDL_DestroyRenderer(Default_rdr);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

// TODO: 恢复所有默认数值(除了历史最高分)
void RefreshGame() {
    // 重开
    // 重置游戏状态
    game.mode = common;
    game.end_game = false;
    game.score = 0;
    // 重置玩家数值
    ChessPos.x = DEFAULT_CHESS_POS_X;
    ChessPos.y = DEFAULT_CHESS_POS_Y;
    // 重置平台位置
    DefaultPlatformPos[0] = (SDL_Rect) {};
    DefaultPlatformPos[1] = (SDL_Rect) {};
}

// TODO: 丰富平台生成位置以及地图移动方式
void MoveMap() {
    ChessPos.y = 600;
    if (DstPlatform.pos.x >= DefaultPlatformPos[0].x) {
        SrcPlatform.pos.x -= 15;
        DstPlatform.pos.x -= 5;
        ChessPos.x -= 5;
    } else {
        // 移动完毕, 加载下一个平台
        SrcPlatform = DstPlatform;
        SrcPlatform.pos = DefaultPlatformPos[0];
        plat_index = (plat_index + 1) % 7;
        DstPlatform = Platform[(plat_index + 1) % 7];
        DstPlatform.pos = DefaultPlatformPos[1];
        game.mode = common;
    }
}

// TODO: 优化碰撞检测
void CheckLanded() {
    SDL_Log("Checking Landed or Not\n");
    bool landed = false;
    // 判断是否成功着陆, 即碰撞检测
    if (DstPlatform.pos.x <= ChessPos.x + ChessPos.w &&
        ChessPos.x <= DstPlatform.pos.x + DstPlatform.pos.w) {
        landed = true;
    } else {
        landed = false;
    }
    if (landed == true) {
        // 加分
        if (plat_index == 2 || plat_index == 5) {
            // 小平台加两分
            game.score += 2;
        } else {
            // 大平台加一分
            game.score++;
        }
        game.history_high = game.score >= game.history_high ? game.score : game.history_high;
        // 修改棋子和平台坐标->视角移动(使用循环和延迟不断更新画面)
        // 恢复玩家数值(跳跃距离)
        player.Vx = 0;
        game.mode = move_map;
    } else {
        // 着陆失败, 显示结束菜单
        SDL_Log("You Lose\n");
        SDL_Log("DstPlatformPos: x = %d, y = %d\n", DstPlatform.pos.x, DstPlatform.pos.y);
        SDL_Log("ChessPos: x = %d, y = %d\n", ChessPos.x, ChessPos.y);
        SDL_Delay(10000);
        game.mode = lose_menu;
    }
}

// TODO: 优化动画和手感
void Jump() {
    // 实现跳跃
    // 修改状态, 还原数值
    BarPos.w = 0;
    game.mode = jumping;
    // 跳跃动画(按照运动的数学建模修改棋子坐标数值)
    static double dt = 1;   // dt: 运动参数
    ChessPos.x += (int) (player.Vx * dt);
    ChessPos.y -= (int) ((player.Vy * dt - player.gravity * dt * dt / 2) * 2);
    dt += 0.1;
    // 完成跳跃
    if (ChessPos.y > DEFAULT_CHESS_POS_Y) {
        // 重置参数并退出跳跃过程
        SDL_Delay(500);
        player.Vx = 0;
        dt = 0;
        game.mode = landing;
    }
}

// TODO: 优化蓄力速率数值
void GainMomentum() {
    // 实现蓄力, 其中同时需要实现读条
    // 蓄力方式 -> 更改CurJumpDistance
    BarPos.x = ChessPos.x - 150;
    BarPos.y = ChessPos.y - 100;
    if (player.Vx <= player.MaxVx) {
        player.Vx += DEFAULT_VX_INC_SPEED;
        BarPos.w += 10;
        SDL_Log("Current Vx = %f", player.Vx);
        SDL_Delay(30);
    } else {
        player.Vx = player.MaxVx;
    }
}

void StartMenuPressed() {
    // 准备启动
    // 修改游戏状态
    game.mode = launching;
    // 显示按下按钮之后的图片
    SDL_RenderClear(Default_rdr);
    SDL_RenderCopy(Default_rdr, StartMenu[1].texture, NULL, NULL);
    SDL_RenderPresent(Default_rdr);
}

void StartGame() {
    // 切换游戏模式
    game.mode = common;
    // 显示一段时间的原始菜单(模拟按钮回弹效果)
    SDL_RenderClear(Default_rdr);
    SDL_RenderCopy(Default_rdr, StartMenu[0].texture, NULL, NULL);
    SDL_RenderPresent(Default_rdr);
    SDL_Delay(100);
    // 正式启动游戏
    // 显示背景
    SDL_SetRenderDrawColor(Default_rdr, 144, 144, 152, 255);
    SDL_RenderClear(Default_rdr);
    // 初始化两个平台一个棋子
    plat_index = 0;
    SrcPlatform = Platform[plat_index];
    DstPlatform = Platform[plat_index + 1];
    SDL_RenderCopy(Default_rdr, SrcPlatform.texture, NULL, &DefaultPlatformPos[0]);
    SDL_RenderCopy(Default_rdr, DstPlatform.texture, NULL, &DefaultPlatformPos[1]);
    SDL_RenderCopy(Default_rdr, Chess.texture, NULL, &ChessPos);
    SDL_Log("Game Start!\n");
}

void WaitForStart() {
    SDL_Log("Waiting For Start\n");
    // 展示开始界面
    SDL_RenderClear(Default_rdr);
    SDL_RenderCopy(Default_rdr, (StartMenu[0]).texture, NULL, NULL);
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
                        SDL_Log("Start Button Pressed\n");
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

void ModeControl() {
    // 处理可能发生的玩家行为
    // 需要实现异步, 必须通过轮询(PollEvent)而不能等待(WaitEvent)
    while (SDL_PollEvent(&MainEvent) && game.end_game == false) {
        // 选择事件类型
        // 综合几个因素, 修改下一帧画面显示对象的种类及位置
        switch (MainEvent.type) {
            case SDL_QUIT:
                // 处理退出事件
                SDL_Log("Quit Game\n");
                game.end_game = true;
                break;
            case SDL_KEYDOWN:
                // 键盘点击事件, 处理玩家操作
                if (MainEvent.key.keysym.sym == SDLK_SPACE) {
                    // 空格键
                    // 游戏中(非跳跃过程中)按下空格 -> 蓄力
                    if (game.mode == common) {
                        SDL_Log("Gaining Momentum\n");
                        game.mode = gaining_momentum;
                    }
                } else if (MainEvent.key.keysym.sym == SDLK_ESCAPE) {
                    // Esc键
                    // 游戏中(除菜单界面外)按下Esc -> 退出游戏, 进入暂停菜单
                    if (game.mode == common || game.mode == jumping ||
                        game.mode == gaining_momentum || game.mode == landing) {
                        SDL_Log("Game Paused\n");
                        last_mode = game.mode;
                        game.mode = pause_menu;
                    }
                }
                break;
            case SDL_KEYUP:
                // 蓄力中松开空格键, 起跳
                if (MainEvent.key.keysym.sym == SDLK_SPACE) {
                    if (game.mode == gaining_momentum) {
                        SDL_Log("Jump!\n");
                        Jump();
                    }
                }
                break;
            case SDL_MOUSEBUTTONDOWN: // 鼠标点击事件, 用于处理点击菜单中的按钮
                SDL_Log("Mouse Click Position: x = %d, y = %d", MainEvent.button.x, MainEvent.button.y);
                int cur_x = MainEvent.button.x, cur_y = MainEvent.button.y;
                if (game.mode == lose_menu || game.mode == pause_menu) {
                    if (MyPointInRect(&QuitButton.pos, cur_x, cur_y)) {
                        // 退出
                        SDL_Log("Quit Game\n");
                        game.end_game = true;
                    } else if (MyPointInRect(&AgainButton.pos, cur_x, cur_y)) {
                        // 重开, 保留历史最高分, 清除现有得分,游戏时间以及难度系数
                        SDL_Log("Game Refreshed\n");
                        RefreshGame();
                    }
                }
                if (game.mode == pause_menu) {
                    if (MyPointInRect(&ResumeButton.pos, cur_x, cur_y)) {
                        // 还原原状态即继续执行原任务
                        game.mode = last_mode;
                    }
                }
                break;
            default:;
        }
    }
}

void UpdateData() {
    if (game.mode == gaining_momentum) {
        GainMomentum();
    }
    if (game.mode == jumping) {
        Jump();
    }
    if (game.mode == landing) {
        CheckLanded();
    }
    if (game.mode == move_map) {
        MoveMap();
    }
}

void DrawGame() {
    // 更新游戏画面
    // 先绘制背景, 顺便清除上一帧
    SDL_SetRenderDrawColor(Default_rdr, 144, 144, 152, 255);
    SDL_RenderClear(Default_rdr);
    // 根据游戏模式分类讨论
    switch (game.mode) {
        case pause_menu:
            // 绘制暂停菜单
            SDL_RenderCopy(Default_rdr, AgainButton.texture, NULL, &AgainButton.pos);
            SDL_RenderCopy(Default_rdr, QuitButton.texture, NULL, &QuitButton.pos);
            SDL_RenderCopy(Default_rdr, ResumeButton.texture, NULL, &ResumeButton.pos);
            // 绘制计分板
            SDL_RenderCopy(Default_rdr, Dashboard.texture, NULL, &Dashboard.pos);
            break;
        case lose_menu:
            // 绘制失败菜单
            SDL_RenderCopy(Default_rdr, AgainButton.texture, NULL, &AgainButton.pos);
            SDL_RenderCopy(Default_rdr, QuitButton.texture, NULL, &QuitButton.pos);
            // 绘制计分板
            SDL_RenderCopy(Default_rdr, Dashboard.texture, NULL, &Dashboard.pos);
            break;
        case jumping:
            // 在指定位置绘制两个平台
            SDL_RenderCopy(Default_rdr, SrcPlatform.texture, NULL, &DefaultPlatformPos[0]);
            SDL_RenderCopy(Default_rdr, DstPlatform.texture, NULL, &DefaultPlatformPos[1]);
            // 绘制棋子
            SDL_RenderCopy(Default_rdr, Chess.texture, NULL, &ChessPos);
            // 绘制计分板
            SDL_RenderCopy(Default_rdr, Dashboard.texture, NULL, &Dashboard.pos);
            break;
        case gaining_momentum:
            // 绘制平台
            SDL_RenderCopy(Default_rdr, SrcPlatform.texture, NULL, &DefaultPlatformPos[0]);
            SDL_RenderCopy(Default_rdr, DstPlatform.texture, NULL, &DefaultPlatformPos[1]);
            // 绘制棋子
            SDL_RenderCopy(Default_rdr, Chess.texture, NULL, &ChessPos);
            // 绘制计分板
            SDL_RenderCopy(Default_rdr, Dashboard.texture, NULL, &Dashboard.pos);
            // 绘制蓄力条
            SDL_SetRenderDrawColor(Default_rdr, 235, 169, 104, 255);
            SDL_RenderFillRect(Default_rdr, &BarPos);
            break;
        case common:
            // 在指定位置绘制两个平台
            SDL_RenderCopy(Default_rdr, SrcPlatform.texture, NULL, &DefaultPlatformPos[0]);
            SDL_RenderCopy(Default_rdr, DstPlatform.texture, NULL, &DefaultPlatformPos[1]);
            // 绘制棋子
            SDL_RenderCopy(Default_rdr, Chess.texture, NULL, &ChessPos);
            // 绘制计分板
            SDL_RenderCopy(Default_rdr, Dashboard.texture, NULL, NULL);
            break;
        case move_map:
            // 绘制平台
            SDL_RenderCopy(Default_rdr, SrcPlatform.texture, NULL, &SrcPlatform.pos);
            SDL_RenderCopy(Default_rdr, DstPlatform.texture, NULL, &DstPlatform.pos);
            // 绘制棋子
            SDL_RenderCopy(Default_rdr, Chess.texture, NULL, &ChessPos);
            // 绘制计分板
            SDL_RenderCopy(Default_rdr, Dashboard.texture, NULL, &Dashboard.pos);
        default:;
    }
    // 统一显示
    SDL_RenderPresent(Default_rdr);
}

int main() {
    // 初始化SDL子系统
    InitSDL();
    // 加载所有图片, 显示开始菜单
    InitAllPics();
    // 等待开始
    WaitForStart();
    // 游戏主体: 两个阶段(处理事件->渲染下一帧)
    // 将几个菜单放在事件函数中实现, 作为分支任务
    while (game.end_game == false) {
        // 接收操作, 更改模式
        ModeControl();
        // 更新参数
        UpdateData();
        // 更新动画
        DrawGame();
        // 控制帧率
        SDL_Delay(16);
    }
    // 有开就有关
    DestroyAll();
    return 0;
}
