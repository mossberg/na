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
