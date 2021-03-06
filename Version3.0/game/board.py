from util import *

WHITE_PIECE = 'O'
BLACK_PIECE = 'X'

class Board:

    def __init__(self, size, map):
        self.size = size
        self.black_stones = 0
        self.white_stones = 0
        self.board = [[EMPTY for _ in range(self.size)]
                      for _ in range(self.size)]

        self.init_starting_position(map)

    def init_starting_position(self, map):
        for i in xrange(8):
            for j in xrange(8):
                self.board[i][j] = map[i][j]
                if map[i][j] == BLACK:
                    self.black_stones += 1
                elif map[i][j] == WHITE:
                    self.white_stones += 1

    def place_stone_at(self, color, x, y):
        self.board[x][y] = color
        if color == WHITE:
            self.white_stones += 1
        elif color == BLACK:
            self.black_stones += 1

    def flip_stone(self, x, y):
        piece = self.piece_at(x, y)
        if piece == WHITE:
            self.white_stones -= 1
            self.place_stone_at(BLACK, x, y)
        elif piece == BLACK:
            self.black_stones -= 1
            self.place_stone_at(WHITE, x, y)
        else:
            raise TypeError

    def get_stone_counts(self):
        return self.black_stones, self.white_stones

    def is_full(self):
        return self.black_stones + self.white_stones == (self.size ** 2)

    def get_size(self):
        return self.size

    def get_board(self):
        """Return the raw board.  Try to avoid this if you can use the other getters."""
        return self.board

    # def is_in_bounds(self, int x, int y):
        # return 0 <= x < self.size and 0 <= y < self.size

    def piece_at(self, x, y):
        return self.board[x][y]

    def __str__(self):
        result = ''
        for y in range(self.size - 1, -1, -1):
            result += str(y) + '| '
            for x in range(self.size):
                if self.board[y][x] == WHITE:
                    result += WHITE_PIECE + ' '
                elif self.board[y][x] == BLACK:
                    result += BLACK_PIECE + ' '
                elif self.board[y][x] == EMPTY:
                    result += '- '
                else:
                    result += '? '
            result += '\n'

        result += '  '
        for x in range(self.size):
            result += '--'
        result += '\n'
        result += '   '
        for x in range(self.size):
            result += str(x) + ' '

        return result

    def __repr__(self):
        return self.__str__()

    def __hash__(self):
        # return hash(str(self.board))
        hash = 5138

        for y in range(self.size):
            for x in range(self.size):
                hash += self.board[y][x]
                hash += (hash << 10)
                hash ^= (hash >> 6)

        hash += (hash << 3)
        hash ^= (hash >> 11)
        hash += (hash << 15)
        return hash

    def __eq__(self, other):
        if self.black_stones != other.black_stones:
            return False
        if self.white_stones != other.white_stones:
            return False
        # if self.__hash__() != other.__hash__():
            # return False
        # return np.array_equal(self.board, other.board)
        return self.board == other.board
