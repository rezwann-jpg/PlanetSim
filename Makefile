all:
	gcc main.c -o bin/main -I"include/SFML" -L"lib" -Wall -Wextra -pedantic -std=c11 -lcsfml-graphics -lcsfml-window -lcsfml-system -lopengl32 -lgdi32 -lm bin/csfml-system-2.dll bin/csfml-window-2.dll bin/csfml-graphics-2.dll
run:
	./bin/main.exe