# Hexa2�Ȳ����

��Ӧ��Ӳ���汾��VC720-S200H2-3XCAX

[����鿴���ͨ��Э��](./Doc/servo_protocal.xlsx)

[����鿴���ط���](./firmware_download.md)

����ڰ�װǰ��Ҫת����λ����ʹ������ָ�

```
# run to middle
FF FF FE 02 08 F7
```

�����Ȱ�װ��Ϻ���Ҫ����У׼��У׼�������£�У׼ʱ��Ҫ�ṹ����ʦ��æ���������ؽڵĽǶȣ���


## 1. У׼����
����У׼���ܺ󣬵������stop״̬���ܽϴ������ɻ��������ָ����λ��Ȼ������ָ��������Сλ�á�

### У׼ָ�

ע�⣺������������λ(mid)��Ȼ���������߼���λ��

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

### PINGָ��

```
# ping ID0~ID2
FF FF 00 02 01 FC
FF FF 01 02 01 FB
FF FF 02 02 01 FA

```

### ID��дָ��

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





