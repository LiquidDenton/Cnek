build: 
	gcc -Wall -o main src/main.c -lSDL2 -lSDL2_ttf  -lSDL2_image -lSDL2_mixer
clean:
	rm main
run:
	./main
