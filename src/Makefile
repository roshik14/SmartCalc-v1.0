SRCS = calc.c parser.c stack.c
OBJS = ${SRCS:.c=.o}
CC = gcc
CFLAGS = -Wall -Werror -Wextra
LINUX_FLAGS= -lm -lsubunit -lrt 
OS:=$(shell uname -s)

all: smart_calc

calc.a: ${SRCS}
	@${CC} ${CFLAGS} -c ${SRCS} -lm 
	@ar rcs calc.a ${OBJS}
	@ranlib calc.a

tests: calc.a 
ifeq ($(OS), Darwin)
	@${CC} ${CFLAGS} -c calc-test.c 
	@${CC} calc-test.o calc.a -o test -lcheck 
else
	@${CC} ${CFLAGS} -c calc-test.c $(LINUX_FLAGS) 
	@${CC} calc-test.o calc.a -o test -lcheck $(LINUX_FLAGS) 
endif
	./test

gcov_report:
ifeq ($(OS), Darwin)
	@gcc --coverage calc-test.c -lcheck ${SRCS} -o unit-test 
else
	@gcc --coverage calc-test.c ${SRCS} -o unit-test -lcheck $(LINUX_FLAGS) 
endif
	@./unit-test
	@lcov -t "test" -o test.info -c -d .
	@genhtml -o report test.info
	
clean:
	@/bin/rm -rf *.o *.a *.log *.aux *.pdf *.tar test unit-test *.gcno *gcda report *.info leak_check SmartCalc

dist:
	tar -cf SmartCalc_v1.0.tar *.c *.h Makefile readme ui

smart_calc: calc.a app.c
ifeq ($(OS), Darwin)
	$(CC) `pkg-config --cflags gtk4` -DGDK_VERSION_MIN_REQUIRED=GDK_VERSION_4_2 app.c checks.c app_common.c calc.a -o SmartCalc `pkg-config --libs gtk4`
else
	$(CC) `pkg-config --cflags gtk4` -DGDK_VERSION_MIN_REQUIRED=GDK_VERSION_4_2 app.c checks.c app_common.c calc.a -o SmartCalc `pkg-config --libs gtk4` $(LINUX_FLAGS)
endif

run: smart_calc
	./SmartCalc

install: uninstall smart_calc
	@mkdir ~/SmartCalc_v1.0
	@install ./SmartCalc ~/SmartCalc_v1.0/	
	@cp -r ui ~/SmartCalc_v1.0/
	@make clean

uninstall:
	@rm -rf ~/SmartCalc_v1.0 
