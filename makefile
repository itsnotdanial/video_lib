

CC = g++
AR = ar 
ARFLAGS = rcs
SOURCE = main.cpp funcs.cpp 
OBJECTS = main.o funcs.o
LIB = libmaster2000.a
EXE = runme
TESTEXE = test
CFLAGS = -Ofast

all : $(EXE) $(LIB)

$(EXE) : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXE)

$(LIB) : funcs.o
	$(AR) $(ARFLAGS) $(LIB) funcs.o

main.o : main.cpp funcs.h
	$(CC) $(CFLAGS) -c main.cpp

funcs.o : funcs.cpp funcs.h
	$(CC) $(CFLAGS) -c funcs.cpp

clean :
	rm -f $(OBJECTS) $(EXE) $(LIB)

test : $(EXE)
	mkdir -p test_output
	./$(EXE) "./data/test.bin" "./test_output/reverse" reverse
	./$(EXE) "./data/test.bin" "./test_output/grayscale" grayscale
	./$(EXE) "./data/test.bin" "./test_output/swap_channel" swap_channel 1,2 
	./$(EXE) "./data/test.bin" "./test_output/clip_channel" clip_channel 1 [10,200]
	./$(EXE) "./data/test.bin" "./test_output/scale_channel" scale_channel 1 1.3  

	./$(EXE) "./data/test.bin" "./test_output/reverse_memory" [-M] reverse
	./$(EXE) "./data/test.bin" "./test_output/grayscale_memory" [-M] grayscale
	./$(EXE) "./data/test.bin" "./test_output/swap_channel_memory" [-M]swap_channel 1,2 
	./$(EXE) "./data/test.bin" "./test_output/clip_channel_memory" [-M ]clip_channel 1 [10,200]
	./$(EXE) "./data/test.bin" "./test_output/scale_channel_memory" [-M] scale_channel 1 1.3 

	./$(EXE) "./data/test.bin" "./test_output/reverse_speed" [-S] reverse
	./$(EXE) "./data/test.bin" "./test_output/grayscale_speed" [-S] grayscale
	./$(EXE) "./data/test.bin" "./test_output/swap_channel" [-S] swap_channel 1,2 
	./$(EXE) "./data/test.bin" "./test_output/clip_channel" [-S] clip_channel 1 [10,200]
	./$(EXE) "./data/test.bin" "./test_output/scale_channel" [-S] scale_channel 1 1.3    

	make clean
