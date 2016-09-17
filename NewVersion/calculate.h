#ifndef CALCULATE_H
#define CALCULATE_H

int find_the_best(int *arr);

class Board
{
private:
  int direction[8][2] = {{1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}};

public:
  int map[8][8];
  int piece_num[3] = {0};

  Board();
  Board(int *arr);
  Board(const Board &obj);
  ~Board();

  bool is_legal_position(int x, int y, int color);
  bool check_black();
  bool check_white();
  void show_board();
  void click(int x, int y, int color);
  int evaluate();
};

int minimax(Board B, int depth, int alpha, int beta, bool MaxmizingPlayer);

#endif
