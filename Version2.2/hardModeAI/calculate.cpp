// 黑白棋（Reversi)
// minimax search & alpha-beta pruning 策略
// 作者：patrick_psq
// 游戏信息：http://www.botzone.org/games#Reversi

//#define UL
//#define DEBUG
//#define TDEBUG
//#define RDEBUG

#include "jsoncpp/json.h"
#include <iostream>
#include <cstdlib>
#include <bitset>
#include <string>
#include <ctime>

using namespace std;

int currBotColor; // 我所执子颜色（1为黑，-1为白，棋盘状态亦同）
int gridInfo[8][8] = { 0 }; // 先x后y，记录棋盘状态
int blackPieceCount = 2, whitePieceCount = 2;

#define TIME_OUT 0.992

#define TOCHARA 31415926

#define MINWINDOW 16

#define ALPHA - (1 << 30)
#define BETA (1 << 30)

#define FRINIT 66125924401152

int rTmp = 1027;
int passCount = 0;
int maxDepth = 5;
size_t PVS = 0;

bool tc = false;
bool ul = false;
bool iv = false;

int BWFACTOR;
int CNFACTOR;
int DCFACTOR;
int SDFACTOR;
int STFACTOR;
int MBFACTOR;
int FRFACTOR;
int CRFACTOR;
int turnID;
int R;

const short coordValue[10][10] =
        {
                {-8,-8,-8,-8,-8,-8,-8,-8,-8,-8},
                {-8,20,-3,11, 8, 8,11,-3,20,-8},
                {-8,-3,-7,-4, 1, 1,-4,-7,-3,-8},
                {-8,11,-4, 2, 2, 2, 2,-4,11,-8},
                {-8, 8, 1, 2,-3,-3, 2, 1, 8,-8},
                {-8, 8, 1, 2,-3,-3, 2, 1, 8,-8},
                {-8,11,-4, 2, 2, 2, 2,-4,11,-8},
                {-8,-3,-7,-4, 1, 1,-4,-7,-3,-8},
                {-8,20,-3,11, 8, 8,11,-3,20,-8},
                {-8,-8,-8,-8,-8,-8,-8,-8,-8,-8}
        };

const short charaTable[10][10] =
        {
                {0,     0,     0,     0,     0,     0,     0,     0,     0, 0},
                {0,     1,     7,    43,    79,    89,   113,   167,   199, 0},
                {0,   769,   709,   677,   541,   449,   313,   281,   233, 0},
                {0,   827,   881,   937,   997,  1009,  1117,  1231,  1373, 0},
                {0,  2311,  2099,  1999,  1811,  1777,  1667,  1511,  1409, 0},
                {0,  2557,  2777,  3121,  3331,  3533,  3631,  4441,  4799, 0},
                {0,  7411,  7177,  6827,  6689,  6311,  5987,  5399,  5077, 0},
                {0,  7717,  8111,  8447,  8663,  8887,  9277,  9511,  9929, 0},
                {0, 10099, 11171, 11467, 12907, 14669, 17041, 18517, 19997, 0},
                {0,     0,     0,     0,     0,     0,     0,     0,     0, 0}
        }; //Hash

const short dir[8][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};

enum Status { White, Black, Empty, Valid };    //Then status<Empty: Have Stone on It, status>=Empty: Truly Empty
//White=false & Black=true

const short sideVal[9] = {0, 1, 1, 1, 2, 3, 4, 6, 7};

void uglyList(int);

clock_t startTime = 0;
clock_t moveTime = 0;
clock_t evalTime = 0;
int moveTimes = 0;
int evalTimes = 0;

class Board
{
public:
    bool sideFlag;
    short statusCount[4];
    Status status[10][10];
    bool validDir[10][10][8];
    bool passFlag[2];
    short validCountFor[2];
    bitset<64> sideFrontline[2];
    bitset<64> Frontline;
    short validCoord[24][2];

    Board()
    {
        for (short i = 0; i < 10; i++)
            for (short j = 0; j < 10; j++)
                status[i][j] = Empty;

        statusCount[2] = 64;
        sideFlag = Black;
    }
    //overload operators
    void operator =(const Board &board)
    {
        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 10; j++)
            {
                status[i][j] = board.status[i][j];
                for (int k = 0; k < 8; k++)
                    validDir[i][j][k] = board.validDir[i][j][k];
            }

        passFlag[Black] = board.passFlag[Black];
        passFlag[White] = board.passFlag[White];

        Frontline = board.Frontline;
        sideFrontline[0] = board.sideFrontline[0];
        sideFrontline[1] = board.sideFrontline[1];

        int i = 0;
        while (board.validCoord[i][0])
        {
            validCoord[i][0] = board.validCoord[i][0];
            validCoord[i][1] = board.validCoord[i][1];
        }

