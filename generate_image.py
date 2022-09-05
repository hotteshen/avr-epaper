#!/usr/bin/env python3

import sys
from PIL import Image, ImageOps
from pathlib import Path

im = Image.open("image.png")
im = im.convert("L")

source = open("fw/image.c","w")
header = open("fw/image.h","w")

def gen_hdr(tgt):
    tgt.write('const PROGMEM unsigned char my_image_[] ')


def gen_image(tgt):
    data_raw = im.tobytes()
    w,h = im.size
    wn = (w+7)//8*8
    print(w,h)
    data = bytearray([255])*(wn//8)*h
    for x in range(w):
        for y in range(h):
            if(data_raw[y*(w//8)+x//8]&(1<<(7-x%8))):
                data[x//8*h+(y//8)] &= ~(1<<(x%8))
    
    line_buffer = []
    for p in data:
        line_buffer.append("0x%02x,"%p)
        if len(line_buffer) >= 2:
            tgt.write('    %s\n'%(''.join(line_buffer)))
            line_buffer = []
    if len(line_buffer):
        tgt.write('    %s\n'%(''.join(line_buffer)))


source.write("/*autogenerated file*/\n")
source.write('#include "image.h"\n')
gen_hdr(source)
source.write("=\n{\n")
gen_image(source)
source.write("};\n")


header.write("/*autogenerated file*/\n")
header.write("#ifndef IMAGE_H\n")
header.write("#define IMAGE_H\n")
header.write("#include <avr/pgmspace.h>\n")
header.write("extern ")
gen_hdr(header)
header.write(";\n")
header.write("#endif\n")

