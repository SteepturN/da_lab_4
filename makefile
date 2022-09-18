test:  main.bin easy.bin
main.bin: main.cpp
	g++-11 -std=c++20 -g -Wall -pedantic -o main.bin main.cpp
easy.bin: easy_main.cpp
	g++-11 -std=c++20 -g -Wall -pedantic -o easy.bin easy_main.cpp
