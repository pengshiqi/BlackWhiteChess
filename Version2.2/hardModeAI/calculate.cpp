#include <iostream>
#include "calculate.h"

using namespace std;

const int max_num = 99999999;
const int max_depth = 5;

Board::Board()
{
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      map[i][j] = 0;
}

Board::Board(int *arr)
{
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      {
        map[i][j] = arr[i * 8 + j];
        piece_num[map[i][j]]++;
      }
}

Board::Board(const Board &obj)
{
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      map[i][j] = obj.map[i][j];
  piece_num[1] = obj.piece_num[1];
  piece_num[2] = obj.piece_num[2];
}

Board::~Board()
{

}

bool Board::is_legal_position(int x, int y, int color)
{
  bool flag = false;
  map[x][y] = color;

  for (int i = 0; i < 8; i++)
  {
    int temp = 0;
    int xx = x + direction[i][0];
    int yy = y + direction[i][1];
    while ((xx >= 0) && (xx < 8)  && (yy >= 0) && (yy < 8) && (map[xx][yy] == 3 - color))
    {
      xx += direction[i][0];
      yy += direction[i][1];
      temp += 1;
    }
    if ((xx >= 0) && (xx < 8)  && (yy >= 0) && (yy < 8) && (map[xx][yy] == color) && (temp > 0))
    {
      flag = true;
      break;
    }
  }
  map[x][y] = 0;
  return flag;
}

bool Board::check_white()
{
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
    {
      if (map[i][j] == 0 and is_legal_position(i, j, 1))
        return true;
    }
  return false;
}

bool Board::check_black()
{
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
    {
      if (map[i][j] == 0 and is_legal_position(i, j, 2))
        return true;
    }
  return false;
}

void Board::show_board()
{
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
      cout << map[i][j] << ' ';
    cout << endl;
  }
  cout << endl;
}

void Board::click(int x, int y, int color)
{
  map[x][y] = color;

  // 需要翻转的方向列表
  int directions[8] = {0};
  int dire_num = 0;

  for (int i = 0; i < 8; i++)
  {
    int xx = x + direction[i][0];
    int yy = y + direction[i][1];
    bool flag = false; // 该方向是否可以落子
    while ((xx >= 0) && (xx < 8)  && (yy >= 0) && (yy < 8) && (map[xx][yy] == 3 - color))
    {
      flag = true;
      xx += direction[i][0];
      yy += direction[i][1];
    }
    if ((xx >= 0) && (xx < 8)  && (yy >= 0) && (yy < 8) && (map[xx][yy] == color) && flag)
    {
      directions[dire_num] = i;
      dire_num += 1;
    }
  }
  for (int i = 0; i < dire_num; i++)
  {
    int xx = x + direction[directions[i]][0];
    int yy = y + direction[directions[i]][1];
    while (map[xx][yy] != color)
    {
      map[xx][yy] = 3 - map[xx][yy];
      piece_num[color]++;
      piece_num[3 - color]--;
      xx += direction[directions[i]][0];
      yy += direction[directions[i]][1];
    }
  }
}

bool Board::is_frontier(int x, int y)
{
  for (int i = 0; i < 8; i++)
  {
    int xx = x + direction[i][0];
    int yy = y + direction[i][1];
    if ((xx >= 0) && (xx < 8)  && (yy >= 0) && (yy < 8) && (map[xx][yy] == 0))
      return true;
  }
  return false;
}

