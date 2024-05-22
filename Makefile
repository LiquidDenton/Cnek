build: 
	gcc -Wall -Wextra -o Cnek src/main.c -lSDL2 -lSDL2_ttf  -lSDL2_image -lSDL2_mixer -lm
clean:
	rm Cnek
run:
	./Cnek
deps:
	sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-mixer-dev
