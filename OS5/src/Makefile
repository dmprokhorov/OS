CC = gcc
LIB = -shared -o
OBJ = -fPIC -c
main: libfunctions1.so libfunctions2.so
	${CC} task1.c -L. -lfunctions1 -o main1.1 -Wl,-rpath -Wl,.	
	${CC} task1.c -L. -lfunctions2 -o main1.2 -Wl,-rpath -Wl,. 
	${CC} task2.c -L. -ldl -o main2 -Wl,-rpath -Wl,.	

libfunctions1.so libfunctions2.so: functions1.o functions2.o
	${CC} ${LIB} libfunctions1.so functions1.o
	${CC} ${LIB} libfunctions2.so functions2.o

functions1.o functions2.o: functions1.c functions2.c
	${CC} ${OBJ} functions1.c -o functions1.o
	${CC} ${OBJ} functions2.c -o functions2.o

remove:
	rm -rf *.o *.so main1.[12] main2