        sideFlag = board.sideFlag;
    }

    //member functions
    void init()
    {
        srand(unsigned(clock()));

        status[4][4] = status[5][5] = White;
        status[4][5] = status[5][4] = Black;
        status[3][4] = status[4][3] = status[6][5] = status[5][6] = Valid;
        statusCount[Black] = 2;
        statusCount[White] = 2;
        statusCount[Empty] = 60;
        statusCount[Valid] = 4;
        sideFlag = Black;

        Frontline = FRINIT;
    }

    void flipSide() { sideFlag ^= 1; }

    void count()
    {
        statusCount[Black] = statusCount[White] = statusCount[Empty] = 0;
        for (int i = 1; i <= 8; i++)
            for (int j = 1; j <= 8; j++)
            {
                statusCount[Black] += (status[i][j] == Black);
                statusCount[White] += (status[i][j] == White);
                statusCount[Empty] += (status[i][j] >= Empty);
            }
    }

    void setFrontlineFor(bool side)
    {
        sideFrontline[side].reset();
        for (short i = 1; i <= 8; i++)
            for (short j = 1; j <= 8; j++)
                if (status[i][j] == Status(side))
                    for (int d = 0; d < 8; d++)
                    {
                        short x = i + dir[d][0];
                        short y = j + dir[d][1];
                        if (inRange(x, y) && (status[x][y] >= Empty))
                            sideFrontline[side][toOne(x, y)] = true;
                    }
    }

    inline void setAllFrontline()
    {
        Frontline = sideFrontline[0] | sideFrontline[1];
    }

    void setFrontline()
    {
        setFrontlineFor(sideFlag);
        setFrontlineFor(!sideFlag);
        setAllFrontline();
    }

    bool isValid(const short pos[2], bool side)const
    {
        if (!inRange(pos))
            return false;
        if (status[pos[0]][pos[1]] < Empty)
            return false;
        for (int i = 0; i < 8; i++)
        {
            short nx = pos[0] + dir[i][0], ny = pos[1] + dir[i][1];
            if (status[nx][ny] == !side)
            {
                for (int p = nx, q = ny; inRange(p, q); p += dir[i][0], q += dir[i][1])
                {
                    if (status[p][q] == Status(!side))
                        continue;
                    if (status[p][q] >= Empty)
                        break;
                    if (status[p][q] == Status(side))
                        return true;
                }
            }
        }
        return false;
    }

    bool isValid(int x, int y, bool side)const
    {
        if (!inRange(x, y))
            return false;
        if (status[x][y] < Empty)
            return false;
        for (int i = 0; i < 8; i++)
        {
            short nx = x + dir[i][0], ny = y + dir[i][1];
            if (status[nx][ny] == !side)
            {
                for (int p = nx, q = ny; inRange(p, q); p += dir[i][0], q += dir[i][1])
                {
                    if (status[p][q] == Status(!side))
                        continue;
                    if (status[p][q] >= Empty)
                        break;
                    if (status[p][q] == Status(side))
                        return true;
                }
            }
        }
        return false;
    }

    bool isValid_fast(const short pos[2], bool side)
    {
        bool R = false;
        short x = pos[0], y = pos[1];
        for (int i = 0; i < 8; i++)
        {
            validDir[x][y][i] = false;

            short nx = x + dir[i][0], ny = y + dir[i][1];
            if (status[nx][ny] == Status(!side))
            {
                for (int p = nx, q = ny; inRange(p, q); p += dir[i][0], q += dir[i][1])
                {
                    if (status[p][q] == Status(!side))
                        continue;
                    if (status[p][q] >= Empty)
                        break;
                    if (status[p][q] == Status(side))
                    {
                        validDir[x][y][i] = true;
                        R = true;
                        break;
                    }
                }
            }
        }
        return R;
    }

    bool isValid_fast(int x, int y, bool side)
    {
        bool R = false;
        for (int i = 0; i < 8; i++)
        {
            validDir[x][y][i] = false;

            short nx = x + dir[i][0], ny = y + dir[i][1];
            if (status[nx][ny] == Status(!side))
            {
                for (int p = nx, q = ny; inRange(p, q); p += dir[i][0], q += dir[i][1])
                {
                    if (status[p][q] == Status(!side))
                        continue;
                    if (status[p][q] >= Empty)
                        break;
                    if (status[p][q] == Status(side))
                    {
                        validDir[x][y][i] = true;
                        R = true;
                        break;
                    }
                }
            }
        }
        return R;
    }

    void setValid()
    {
        setFrontline();
        statusCount[Valid] = 0;
        validCountFor[Black] = validCountFor[White] = 0;
        for (unsigned i = 0; i < 64; i++)
            if (Frontline[i])
            {
                short x = getX(i);
                short y = getY(i);
                if (isValid(x, y, sideFlag))
                {
                    status[x][y] = Valid;
                    validCoord[statusCount[Valid]][0] = x;
                    validCoord[statusCount[Valid]][1] = y;
                    statusCount[Valid]++;
                    validCountFor[sideFlag]++;
                }
                else
                {
                    status[x][y] = Empty;
                    if (isValid(x, y, !sideFlag))
                        validCountFor[!sideFlag]++;
                }
            }
        coordSort();
    }

    void setValid_fast()
    {
        setFrontline();
        statusCount[Valid] = 0;
        validCountFor[Black] = validCountFor[White] = 0;
        for (unsigned i = 0; i < 64; i++)
            if (Frontline[i])
            {
                short x = getX(i);
                short y = getY(i);
                if (isValid_fast(x, y, sideFlag))
                {
                    status[x][y] = Valid;
                    validCoord[statusCount[Valid]][0] = x;
                    validCoord[statusCount[Valid]][1] = y;
                    statusCount[Valid]++;
                    validCountFor[sideFlag]++;
                }
                else
                {
                    status[x][y] = Empty;
                    if (isValid(x, y, !sideFlag))
                        validCountFor[!sideFlag]++;
                }
            }
        coordSort();
    }

    void move(const short pos[2])
    {
        for (int i = 0; i < 8; i++)
        {
            int dx = dir[i][0], dy = dir[i][1];

            if (status[pos[0] + dx][pos[1] + dy] == !sideFlag)
            {
                for (int p = pos[0] + dx, q = pos[1] + dy; inRange(p, q); p += dx, q += dy)
                {
                    if (status[p][q] >= Empty)
                        break;
                    if (status[p][q] == Status(sideFlag))
                    {
                        status[pos[0]][pos[1]] = Status(sideFlag);

                        for (int r = p - dx, s = q - dy; status[r][s] != Status(sideFlag); r -= dx, s -= dy)
                            status[r][s] = Status(sideFlag);
                        break;
                    }
                }
            }
        }
        flipSide();
    }

    void move(int x, int y)
    {
        if (!x && !y)
            passFlag[sideFlag] = true;

        else if (inRange(x, y))
        {
            for (int i = 0; i < 8; i++)
            {
                int dx = dir[i][0], dy = dir[i][1];

                if (status[x + dx][y + dy] == !sideFlag)
                {
                    for (int p = x + dx, q = y + dy; inRange(p, q); p += dx, q += dy)
                    {
                        if (status[p][q] >= Empty)
                            break;
                        if (status[p][q] == Status(sideFlag))
                        {
                            status[x][y] = Status(sideFlag);

                            for (int r = p - dx, s = q - dy; status[r][s] != Status(sideFlag); r -= dx, s -= dy)
                                status[r][s] = Status(sideFlag);
                            break;
                        }
                    }
                }
            }
            passFlag[sideFlag] = false;
        }
        flipSide();
    }

    void fastmove(const short pos[2])
    {
#ifdef TDEBUG
        clock_t fmT = clock();
#endif
        for (int i = 0; i < 8; i++)
        {
            short x = pos[0], y = pos[1];
            if (validDir[x][y][i])
                do
                {
                    status[x][y] = Status(sideFlag);
                    x += dir[i][0];
                    y += dir[i][1];
                } while (status[x][y] == Status(!sideFlag));
        }

        flipSide();
        setValid_fast();
        count();
#ifdef TDEBUG
        moveTime += clock() - fmT;
        moveTimes++;
#endif
    }

    void fastmove(int x, int y)
    {
#ifdef TDEBUG
        clock_t fmT = clock();
#endif
        for (int i = 0; i < 8; i++)
            if (validDir[x][y][i])
            {
                short tx = x, ty = y;
                do
                {
                    status[tx][ty] = Status(sideFlag);
                    tx += dir[i][0];
                    ty += dir[i][1];
                } while (status[tx][ty] == Status(!sideFlag));
            }

        flipSide();
        setValid_fast();
        count();
#ifdef TDEBUG
        moveTime += clock() - fmT;
        moveTimes++;
#endif
    }

    int allEvalFor(bool side)const //Evaluation for all cells of the side
    {
        int aval = 0;
        for (int i = 1; i <= 8; i++)
            for (int j = 1; j <= 8; j++)
                if (status[i][j] == Status(side))
                    aval += coordValue[i][j];

        return aval;
    }

    int charaCalc()const
    {
        int chara = 0;
        for (int i = 1; i <= 8; i++)
            for (int j = 1; j <= 8; j++)
                chara += status[i][j] * charaTable[i][j];
        return chara;
    }

    void coordSort()
    {
        int sgn;
        for (int i = 1; i < statusCount[Valid]; i++)
        {
            short tmp[2];
            tmp[0] = validCoord[i][0];
            tmp[1] = validCoord[i][1];
            sgn = i - 1;
            while (sgn >= 0 && coordValue[tmp[0]][tmp[1]] > coordValue[validCoord[sgn][0]][validCoord[sgn][1]])
            {
                validCoord[sgn + 1][0] = validCoord[sgn][0];
                validCoord[sgn + 1][1] = validCoord[sgn][1];
                sgn--;
            }
            validCoord[sgn + 1][0] = tmp[0];
            validCoord[sgn + 1][1] = tmp[1];
        }
    }

    bool isStable(int x, int y)const
    {
        for (short i = 0; i < 8; i++)
            for (short nx = x + dir[i][0], ny = y + dir[i][1]; inRange(nx, ny); nx += dir[i][0], ny += dir[i][1])
                if (Frontline[toOne(nx, ny)])
                    return false;
        return true;
    }

    const int eval()const;

    void show()
    {
        cout << "SideFlag: " << sideFlag << endl;
        cout << "StatusCount: " << statusCount[0] << ' ' << statusCount[1] << ' ' << statusCount[2] << ' ' << statusCount[3] << endl;
        cout << "Status: " << endl;
        for (int i = 1; i <= 8; i++)
        {
            for (int j = 1; j <= 8; j++)
                cout << status[j][i] << ' ';
            cout << endl;
        }
    }

    void update(int *arr, int currBotColor)
    {
        init();
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++)
                if (arr[i * 8 + j] == 1)
                    status[i + 1][j + 1] = Black;
                else if (arr[i * 8 + j] == 0)
                    status[i + 1][j + 1] = White;
                else
                    status[i + 1][j + 1] = Empty;

        if (currBotColor == 1)
            sideFlag = true;
        else
            sideFlag = false;

        setValid_fast();
        count();
    }

    inline bool end()const
    {
        return (!statusCount[Empty] || !statusCount[Black] || !statusCount[White] || (passFlag[Black] && passFlag[White]));
    }

    inline int isWin(bool side)const
    {
        return (end() && (statusCount[side] > statusCount[!side])) ? statusCount[side] - statusCount[!side] : 0;
    }

    inline bool inRange(short p, short q)const { return p >= 1 && p <= 8 && q >= 1 && q <= 8; }
    inline bool inRange(const short C[2])const { return C[0] >= 1 && C[0] <= 8 && C[1] >= 1 && C[1] <= 8; }
    inline short getX(int a) { return a / 8 + 1; }
    inline short getY(int a) { return a % 8 + 1; }
    inline short toOne(short x, short y)const { return 8 * (x - 1) + y - 1; }
    inline short toOne(const short C[2])const { return 8 * (C[0] - 1) + C[1] - 1; }
};

