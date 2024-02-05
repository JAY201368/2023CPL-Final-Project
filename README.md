# CPL项目文档

## 项目作者

231250120 胡凌翔

## 项目选题

跳一跳

## 编译环境

### 操作系统

macOS Sonoma 14.3

### 编译器

- Apple clang version 15.0.0 (clang-1500.0.40.1)
- Target: arm64-apple-darwin23.3.0
- Thread model: posix
- InstalledDir: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin

## 项目亮点

- 函数封装合理, 借鉴了Linux的设计理念--一个函数只做好一件事
- 代码注释丰富, 显示了构建项目的完整历程
- 实现了点击"开始游戏"按钮的动态效果

## 项目结构

- 初始化SDL
- 初始化所有图片
- 初始化玩家数值
- 绘制开始菜单
- 设置第一个场景
- 进入事件循环
- 接收玩家操作(分支出支线任务)
    - 因为与跑酷游戏性质不同, 所有的画面更新都归Jump过程管, 所以应该为Jump函数定制Draw函数
    - 支线任务1 -> 退出事件
    - 支线任务2 -> 空格键 -> 蓄力 -> 切换到蓄力状态, 修改跳跃距离数值, 显示蓄力条动画
    - 支线任务3 -> Esc键 -> 进入暂停菜单
