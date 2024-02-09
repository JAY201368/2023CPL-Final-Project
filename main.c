// 头文件
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

// 宏
// 初始游戏窗口大小设定
#define WINDOW_WIDTH 620
#define WINDOW_HEIGHT 822
// 初始数值设定
// TODO: 完善合理的初始数值设定
#define DEFAULT_CHESS_POS_X 160
#define DEFAULT_CHESS_POS_Y 400
#define DEFAULT_GRAVITY 12
#define DEFAULT_MAX_VX 60
#define DEFAULT_VY 50
#define DEFAULT_VX_INC_SPEED 1

// 枚举
// 几个游戏状态: 菜单栏 / 游戏中, 通过枚举类型实现
enum status {
    start_menu,
    launching,
    confirm_launch,
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

PLAYER player;

GAME game;

// SDL相关对象
SDL_Window *win;
SDL_Event MainEvent;
SDL_Renderer *Default_rdr = NULL;
IMG Platform[10];
IMG Chess;
IMG SrcPlatform, DstPlatform;
IMG StartButton, AgainButton, QuitButton, ResumeButton;
IMG Bar, BarWrapper;
IMG CurMark, HistoryMark;
IMG StartWords, StartChess;
IMG FailMsg, PauseMsg;
IMG LevelMsg[3];

SDL_Rect StartButtonPos[2] = {
        {170, 656, 280, 90},
        {185, 656, 260, 80}
};
SDL_Rect DefaultSrcPlatformPos = {80, 450, 160, 160};
// TODO: 丰富平台位置
SDL_Rect ExtendedPlatformPos[10] = {
        {250, 450, 160, 160},
        {300, 450, 160, 160},
        {320, 450, 160, 160},
        {350, 450, 130, 130},
        {380, 450, 130, 130},
        {400, 450, 130, 130},
        {420, 450, 100, 100},
        {440, 450, 100, 100},
        {450, 450, 100, 100},
        {460, 450, 100, 100},
};
TTF_Font *DefaultFont;
// plat_index指示当前使用的平台, position_index指示生成平台的位置
int plat_index = 0, position_index = 0;
int last_mode = common;
int previous_chess_pos_x = 0;

// 函数
void CheckError(void *name);

bool MyPointInRect(SDL_Rect *ButtonPos, int x, int y);

int GetRandomIndex();

int InitSDL();

int InitPic(SDL_Surface **surf, SDL_Texture **text, char *file_name);

int InitWordMsg(IMG *word, char *msg, SDL_Rect position, SDL_Color color);

int InitAllPics();

void RestartGame();

void DestroyPic(IMG pic);

void DestroyAll();

void GainMomentum();

void Jump();

void CheckLanded();

void MoveMap();

void ShowMark();

void ModeControl();

void UpdateData();

void DrawGame();

int main() {
    // 初始化SDL子系统
    InitSDL();
    // 加载所有图片, 显示开始菜单
    InitAllPics();
    // 等待开始
//    WaitForStart();
    // 游戏主体: 两个阶段(处理事件->渲染下一帧)
    // 将几个菜单放在事件函数中实现, 作为分支任务
    RestartGame();
    game.mode = start_menu;
    while (game.end_game == false) {
        // 接收操作, 更改模式
        ModeControl();
        // 更新参数
        UpdateData();
        // 更新动画
        DrawGame();
        // 控制帧率
        SDL_Delay(4);
    }
    // 有开就有关
    DestroyAll();
    return 0;
}

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

// SDL相关函数
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
    // 注意: 由于macOS平台本身高DPI功能
    win = SDL_CreateWindow("Jump, Jump!",
                           SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                           WINDOW_WIDTH, WINDOW_HEIGHT,
                           SDL_WINDOW_SHOWN);
    CheckError(win);
    // 初始化win窗口的渲染器(画笔), 自动选择, 硬件加速
    Default_rdr = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    CheckError(Default_rdr);

    return 0;
}

int InitPic(SDL_Surface **surf, SDL_Texture **text, char *file_name) {
    *surf = IMG_Load(file_name);
    CheckError(*surf);
    *text = SDL_CreateTextureFromSurface(Default_rdr, *surf);
    CheckError(*text);
    return 0;
}