int MiniMax(const Board&, short, int, int, short[2]);

Board gameBoard;
bool playerSide;
short inputCoord[2];

inline bool timeOut()
{
    return tc || (double(clock() - startTime) / CLOCKS_PER_SEC > TIME_OUT);
}

inline void swap(int *const a, int *const b)
{
    short tmp = a[0];
    a[0] = b[0];
    b[0] = tmp;
    tmp = a[1];
    a[1] = b[1];
    b[1] = tmp;
}

// 向Direction方向改动坐标，并返回是否越界
inline bool MoveStep(int &x, int &y, int Direction)
{
    if (Direction == 0 || Direction == 6 || Direction == 7)
        x++;
    if (Direction == 0 || Direction == 1 || Direction == 2)
        y++;
    if (Direction == 2 || Direction == 3 || Direction == 4)
        x--;
    if (Direction == 4 || Direction == 5 || Direction == 6)
        y--;
    if (x < 0 || x > 7 || y < 0 || y > 7)
        return false;
    return true;
}

// 在坐标处落子，检查是否合法或模拟落子
bool ProcStep(int xPos, int yPos, int color, bool checkOnly = false)
{
    int effectivePoints[8][2];
    int dir, x, y, currCount;
    bool isValidMove = false;
    if (gridInfo[xPos][yPos] != 0)
        return false;
    for (dir = 0; dir < 8; dir++)
    {
        x = xPos;
        y = yPos;
        currCount = 0;
        while (1)
        {
            if (!MoveStep(x, y, dir))
            {
                currCount = 0;
                break;
            }
            if (gridInfo[x][y] == -color)
            {
                currCount++;
                effectivePoints[currCount][0] = x;
                effectivePoints[currCount][1] = y;
            }
            else if (gridInfo[x][y] == 0)
            {
                currCount = 0;
                break;
            }
            else
            {
                break;
            }
        }
        if (currCount != 0)
        {
            isValidMove = true;
            if (checkOnly)
                return true;
            if (color == 1)
            {
                blackPieceCount += currCount;
                whitePieceCount -= currCount;
            }
            else
            {
                whitePieceCount += currCount;
                blackPieceCount -= currCount;
            }
            while (currCount > 0)
            {
                x = effectivePoints[currCount][0];
                y = effectivePoints[currCount][1];
                gridInfo[x][y] *= -1;
                currCount--;
            }
        }
    }
    if (isValidMove)
    {
        gridInfo[xPos][yPos] = color;
        if (color == 1)
            blackPieceCount++;
        else
            whitePieceCount++;
        return true;
    }
    else
        return false;
}

// 检查color方有无合法棋步
bool CheckIfHasValidMove(int color)
{
    int x, y;
    for (y = 0; y < 8; y++)
        for (x = 0; x < 8; x++)
            if (ProcStep(x, y, color, true))
                return true;
    return false;
}

