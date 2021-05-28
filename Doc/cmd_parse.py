import keyboard
import threading
import time
import ctypes

def get_sum(param):
    check_sum = 0
    for para in param:
        check_sum += para
    return ctypes.c_uint8(~check_sum).value

FIND_FIRST_FF = 0
FIND_SECOND_FF = 1
FIND_ID = 2
FIND_LEN = 3
FIND_CMD = 4
FIND_PARAM = 5
FIND_SUM = 6

step = FIND_FIRST_FF

param = []

cnt = 0

f = open("data.txt","r")
f1 = open("res.txt","w")

# while True:
#
#     num_str = f.readline().replace("0x","")
#     if(num_str!=""):
#         byte = int(num_str,16)
#         print(byte)

while True:

    num_str = f.readline().replace("0x", "")
    if (num_str == ""):
        f1.close()
        continue
    else:
        byte = int(num_str, 16)

    if(step==FIND_FIRST_FF):
        if(byte==0xFF):

            step = FIND_SECOND_FF;
            # print("get first FF")
        else:
            param.clear()
            step = FIND_FIRST_FF
    elif(step==FIND_SECOND_FF):
        if(byte==0xFF):
            step = FIND_ID
            # print("get second FF")
        else:
            param.clear()
            step = FIND_FIRST_FF
    elif(step==FIND_ID):
        step = FIND_LEN
        param.append(byte)
        # print("get ID")

    elif(step==FIND_LEN):
        step = FIND_CMD
        cnt = byte
        # print("get LEN")
        param.append(byte)
    elif(step==FIND_CMD):
        step = FIND_PARAM
        param.append(byte)
        # print("get CMD")
    elif(step==FIND_PARAM):
        if(cnt):
            param.append(byte)
            cnt = cnt-1
            if(cnt==2):
                step = FIND_SUM
                # print("get PARAM %d", cnt)
    elif(step==FIND_SUM):
        sum = get_sum(param)
        if(byte==sum):
            ress = "%d %d"%((param[16]<<8|param[17])/10,param[18]<<8|param[19])
            print(ress)
            f1.write(ress+"\n")
            # print("Right: %d"%len(param))
        else:
            print("ERROR")

        cnt = 2
        param.clear()
        step = FIND_FIRST_FF
