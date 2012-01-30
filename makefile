# makefile for adis

CC = gcc
CFLAGS = -g -Wall
OBJECTS = common.o dataproc.o multi.o dataswap.o branch.o dt_block.o dt_single.o \
          dataop_coproc.o sw_interrupt.o predicates.o main.o

EXEC = adis

all : ${EXEC}

${EXEC} : ${OBJECTS}
	${CC} ${CFLAGS} ${OBJECTS} -o ${EXEC}

predicates.o : predicates.c

common.o : common.c

dataproc.o : dataproc.c common.c

multi.o : multi.c common.c

dataswap.o : dataswap.c common.c

branch.o : branch.c common.c

dt_block.o : dt_block.c common.c

dt_single.o : dt_single.c common.c

dataop_coproc.o: dataop_coproc.c common.c

sw_interrupt.o : sw_interrupt.c common.c

main.o : predicates.c dataproc.c multi.c dataswap.c branch.c dt_single.c \
         dataop_coproc.c sw_interrupt.c main.c

clean:
	rm ${EXEC} ${OBJECTS}
