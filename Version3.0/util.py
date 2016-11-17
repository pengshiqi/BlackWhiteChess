import numpy as np
import math
import random
BLACK = 1
WHITE = -1
EMPTY = 0

sim_time = 10

color_name = {BLACK: 'Black', WHITE: 'White'}
opponent = {BLACK: WHITE, WHITE: BLACK}

def is_in_bounds(x, y, size):
    return 0 <= x < size and 0 <= y < size

def info(message):
    if not message:
        print()
    else:
        print(message)