const int Board::eval()const
{
#ifdef TDEBUG
    clock_t bTime = clock();
#endif
    //Evaluation Based on Character Value
    int CharaEval = 0;

    if (CRFACTOR)
    {
        int myChara = allEvalFor(sideFlag);
        int opChara = allEvalFor(!sideFlag);

        CharaEval = myChara - opChara;
    }


    //Evaluation Based on Stone Rate
    int BWRateEval = 0;

    if (BWFACTOR)
    {
        short myStoneCount = statusCount[sideFlag];
        short opStoneCount = statusCount[!sideFlag];

        if (myStoneCount > opStoneCount)
            BWRateEval = 200 * myStoneCount / (myStoneCount + opStoneCount);
        else if (myStoneCount < opStoneCount)
            BWRateEval = -200 * opStoneCount / (myStoneCount + opStoneCount);
        else BWRateEval = 0;
    }


    //Evaluation Based on Stone Frontline Rate
    int FrontlineRateEval = 0;

    if (FRFACTOR)
    {
        int myFrontlineCount = int(sideFrontline[sideFlag].count());
        int opFrontlineCount = int(sideFrontline[!sideFlag].count());

        if (myFrontlineCount > opFrontlineCount)
            FrontlineRateEval = -200 * myFrontlineCount / (myFrontlineCount + opFrontlineCount);
        else if (myFrontlineCount < opFrontlineCount)
            FrontlineRateEval = 200 * opFrontlineCount / (myFrontlineCount + opFrontlineCount);
        else FrontlineRateEval = 0;
    }


    //Evaluation Based on Corner Stone Count
    int CornerEval = 0;
    short myCornerCount = 0;
    short opCornerCount = 0;
    if (CNFACTOR)
    {
        myCornerCount =
                (status[1][1] == Status(sideFlag)) +
                (status[1][8] == Status(sideFlag)) +
                (status[8][1] == Status(sideFlag)) +
                (status[8][8] == Status(sideFlag));

        opCornerCount =
                (status[1][1] == Status(!sideFlag)) +
                (status[1][8] == Status(!sideFlag)) +
                (status[8][1] == Status(!sideFlag)) +
                (status[8][8] == Status(!sideFlag));

        CornerEval = 25 * (myCornerCount - opCornerCount);
    }


    //Evaluation Based on Dangerous Corner-Beside-Stone Count
    int DCornerEval = 0;
    if (DCFACTOR)
    {
        short myDCornerCount =
                (status[1][1] >= Empty) && (status[1][2] == Status(sideFlag)) +
                                           2 * (status[1][1] >= Empty) && (status[2][2] == Status(sideFlag)) +
                                                                          (status[1][1] >= Empty) && (status[2][1] == Status(sideFlag)) +
                                                                                                     (status[1][8] >= Empty) && (status[1][7] == Status(sideFlag)) +
                                                                                                                                2 * (status[1][8] >= Empty) && (status[2][7] == Status(sideFlag)) +
                                                                                                                                                               (status[1][8] >= Empty) && (status[2][8] == Status(sideFlag)) +
                                                                                                                                                                                          (status[8][1] >= Empty) && (status[8][2] == Status(sideFlag)) +
                                                                                                                                                                                                                     (status[8][1] >= Empty) && (status[7][1] == Status(sideFlag)) +
                                                                                                                                                                                                                                                2 * (status[8][1] >= Empty) && (status[7][2] == Status(sideFlag)) +
                                                                                                                                                                                                                                                                               (status[8][8] >= Empty) && (status[8][7] == Status(sideFlag)) +
                                                                                                                                                                                                                                                                                                          2 * (status[8][8] >= Empty) && (status[7][7] == Status(sideFlag)) +
                                                                                                                                                                                                                                                                                                                                         (status[8][8] >= Empty) && (status[7][8] == Status(sideFlag));

        short opDCornerCount =
                (status[1][1] >= Empty) && (status[1][2] == Status(!sideFlag)) +
                                           2 * (status[1][1] >= Empty) && (status[2][2] == Status(!sideFlag)) +
                                                                          (status[1][1] >= Empty) && (status[2][1] == Status(!sideFlag)) +
                                                                                                     (status[1][8] >= Empty) && (status[1][7] == Status(!sideFlag)) +
                                                                                                                                2 * (status[1][8] >= Empty) && (status[2][7] == Status(!sideFlag)) +
                                                                                                                                                               (status[1][8] >= Empty) && (status[2][8] == Status(!sideFlag)) +
                                                                                                                                                                                          (status[8][1] >= Empty) && (status[8][2] == Status(!sideFlag)) +
                                                                                                                                                                                                                     (status[8][1] >= Empty) && (status[7][1] == Status(!sideFlag)) +
                                                                                                                                                                                                                                                2 * (status[8][1] >= Empty) && (status[7][2] == Status(!sideFlag)) +
                                                                                                                                                                                                                                                                               (status[8][8] >= Empty) && (status[8][7] == Status(!sideFlag)) +
                                                                                                                                                                                                                                                                                                          2 * (status[8][8] >= Empty) && (status[7][7] == Status(!sideFlag)) +
                                                                                                                                                                                                                                                                                                                                         (status[8][8] >= Empty) && (status[7][8] == Status(!sideFlag));


        DCornerEval = -25 * (myDCornerCount - opDCornerCount);
    }

    int SideEval = 0;
    if (SDFACTOR)
    {
        short mySideVal = 0, opSideVal = 0;

        if (!myCornerCount && !opCornerCount)
            SideEval = 0;
        else
        {
            if (status[1][1] == Status(sideFlag))
            {
                for (int i = 1; i <= 8; i++)
                {
                    if (status[1][i] == Status(sideFlag))
                        mySideVal += sideVal[i];
                    else
                        break;
                }
                for (int i = 1; i <= 8; i++)
                {
                    if (status[i][1] == Status(sideFlag))
                        mySideVal += sideVal[i];
                    else
                        break;
                }
            }
            else if (status[1][1] == Status(!sideFlag))
            {
                for (int i = 1; i <= 8; i++)
                {
                    if (status[1][i] == Status(!sideFlag))
                        opSideVal += sideVal[i];
                    else
                        break;
                }
                for (int i = 1; i <= 8; i++)
                {
                    if (status[i][1] == Status(!sideFlag))
                        opSideVal += sideVal[i];
                    else
                        break;
                }
            }

            if (status[1][8] == Status(sideFlag))
            {
                for (int i = 1; i <= 8; i++)
                {
                    if (status[i][8] == Status(sideFlag))
                        mySideVal += sideVal[i];
                    else
                        break;
                }
                for (int i = 8; i >= 1; i--)
                {
                    if (status[1][i] == Status(sideFlag))
                        mySideVal += sideVal[9 - i];
                    else
                        break;
                }
            }
            else if (status[1][8] == Status(!sideFlag))
            {
                for (int i = 1; i <= 8; i++)
                {
                    if (status[i][8] == Status(!sideFlag))
                        opSideVal += sideVal[i];
                    else
                        break;
                }
                for (int i = 8; i >= 1; i--)
                {
                    if (status[1][i] == Status(!sideFlag))
                        opSideVal += sideVal[9 - i];
                    else
                        break;
                }
            }

            if (status[8][8] == Status(sideFlag))
            {
                for (int i = 8; i >= 1; i--)
                {
                    if (status[8][i] == Status(sideFlag))
                        mySideVal += sideVal[9 - i];
                    else
                        break;
                }
                for (int i = 8; i >= 1; i--)
                {
                    if (status[i][8] == Status(sideFlag))
                        mySideVal += sideVal[9 - i];
                    else
                        break;
                }
            }
            else if (status[8][8] == Status(!sideFlag))
            {
                for (int i = 8; i >= 1; i--)
                {
                    if (status[8][i] == Status(!sideFlag))
                        opSideVal += sideVal[9 - i];
                    else
                        break;
                }
                for (int i = 8; i >= 1; i--)
                {
                    if (status[i][8] == Status(!sideFlag))
                        opSideVal += sideVal[9 - i];
                    else
                        break;
                }
            }

            if (status[8][1] == Status(sideFlag))
            {
                for (int i = 8; i >= 1; i--)
                {
                    if (status[i][1] == Status(sideFlag))
                        mySideVal += sideVal[9 - i];
                    else
                        break;
                }
                for (int i = 1; i <= 8; i++)
                {
                    if (status[8][i] == Status(sideFlag))
                        mySideVal += sideVal[i];
                    else
                        break;
                }
            }
            else if (status[8][1] == Status(!sideFlag))
            {
                for (int i = 8; i >= 1; i--)
                {
                    if (status[i][1] == Status(!sideFlag))
                        opSideVal += sideVal[9 - i];
                    else
                        break;
                }
                for (int i = 1; i <= 8; i++)
                {
                    if (status[8][i] == Status(!sideFlag))
                        opSideVal += sideVal[i];
                    else
                        break;
                }
            }

            SideEval = 5 * (mySideVal - opSideVal);
        }
    }


    //Evaluation Based on Stability
    int StableEval = 0;

    if (STFACTOR)
    {
        short myStableCount = 0;
        short opStableCount = 0;

        for (int i = 2; i <= 7; i++)
            for (int j = 2; j <= 7; j++)
                if (isStable(i, j))
                {
                    if (status[i][j] == Status(sideFlag))
                        myStableCount++;
                    else
                        opStableCount++;
                }
        StableEval = 25 * (myStableCount - opStableCount);
    }


    //Evaluation Based on Mobility
    int MobEval = 0;

    if (MBFACTOR)
    {
        short myValidCount = validCountFor[sideFlag];
        short opValidCount = validCountFor[!sideFlag];

        if (!opValidCount)
            MobEval = 200;
        else if (!myValidCount)
            MobEval = -800;
        else if (myValidCount > opValidCount)
            MobEval = 200 * myValidCount / (myValidCount + opValidCount);
        else if (myValidCount < opValidCount)
            MobEval = -200 * opValidCount / (myValidCount + opValidCount);
        else MobEval = 0;
    }



    //Weighed Evaluation
    int Eval =
            (BWFACTOR * BWRateEval) +
            (CNFACTOR * CornerEval) +
            (DCFACTOR * DCornerEval) +
            (SDFACTOR * SideEval) +
            (STFACTOR * StableEval) +
            (MBFACTOR * MobEval) +
            (FRFACTOR * FrontlineRateEval) +
            (CRFACTOR * CharaEval);

#ifdef TDEBUG
    evalTime += clock() - bTime;
    evalTimes++;
#endif

    return Eval;
}

