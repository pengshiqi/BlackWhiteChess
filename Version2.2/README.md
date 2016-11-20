# BlackWhiteChess

A simple but funny game implemented by python and C++ in autumn 2016.

Using Minimax Algorithm and Alpha-Beta Pruning.

Having different modes(Human VS Human and Human VS AI) and different levels.

----

Run othello.py to play the game!

```
python othello.py
```

----

Commands to compile the .cpp files

```
swig -python calculate.i
g++ -fPIC -shared calculate_wrap.c -o _calculate.so -I/usr/include/python2.7/ -lpython2.7
```
