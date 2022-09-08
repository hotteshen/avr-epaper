#!/usr/bin/env python3

import sys
import serial
import crcmod
import threading
import socket
import os

ser = serial.Serial(sys.argv[1], 38400)
ser.timeout = 1
crc8 = crcmod.predefined.mkCrcFun('crc-8')


def sendpkt(data):
    buff=b''

    data += bytes([crc8(data)])
    for byte in data:
        if byte == 0x0a or byte == 0xdc:
            buff+=bytes([0xdc])
            byte ^= 0x80
        buff+=bytes([byte])
    buff+=b'\n'
    ser.write(buff)


def recv():
    buff=b""
    while True:
        c= ser.read()
        if c == b"":
            raise Exception("commerr")
        if c == b'\n':
            if len(buff) < 1:
                print("Too short packet",buff)
            elif crc8(buff) != 0:
                print("CRC ERROR")
            else:
                return buff[:-1]
            buff=b""
        if c == b"\xdc":
            c = bytes([(ser.read()[0] ^ 0x80)])
        buff+=c


sendpkt(b"\x00")
print(recv())


from PIL import Image
import struct

im = Image.open(sys.argv[2])
im = im.convert("1")
im = im.transpose(Image.Transpose.TRANSVERSE)

w,h = im.size

if w != 128 or h!=250:
    raise Exception("Bad image: image size must be 250 * 128.")

data_raw = im.tobytes()
for y in range(h):
    retries=0
    while True:
        try:
            sendpkt(struct.pack("<BH",0x01,y)+data_raw[y*16:(y+1)*16])
            recv()
            break
        except:
            retires +=1
            if retries == 3:
                raise Exception("Sending Err")


sendpkt(struct.pack("<B",0x02))
recv()

print("image sent")