inline int rsgn(int x) { return x % 2 ? 1 : -1; }

int MiniMax(const Board &board, short depth, int alpha, int beta, short bestCoord[2])
{
    // 输/赢时返回极大估值
    // 且赢时的回合数越小、赢的子越多，返回的值越大，反之亦然
    if (board.isWin(board.sideFlag))
        return BETA - 2000 + 24 * board.isWin(board.sideFlag) + depth;
    if (board.isWin(!board.sideFlag))
        return ALPHA + 2000 - 24 * board.isWin(!board.sideFlag) - depth;

    // 如果超时或搜索到达子节点，立即退出
    if (timeOut())
    {
        tc = true;
        return rsgn(maxDepth - depth) * TOCHARA;
    }
    else if (!depth)
        return board.eval();

    // 无可行位置时，直接进入下一层继续搜索
    if (!board.statusCount[Valid])
    {
        Board mainTmp = board;
        mainTmp.flipSide();
        mainTmp.setValid_fast();
        mainTmp.count();

        int Eval;

        // 带符号搜索下一层
        Eval = -MiniMax(mainTmp, depth - 1, -beta, -alpha, bestCoord);

        if (Eval >= beta)  // 如果返回值大于beta，被剪枝
            return beta;
        if (Eval > alpha)  // 大于alpha，刷新下界
            alpha = Eval;
    }
    else
    {
        // 先将编号为0的节点搜索到底
        int Eval;

        Board mainTmp = board;
        mainTmp.fastmove(board.validCoord[0]);

        Eval = -MiniMax(mainTmp, depth - 1, -beta, -alpha, bestCoord);
        if (Eval >= beta)
        {
            if (depth == maxDepth)
            {
                bestCoord[0] = board.validCoord[0][0];
                bestCoord[1] = board.validCoord[0][1];
            }
            return beta;
        }
        if (Eval > alpha)
        {
            alpha = Eval;
            if (depth == maxDepth)
            {
                bestCoord[0] = board.validCoord[0][0];
                bestCoord[1] = board.validCoord[0][1];
            }
        }

        // 再用最小窗口搜索其余节点
        size_t vSize = board.statusCount[Valid];
        for (size_t i = 1; i < vSize; i++)
        {
            Board tmpBoard = board;
            tmpBoard.fastmove(board.validCoord[i]);

            Eval = -MiniMax(tmpBoard, depth - 1, -alpha - MINWINDOW, -alpha, bestCoord);

            // 最小窗口验证失败，重新搜索
            if (Eval > alpha && Eval < beta)
            {
                Eval = -MiniMax(tmpBoard, depth - 1, -beta, -alpha - MINWINDOW, bestCoord);
            }
            if (Eval >= beta)
            {
                if (depth == maxDepth) // 返回到最底层时
                {
                    bestCoord[0] = board.validCoord[i][0];
                    bestCoord[1] = board.validCoord[i][1];
                    PVS = i;
                }
                return beta;
            }
            if (Eval > alpha)
            {
                alpha = Eval;
                if (depth == maxDepth) // 返回到最底层时
                {
                    bestCoord[0] = board.validCoord[i][0];
                    bestCoord[1] = board.validCoord[i][1];
                    PVS = i;
                }
            }
        }
    }
    return alpha;
}


