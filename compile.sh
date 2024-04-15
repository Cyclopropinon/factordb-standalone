chmod +x compile.sh
g++ -o factordb main.cpp -O3 -march=native -lcurl
strip factordb
