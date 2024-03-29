# CPL项目文档

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

- 使用SDL2, SDL2_image, SDL2_ttf库
- 均使用包管理软件homebrew安装, 链接方式包含在CMakeLists中
- SDL2 -> /opt/homebrew/Cellar/sdl2
- SDL2_image -> /opt/homebrew/Cellar/sdl2_image
- SDL2_ttf -> /opt/homebrew/Cellar/sdl2_ttf

## 游戏操作说明

- 开始菜单:
    - 鼠标点击"开始游戏"按钮即可进入游戏
- 游戏过程中:
    - 按住空格键蓄力(期间有进度条指示蓄力进度), 松开空格键跳跃
    - 单次跳跃得分与跳跃难度成正相关, 跳跃到平台中央附近位置可获得额外加分
    - 难度与当前得分正相关, 0到10分第一档, 10到30分第二档, 30分以上第三档
    - 任意时刻按下Esc键进入暂停菜单, 鼠标点击可选择继续/重开/退出, 重开保留历史最高分
- 失败菜单:
    - 鼠标点击可选择重开/退出, 重开保留历史最高分

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
    - 将所有图片/文字等需要surface, texture和rect的事物封装成IMG类, 尝试面向对象思想, 代码可读性强且数据方便管理
    - 只设置一个事件循环, 通过调整游戏"模式"控制每个周期内执行的任务, 执行逻辑清晰, 修改方便
    - 函数封装合理, 模块分工清楚, 维护/扩展方便, main函数简洁扼要
    - 代码注释丰富, 显示了构建项目的完整历程和各部分功能
    - 运用动态分配, 避免内存泄漏, 尽可能减少内存开销
    - 错误处理机制完善, 并且各种操作触发时均在终端有生成SDL日志信息, 对调试很友好

## 不足之处

- 没有尝试多文件写法
- 开发时使用macOS平台, 对windows平台的编译/运行支持性未知(本质上还是没有深入学习编译工具)
