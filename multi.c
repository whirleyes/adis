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
#include <string.h>

#include "multi.h"
#include "common.h"

static void get_destination_string(uint32_t op, char* buffer, int bsize) {
    
    int regnum = (op & 0x000F0000) >> 16;
    snprintf(buffer, ADIS_MIN(bsize, sizeof("Rxx")), "R%d", regnum);
}

static void get_first_operand_string(uint32_t op, char* buffer, int bsize) {

    int regnum = (op & 0x00000F00) >> 8;
    snprintf(buffer, ADIS_MIN(bsize, sizeof("Rxx")), "R%d", regnum);
}

static void get_second_operand_string(uint32_t op, char* buffer, int bsize) {

    int regnum = (op & 0x0000000F);
    snprintf(buffer, ADIS_MIN(bsize, sizeof("Rxx")), "R%d", regnum);
}

static void get_third_operand_string(uint32_t op, char* buffer, int bsize) {
    
    int regnum = (op & 0x0000F000) >> 12;
    snprintf(buffer, ADIS_MIN(bsize, sizeof("Rxx")), "R%d", regnum);
}

void multi_instr(uint32_t op) {

    char cond[4], rDest[4], rFirst[4], rSecond[4];
    char *setcond;

    get_condition_string(op, cond, sizeof(cond));
    get_destination_string(op, rDest, sizeof(rDest));
    get_first_operand_string(op, rFirst, sizeof(rFirst));
    get_second_operand_string(op, rSecond, sizeof(rSecond));

    if (op & 0x00100000) {
        setcond = "S";
    } else {
        setcond = "";
    }

    if (op & 0x00200000) {
        // multiply and accumulate
        char rThird[4];

        get_third_operand_string(op, rThird, sizeof(rThird));
        printf("MLA%s%s %s,%s,%s,%s\n", cond, setcond, rDest, 
            rFirst, rSecond, rThird);
    } else {
        printf("MUL%s%s %s,%s,%s\n", cond, setcond, rDest,
            rFirst, rSecond);
    }
}