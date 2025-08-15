# Kiran Installation

## Introduction

Kiran desktop environment, developed by Kylinsec, is a stable, efficient, and easy-to-use desktop environment oriented towards user and market requirements. Kiran supports x86 and AArch64 architectures.

## Procedure

You are advised to install Kiran as the **root** user or a newly created administrator.

1. Download the openEuler 22.09 ISO file and install the OS.

2. Update the software repository.

```shell
sudo dnf update
```

1. Install kiran-desktop.

```shell
sudo dnf -y install kiran-desktop
```

1. Set the system to start with the graphical interface, and then restart the system using the `reboot` command.

```shell
systemctl set-default graphical.target
```

After the reboot is complete, log in to the Kiran desktop.
