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

#include "dataproc.h"
#include "common.h"

typedef enum _PSRFlag {
    PSRFlagNone,
    PSRFlagBits
} PSRFlag;

static inline int is_mrs(uint32_t op) {
    return !((op & 0x0FBF0FFF) ^ 0x010F0000);
}

static inline int is_msr(uint32_t op) {
    return !((op & 0x0FBFFFF0) ^ 0x012CF000);
}

static inline int is_msr_flg(uint32_t op) {
    return !((op & 0x0DBFF000) ^ 0x0128F000);
}

static inline int is_single_op(uint32_t op) {

    int opcode = (op & 0x01E00000) >> 21;

    return (opcode == 0x1101) || (opcode == 0x1111);
}

static inline int is_no_result(uint32_t op) {
    
    int opcode = (op & 0x01E00000) >> 21;

    return (opcode >= 0x1000) && (opcode <= 0x1011);
}

static void get_op_string(uint32_t op, char *buffer, size_t bsize) {

    char *tmp;
    uint32_t opcode = (op & 0x01E00000) >> 21;

    switch(opcode) {
        case 0x00:
            // AND operation
            tmp = "AND";
            break;
        case 0x01:
            // XOR / EOR operation
            tmp = "TMP";
            break;
        case 0x02:
            // subtraction
            tmp = "SUB";
            break;
        case 0x03:
            // reverse subtraction
            tmp = "RSB";
            break;
        case 0x04:
            // addition
            tmp = "ADD";
            break;
        case 0x05:
            // add + carry
            tmp = "ADC";
            break;
        case 0x06:
            // subtract + carry
            tmp = "SBC";
            break;
        case 0x07:
            // reverse subtract + carry
            tmp = "RSC";
            break;
        case 0x08:
            // set condition codes op1 AND op2
            tmp = "TST";
            break;
        case 0x09:
            // set condition codes op1 EOR op2
            tmp = "TEQ";
            break;
        case 0x0A:
            // set condition codes op1 - op2
            tmp = "CMP";
            break;
        case 0x0B:
            // set condition codes op1 + op2
            tmp = "CMN";
            break;
        case 0x0C:
            // OR operation
            tmp = "ORR";
            break;
        case 0x0D:
            // MOV (Rd := op2)
            tmp = "MOV";
            break;
        case 0x0E:
            // BIC (Rd := op1 AND NOT op2)
            tmp = "BIC";
            break;
        case 0x0F:
            // MVN (Rd := NOT op2)
            tmp = "MVN";
            break;
    }

    memcpy(buffer, tmp, ADIS_MIN(bsize, sizeof(tmp)));
}

static inline uint8_t get_destination_register(uint32_t op) {
    return (uint8_t)((op & 0x0000F000) >> 12);
}

static inline uint8_t get_first_operand_register(uint32_t op) {
     return (uint8_t)((op & 0x000F0000) >> 16);
}

static void data_proc_instr(uint32_t op) {

    char opstr[4], offset[16], *cond, *setcond;
    
    get_op_string(op, opstr, sizeof(opstr));
    cond = get_condition_string(op);
    get_offset_string(op, offset, sizeof(offset), 1);

    if (is_no_result(op)) {
        uint8_t r_first = get_first_operand_register(op);
       
        printf("%s%s R%d,%s\n", opstr, cond, r_first, offset);
    } else {
        uint8_t r_dest;
        // check if condition code flag is set
        if (op & 0x00100000) {
            setcond = "S";
        } else {
            setcond = "";
        }

        r_dest = get_destination_register(op);

        if (is_single_op(op)) {
            printf("%s%s%s R%d,%s\n", opstr, cond, setcond, 
                r_dest, offset);
        } else {
            uint8_t r_first;
            r_first = get_first_operand_register(op);
            printf("%s%s%s R%d,R%d,%s\n", opstr, cond, setcond,
                r_dest, r_first, offset);
        }
    }
}

static void mrs_instr(uint32_t op) {
    
    char *cond, *psr;
    uint8_t r_dest;

    cond = get_condition_string(op);
    r_dest = get_destination_register(op);

    if (op & 0x00400000) {
        // source PSR = SPSR_<currentmode>
        psr = "SPSR";
    } else {
        psr = "CPSR";
    }

    printf("MRS%s R%d,%s\n", cond, r_dest, psr);
}

static void msr_instr(uint32_t op, PSRFlag flg) {
    
    char offset[16], *cond, *psr;

    cond = get_condition_string(op);
    get_offset_string(op, offset, sizeof(offset), 1);

    if (op & 0x00400000) {
        // destination psr = SPSR_<currentmode>
        if (flg == PSRFlagBits) {
            psr = "SPSR_flg";
        } else {
            psr = "SPSR";
        }
    } else {
        if (flg == PSRFlagBits) {
            psr = "CPSR_flg";
        } else {
            psr = "CPSR";
        }
    }

    printf("MSR%s %s,%s\n", cond, psr, offset);
}

void dp_psr_instr(uint32_t op) {

    if (is_msr(op)) {
        msr_instr(op, PSRFlagNone);
    } else if (is_msr_flg(op)) { 
        msr_instr(op, PSRFlagBits);
    } else if (is_mrs(op)) {
        mrs_instr(op);
    } else {
        data_proc_instr(op);
    }

}
