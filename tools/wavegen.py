#!/usr/bin/env python3

import math
import struct
from functools import reduce
from PIL import Image
import argparse
from pathlib import Path


parser = argparse.ArgumentParser("wavegen")
# parser.add_argument("-o", "--output", help="Output Filename", required=True)
parser.add_argument("-n", "--length", type=int, help="Length of Wave in bytes", default=256)
parser.add_argument("-o", "--osc", type=int, help="Number of oscillations", default=2)
parser.add_argument("-u", "--upper", type=int, help="Upper boundary of wave", default=15)
parser.add_argument("-l", "--lower", type=int, help="Lower boundary of wave", default=-1)

def int_to_bytes(value, length=1):
    return struct.pack('b' * length, *value)

def get_cos(len=256, angle=16, upper=15, lower=15):
  if(lower < 0): lower = upper
  data=[]
  for x in range(0,len):
    cos = int(((math.cos(angle * x) + 1) * upper) - lower)
    data.append(cos)
  return data

def bytes_to_carray(bytes):
  output = ""
  for x, byte in enumerate(bytes):
    # print("byte", byte)
    if(x % 16 == 0):
      output += "\n"
    output += str(byte) + ", "

  return output


def main():
  args = parser.parse_args()
  print("args", args)

  angle = args.osc * 2 * math.pi / 256
  print("math", "pi", math.pi, 2 * 2 * math.pi / 256)
  print("oscillations", args.osc, angle)



  intdata = get_cos(args.length, angle, args.upper, args.lower)
  # bytedata = int_to_bytes(intdata, len(intdata))
  # print("intdata", intdata)
  # print("bytedata", bytedata)
  print(bytes_to_carray(intdata))

  # intdata = get_cos(256, 31, 31)
  # bytedata = int_to_bytes(intdata, len(intdata))
  # print("intdata", intdata)
  # print("bytedata", bytedata)

if __name__ == "__main__":
  main()