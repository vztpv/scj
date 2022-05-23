default:
    g++  -std=c++1z  -pthread -lpthread  main.cpp simdjson.cpp  -O2 -o scj
