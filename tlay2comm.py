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
            if len(buff) < 2:
                print("Too short packet",buff)
            elif crc8(buff) != 0:
                print("CRC ERROR")
            else:
                return buff
            buff=b""
        if c == b"\xdc":
            c = bytes([(ser.read()[0] ^ 0x80)])
        buff+=c
    
sendpkt(b"")
print(recv())