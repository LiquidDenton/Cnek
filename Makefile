build: 
	gcc -Wall -Wextra -o Cnek src/main.c -lSDL2 -lSDL2_ttf  -lSDL2_image -lSDL2_mixer -lm
clean:
	rm Cnek
run:
	./Cnek
deps:
	sudo apt-get install SDL2-dev SDL2-ttf-dev SDL2-image-dev SDL2-mixer-dev
