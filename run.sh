rm test
g++ $1 -o test -lGL -lGLU -lglut -std=c++17 -O2 -lm
./test
