# robomaster-inschool

RoboMaster 2027 校内赛电控代码仓库。

## 仓库内容

- [`RM2027_School_Comp_Frame/`](RM2027_School_Comp_Frame/)：官方 STM32F407 工程框架
  - `Src/`、`Inc/`：CubeMX 生成的外设初始化代码
  - `userCode/devices/`：电机、舵机、遥控器驱动
  - `userCode/tasks/`：底盘、控制、用户任务
  - `MDK-ARM/RM_Frame_C.uvprojx`：Keil 工程文件
  - `documents/`：任务说明

## 开发环境

- Keil MDK 5.38a
- STM32F4xx 器件包
- RoboMaster 开发板 C 型 + ST-Link

打开 `RM2027_School_Comp_Frame/MDK-ARM/RM_Frame_C.uvprojx` 即可编译下载。

## 说明

Keil 安装包、器件手册、培训课件等资料体积较大，未纳入 Git 仓库；仓库只保留工程代码与说明文档。
