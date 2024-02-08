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

### SDL库路径

- 均使用包管理软件homebrew安装, 链接方式包含在CMakeLists中
- SDL2 -> /opt/homebrew/Cellar/sdl2
- SDL2_image -> /opt/homebrew/Cellar/sdl2_image
- SDL2_ttf -> /opt/homebrew/Cellar/sdl2_ttf

## 项目亮点

- 游戏体验方面
    - 基本实现原"跳一跳"游戏的全部功能
    - 动画效果流畅丝滑, 跳跃手感较好
    - 实现了点击"开始游戏"按钮的动态效果, 手感Q弹
    - 平台样式丰富
    - 设置了难度梯度, 难度与当前得分正相关
    - 得分记录功能完善, 单次跳跃得分与难度正相关
    - 除开始/暂停/结束菜单之外, 在游戏的任何时刻均可按Esc暂停游戏, 且暂停后可以完美继续游戏(包括在跳跃过程中)
    - 随时可以重新开始一局游戏, 历史最高分记录会被保存
- 代码方面
    - 只设置一个事件循环, 通过调整游戏"模式"控制每个周期内执行的任务, 执行逻辑清晰, 修改方便
    - 函数封装合理, 模块分工清楚, 维护/扩展方便
    - 代码注释丰富, 显示了构建项目的完整历程
