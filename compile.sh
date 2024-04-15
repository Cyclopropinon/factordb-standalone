chmod +x compile.sh
g++-13 -o factordb main.cpp -O3 -march=native -lcurl
strip factordb