void uglyList(int chara)
{
#ifdef UL
    int Ori[2];
    Ori[0] = inputCoord[0];
    Ori[1] = inputCoord[1];

    switch (chara)
    {
        case 583350:
            inputCoord[0] = 3;
            inputCoord[1] = 4;
            break;

        case 595238:
            inputCoord[0] = 6;
            inputCoord[1] = 4;
            break;

        case 582333:
            inputCoord[0] = 6;
            inputCoord[1] = 5;
            break;

        case 514982:
            inputCoord[0] = 2;
            inputCoord[1] = 5;
            break;

        case 547116:
            inputCoord[0] = 5;
            inputCoord[1] = 3;
            break;

        case 576293:
            inputCoord[0] = 5;
            inputCoord[1] = 3;
            break;

        case 566910:
            inputCoord[0] = 3;
            inputCoord[1] = 5;
            break;

        case 562701:
            inputCoord[0] = 3;
            inputCoord[1] = 4;
            break;

        case 577652:
            inputCoord[0] = 2;
            inputCoord[1] = 4;
            break;

        case 567560:
            inputCoord[0] = 5;
            inputCoord[1] = 3;
            break;

        case 568495:
            inputCoord[0] = 4;
            inputCoord[1] = 6;
            break;

        case 567639:
            inputCoord[0] = 4;
            inputCoord[1] = 6;
            break;

        case 586678:
            inputCoord[0] = 5;
            inputCoord[1] = 3;
            break;

        case 584657:
            inputCoord[0] = 6;
            inputCoord[1] = 4;
            break;

        case 581997:
            inputCoord[0] = 6;
            inputCoord[1] = 6;
            break;

        case 575291:
            inputCoord[0] = 3;
            inputCoord[1] = 5;
            break;

        case 578389:
            inputCoord[0] = 4;
            inputCoord[1] = 2;
            break;

        case 578873:
            inputCoord[0] = 5;
            inputCoord[1] = 2;
            break;

        case 551335:
            inputCoord[0] = 7;
            inputCoord[1] = 4;
            break;

        case 577910:
            inputCoord[0] = 6;
            inputCoord[1] = 4;
            break;

        case 580868:
            inputCoord[0] = 4;
            inputCoord[1] = 6;
            break;

        case 552914:
            inputCoord[0] = 3;
            inputCoord[1] = 6;
            break;

        case 553163:
            inputCoord[0] = 3;
            inputCoord[1] = 7;
            break;

        case 557237:
            inputCoord[0] = 5;
            inputCoord[1] = 6;
            break;

        case 563667:
            inputCoord[0] = 7;
            inputCoord[1] = 5;
            break;

        case 564505:
            inputCoord[0] = 3;
            inputCoord[1] = 2;
            break;

        case 543476:
            inputCoord[0] = 6;
            inputCoord[1] = 6;
            break;

        case 569020:
            inputCoord[0] = 6;
            inputCoord[1] = 7;
            break;

        case 558533:
            inputCoord[0] = 5;
            inputCoord[1] = 2;
            break;

        case 541029:
            inputCoord[0] = 3;
            inputCoord[1] = 6;
            break;

        case 542123:
            inputCoord[0] = 6;
            inputCoord[1] = 3;
            break;

        case 445327:
            inputCoord[0] = 5;
            inputCoord[1] = 2;
            break;

        case 569975:
            inputCoord[0] = 6;
            inputCoord[1] = 2;
            break;

        case 515973:
            inputCoord[0] = 5;
            inputCoord[1] = 8;
            break;

        case 520171:
            inputCoord[0] = 5;
            inputCoord[1] = 7;
            break;

        case 484810:
            inputCoord[0] = 7;
            inputCoord[1] = 5;
            break;

        case 509444:
            inputCoord[0] = 6;
            inputCoord[1] = 2;
            break;

        case 500026:
            inputCoord[0] = 1;
            inputCoord[1] = 4;
            break;

        case 534699:
            inputCoord[0] = 8;
            inputCoord[1] = 3;
            break;

        case 541013:
            inputCoord[0] = 6;
            inputCoord[1] = 5;
            break;

        case 541038:
            inputCoord[0] = 1;
            inputCoord[1] = 5;
            break;

        case 495801:
            inputCoord[0] = 6;
            inputCoord[1] = 3;
            break;

        case 548690:
            inputCoord[0] = 4;
            inputCoord[1] = 7;
            break;

        case 536128:
            inputCoord[0] = 4;
            inputCoord[1] = 2;
            break;

        case 494840:
            inputCoord[0] = 5;
            inputCoord[1] = 6;
            break;

        case 537812:
            inputCoord[0] = 3;
            inputCoord[1] = 3;
            break;

        case 524337:
            inputCoord[0] = 8;
            inputCoord[1] = 6;
            break;

        case 433621:
            inputCoord[0] = 8;
            inputCoord[1] = 4;
            break;

        case 539854:
            inputCoord[0] = 3;
            inputCoord[1] = 4;
            break;

        case 527822:
            inputCoord[0] = 2;
            inputCoord[1] = 5;
            break;

        case 523498:
            inputCoord[0] = 2;
            inputCoord[1] = 5;
            break;

        case 531782:
            inputCoord[0] = 8;
            inputCoord[1] = 5;
            break;

        case 508041:
            inputCoord[0] = 2;
            inputCoord[1] = 5;
            break;

        case 536880:
            inputCoord[0] = 3;
            inputCoord[1] = 8;
            break;

        case 482726:
            inputCoord[0] = 3;
            inputCoord[1] = 3;
            break;

        case 524578:
            inputCoord[0] = 8;
            inputCoord[1] = 6;
            break;

        case 525229:
            inputCoord[0] = 2;
            inputCoord[1] = 4;
            break;

        case 264702:
            inputCoord[0] = 1;
            inputCoord[1] = 3;
            break;

        case 550172:
            inputCoord[0] = 2;
            inputCoord[1] = 3;
            break;

        case 475970:
            inputCoord[0] = 7;
            inputCoord[1] = 5;
            break;

        case 448266:
            inputCoord[0] = 7;
            inputCoord[1] = 4;
            break;

        case 589149:
            inputCoord[0] = 5;
            inputCoord[1] = 3;
            break;

        case 434952:
            inputCoord[0] = 8;
            inputCoord[1] = 3;
            break;

        case 463482:
            inputCoord[0] = 3;
            inputCoord[1] = 8;
            break;

        case 542587:
            inputCoord[0] = 4;
            inputCoord[1] = 6;
            break;

        case 487073:
            inputCoord[0] = 6;
            inputCoord[1] = 1;
            break;

        case 489484:
            inputCoord[0] = 5;
            inputCoord[1] = 6;
            break;

        case 530294:
            inputCoord[0] = 7;
            inputCoord[1] = 5;
            break;

        case 446632:
            inputCoord[0] = 4;
            inputCoord[1] = 1;
            break;

        case 351616:
            inputCoord[0] = 2;
            inputCoord[1] = 8;
            break;

        case 535278:
            inputCoord[0] = 2;
            inputCoord[1] = 4;
            break;

        case 534090:
            inputCoord[0] = 3;
            inputCoord[1] = 7;
            break;

        case 543407:
            inputCoord[0] = 2;
            inputCoord[1] = 6;
            break;

        case 370544:
            inputCoord[0] = 6;
            inputCoord[1] = 2;
            break;

        case 544722:
            inputCoord[0] = 5;
            inputCoord[1] = 8;
            break;

        case 436678:
            inputCoord[0] = 1;
            inputCoord[1] = 3;
            break;

        case 367376:
            inputCoord[0] = 7;
            inputCoord[1] = 7;
            break;

        case 249093:
            inputCoord[0] = 7;
            inputCoord[1] = 7;
            break;

        case 446951:
            inputCoord[0] = 5;
            inputCoord[1] = 2;
            break;

        case 485842:
            inputCoord[0] = 2;
            inputCoord[1] = 7;
            break;

        case 486744:
            inputCoord[0] = 2;
            inputCoord[1] = 7;
            break;

        case 504515:
            inputCoord[0] = 2;
            inputCoord[1] = 3;
            break;

        case 344060:
            inputCoord[0] = 8;
            inputCoord[1] = 7;
            break;

        case 520660:
            inputCoord[0] = 2;
            inputCoord[1] = 5;
            break;

        case 568526:
            inputCoord[0] = 6;
            inputCoord[1] = 3;
            break;

        case 351747:
            inputCoord[0] = 5;
            inputCoord[1] = 1;
            break;

        case 564796:
            inputCoord[0] = 6;
            inputCoord[1] = 4;
            break;

        case 290917:
            inputCoord[0] = 7;
            inputCoord[1] = 3;
            break;

        case 469680:
            inputCoord[0] = 6;
            inputCoord[1] = 8;
            break;

        case 535041:
            inputCoord[0] = 3;
            inputCoord[1] = 7;
            break;

        case 542914:
            inputCoord[0] = 5;
            inputCoord[1] = 8;
            break;

        case 508710:
            inputCoord[0] = 6;
            inputCoord[1] = 3;
            break;

        case 577762:
            inputCoord[0] = 3;
            inputCoord[1] = 6;
            break;

        case 549345:
            inputCoord[0] = 6;
            inputCoord[1] = 6;
            break;

        case 567085:
            inputCoord[0] = 2;
            inputCoord[1] = 5;
            break;

        case 546993:
            inputCoord[0] = 3;
            inputCoord[1] = 6;
            break;

        case 531024:
            inputCoord[0] = 7;
            inputCoord[1] = 4;
            break;

        case 490968:
            inputCoord[0] = 7;
            inputCoord[1] = 7;
            break;

        case 411651:
            inputCoord[0] = 2;
            inputCoord[1] = 8;
            break;

        case 515158:
            inputCoord[0] = 6;
            inputCoord[1] = 3;
            break;

        case 471734:
            inputCoord[0] = 5;
            inputCoord[1] = 2;
            break;

        case 561561:
            inputCoord[0] = 7;
            inputCoord[1] = 4;
            break;

        case 570535:
            inputCoord[0] = 6;
            inputCoord[1] = 3;
            break;

        case 551430:
            inputCoord[0] = 1;
            inputCoord[1] = 4;
            break;

        case 537192:
            inputCoord[0] = 6;
            inputCoord[1] = 1;
            break;

        case 534239:
            inputCoord[0] = 3;
            inputCoord[1] = 6;
            break;

        case 501134:
            inputCoord[0] = 7;
            inputCoord[1] = 3;
            break;

        case 440459:
            inputCoord[0] = 3;
            inputCoord[1] = 7;
            break;

        case 499624:
            inputCoord[0] = 2;
            inputCoord[1] = 1;
            break;

        case 427839:
            inputCoord[0] = 1;
            inputCoord[1] = 5;
            break;

        case 448699:
            inputCoord[0] = 6;
            inputCoord[1] = 7;
            break;

        case 557572:
            inputCoord[0] = 2;
            inputCoord[1] = 6;
            break;

        case 415933:
            inputCoord[0] = 7;
            inputCoord[1] = 5;
            break;

        case 574618:
            inputCoord[0] = 7;
            inputCoord[1] = 5;
            break;

        case 564797:
            inputCoord[0] = 7;
            inputCoord[1] = 4;
            break;

        case 539541:
            inputCoord[0] = 4;
            inputCoord[1] = 7;
            break;

        case 429900:
            inputCoord[0] = 4;
            inputCoord[1] = 7;
            break;

        case 539861:
            inputCoord[0] = 7;
            inputCoord[1] = 4;
            break;

        case 571971:
            inputCoord[0] = 2;
            inputCoord[1] = 4;
            break;

        case 479029:
            inputCoord[0] = 8;
            inputCoord[1] = 5;
            break;

        case 528399:
            inputCoord[0] = 1;
            inputCoord[1] = 6;
            break;

        case 554217:
            inputCoord[0] = 3;
            inputCoord[1] = 6;
            break;

        case 550482:
            inputCoord[0] = 6;
            inputCoord[1] = 7;
            break;

        case 556157:
            inputCoord[0] = 4;
            inputCoord[1] = 7;
            break;

        case 576681:
            inputCoord[0] = 2;
            inputCoord[1] = 5;
            break;

        case 518709:
            inputCoord[0] = 2;
            inputCoord[1] = 5;
            break;

        case 520159:
            inputCoord[0] = 5;
            inputCoord[1] = 8;
            break;

        case 522007:
            inputCoord[0] = 5;
            inputCoord[1] = 8;
            break;

        case 538231:
            inputCoord[0] = 5;
            inputCoord[1] = 7;
            break;

        case 553480:
            inputCoord[0] = 6;
            inputCoord[1] = 4;
            break;

        case 522436:
            inputCoord[0] = 6;
            inputCoord[1] = 3;
            break;

        case 568691:
            inputCoord[0] = 6;
            inputCoord[1] = 3;
            break;

        case 560132:
            inputCoord[0] = 6;
            inputCoord[1] = 6;
            break;

        case 549501:
            inputCoord[0] = 6;
            inputCoord[1] = 3;
            break;

        case 555429:
            inputCoord[0] = 6;
            inputCoord[1] = 7;
            break;

        case 517149:
            inputCoord[0] = 8;
            inputCoord[1] = 6;
            break;

        case 463046:
            inputCoord[0] = 4;
            inputCoord[1] = 3;
            break;

        case 573381:
            inputCoord[0] = 7;
            inputCoord[1] = 6;
            break;

        case 458785:
            inputCoord[0] = 3;
            inputCoord[1] = 6;
            break;

        case 521683:
            inputCoord[0] = 2;
            inputCoord[1] = 3;
            break;

        case 561315:
            inputCoord[0] = 3;
            inputCoord[1] = 2;
            break;

        case 468953:
            inputCoord[0] = 5;
            inputCoord[1] = 2;
            break;

        case 364131:
            inputCoord[0] = 3;
            inputCoord[1] = 2;
            break;

        case 522009:
            inputCoord[0] = 7;
            inputCoord[1] = 3;
            break;

        case 557807:
            inputCoord[0] = 4;
            inputCoord[1] = 2;
            break;

        case 459333:
            inputCoord[0] = 3;
            inputCoord[1] = 6;
            break;

        case 505211:
            inputCoord[0] = 1;
            inputCoord[1] = 3;
            break;

        case 503649:
            inputCoord[0] = 6;
            inputCoord[1] = 4;
            break;

        case 567248:
            inputCoord[0] = 6;
            inputCoord[1] = 6;
            break;

        case 493934:
            inputCoord[0] = 2;
            inputCoord[1] = 6;
            break;

        case 545244:
            inputCoord[0] = 3;
            inputCoord[1] = 5;
            break;

        case 460794:
            inputCoord[0] = 3;
            inputCoord[1] = 6;
            break;

        case 500062:
            inputCoord[0] = 6;
            inputCoord[1] = 3;
            break;

        case 562077:
            inputCoord[0] = 4;
            inputCoord[1] = 6;
            break;

        case 437643:
            inputCoord[0] = 4;
            inputCoord[1] = 1;
            break;

        case 391261:
            inputCoord[0] = 7;
            inputCoord[1] = 7;
            break;

        case 473696:
            inputCoord[0] = 8;
            inputCoord[1] = 7;
            break;

        case 468097:
            inputCoord[0] = 2;
            inputCoord[1] = 6;
            break;

        case 374560:
            inputCoord[0] = 7;
            inputCoord[1] = 4;
            break;

        case 437721:
            inputCoord[0] = 7;
            inputCoord[1] = 2;
            break;

        case 339763:
            inputCoord[0] = 4;
            inputCoord[1] = 1;
            break;

        case 474238:
            inputCoord[0] = 4;
            inputCoord[1] = 2;
            break;

        case 286959:
            inputCoord[0] = 2;
            inputCoord[1] = 2;
            break;

        case 537765:
            inputCoord[0] = 5;
            inputCoord[1] = 6;
            break;

        case 553254:
            inputCoord[0] = 2;
            inputCoord[1] = 6;
            break;

        case 470169:
            inputCoord[0] = 5;
            inputCoord[1] = 2;
            break;

        case 504725:
            inputCoord[0] = 2;
            inputCoord[1] = 6;
            break;

        case 572863:
            inputCoord[0] = 3;
            inputCoord[1] = 6;
            break;

        case 561439:
            inputCoord[0] = 8;
            inputCoord[1] = 6;
            break;

        case 582985:
            inputCoord[0] = 5;
            inputCoord[1] = 2;
            break;

        case 547650:
            inputCoord[0] = 7;
            inputCoord[1] = 3;
            break;

        case 553306:
            inputCoord[0] = 7;
            inputCoord[1] = 3;
            break;

        case 516955:
            inputCoord[0] = 4;
            inputCoord[1] = 7;
            break;

        default:
            return;
    }

    //VALIDCHECK
    if (!gameBoard.isValid(inputCoord, gameBoard.sideFlag))
    {
        inputCoord[0] = Ori[0];
        inputCoord[1] = Ori[1];
    }
    if (inputCoord[0] != Ori[0] || inputCoord[1] != Ori[1])
        ul = true;
#endif
}

