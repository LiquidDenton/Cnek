build: 
	gcc -Wall -Wextra -o Cnek src/main.c -lSDL2 -lSDL2_ttf  -lSDL2_image -lSDL2_mixer -lm
clean:
	rm Cnek
run:
	./Cnek
