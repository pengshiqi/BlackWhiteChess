// 黑白棋（Reversi)
// minimax search & alpha-beta pruning 策略
// 作者：patrick_psq
// 游戏信息：http://www.botzone.org/games#Reversi

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <bitset>
#include <vector>
#include <ctime>
#include <unistd.h>
#include "jsoncpp/json.h" // C++编译时默认包含此库

using namespace std;

int currBotColor; // 我所执子颜色（1为黑，-1为白，棋盘状态亦同）  在AI中，1为白，2为黑
int gridInfo[8][8] = { 0 }; // 先x后y，记录棋盘状态
int blackPieceCount = 2, whitePieceCount = 2;

// value函数的权重
int BW_FACTOR, CN_FACTOR, DC_FACTOR, SD_FACTOR, ST_FACTOR, MB_FACTOR, FR_FACTOR, CR_FACTOR;

int turnID;
int result;
int best_x = -1, best_y = -1;

int maxDepth = 5;
int PVS = 0;
const int maxNum = 1 << 30;
const int ALPHA = - (1 << 30);
const int BETA = 1 << 30;
const int MinWindow = 16;
const double TIME_OUT = 0.99;

const long long FRINIT = 66125924401152;
//00000000
//00000000
//00111100
//00100100
//00100100
//00111100
//00000000
//00000000

time_t startTime;

const short direction[8][2] = {{1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}};

const short coordValue[8][8] = { {20, -3, 11,  8,  8, 11, -3, 20},
                                 {-3, -7, -4,  1,  1, -4, -7, -3},
                                 {11, -4,  2,  2,  2,  2, -4, 11},
                                 { 8,  1,  2, -3, -3,  2,  1,  8},
                                 { 8,  1,  2, -3, -3,  2,  1,  8},
                                 {11, -4,  2,  2,  2,  2, -4, 11},
                                 {-3, -7, -4,  1,  1, -4, -7, -3},
                                 {20, -3, 11,  8,  8, 11, -3, 20}};

// 棋盘特征值，用于hash
const short charaTable[8][8] = {{     1,     7,    43,    79,    89,   113,   167,   199},
                                {   769,   709,   677,   541,   449,   313,   281,   233},
                                {   827,   881,   937,   997,  1009,  1117,  1231,  1373},
                                {  2311,  2099,  1999,  1811,  1777,  1667,  1511,  1409},
                                {  2557,  2777,  3121,  3331,  3533,  3631,  4441,  4799},
                                {  7411,  7177,  6827,  6689,  6311,  5987,  5399,  5077},
                                {  7717,  8111,  8447,  8663,  8887,  9277,  9511,  9929},
                                { 10099, 11171, 11467, 12907, 14669, 17041, 18517, 19997}};

const short sideVal[8] = {1, 1, 1, 2, 3, 4, 6, 7};

struct Coord
{
    short x;
    short y;
};


// White: false, Black: true
enum Status {White, Black, Empty, Valid};

// 传入棋盘状态 & 当前落子方，输出最佳落子位置
int find_the_best(int *arr, int currentBotColor);

class Board
{
public:
    // 当前下子用户，Black:true; White:false
    bool sideFlag;
    // 上回合是否pass
    bool passFlag[2];
    // 棋盘上各种状态位置的计数
    short statusCount[4];
    // 棋盘上每个格子的状态
    Status status[8][8];
    bool validDir[8][8][8];
    //Cell cell[8][8];
    short validCountFor[2];
    // 棋局边界图
    bitset<64> FrontLine;
    // 黑白各自的边界图
    bitset<64> sideFrontLine[2];
    // 存储棋盘当前所有可行位置
    short validCoord[24][2];
    //vector<Coord> validCoord;
    // 棋局着子记录
    vector<Coord> movesRecord;

    // 当前棋盘hash值
    int chara;

public:
    Board();
    Board(int *arr, int currentBotColor);
    ~Board();

    void operator =(const Board &board);
    void init();
    void flipSide();
    void count();
    void setFrontlineFor(bool side);
    inline void setAllFrontline();
    void setFrontline();

    bool isValid(const short pos[2], bool side) const;
    bool isValid(int x, int y, bool side) const;
    bool isValid_fast(const short pos[2], bool side);
    bool isValid_fast(int x, int y, bool side);
    void setValid();
    void setValid_fast();
    void move(const short pos[2]);
    void move(int x, int y);
    void fastmove(const short pos[2]);
    void fastmove(int x, int y);

