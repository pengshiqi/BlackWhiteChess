from game.reversi import Reversi

def find_the_best(map):
    r = Reversi(map)
    return r.pick_move()