int Board::evaluate()
{
  int weight[6] = {10, 801, 382, 79, 74, 10};
  int p, c, l, m, f, d;

  // piece difference: p
  if (piece_num[2] > piece_num[1])
    p = int(100 * double(piece_num[2]) / (piece_num[1] + piece_num[2]));
  else if (piece_num[2] < piece_num[1])
    p = int(100 * double(piece_num[1]) / (piece_num[1] + piece_num[2]));
  else
    p = 0;

  // corner occupancy: c
  int corner_b = 0, corner_w = 0;
  int corner[4][2] = {{0, 0}, {0, 7}, {7, 0}, {7, 7}};
  for (int i = 0; i < 4; i++)
    if (map[corner[i][0]][corner[i][1]] == 1)
      corner_w++;
    else if (map[corner[i][0]][corner[i][1]] == 2)
      corner_b++;
  c = 25 * (corner_b - corner_w);

  // corner closeness: l
  int adj_b = 0, adj_w = 0;
  int adj_area[12][2] = {{0, 1}, {1, 0}, {1, 1},
                        {0, 6}, {1, 7}, {1, 6},
                        {6, 0}, {6, 1}, {7, 1},
                        {6, 6}, {6, 7}, {7, 6}};
  for (int i = 0; i < 12; i++)
    if (map[adj_area[i][0]][adj_area[i][1]] == 1)
      adj_w ++;
    else if (map[adj_area[i][0]][adj_area[i][1]] == 2)
      adj_b ++;
  l = int(8.33 * (adj_w - adj_b));

  // Mobility: m
  int mob_b = 0, mob_w = 0;
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      if (map[i][j] == 0)
      {
        if (is_legal_position(i, j, 1))
          mob_w++;
        if (is_legal_position(i, j, 2))
          mob_b++;
      }
  if (mob_b > mob_w)
    m = int(100 * double(mob_b) / (mob_b + mob_w));
  else if (mob_b < mob_w)
    m = int(100 * double(mob_w) / (mob_b + mob_w));
  else
    m = 0;
  if (mob_b == 0 || mob_w == 0)
    m = 0;

  // frontier discs: f
  int frontier_b = 0, frontier_w = 0;
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
    {
      if (map[i][j] == 1 && is_frontier(i, j))
        frontier_w ++;
      else if (map[i][j] == 2 && is_frontier(i, j))
        frontier_b ++;
    }
  if (frontier_b > frontier_w)
    f = int(-100 * frontier_b / double(frontier_b + frontier_w));
  else if (frontier_b < frontier_w)
    f = int(100 * frontier_w / double(frontier_b + frontier_w));
  else f = 0;

  // disc squares: d
  int squares[8][8] = {{20, -3, 11, 8, 8, 11, -3, 20},
                       {-3, -7, -4, 1, 1, -4, -7, -3},
                       {11, -4, 2, 2, 2, 2, -4, 11},
                       {8, 1, 2, -3, -3, 2, 1, 8},
                       {8, 1, 2, -3, -3, 2, 1, 8},
                       {11, -4, 2, 2, 2, 2, -4, 11},
                       {-3, -7, -4, 1, 1, -4, -7, -3},
                       {20, -3, 11, 8, 8, 11, -3, 20}};
  d = 0;
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      if (map[i][j] == 1)
        d -= squares[i][j];
      else if (map[i][j] == 2)
        d += squares[i][j];

  return weight[0] * p + weight[1] * c + weight[2] * l + weight[3] * m + weight[4] * f + weight[5] * d;
}

int minimax(Board B, int depth, int alpha, int beta, bool MaxmizingPlayer)
{
  bool is_terminal;
  if (MaxmizingPlayer)
    is_terminal = !B.check_black();
  if (!MaxmizingPlayer)
    is_terminal = !B.check_white();

  if (depth == 0 || is_terminal)
    return B.evaluate();

  if (MaxmizingPlayer)
  {
    int bestValue = -max_num;
    for (int i = 0; i < 8; i++)
      for (int j = 0; j < 8; j++)
        if (B.map[i][j] == 0 && B.is_legal_position(i, j, 2))
        {
          Board new_B = B;
          new_B.click(i, j, 2);
          int v = minimax(new_B, depth - 1, alpha, beta, false);
          bestValue = max(bestValue, v);
          alpha = max(alpha, bestValue);
          if (beta <= alpha)
            break;
        }
    return bestValue;
  }
  else
  {
    int bestValue = max_num;
    for (int i = 0; i < 8; i++)
      for (int j = 0; j < 8; j++)
        if (B.map[i][j] == 0 && B.is_legal_position(i, j, 1))
        {
          Board new_B = B;
          new_B.click(i, j, 1);
          int v = minimax(new_B, depth - 1, alpha, beta, true);
          bestValue = min(bestValue, v);
          beta = min(beta, bestValue);
          if (beta <= alpha)
            break;
        }
    return bestValue;
  }
}

int find_the_best(int *arr)
{
  Board B(arr);

  // B.show_board();

  int best_x = -1, best_y = -1;
  int min_value = max_num;

  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
    {
      if (B.map[i][j] == 0 && B.is_legal_position(i, j, 1))
      {
          Board new_B = B;
          new_B.click(i, j, 1);
          int value = minimax(new_B, max_depth, -max_num, max_num, true);
          if (value < min_value)
          {
            min_value = value;
            best_x = i;
            best_y = j;
          }
      }
    }

  return best_x * 10 + best_y;
}