int find_the_best(int *arr, int currBotColor)
{
    rTmp = 1027;
    passCount = 0;
    maxDepth = 5;
    PVS = 0;

    tc = false;
    ul = false;
    iv = false;

    startTime = clock();

    //tc = false;
    //ul = false;
    gameBoard.update(arr, currBotColor);

    gameBoard.setValid_fast();
    gameBoard.count();

    // gameBoard.show();
    turnID = gameBoard.statusCount[0] + gameBoard.statusCount[1] - 4;
    // cout << "TurnID: " << turnID << endl;

    if (gameBoard.statusCount[Valid] == 1)
        for (int i = 1; i <= 8; i++)
            for (int j = 1; j <= 8; j++)
                if (gameBoard.status[i][j] == Valid)
                {
                    // cout << "find_the_best only one: " <<  (i - 1) * 10 + j - 1 << endl;
                    return (i - 1) * 10 + j - 1;
                }

    int chara = gameBoard.charaCalc();
    uglyList(chara);

    if (!ul)
    {
        moveTime = 0;
        moveTimes = 0;

        if (!gameBoard.statusCount[Valid])
        {
            return -11;
        }

        size_t validCount = gameBoard.statusCount[Valid];
        if (turnID > 21)
        {
            if (turnID > 23)
                maxDepth = 12;
            else if (turnID > 22)
            {
                if (validCount < 9)
                    maxDepth = 12;
                else if (validCount < 10)
                    maxDepth = 11;
                else
                    maxDepth = 10;
            }
            else
            {
                if (validCount < 8)
                    maxDepth = 11;
                else if (validCount < 10)
                    maxDepth = 10;
                else if (validCount < 12)
                    maxDepth = 9;
                else
                    maxDepth = 8;
            }

            if (turnID > 23)
            {
                BWFACTOR = 4;
                DCFACTOR = 30;
            }
            else
            {
                BWFACTOR = 2;
                DCFACTOR = 60;
            }

            CNFACTOR = 800;
            SDFACTOR = 300;
            STFACTOR = 300;
            FRFACTOR = 12;
            MBFACTOR = 12;
            CRFACTOR = 0;
        }
        else if (turnID > 18)
        {
            if (validCount > 9)
                maxDepth = 7;
            else
                maxDepth = 8;

            BWFACTOR = 2;
            CNFACTOR = 800;
            DCFACTOR = 75;
            SDFACTOR = 250;
            STFACTOR = 200;
            FRFACTOR = 33;
            MBFACTOR = 23;
            CRFACTOR = 4;
        }
        else if (turnID > 14)
        {
            if (validCount > 12)
                maxDepth = 6;
            else
                maxDepth = 7;

            BWFACTOR = 2;
            CNFACTOR = 950;
            DCFACTOR = 240;
            SDFACTOR = 200;
            STFACTOR = 0;
            FRFACTOR = 45;
            MBFACTOR = 30;
            CRFACTOR = 9;
        }
        else if (turnID > 8)
        {
            maxDepth = 6;

            BWFACTOR = 2;
            CNFACTOR = 900;
            DCFACTOR = 240;
            SDFACTOR = 0;
            STFACTOR = 0;
            FRFACTOR = 51 - turnID;
            MBFACTOR = 36 - turnID;
            CRFACTOR = 12;
        }
        else
        {
            if (validCount > 12)
                maxDepth = 5;
            else
                maxDepth = 6;

            BWFACTOR = 1;
            CNFACTOR = 750;
            DCFACTOR = 195;
            SDFACTOR = 0;
            STFACTOR = 0;
            FRFACTOR = 64 - turnID;
            MBFACTOR = 43 - turnID;
            CRFACTOR = 13;
        }

        int lastR = 0;
        short lastCoord[2] = {};

        // cout << "Search function called!" << endl;
        for (; !timeOut() && maxDepth < 25;
               lastR = R, lastCoord[0] = inputCoord[0], lastCoord[1] = inputCoord[1], maxDepth++)
        {
            R = MiniMax(gameBoard, maxDepth, ALPHA, BETA, inputCoord);

            if (abs(R) > BETA - 3000 && abs(lastR) > BETA - 3000)
                break;
            if (tc)
            {
                inputCoord[0] = lastCoord[0];
                inputCoord[1] = lastCoord[1];
                break;
            }
            for (size_t i = PVS; i > 0; --i)
                swap(gameBoard.validCoord[i], gameBoard.validCoord[i - 1]);
        }
    }

    if (!gameBoard.isValid(inputCoord, gameBoard.sideFlag))
    {
        inputCoord[0] = gameBoard.validCoord[1 % gameBoard.statusCount[Valid]][0];
        inputCoord[1] = gameBoard.validCoord[1 % gameBoard.statusCount[Valid]][1];
        iv = true;
    }

    /*
    Json::Value ret;
    ret["response"]["x"] = inputCoord[0] - 1;
    ret["response"]["y"] = inputCoord[1] - 1;

    ret["debug"]["R"] = R;
    ret["debug"]["TC"] = tc;
    //ret["debug"]["UL"] = ul;
    ret["debug"]["IV"] = iv;
    ret["debug"]["MT"] = int(moveTime);
    ret["debug"]["MTs"] = moveTimes;
    ret["debug"]["ET"] = int(evalTime);
    ret["debug"]["ETs"] = evalTimes;
    ret["debug"]["ALL"] = int(clock() - startTime);
    ret["debug"]["MD"] = maxDepth + R;
    ret["debug"]["CHAR"] = gameBoard.charaCalc();

    Json::FastWriter writer;
    cout << writer.write(ret) << endl;
    */
    // cout << "find_the_best: " <<  (inputCoord[0] - 1) * 10 + (inputCoord[1] - 1) << endl;
    return (inputCoord[0] - 1) * 10 + (inputCoord[1] - 1);
}
