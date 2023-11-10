# 项目介绍

这代码是用于我们的第二台小车的，平台是Arduion，单片机是Arduino nano

## 项目工具
- C++ 17
- vscode + pio插件
- 特定arduino小车一台

## 文件结构
- Configs:

    存全局常量和针脚配置

- Debug:

    用于debug，在DEBUG为true的时候执行

- Functional_interface:

    存函数式的函数，还是处于实验性质，慎用

- MK2System:

    多任务系统，可以有抢占式任务和非抢占式任务

- OLED_utils:

    用于OLED屏幕的代码

- Tools:

    工具库，因为Arduino没有C++的标准库，所以需要自己造一个

- Vehicle_actions

    小车的一些搜索或者攻击的策略

- Vehicle_types

    小车的某些数据类型

- Vehicle_utils

    小车的操作都在这，像是移动，或者QTR的使用