int InitWordMsg(IMG *word, char *msg, SDL_Rect position, SDL_Color color) {
    word->surface = TTF_RenderText_Blended(DefaultFont, msg, color);
    CheckError(word->surface);
    word->texture = SDL_CreateTextureFromSurface(Default_rdr, word->surface);
    CheckError(word->texture);
    word->pos = position;
    return 0;
}

int InitAllPics() {
    // 为所有图片资源分配surface和texture
    // 开始菜单贴图
    InitPic(&StartButton.surface, &StartButton.texture, "res/buttons/start_button.PNG");
    InitPic(&StartWords.surface, &StartWords.texture, "res/start_menu/Word.PNG");
    InitPic(&StartChess.surface, &StartChess.texture, "res/start_menu/Chess.PNG");
    StartWords.pos = (SDL_Rect) {200, 100, 200, 60};
    StartChess.pos = (SDL_Rect) {175, 300, 200, 200};
    // 菜单按键贴图
    InitPic(&AgainButton.surface, &AgainButton.texture, "res/buttons/again_button.PNG");
    InitPic(&QuitButton.surface, &QuitButton.texture, "res/buttons/quit_button.PNG");
    InitPic(&ResumeButton.surface, &ResumeButton.texture, "res/buttons/resume_button.PNG");
    AgainButton.pos = (SDL_Rect) {50, 450, 280, 90};
    ResumeButton.pos = (SDL_Rect) {480, 450, 80, 80};
    QuitButton.pos = (SDL_Rect) {360, 450, 80, 80};
    // 字体
    DefaultFont = TTF_OpenFont("res/GenshinDefault.ttf", 50);
    CurMark.pos = (SDL_Rect) {20, 20, 100, 50};
    HistoryMark.pos = (SDL_Rect) {20, 100, 200, 50};
    for (int i = 0; i < 3; ++i) {
        char *msg = malloc(sizeof(char) * 10);
        sprintf(msg, "Level %d", i + 1);
        InitWordMsg(&(LevelMsg[i]), msg, (SDL_Rect) {500, 20, 100, 50}, (SDL_Color) {255, 255, 255, 255});
        free(msg);
    }
    InitWordMsg(&PauseMsg, "Game Paused", (SDL_Rect) {200, 300, 200, 60}, (SDL_Color) {255, 255, 255, 255});
    InitWordMsg(&FailMsg, "You Lose", (SDL_Rect) {200, 300, 200, 60}, (SDL_Color) {255, 255, 255, 255});
    // 平台贴图
    for (int i = 0; i < 10; ++i) {
        char filename[30] = {0};
        // 生成文件名
        sprintf(filename, "res/platforms/platform_%d.png", i);
        InitPic(&Platform[i].surface, &Platform[i].texture, filename);
    }
    // 玩家棋子贴图
    InitPic(&Chess.surface, &Chess.texture, "res/chess.png");
    Chess.pos.w = 30, Chess.pos.h = 100;
    // 蓄力条
    Bar.pos = (SDL_Rect) {DEFAULT_CHESS_POS_X - 40, DEFAULT_CHESS_POS_Y - 40, 0, 10};
    BarWrapper.pos = (SDL_Rect) {DEFAULT_CHESS_POS_X - 40, DEFAULT_CHESS_POS_Y - 40, 120, 10};
    return 0;
}

void DestroyPic(IMG pic) {
    SDL_FreeSurface(pic.surface);
    SDL_DestroyTexture(pic.texture);
}

