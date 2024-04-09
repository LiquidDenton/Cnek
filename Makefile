build: 
	gcc -Wall -o main src/main.c -lSDL2 -lSDL2_ttf 
clean:
	rm main
run:
	./main