    int allEvalFor(bool side) const;
    int charaCalc() const;
    void coordSort();
    bool isStable(int x, int y) const;
    const int eval() const;

    inline bool end() const;
    inline int isWin(bool side) const;

    inline bool inRange(short p, short q) const;
    inline bool inRange(const short C[2]) const;
    inline short getX(int a);
    inline short getY(int a);
    inline short toOne(short x, short y) const;
    inline short toOne(const short C[2]) const;

    void show();
};

int minimax(Board &B, int depth, int alpha, int beta, int &x, int &y);

Board::Board()
{
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            status[i][j] = Empty;

    statusCount[2] = 64;
    sideFlag = Black;
}

Board::Board(int *arr, int currentBotColor)
{
    init();
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (arr[i * 8 + j] == 1)
                status[i][j] = Black;
            else if (arr[i * 8 + j] == 0)
                status[i][j] = White;
            else
                status[i][j] = Empty;

    if (currentBotColor == 1)
        sideFlag = true;
    else
        sideFlag = false;

    setValid_fast();
    count();
}

Board::~Board()
{

}

void Board::operator=(const Board &board)
{
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            status[i][j] = board.status[i][j];
            for (int k = 0; k < 8; k++)
                validDir[i][j][k] = board.validDir[i][j][k];
        }

    passFlag[Black] = board.passFlag[Black];
    passFlag[White] = board.passFlag[White];

    FrontLine = board.FrontLine;
    sideFrontLine[0] = board.sideFrontLine[0];
    sideFrontLine[1] = board.sideFrontLine[1];

    for (int i = 0; i < 24; i++)
    {
        validCoord[i][0] = board.validCoord[i][0];
        validCoord[i][1] = board.validCoord[i][1];
    }

    sideFlag = board.sideFlag;
}

void Board::init()
{
    status[3][3] = status[4][4] = White;
    status[3][4] = status[4][3] = Black;
    status[2][3] = status[3][2] = status[5][4] = status[4][5] = Valid;
    statusCount[Black] = 2;
    statusCount[White] = 2;
    statusCount[Empty] = 60;
    statusCount[Valid] = 4;
    sideFlag = Black;

    FrontLine = FRINIT;
}

void Board::flipSide() { sideFlag ^= 1; }

void Board::count()
{
    statusCount[Black] = statusCount[White] = statusCount[Empty] = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            statusCount[Black] += (status[i][j] == Black);
            statusCount[White] += (status[i][j] == White);
            statusCount[Empty] += (status[i][j] >= Empty);
        }
}

void Board::setFrontlineFor(bool side)
{
    sideFrontLine[side].reset();
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (status[i][j] == Status(side))
                for (int d = 0; d < 8; d++)
                {
                    int x = i + direction[d][0];
                    int y = j + direction[d][1];
                    if (inRange(x, y) && (status[x][y] >= Empty))
                        sideFrontLine[side][toOne(x, y)] = true;
                }
}

void Board::setAllFrontline() { FrontLine = sideFrontLine[0] | sideFrontLine[1]; }

void Board::setFrontline()
{
    setFrontlineFor(sideFlag);
    setFrontlineFor(!sideFlag);
    setAllFrontline();
}