void DestroyAll() {
    DestroyPic(StartChess);
    DestroyPic(StartWords);
    DestroyPic(Chess);
    DestroyPic(SrcPlatform);
    DestroyPic(DstPlatform);
    DestroyPic(StartButton);
    DestroyPic(AgainButton);
    DestroyPic(QuitButton);
    DestroyPic(ResumeButton);
    DestroyPic(FailMsg);
    DestroyPic(PauseMsg);
    for (int i = 0; i < 3; ++i) {
        DestroyPic(LevelMsg[i]);
    }
    for (int i = 0; i < 10; ++i) {
        DestroyPic(Platform[i]);
    }
    SDL_DestroyRenderer(Default_rdr);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

// 游戏相关函数
void RestartGame() {
    // 重开
    // 重置游戏状态
    game.mode = common;
    game.end_game = false;
    game.score = 0;
    // 重置玩家数值
    Chess.pos.x = DEFAULT_CHESS_POS_X;
    Chess.pos.y = DEFAULT_CHESS_POS_Y;
    player = (PLAYER) {
            .gravity = DEFAULT_GRAVITY,
            .MaxVx = DEFAULT_MAX_VX,
            .Vy = DEFAULT_VY,
            .Vx = 0
    };
    // 重置平台款式
    plat_index = 0;
    SrcPlatform = Platform[plat_index];
    DstPlatform = Platform[plat_index + 1];
    // 重置平台位置
    SrcPlatform.pos = DefaultSrcPlatformPos;
    DstPlatform.pos = ExtendedPlatformPos[0];
    // 初始化随机数种子
    srand((unsigned int) time(NULL));
}

int GetRandomIndex() {
    long long index;
    if (game.score <= 10) {
        index = rand() % 3;
    } else if (10 < game.score && game.score <= 30) {
        index = rand() % 6;
    } else {
        index = rand() % 10;
    }
    return (int) index;
}

void MoveMap() {
    Chess.pos.y = 400;
    if (DstPlatform.pos.x >= DefaultSrcPlatformPos.x) {
        SrcPlatform.pos.x -= 15;
        DstPlatform.pos.x -= 5;
        Chess.pos.x -= 5;
    } else {
        // 移动完毕, 加载下一个平台
        SrcPlatform = DstPlatform;
        // 游戏难度依目前得分划分档次
        // 生成随机数
        plat_index = GetRandomIndex();
        DstPlatform = Platform[plat_index];
        position_index = GetRandomIndex();
        DstPlatform.pos = ExtendedPlatformPos[position_index];
        game.mode = common;
    }
}

void CheckLanded() {
    bool landed = false;
    // 判断是否成功着陆, 即碰撞检测
    if (DstPlatform.pos.x <= Chess.pos.x + Chess.pos.w / 2 &&
        Chess.pos.x + Chess.pos.w / 2 <= DstPlatform.pos.x + DstPlatform.pos.w) {
        landed = true;
    } else {
        landed = false;
    }
    if (landed == true) {
        SDL_Log("Nice Jump!\n");
        // 加分
        // 平台形状加分
        if (plat_index >= 7) {
            // 圆形平台加两分
            game.score += 2;
        } else if (plat_index < 7) {
            // 方形平台加一分
            game.score++;
        }
        // 平台大小加分
        if (position_index >= 7) {
            // 小平台加两分
            game.score += 2;
        } else if (position_index < 7) {
            // 大平台加一分
            game.score++;
        }
        // 着陆在中间位置额外加分
        if (DstPlatform.pos.x + DstPlatform.pos.w / 3 <= Chess.pos.x + Chess.pos.w / 2 &&
            Chess.pos.x + Chess.pos.w / 2 <= DstPlatform.pos.x + DstPlatform.pos.w - DstPlatform.pos.w / 3) {
            game.score++;
        }
        game.history_high = (game.score >= game.history_high) ? game.score : game.history_high;
        // 修改棋子和平台坐标->视角移动(使用循环和延迟不断更新画面)
        // 恢复玩家数值(跳跃距离)
        player.Vx = 0;
        // 更改游戏模式
        SDL_Log("Moving Map\n");
        game.mode = move_map;
    } else {
        // 着陆失败, 显示结束菜单
        SDL_Log("You Lose\n");
        SDL_Log("DstPlatformPos: x = %d, y = %d\n", DstPlatform.pos.x, DstPlatform.pos.y);
        SDL_Log("Chess.pos: x = %d, y = %d\n", Chess.pos.x, Chess.pos.y);
        SDL_Delay(1000);
        game.mode = lose_menu;
    }
}

void Jump() {
    // 实现跳跃
    // 修改状态, 还原数值
    Bar.pos.w = 0;
    game.mode = jumping;
    // 跳跃动画(按照运动的数学建模修改棋子坐标数值)
    static double dt = 0;   // dt: 运动参数
    if (Chess.pos.y <= DEFAULT_CHESS_POS_Y) {
        Chess.pos.x = previous_chess_pos_x + (int) (player.Vx * dt);
        Chess.pos.y = DEFAULT_CHESS_POS_Y - (int) ((player.Vy * dt - player.gravity * dt * dt / 2) * 2);
        dt += 0.05;
    } else {
        // 重置参数并退出跳跃过程
        Chess.pos.y = DEFAULT_CHESS_POS_Y;
        player.Vx = 0;
        dt = 0;
        game.mode = landing;
        SDL_Delay(500);
    }
}

// TODO: 优化蓄力速率数值
void GainMomentum() {
    // 实现蓄力, 其中同时需要实现读条
    // 蓄力方式 -> 更改CurJumpDistance
    previous_chess_pos_x = Chess.pos.x;
    Bar.pos.x = Chess.pos.x - 40;
    Bar.pos.y = Chess.pos.y - 40;
    BarWrapper.pos.x = Bar.pos.x;
    if (player.Vx <= player.MaxVx) {
        player.Vx += DEFAULT_VX_INC_SPEED;
        Bar.pos.w += 2;
        if (Bar.pos.w >= 120) {
            Bar.pos.w = 120;
        }
        SDL_Log("Current Vx = %.1f", player.Vx);
        SDL_Delay(30);
    } else {
        player.Vx = player.MaxVx;
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
                    if (game.mode == common || game.mode == jumping
                        || game.mode == gaining_momentum || game.mode == landing
                        || game.mode == move_map) {
                        SDL_Log("Game Paused\n");
                        // 保存暂停前状态
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
                        game.mode = jumping;
                    }
                }
                break;
            case SDL_MOUSEBUTTONDOWN: // 鼠标点击事件, 用于处理点击菜单中的按钮
                SDL_Log("Mouse Click Position: x = %d, y = %d", MainEvent.button.x, MainEvent.button.y);
                int cur_x = MainEvent.button.x, cur_y = MainEvent.button.y;
                if (game.mode == start_menu &&
                    MyPointInRect(&StartButtonPos[0], MainEvent.button.x, MainEvent.button.y)) {
                    // 开始菜单点击
                    SDL_Log("Start Button Pressed\n");
                    game.mode = launching;
                }
                if (game.mode == lose_menu || game.mode == pause_menu) {
                    if (MyPointInRect(&QuitButton.pos, cur_x, cur_y)) {
                        // 退出
                        SDL_Log("Quit Game\n");
                        game.end_game = true;
                    } else if (MyPointInRect(&AgainButton.pos, cur_x, cur_y)) {
                        // 重开, 保留历史最高分, 清除现有得分,游戏时间以及难度系数
                        SDL_Log("Game Refreshed\n");
                        RestartGame();
                    }
                }
                if (game.mode == pause_menu) {
                    if (MyPointInRect(&ResumeButton.pos, cur_x, cur_y)) {
                        // 还原原状态即继续执行原任务
                        SDL_Log("Game Resumed\n");
                        game.mode = last_mode;
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (game.mode == launching) {
                    game.mode = confirm_launch;
                }
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

void ShowMark() {
    // 绘制计分板
    char *cur_mark = malloc(sizeof(char) * 20);
    char *history_mark = malloc(sizeof(char) * 20);
    sprintf(cur_mark, "Score: %d", game.score);
    sprintf(history_mark, "History High: %d", game.history_high);
    // 为防止内存泄漏, 记得及时清除
    InitWordMsg(&CurMark, cur_mark, CurMark.pos, (SDL_Color) {255, 255, 255, 255});
    InitWordMsg(&HistoryMark, history_mark, HistoryMark.pos, (SDL_Color) {235, 169, 104, 255});
    SDL_RenderCopy(Default_rdr, CurMark.texture, NULL, &CurMark.pos);
    SDL_RenderCopy(Default_rdr, HistoryMark.texture, NULL, &HistoryMark.pos);
    if (game.score <= 10) {
        SDL_RenderCopy(Default_rdr, LevelMsg[0].texture, NULL, &LevelMsg[0].pos);
    } else if (10 < game.score && game.score <= 30) {
        SDL_RenderCopy(Default_rdr, LevelMsg[1].texture, NULL, &LevelMsg[1].pos);
    } else {
        SDL_RenderCopy(Default_rdr, LevelMsg[2].texture, NULL, &LevelMsg[2].pos);
    }
    // 防止内存泄漏
    DestroyPic(CurMark);
    DestroyPic(HistoryMark);
    free(cur_mark), free(history_mark);
}

void DrawGame() {
    // 更新游戏画面
    // 先绘制背景, 顺便清除上一帧
    SDL_SetRenderDrawColor(Default_rdr, 144, 144, 152, 255);
    SDL_RenderClear(Default_rdr);
    // 根据游戏模式分类讨论
    switch (game.mode) {
        case start_menu:
            // 绘制开始菜单
            SDL_RenderCopy(Default_rdr, StartWords.texture, NULL, &StartWords.pos);
            SDL_RenderCopy(Default_rdr, StartChess.texture, NULL, &StartChess.pos);
            SDL_RenderCopy(Default_rdr, StartButton.texture, NULL, &StartButtonPos[0]);
            break;
        case launching:
            // 绘制按下按键后画面
            SDL_RenderCopy(Default_rdr, StartWords.texture, NULL, &StartWords.pos);
            SDL_RenderCopy(Default_rdr, StartChess.texture, NULL, &StartChess.pos);
            SDL_RenderCopy(Default_rdr, StartButton.texture, NULL, &StartButtonPos[1]);
            break;
        case confirm_launch:
            // 回显一段时间原画面
            SDL_RenderCopy(Default_rdr, StartWords.texture, NULL, &StartWords.pos);
            SDL_RenderCopy(Default_rdr, StartChess.texture, NULL, &StartChess.pos);
            SDL_RenderCopy(Default_rdr, StartButton.texture, NULL, &StartButtonPos[0]);
            break;
        case pause_menu:
            // 绘制暂停信息
            SDL_RenderCopy(Default_rdr, PauseMsg.texture, NULL, &PauseMsg.pos);
            // 绘制暂停菜单
            SDL_RenderCopy(Default_rdr, AgainButton.texture, NULL, &AgainButton.pos);
            SDL_RenderCopy(Default_rdr, QuitButton.texture, NULL, &QuitButton.pos);
            SDL_RenderCopy(Default_rdr, ResumeButton.texture, NULL, &ResumeButton.pos);
            // 绘制计分板
            ShowMark();
            break;
        case lose_menu:
            // 绘制暂停信息
            SDL_RenderCopy(Default_rdr, FailMsg.texture, NULL, &FailMsg.pos);
            // 绘制失败菜单
            SDL_RenderCopy(Default_rdr, AgainButton.texture, NULL, &AgainButton.pos);
            SDL_RenderCopy(Default_rdr, QuitButton.texture, NULL, &QuitButton.pos);
            // 绘制计分板
            ShowMark();
            break;
        case gaining_momentum:
            // 绘制平台
            SDL_RenderCopy(Default_rdr, SrcPlatform.texture, NULL, &SrcPlatform.pos);
            SDL_RenderCopy(Default_rdr, DstPlatform.texture, NULL, &DstPlatform.pos);
            // 绘制棋子
            SDL_RenderCopy(Default_rdr, Chess.texture, NULL, &Chess.pos);
            // 绘制蓄力条
            SDL_SetRenderDrawColor(Default_rdr, 235, 169, 104, 255);
            SDL_RenderFillRect(Default_rdr, &Bar.pos);
            SDL_RenderDrawRect(Default_rdr, &BarWrapper.pos);
            // 绘制计分板
            ShowMark();
            break;
        default:
            // 绘制平台
            SDL_RenderCopy(Default_rdr, SrcPlatform.texture, NULL, &SrcPlatform.pos);
            SDL_RenderCopy(Default_rdr, DstPlatform.texture, NULL, &DstPlatform.pos);
            // 绘制棋子
            SDL_RenderCopy(Default_rdr, Chess.texture, NULL, &Chess.pos);
            // 绘制计分板
            ShowMark();
    }
    // 统一显示, 防止RenderPresent复用造成闪屏
    SDL_RenderPresent(Default_rdr);
    if (game.mode == confirm_launch) {
        SDL_Delay(100);
        game.mode = common;
        SDL_Log("Game Start!\n");
    }
}

void CheckError(void *name) {
    if (!(name)) {
        SDL_Log("Failed because of: %s\n", SDL_GetError());
    }
}