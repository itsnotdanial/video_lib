Makefile

CC = g++
AR = ar 
ARFLAGS = rcs
SOURCE = main.cpp funcs.cpp 
OBJECTS = main.o funcs.o
LIB = libmaster2000.a
EXE = runme
TESTEXE = test

all : $(EXE) $(LIB)


$(EXE) : $(OBJECTS)
    $(CC) $(OBJECTS) -o $(EXE)

$(LIB) : funcs.o
    $(AR) $(ARFLAGS) $(LIB) funcs.o

main.o : main.cpp funcs.h
    $(CC) -c main.cpp

funcs.o : funcs.cpp funcs.h
    $(CC) -c funcs.cpp

clean :
    rm -f $(OBJECTS) $(EXE) $(LIB)

test : $(EXE)
    mkdir -p test_output
    ./$(EXE) "./Example Input Data/data/test.bin" "./test_output/reverse" reverse
    ./$(EXE) "Example Input Data/data/test.bin" "./test_output/grayscale" grayscale
    ./$(EXE) "Example Input Data/data/test.bin" "./test_output/swap_channel" swap_channel 1,2 
    ./$(EXE) "Example Input Data/data/test.bin" "./test_output/clip_channel" clip_channel 1 [10,200]
    ./$(EXE) "Example Input Data/data/test.bin" "./test_output/scale_channel" scale_channel 1 1.3  

    ./$(EXE) "Example Input Data/data/test.bin" "./test_output/reverse_memory" reverse_memory
    ./$(EXE) "Example Input Data/data/test.bin" "./test_output/grayscale_memory" grayscale_memory
    ./$(EXE) "Example Input Data/data/test.bin" "./test_output/swap_channel_memory" swap_channel_memory 1,2 
    ./$(EXE) "Example Input Data/data/test.bin" "./test_output/clip_channe_memoryl" clip_channel_memory 1 [10,200]
    ./$(EXE) "Example Input Data/data/test.bin" "./test_output/scale_channel_memory" scale_channel_memory 1 1.3 

    ./$(EXE) "Example Input Data/data/test.bin" "./test_output/reverse_speed" reverse_speed
    ./$(EXE) "Example Input Data/data/test.bin" "./test_output/grayscale" grayscale_speed
    ./$(EXE) "Example Input Data/data/test.bin" "./test_output/swap_channel" swap_channel_speed 1,2 
    ./$(EXE) "Example Input Data/data/test.bin" "./test_output/clip_channel" clip_channel_speed 1 [10,200]
    ./$(EXE) "Example Input Data/data/test.bin" "./test_output/scale_channel" scale_channel_speed 1 1.3    

    make clean
