# Release 2.4 (2022-11-04)

- 控制中心前端
  - 插件接口调整,更灵活,搜索信息更完整
  - 控制中心调整为色块设计
  - 网络设置重构
- 会话管理
  - 会话管理拉起登陆器
  - 桌面会话提供两个选项：kiran和kiran-wayland
  - 图形相关的功能都放入单独进程中进行实现
  - 整个项目由GTK改为QT
- kiran-widgets-qt5和kiran-qt5-integration：根据新的UI设计调整QT主题。

# Release 2.3 (2022-07-11)

- 添加Qt5主题(kiran-qt5-integration)
- 控制中心前端：
  - 整合控制中心插件
  - 添加音频设置工具
  - 添加网络设置工具
- 托盘
  - 音频设置托盘
  - 网络设置托盘
# Release 2.2 (2021-11-22)

- 控制中心前端：
  - 添加个性化设置工具(kiran-cpanel-appearance)
  - 添加键盘设置工具(kiran-cpanel-keyboard)
  - 添加快捷键设置工具(kiran-cpanel-keybinding)
  - 添加电源设置工具(kiran-cpanel-power)
- 控制中心后端
  - 开启appearance/keyboard/keybinding/mouse/power/touchpad/xsettings插件
- 底部面板
  - 添加kiran-panel（mate-panel改名&适配） 
  - 修改面板插件，支持面板和插件的样式跟随主题变化
- 添加屏幕保护程序(kiran-screensaver)
- 登陆锁屏
  - 登陆锁屏代码整合到同一个项目(kiran-session-guard)
- 添加新GTK主题(kiran-gtk-theme)
- 文件管理器布局调整&可跟随主题变化(caja)
- 添加会话管理(kiran-session-manager)
- 小程序
  - 添加计算器(kiran-calculator)
