# 在 openEuler 上安装 Kiran

## 简介

kiran 桌面是湖南麒麟信安团队以用户和市场需求为导向，研发的一个安全、稳定、高效、易用的桌面环境。Kiran 可以支持 x86 和 aarch64 架构。

## 安装方法

安装时建议使用 root 用户或者新建一个管理员用户。

1.下载 openEuler 23.09 镜像并安装系统。

2.更新软件源：

```sh
sudo dnf update
```

3.安装 kiran-desktop：

```sh
sudo dnf -y install kiran-desktop
```

4.设置以图形界面的方式启动，并重启（`reboot`）。

```sh
systemctl set-default graphical.target
```

重启系统即可通过 Kiran 桌面登录，您就可以尽情使用 Kiran 桌面了。
