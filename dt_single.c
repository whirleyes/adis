/*
 *  Copyright (C) 2011 Matthew Rheaume
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

#include "dt_single.h"
#include "common.h"

static inline uint8_t get_r_base(uint32_t op) {
    return (uint8_t)((op & 0x000F0000) >> 16);
}

static void get_srcdest_string(uint32_t op, char *buffer, size_t bsize) {

    uint32_t regnum = (op & 0x0000F000) >> 12;
    snprintf(buffer, ADIS_MIN(bsize, sizeof("Rxx")), "R%d", regnum);
}

void dt_single_instr(uint32_t op) {

    char cond[4], r_srcdest[4], addr[32];
    char *tsize; 

    get_condition_string(op, cond, sizeof(cond));
    get_srcdest_string(op, r_srcdest, sizeof(r_srcdest));
    get_addr_string(op, get_r_base(op), addr, sizeof(addr));

    // byte / word
    if (op & 0x00400000) {
        tsize = "B";
    } else {
        tsize = "";
    }

    // load / store
    if (op & 0x00100000) {
        printf("LDR%s%s %s,%s\n", cond, tsize, r_srcdest, addr);
    } else {
        printf("STR%s%s %s,%s\n", cond, tsize, r_srcdest, addr);
    }
}
