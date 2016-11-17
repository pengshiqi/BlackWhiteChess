from copy import deepcopy

from util import *
from game.board import Board
from agents import agent, monte_carlo_agent, random_agent


class Reversi:
    """This class enforces the rules of the game of Reversi"""

    def __init__(self, map):
        self.size = 8
        self.board = Board(self.size, map)
        self.game_state = (self.board, WHITE)
        # self.legal_cache = CacheDict()

        self.white_agent = monte_carlo_agent.MonteCarloAgent(self, WHITE)
        self.black_agent = random_agent.RandomAgent(self, BLACK)

    def pick_move(self):
        state = self.game_state
        legal_moves = self.legal_moves(state)
        picked = None
        # Get the best action
        picked = self.white_agent.get_action(state, legal_moves)

        if picked is None:
            return None
        elif picked not in legal_moves:
            print(str(picked) + ' is not a legal move! Game over.')
            quit()

        return picked

    def legal_moves(self, game_state, force_cache=False):
        # Note: this is a very naive and inefficient way to find
        # all available moves by brute force.  I am sure there is a
        # more clever way to do this.  If you want better performance
        # from agents, this would probably be the first area to improve.
        # if force_cache:
        #     return self.legal_cache.get(game_state)

        board = game_state[0]
        if board.is_full():
            return []

        # cached = self.legal_cache.get(game_state)
        # if cached is not None:
        #     return cached

        board_size = board.get_size()
        moves = []  # list of x,y positions valid for color

        for y in xrange(board_size):
            for x in xrange(board_size):
                if self.is_valid_move(game_state, x, y):
                    moves.append((x, y))

        # self.legal_cache.update(game_state, moves)
        return moves

    @staticmethod
    def is_valid_move(game_state, x, y):
        board, color = game_state
        piece = board.board[x][y]
        if piece != EMPTY:
            return False

        enemy = opponent[color]

        # now check in all directions, including diagonal
        for dx in range(-1, 2):
            for dy in range(-1, 2):
                if dx == 0 and dy == 0:
                    continue

                # there needs to be >= 1 opponent piece
                # in this given direction, followed by 1 of player's piece
                distance = 1
                xp = (distance * dx) + x
                yp = (distance * dy) + y

                while is_in_bounds(xp, yp, board.size) and board.board[xp][yp] == enemy:
                    distance += 1
                    xp = (distance * dx) + x
                    yp = (distance * dy) + y

                if distance > 1 and is_in_bounds(xp, yp, board.size) and board.board[xp][yp] == color:
                    return True
        return False

    def next_state(self, game_state, move):
        """Given a game_state and a position for a new piece, return a new game_state
        reflecting the change.  Does not modify the input game_state."""
        return self.apply_move(deepcopy(game_state), move)

    @staticmethod
    def apply_move(game_state, move):
        """Given a game_state (which includes info about whose turn it is) and an x,y
        position to place a piece, transform it into the game_state that follows this play."""

        # if move is None, then the player simply passed their turn
        if not move:
            game_state = (game_state[0], opponent[game_state[1]])
            return game_state

        x, y = move
        color = game_state[1]
        board = game_state[0]
        board.place_stone_at(color, x, y)

        # now flip all the stones in every direction
        enemy_color = BLACK
        if color == BLACK:
            enemy_color = WHITE

        # now check in all directions, including diagonal
        to_flip = []
        for dx in range(-1, 2):
            for dy in range(-1, 2):
                if dy == 0 and dx == 0:
                    continue

                # there needs to be >= 1 opponent piece
                # in this given direction, followed by 1 of player's piece
                distance = 1
                yp = (distance * dy) + y
                xp = (distance * dx) + x

                flip_candidates = []
                while is_in_bounds(xp, yp, board.size) and board.board[xp][yp] == enemy_color:
                    flip_candidates.append((xp, yp))
                    distance += 1
                    yp = (distance * dy) + y
                    xp = (distance * dx) + x

                if distance > 1 and is_in_bounds(xp, yp, board.size) and board.board[xp][yp] == color:
                    to_flip.extend(flip_candidates)

        for each in to_flip:
            board.flip_stone(each[0], each[1])
            # board.place_stone_at(color, each[0], each[1])

        game_state = (game_state[0], opponent[game_state[1]])
        return game_state

    def winner(self, game_state):
        """Given a game_state, return the color of the winner if there is one,
        otherwise return False to indicate the game isn't won yet.
        Note that legal_moves() is a slow operation, so this method
        tries to call it as few times as possible."""
        board = game_state[0]
        black_count, white_count = board.get_stone_counts()

        # a full board means no more moves can be made, game over.
        if board.is_full():
            if black_count > white_count:
                return BLACK
            else:
                # tie goes to white
                return WHITE

        # a non-full board can still be game-over if neither player can move.
        black_legal = self.legal_moves((game_state[0], BLACK))
        if black_legal:
            return False

        white_legal = self.legal_moves((game_state[0], WHITE))
        if white_legal:
            return False

        # neither black nor white has valid moves
        if black_count > white_count:
            return BLACK
        else:
            # tie goes to white
            return WHITE

    def get_board(self):
        """Return the board from the current game_state."""
        return self.game_state[0]

    def get_state(self):
        """Returns a tuple representing the board state."""
        return self.game_state
