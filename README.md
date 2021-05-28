# Hexa2腿部舵机

对应的硬件版本：VC720-S200H2-3XCAX

[点击查看舵机通信协议](./Doc/servo_protocal.xlsx)

[点击查看下载方法](./firmware_download.md)

舵机在安装前需要转到中位，可使用如下指令：

```
# run to middle
FF FF FE 02 08 F7
```

整条腿安装完毕后，需要进行校准，校准方法如下（校准时需要结构工程师帮忙调整各个关节的角度）：


## 1. 校准功能
进入校准功能后，电机处于stop状态（受较大外力可活动），首先指定中位，然后依次指定最大和最小位置。

### 校准指令：

注意：必须先设置中位(mid)，然后设置两边极限位置

* ID0

```
# set mid 60
FF FF 00 05 03 14 00 3C A7

# set min 0
FF FF 00 05 03 09 00 00 EE

# set max 120
FF FF 00 05 03 0B 00 78 74
```


* ID1

```
# set mid 75
FF FF 01 05 03 14 00 4B 97

# set min 0
FF FF 01 05 03 09 00 00 ED

# set max 165
FF FF 01 05 03 0B 00 A5 46
```

* ID 2

```
# set mid 90
FF FF 02 05 03 14 00 5A 87

# set min 0
FF FF 02 05 03 09 00 00 EC

# set max 175
FF FF 02 05 03 0B 00 AF 3B
```

### PING指令

```
# ping ID0~ID2
FF FF 00 02 01 FC
FF FF 01 02 01 FB
FF FF 02 02 01 FA

```

### ID改写指令

```
# ID0 -> ID1
FF FF 00 04 03 05 01 F2

# ID0 -> ID2
FF FF 00 04 03 05 02 F1 

# ID1 -> ID0
FF FF 01 04 03 05 00 F2

# ID2 -> ID0
FF FF 02 04 03 05 00 F1

```