bool Board::isValid(const short pos[2], bool side) const
{
    if (!inRange(pos))
        return false;
    if (status[pos[0]][pos[1]] < Empty)
        return false;
    for (int i = 0; i < 8; i++)
    {
        short nx = pos[0] + direction[i][0], ny = pos[1] + direction[i][1];
        if (status[nx][ny] == !side)
        {
            for (int p = nx, q = ny; inRange(p, q); p += direction[i][0], q += direction[i][1])
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

bool Board::isValid(int x, int y, bool side) const
{
    if (!inRange(x, y))
        return false;
    if (status[x][y] < Empty)
        return false;
    for (int i = 0; i < 8; i++)
    {
        short nx = x + direction[i][0], ny = y + direction[i][1];
        if (status[nx][ny] == !side)
        {
            for (int p = nx, q = ny; inRange(p, q); p += direction[i][0], q += direction[i][1])
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

bool Board::isValid_fast(const short pos[2], bool side)
{
    bool R = false;
    short x = pos[0], y = pos[1];
    for (int i = 0; i < 8; i++)
    {
        validDir[x][y][i] = false;

        short nx = x + direction[i][0], ny = y + direction[i][1];
        if (status[nx][ny] == Status(!side))
        {
            for (int p = nx, q = ny; inRange(p, q); p += direction[i][0], q += direction[i][1])
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

bool Board::isValid_fast(int x, int y, bool side)
{
    bool R = false;
    for (int i = 0; i < 8; i++)
    {
        validDir[x][y][i] = false;

        short nx = x + direction[i][0], ny = y + direction[i][1];
        if (status[nx][ny] == Status(!side))
        {
            for (int p = nx, q = ny; inRange(p, q); p += direction[i][0], q += direction[i][1])
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

void Board::setValid()
{
    setFrontline();
    statusCount[Valid] = 0;
    validCountFor[Black] = validCountFor[White] = 0;
    for (int i = 0; i < 64; i++)
        if (FrontLine[i])
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

void Board::setValid_fast()
{
    setFrontline();
    statusCount[Valid] = 0;
    validCountFor[Black] = validCountFor[White] = 0;
    for (int i = 0; i < 64; i++)
        if (FrontLine[i])
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

void Board::move(const short pos[2])
{
    for (int i = 0; i < 8; i++)
    {
        int dx = direction[i][0], dy = direction[i][1];
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

void Board::move(int x, int y)
{
    if (x == -1 && y == -1)
        passFlag[sideFlag] = true;
    else if (inRange(x, y))
    {
        for (int i = 0; i < 8; i++)
        {
            int dx = direction[i][0], dy = direction[i][1];
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

void Board::fastmove(const short pos[2])
{
    for (int i = 0; i < 8; i++)
    {
        short x = pos[0], y = pos[1];
        if (validDir[x][y][i])
            do
            {
                status[x][y] = Status(sideFlag);
                x += direction[i][0];
                y += direction[i][1];
            } while (status[x][y] == Status(!sideFlag));
    }

    flipSide();
    setValid_fast();
    count();
}

void Board::fastmove(int x, int y)
{
    for (int i = 0; i < 8; i++)
        if (validDir[x][y][i])
        {
            short tx = x, ty = y;
            do
            {
                status[tx][ty] = Status(sideFlag);
                tx += direction[i][0];
                ty += direction[i][1];
            } while (status[tx][ty] == Status(!sideFlag));
        }


    flipSide();
    setValid_fast();
    count();
}

int Board::allEvalFor(bool side) const
{
    // Evaluation for all cells of the side
    int allVal = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (status[i][j] == Status(side))
                allVal += coordValue[i][j];
    return allVal;
}

int Board::charaCalc() const
{
    int chara = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            chara += status[i][j] * charaTable[i][j];
    return chara;
}

void Board::coordSort()
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

bool Board::isStable(int x, int y) const
{
    for (int i = 0; i < 8; i++)
        for (int nx = x + direction[i][0], ny = y + direction[i][1]; inRange(nx, ny); nx += direction[i][0], ny += direction[i][1])
            if (FrontLine[toOne(nx, ny)])
                return false;
    return true;
}

inline bool Board::end() const
{
    return (!statusCount[Empty] || !statusCount[Black] || !statusCount[White] || (passFlag[Black] && passFlag[White]));
}

inline int Board::isWin(bool side) const
{
    return (end() && (statusCount[side] > statusCount[!side])) ? statusCount[side] - statusCount[!side] : 0;
}

inline bool Board::inRange(short p, short q) const
{
    return (p >= 0 && p < 8 && q >= 0 && q < 8);
}

inline bool Board::inRange(const short *C) const
{
    return (C[0] >= 0 && C[0] < 8 && C[1] >= 0 && C[1] < 8);
}

inline short Board::getX(int a) { return a / 8; }

inline short Board::getY(int a) { return a % 8; }

inline short Board::toOne(short x, short y) const { return 8 * x + y; }

inline short Board::toOne(const short C[2]) const { return 8 * C[0] + C[1]; }

void Board::show()
{
    cout << "SideFlag: " << sideFlag << endl;
    cout << "StatusCount: " << statusCount[0] << ' ' << statusCount[1] << ' ' << statusCount[2] << ' ' << statusCount[3] << endl;
    cout << "Status: " << endl;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
            cout << status[i][j] << ' ';
        cout << endl;
    }
}

// 估值函数
const int Board::eval() const
{
    // Evaluation based on character value
    int CharaEval = 0;
    if (CR_FACTOR)
    {
        int myChara = allEvalFor(sideFlag);
        int opChara = allEvalFor(!sideFlag);

        CharaEval = myChara - opChara;
    }

    // Evaluation based on stone rate
    int BWRateEval = 0;
    if (BW_FACTOR)
    {
        short myStoneCount = statusCount[sideFlag];
        short opStoneCount = statusCount[!sideFlag];

        if (myStoneCount > opStoneCount)
            BWRateEval = 200 * myStoneCount / (myStoneCount + opStoneCount);
        else if (myStoneCount < opStoneCount)
            BWRateEval = -200 * opStoneCount / (myStoneCount + opStoneCount);
        else BWRateEval = 0;
    }

    // Evaluation based on stone frontline rate
    int FrontlineRateEval = 0;
    if (FR_FACTOR)
    {
        int myFrontlineCount = int(sideFrontLine[sideFlag].count());
        int opFrontlineCount = int(sideFrontLine[!sideFlag].count());

        if (myFrontlineCount > opFrontlineCount)
            FrontlineRateEval = -200 * myFrontlineCount / (myFrontlineCount + opFrontlineCount);
        else if (myFrontlineCount < opFrontlineCount)
            FrontlineRateEval = 200 * opFrontlineCount / (myFrontlineCount + opFrontlineCount);
        else
            FrontlineRateEval = 0;
    }

    // Evaluation based on corner stone count
    int CornerEval = 0;
    short myCornerCount = 0, opCornerCount = 0;
    if (CN_FACTOR)
    {
        myCornerCount =
                (status[0][0] == Status(sideFlag)) +
                (status[0][7] == Status(sideFlag)) +
                (status[7][0] == Status(sideFlag)) +
                (status[7][7] == Status(sideFlag));

        opCornerCount =
                (status[0][0] == Status(!sideFlag)) +
                (status[0][7] == Status(!sideFlag)) +
                (status[7][0] == Status(!sideFlag)) +
                (status[7][7] == Status(!sideFlag));

        CornerEval = 25 * (myCornerCount - opCornerCount);
    }

    // Evaluation based on dangerous corner-beside-stone count
    int DCornerEval = 0;
    if (DC_FACTOR)
    {
        short myDCornerCount =
                ((status[0][0] >= Empty) && (status[0][1] == Status(sideFlag))) +
                2 * ((status[0][0] >= Empty) && (status[1][1] == Status(sideFlag))) +
                ((status[0][0] >= Empty) && (status[1][0] == Status(sideFlag))) +
                ((status[0][7] >= Empty) && (status[0][6] == Status(sideFlag))) +
                2 * ((status[0][7] >= Empty) && (status[1][6] == Status(sideFlag))) +
                ((status[0][7] >= Empty) && (status[1][7] == Status(sideFlag))) +
                ((status[7][0] >= Empty) && (status[7][1] == Status(sideFlag))) +
                2 * ((status[7][0] >= Empty) && (status[6][1] == Status(sideFlag))) +
                ((status[7][0] >= Empty) && (status[6][0] == Status(sideFlag))) +
                ((status[7][7] >= Empty) && (status[7][6] == Status(sideFlag))) +
                2 * ((status[7][7] >= Empty) && (status[6][6] == Status(sideFlag))) +
                ((status[7][7] >= Empty) && (status[6][7] == Status(sideFlag)));

        short opDCornerCount =
                ((status[0][0] >= Empty) && (status[0][1] == Status(!sideFlag))) +
                2 * ((status[0][0] >= Empty) && (status[1][1] == Status(!sideFlag))) +
                ((status[0][0] >= Empty) && (status[1][0] == Status(!sideFlag))) +
                ((status[0][7] >= Empty) && (status[0][6] == Status(!sideFlag))) +
                2 * ((status[0][7] >= Empty) && (status[1][6] == Status(!sideFlag))) +
                ((status[0][7] >= Empty) && (status[1][7] == Status(!sideFlag))) +
                ((status[7][0] >= Empty) && (status[7][1] == Status(!sideFlag))) +
                2 * ((status[7][0] >= Empty) && (status[6][1] == Status(!sideFlag))) +
                ((status[7][0] >= Empty) && (status[6][0] == Status(!sideFlag))) +
                ((status[7][7] >= Empty) && (status[7][6] == Status(!sideFlag))) +
                2 * ((status[7][7] >= Empty) && (status[6][6] == Status(!sideFlag))) +
                ((status[7][7] >= Empty) && (status[6][7] == Status(!sideFlag)));

        DCornerEval = -25 * (myDCornerCount - opDCornerCount);
    }

    // Evaluation based on stable side stones count
    int SideEval = 0;
    if (SD_FACTOR)
    {
        short mySideVal = 0, opSideVal = 0;

        if (!myCornerCount && !opCornerCount)
            SideEval = 0;
        else
        {
            if (status[0][0] == Status(sideFlag))
            {
                for (int i = 0; i < 8; i++)
                {
                    if (status[0][i] == Status(sideFlag))
                        mySideVal += sideVal[i];
                    else
                        break;
                }
                for (int i = 0; i < 8; i++)
                {
                    if (status[i][0] == Status(sideFlag))
                        mySideVal += sideVal[i];
                    else
                        break;
                }
            }
            else if (status[0][0] == Status(!sideFlag))
            {
                for (int i = 0; i < 8; i++)
                {
                    if (status[0][i] == Status(!sideFlag))
                        opSideVal += sideVal[i];
                    else
                        break;
                }
                for (int i = 0; i < 8; i++)
                {
                    if (status[i][0] == Status(!sideFlag))
                        opSideVal += sideVal[i];
                    else
                        break;
                }
            }

            if (status[0][7] == Status(sideFlag))
            {
                for (int i = 0; i < 8; i++)
                {
                    if (status[i][7] == Status(sideFlag))
                        mySideVal += sideVal[i];
                    else
                        break;
                }
                for (int i = 7; i >= 0; i--)
                {
                    if (status[0][i] == Status(sideFlag))
                        mySideVal += sideVal[7 - i];
                    else
                        break;
                }
            }
            else if (status[0][7] == Status(!sideFlag))
            {
                for (int i = 0; i < 8; i++)
                {
                    if (status[i][7] == Status(!sideFlag))
                        opSideVal += sideVal[i];
                    else
                        break;
                }
                for (int i = 7; i >= 0; i--)
                {
                    if (status[0][i] == Status(!sideFlag))
                        opSideVal += sideVal[7 - i];
                    else
                        break;
                }
            }

            if (status[7][7] == Status(sideFlag))
            {
                for (int i = 7; i >= 0; i--)
                {
                    if (status[7][i] == Status(sideFlag))
                        mySideVal += sideVal[7 - i];
                    else
                        break;
                }
                for (int i = 7; i >= 0; i--)
                {
                    if (status[i][7] == Status(sideFlag))
                        mySideVal += sideVal[7 - i];
                    else
                        break;
                }
            }
            else if (status[7][7] == Status(!sideFlag))
            {
                for (int i = 7; i >= 0; i--)
                {
                    if (status[7][i] == Status(!sideFlag))
                        opSideVal += sideVal[7 - i];
                    else
                        break;
                }
                for (int i = 7; i >= 0; i--)
                {
                    if (status[i][7] == Status(!sideFlag))
                        opSideVal += sideVal[7 - i];
                    else
                        break;
                }
            }

            if (status[7][0] == Status(sideFlag))
            {
                for (int i = 7; i >= 0; i--)
                {
                    if (status[i][0] == Status(sideFlag))
                        mySideVal += sideVal[7 - i];
                    else
                        break;
                }
                for (int i = 0; i < 8; i++)
                {
                    if (status[7][i] == Status(sideFlag))
                        mySideVal += sideVal[i];
                    else
                        break;
                }
            }
            else if (status[7][0] == Status(!sideFlag))
            {
                for (int i = 7; i >= 0; i--)
                {
                    if (status[i][0] == Status(!sideFlag))
                        opSideVal += sideVal[7 - i];
                    else
                        break;
                }
                for (int i = 0; i < 8; i++)
                {
                    if (status[7][i] == Status(!sideFlag))
                        opSideVal += sideVal[i];
                    else
                        break;
                }
            }

            SideEval = 5 * (mySideVal - opSideVal);
        }
    }

    // Evaluation based on inner stable stone count
    int StableEval = 0;

    if (ST_FACTOR)
    {
        short myStableCount = 0, opStableCount = 0;

        for (int i = 1; i < 7; i++)
            for (int j = 1; j < 7; j++)
                if (isStable(i, j))
                {
                    if (status[i][j] == Status(sideFlag))
                        myStableCount++;
                    else
                        opStableCount++;
                }

        StableEval = 25 * (myStableCount - opStableCount);
    }

    // Evaluation based on mobility
    int MobEval = 0;

    if (MB_FACTOR)
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
        else
            MobEval = 0;
    }

    // weight evaluation
    int Eval =
            BW_FACTOR * BWRateEval +
            CN_FACTOR * CN_FACTOR +
            DC_FACTOR * DCornerEval +
            SD_FACTOR * SideEval +
            ST_FACTOR * StableEval +
            MB_FACTOR * MobEval +
            FR_FACTOR * FrontlineRateEval +
            CR_FACTOR * CharaEval;

    return Eval;
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

inline bool timeOut()
{
    return time(NULL) - startTime > TIME_OUT;
}

int minimax(Board &B, int depth, int alpha, int beta, int &x, int &y)
{
    if (B.isWin(B.sideFlag))
        return BETA - 2000 + 24 * B.isWin(B.sideFlag) + depth;
    if (B.isWin(!B.sideFlag))
        return ALPHA + 2000 - 24 * B.isWin(!B.sideFlag) - depth;

    if (!depth)
        return B.eval();

    if (!B.statusCount[Valid])
    {
        // 1. 无可行位置，直接进入下一层搜索
        Board newBoard = B;
        newBoard.flipSide();
        newBoard.setValid_fast();
        newBoard.count();

        int Eval;

        Eval = -minimax(newBoard, depth - 1, -beta, -alpha, x, y);

        // 如果返回值大于beta,被剪枝
        if (Eval >= beta)
            return beta;
        // 大于alpha,刷新下届
        if (Eval > alpha)
            alpha = Eval;
    }
    else
    {
        // 2. 先将编号为0的节点搜索到底
        int Eval;

        Board newBoard = B;
        newBoard.fastmove(x, y);

        Eval = -minimax(newBoard, depth - 1, -beta, -alpha, x, y);

        if (Eval >= beta)
        {
            if (depth == maxDepth)
            {
                x = newBoard.validCoord[0][0];
                y = newBoard.validCoord[0][1];
            }
            return beta;
        }

        if (Eval > alpha)
        {
            alpha = Eval;
            if (depth == maxDepth)
            {
                x = newBoard.validCoord[0][0];
                y = newBoard.validCoord[0][1];
            }
        }

        // 3. 再用最小窗口搜索其余节点
        int vSize = newBoard.statusCount[Valid];

        for (int i = 0; i < vSize; i++)
        {
            Board tmpBoard = B;
            tmpBoard.fastmove(B.validCoord[i]);

            Eval = -minimax(tmpBoard, depth - 1, -alpha - MinWindow, -alpha, x, y);

            if (Eval > alpha && Eval < beta) //最小窗口验证失败，重新搜索
                Eval = -minimax(tmpBoard, depth - 1, -beta, -alpha - MinWindow, x, y);

            if (Eval >= beta)
            {
                if (depth == maxDepth)
                {
                    x = B.validCoord[i][0];
                    y = B.validCoord[i][1];
                    PVS = i;
                }
                return beta;
            }
            if (Eval > alpha)
            {
                alpha = Eval;
                if (depth == maxDepth)
                {
                    x = B.validCoord[i][0];
                    y = B.validCoord[i][1];
                    PVS = i;
                }
            }
        }
    }
    return alpha;
}

int find_the_best(int *arr, int currentBotColor)
{
    // 我所执子颜色（1为黑，-1为白，棋盘状态亦同）  在AI中，1为白，2为黑
    Board B(arr, currentBotColor);

    B.show();

    int turnID = B.statusCount[Black] + B.statusCount[White] - 4;
    int validCount = B.statusCount[Valid];

    maxDepth = 9;

    if (turnID > 21)
    {
        BW_FACTOR = 4;
        DC_FACTOR = 30;
        CN_FACTOR = 800;
        SD_FACTOR = 300;
        ST_FACTOR = 300;
        FR_FACTOR = 12;
        MB_FACTOR = 12;
        CR_FACTOR = 0;
    }
    else if (turnID > 14)
    {
        maxDepth = 7;

        BW_FACTOR = 2;
        CN_FACTOR = 950;
        DC_FACTOR = 240;
        SD_FACTOR = 200;
        ST_FACTOR = 0;
        FR_FACTOR = 45;
        MB_FACTOR = 30;
        CR_FACTOR = 9;
    }
    else if (turnID > 8)
    {
        maxDepth = 6;

        BW_FACTOR = 2;
        CN_FACTOR = 900;
        DC_FACTOR = 240;
        SD_FACTOR = 0;
        ST_FACTOR = 0;
        FR_FACTOR = 51 - turnID;
        MB_FACTOR = 36 - turnID;
        CR_FACTOR = 12;
    }
    else
    {
        maxDepth = 5;

        BW_FACTOR = 1;
        CN_FACTOR = 750;
        DC_FACTOR = 195;
        SD_FACTOR = 0;
        ST_FACTOR = 0;
        FR_FACTOR = 64 - turnID;
        MB_FACTOR = 43 - turnID;
        CR_FACTOR = 13;
    }

    int lastResult;
    short last_x, last_y;

    for (; !timeOut() && maxDepth < 25;
            lastResult = result, last_x = best_x, last_y = best_y, maxDepth++)
    {
        result = minimax(B, maxDepth, ALPHA, BETA, best_x, best_y);

        if (abs(result) > BETA - 3000 && abs(lastResult) > BETA - 3000)
            break;
        for (int i = PVS; i > 0; --i)
            swap(B.validCoord[i], B.validCoord[i - 1]);

    }

//    res = minimax(B, maxDepth, ALPHA, BETA, best_x, best_y);

    return best_x * 10 + best_y;
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

int main()
{
    int x, y;
    startTime = time(NULL);

    // 初始化棋盘
    gridInfo[3][4] = gridInfo[4][3] = 1;  //|白|黑|
    gridInfo[3][3] = gridInfo[4][4] = -1; //|黑|白|

    // 读入JSON
    string str;
    getline(cin, str);
    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);

    // 分析自己收到的输入和自己过往的输出，并恢复状态
    int turnID = input["responses"].size();
    currBotColor = input["requests"][(Json::Value::UInt) 0]["x"].asInt() < 0 ? 1 : -1; // 第一回合收到坐标是-1, -1，说明我是黑方
    for (int i = 0; i < turnID; i++)
    {
        // 根据这些输入输出逐渐恢复状态到当前回合
        x = input["requests"][i]["x"].asInt();
        y = input["requests"][i]["y"].asInt();
        if (x >= 0)
            ProcStep(x, y, -currBotColor); // 模拟对方落子
        x = input["responses"][i]["x"].asInt();
        y = input["responses"][i]["y"].asInt();
        if (x >= 0)
            ProcStep(x, y, currBotColor); // 模拟己方落子
    }

    // 看看自己本回合输入
    x = input["requests"][turnID]["x"].asInt();
    y = input["requests"][turnID]["y"].asInt();
    if (x >= 0)
        ProcStep(x, y, -currBotColor); // 模拟对方落子

    // 找出合法落子点
    int possiblePos[64][2], posCount = 0, choice;

    for (y = 0; y < 8; y++)
        for (x = 0; x < 8; x++)
            if (ProcStep(x, y, currBotColor, true))
            {
                possiblePos[posCount][0] = x;
                possiblePos[posCount++][1] = y;
            }

    // 做出决策（你只需修改以下部分）

    int resultX, resultY;
    // cout << "pos count: " << posCount << endl;
    if (posCount > 0)
    {
    //  srand(time(0));
    //  choice = rand() % posCount;
    //  resultX = possiblePos[choice][0];
    //  resultY = possiblePos[choice][1];
    int arr[64] = {0};
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (gridInfo[i][j] == 1) arr[i * 8 + j] = 1;
            else if (gridInfo[i][j] == -1) arr[i * 8 + j] = 0;
            else arr[i * 8 + j] = 2;
    int res = find_the_best(arr, currBotColor);
    // cout << res << endl;
    resultX = res / 10;
    resultY = res % 10;
    }
    else
    {
    resultX = -1;
    resultY = -1;
    }

    // 决策结束，输出结果（你只需修改以上部分）

    Json::Value ret;
    ret["response"]["x"] = resultX;
    ret["response"]["y"] = resultY;
    Json::FastWriter writer;
    cout << writer.write(ret) << endl;
    return 0;
}
