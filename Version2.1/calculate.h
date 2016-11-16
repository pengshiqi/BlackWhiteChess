#ifndef MINIMAX_H
#define MINIMAX_H

using namespace std;

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

inline void swap(int *const a, int *const b);

inline bool timeOut();

#endif
