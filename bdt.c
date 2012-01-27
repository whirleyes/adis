/*
 *  Copyright (C) 2012 Matthew Rheaume
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdio.h>
#include <stdlib.h>

#include "bdt.h"
#include "common.h"

#define ADIS_INIT_ALLOC 16

static void get_base_string(uint32_t op, char *buffer, size_t bsize) {
    
    uint32_t regnum = (op & 0x000F0000) >> 16;
    snprintf(buffer, ADIS_MIN(bsize, sizeof("Rxx")), "R%d", regnum);
}

static void get_addr_mode_string(uint32_t op, char *buffer, size_t bsize) {
 
    uint32_t pre = op & 0x01000000, up = op & 0x00800000;
    if (pre && up) {
        snprintf(buffer, ADIS_MIN(bsize, sizeof("xx")), "IB");
    } else if (pre && !up) {
        snprintf(buffer, ADIS_MIN(bsize, sizeof("xx")), "DB");
    } else if (!pre && up) {
        snprintf(buffer, ADIS_MIN(bsize, sizeof("xx")), "IA");
    } else {
        snprintf(buffer, ADIS_MIN(bsize, sizeof("xx")), "DA");
    }
}

static void get_register_list_string(uint32_t op, char **buffer) {

    char *ret = malloc(sizeof(char)*ADIS_INIT_ALLOC);
    uint8_t pos = 0, max = ADIS_INIT_ALLOC, before = 0, i = 0;

    if (ret == NULL) {
        printf("BDT: Failed to allocate memory.\n");
        return;
    }

    // mask away top bits
    op &= 0x0000FFFF;
    ret[0] = '{';

    for ( ; i < 16; i++) {
        if ((pos + sizeof("Rxx,") + 1) >= max) {
            // reallocate
            max *= 2;
            if (realloc(ret, sizeof(char)*max) == NULL) {
                printf("BDT: Failed to allocate memory.\n");
                return;
            }
        }

        if ((op & 0x00000001) && !before) {
            // add to list
            pos++;
            snprintf(ret + pos, sizeof("Rxx,"), "R%d,", i);
            pos += i < 10 ? 2 : 3;
        } else if (!(op & 0x00000001) && before) {
            // add to list with dash preceding
            snprintf(ret + pos, sizeof("-Rxx,"), "-R%d,", i - 1);
            pos += i < 10 ? 3 : 4;
        }
    }

    ret[pos] = '}';
    *buffer = ret;
}

void bdt_instr(uint32_t op) {

    char cond[4], addr_mode[4], r_base[4];
    char *psr, *wb, *r_list = NULL;

    
    get_register_list_string(op, &r_list);
    
    if (r_list == NULL) {
        return;
    }

    get_condition_string(op, cond, sizeof(cond));
    get_base_string(op, r_base, sizeof(r_base));
    get_addr_mode_string(op, addr_mode, sizeof(addr_mode));

    if (op & 0x00400000) {
        psr = "^";
    } else {
        psr = "";
    }

    if (op & 0x00200000) {
        wb = "!";
    } else {
        wb = "";
    }

    if (op & 0x00100000) {
        printf("LDR%s%s %s%s,%s%s\n", cond, addr_mode, r_base, wb,
            r_list, psr);
    } else {
        printf("STM%s%s %s%s,%s%s\n", cond, addr_mode, r_base, wb,
            r_list, psr);
    }

    free(r_list);
}
