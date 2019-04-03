
LD_FLAGS = -L../linux -lOGLES -lX11 -lXext
CFLAGS = -I../../include 

main:main.cpp
	gcc $^ -o $@ $(CFLAGS) $(LD_FLAGS)
run:
	@LD_LIBRARY_PATH=../linux;./main
clean:
	rm main

.PHONY: clean run
