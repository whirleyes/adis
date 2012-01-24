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
#include <stdint.h>

#include "predicates.h"
#include "dataproc.h"
#include "multi.h"
#include "dataswap.h"
#include "branch.h"
#include "sdt.h"

static int readop(/*out */ uint32_t * op) {

    char c;
    int i;

    for (i = 0; i < 4; i++) {
        c = getc(stdin);
        if (c == EOF) {
            return 0;
        }

        *op |= (c & 0xFF);

        if (i != 3) {
            *op <<= 8;
        }
    }

    return 1;
}

int main(int argc, char* argv[]) {

    uint32_t op;
    uint32_t count = 0;

    while (readop(&op)) {
        printf("op: 0x%.8X\n", op);
        printf("0x%.8X:\t", count);
        if (is_data_swap(op)) {
            data_swap_instr(op);
        }  else if (is_multi(op)) {
            multi_instr(op);
        } else if (is_dp_psr(op)) {
            dp_psr_instr(op);
        } else if (is_branch(op)) {
            branch_instr(op);
        } else if (is_single_data_transfer(op)) {
            sdt_instr(op);
        } else {
            printf("Unrecognized instruction 0x%x\n", op);
            return 1;
        }

        count += 4;
    }

    return 0;
}