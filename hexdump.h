/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Mark Mossberg <mark.mossberg@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once


void print_byte(char c) {
    if (c >= ' ' && c <= '~') {
        printf("%c", c);
    } else {
        printf(".");
    }
}

void print_ascii(size_t off, size_t len, const unsigned char *hex) {
    for (size_t i = 0; i < len; i++) {
        print_byte(hex[off+i]);
    }
}

void hexdump(const void *_hex, const size_t sz) {
    const unsigned char *hex = _hex;
    size_t i = 0;
    for (; i < sz; i++) {
        if (i % 2 == 0) {
            if (i % 16 == 0) {
                if (i != 0) {
                    printf("  ");
                    print_ascii(i - 16, 16, hex);
                    printf("\n");
                }
                printf("%08lx: ", i);
            } else {
                printf(" ");
            }
        }
        printf("%02x", hex[i]);
    }
    size_t tmp = i;
    // add padding
    while (tmp++ % 16) {
        if (tmp % 2 == 0) {
            printf(" ");
        }
        printf("  ");
    }
    printf("  ");
    if (i % 16) {
        print_ascii(i - (i%16), i%16, hex);
    }else {
        print_ascii(i - 16, 16, hex);
    }
    printf("\n");
}
