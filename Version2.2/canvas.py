# -*- coding:utf-8 -*-
from graphics import *

__author__ = 'patrick_psq'


class Button:
    def __init__(self, win1, center, width, height, label):
        w, h = width / 2.0, height / 2.0
        x, y = center.getX(), center.getY()
        self.xmax, self.xmin = x+w, x-w
        self.ymax, self.ymin = y+h, y-h
        p1 = Point(self.xmin, self.ymin)  # left bottom point
        p2 = Point(self.xmax, self.ymax)  # right top point
        self.rect = Rectangle(p1, p2)  # Button 矩形框
        self.rect.setFill('lightgray')
        self.rect.draw(win1)  # 画矩形框
        self.label = Text(center, label)  # 设置标签的文字内容
        self.label.draw(win1)  # 画文本内容
        self.active = False
        self.deactivate()
        self.win = win1

    def activate(self):
        self.label.setFill('black')
        self.active = True

    def deactivate(self):
        self.label.setFill('white')
        self.active = False

    def clicked(self, p):
        return self.active and self.xmin <= p.getX() <= self.xmax and self.ymin <= p.getY() <= self.ymax

    def clear(self):
        self.rect.undraw()
        self.label.undraw()
        self.deactivate()

    def redraw(self):
        self.rect.draw(self.win)
        self.label.draw(self.win)
        self.activate()


class Cell:
    def __init__(self, color, x, y, win):
        self.win = win
        self.x = x
        self.y = y
        self.color = color
        self.rect = Rectangle(Point(self.x * 60 + 60, self.y * 60 + 60), Point(self.x * 60 + 120, self.y * 60 + 120))
        self.rect.draw(win)
        self.circle = Circle(Point(self.x * 60 + 60 + 30, self.y * 60 + 60 + 30), 27)
        self.draw_cir(self.color)

    # 画颜色为color的棋子
    def draw_cir(self, color):
        self.color = color
        # color = 1 WHITE
        # color = 2 BLACK
        if self.color == 1:
            self.circle.setFill('white')
            self.circle.draw(self.win)
        elif self.color == 2:
            self.circle.setFill('black')
            self.circle.draw(self.win)

    def clicked(self, p):
        return self.color == 0 and self.x * 60 + 60 < p.getX() < self.x * 60 + 120 and \
               self.y * 60 + 60 < p.getY() < self.y * 60 + 120

    # 翻转棋子的颜色
    def change(self):
        self.circle.undraw()
        self.color = 3 - self.color
        self.draw_cir(self.color)

    def clear(self):
        self.rect.undraw()
        self.circle.undraw()


class Canvas:
    def __init__(self, win):
        self.board = []
        for i in range(8):
            line = []
            for j in range(8):
                if (i == 3 and j == 3) or (i == 4 and j == 4):
                    line.append(Cell(1, i, j, win))
                elif (i == 3 and j == 4) or (i == 4 and j == 3):
                    line.append(Cell(2, i, j, win))
                else:
                    line.append(Cell(0, i, j, win))
            self.board.append(line)
        # self.board[3][3] = Cell(1, 3, 3, win)
        # self.board[4][4] = Cell(1, 4, 4, win)
        # self.board[3][4] = Cell(2, 3, 4, win)
        # self.board[4][3] = Cell(2, 4, 3, win)
        self.piece_num = [0, 2, 2]  # 各种颜色棋子的数量
        self.total_num = 4  # 总棋子数量
        self.fx = [[1, 1], [1, 0], [1, -1], [0, -1], [-1, -1], [-1, 0], [-1, 1], [0, 1]]

    # 查询x, y位置是否可以落颜色为color的子
    def check(self, x, y, color):
        self.board[x][y].color = color
        flag = False
        for i in range(8):
            temp = 0
            xx = x + self.fx[i][0]
            yy = y + self.fx[i][1]
            while 0 <= xx < 8 and 0 <= yy < 8 and self.board[xx][yy].color == 3 - color:
                xx += self.fx[i][0]
                yy += self.fx[i][1]
                temp += 1
            if 0 <= xx < 8 and 0 <= yy < 8 and self.board[xx][yy].color == color and temp > 0:
                flag = True
                break
        self.board[x][y].color = 0
        return flag

    # 查询黑色是否可以落子
    def check_black(self):
        for i in range(8):
            for j in range(8):
                if self.board[i][j].color == 0 and self.check(i, j, 2):
                    return True
        return False

    # 查询白色是否可以落子
    def check_white(self):
        for i in range(8):
            for j in range(8):
                if self.board[i][j].color == 0 and self.check(i, j, 1):
                    return True
        return False

    # 在坐标（x, y）处落颜色为color的子
    def click(self, x, y, color):
        self.board[x][y].color = color
        self.board[x][y].draw_cir(color)
        # 需要翻转棋子的方向列表
        kz_fx=[]
        for i in range(8):  # 查询8个方向
            xx = x + self.fx[i][0]
            yy = y + self.fx[i][1]
            flag = False  # 该方向是否会有翻转发生
            while 0 <= xx < 8 and 0 <= yy < 8 and self.board[xx][yy].color == 3 - color:
                flag = True
                xx += self.fx[i][0]
                yy += self.fx[i][1]
            if 0 <= xx < 8 and 0 <= yy < 8 and self.board[xx][yy].color == color and flag:
                kz_fx.append(i)
        if kz_fx != []:
            for i in kz_fx:
                xx = x + self.fx[i][0]
                yy = y + self.fx[i][1]
                while not (self.board[xx][yy].color == color):
                    self.board[xx][yy].change()
                    self.piece_num[color] += 1
                    self.piece_num[3 - color] -= 1
                    xx += self.fx[i][0]
                    yy += self.fx[i][1]

    # 落子但不作图
    def imaginary_click(self, x, y, color):
        self.board[x][y].color = color

        # 需要翻转棋子的方向列表
        kz_fx=[]
        for i in range(8):  # 查询8个方向
            xx = x + self.fx[i][0]
            yy = y + self.fx[i][1]
            flag = False  # 该方向是否可以落子
            while 0 <= xx < 8 and 0 <= yy < 8 and self.board[xx][yy].color == 3 - color :
                flag = True
                xx += self.fx[i][0]
                yy += self.fx[i][1]
            if 0 <= xx < 8 and 0 <= yy < 8 and self.board[xx][yy].color == color and flag:
                kz_fx.append(i)
        if kz_fx != []:
            for i in kz_fx:
                xx = x + self.fx[i][0]
                yy = y + self.fx[i][1]
                while not (self.board[xx][yy].color == color):
                    self.board[xx][yy].color = 3 - self.board[xx][yy].color
                    self.piece_num[color] += 1
                    self.piece_num[3 - color] -= 1
                    xx += self.fx[i][0]
                    yy += self.fx[i][1]

    def restart(self, win):
        self.piece_num = [0, 2, 2]  # 各种颜色棋子的数量
        self.total_num = 4  # 总棋子数量
        for i in range(8):
            for j in range(8):
                self.board[i][j].clear()
        self.board = []
        for i in range(8):
            line = []
            for j in range(8):
                line.append(Cell(0, i, j, win))
            self.board.append(line)
        self.board[3][3] = Cell(1, 3, 3, win)
        self.board[4][4] = Cell(1, 4, 4, win)
        self.board[3][4] = Cell(2, 3, 4, win)
        self.board[4][3] = Cell(2, 4, 3, win)

    def undo(self, undoList, win):
        self.piece_num = [0, 0, 0]
        self.total_num = 0
        for i in xrange(8):
            for j in xrange(8):
                if self.board[i][j].color != undoList[i][j]:
                    self.board[i][j].clear()
                    self.board[i][j] = Cell(undoList[i][j], i, j, win)
                if undoList[i][j] == 1 or undoList[i][j] == 2:
                    self.total_num += 1
                    self.piece_num[undoList[i][j]] += 1

    def show(self):
        for i in xrange(8):
            for j in xrange(8):
                print self.board[j][i].color,
            print
        print "White: ", self.piece_num[1]
        print "Black: ", self.piece_num[2]
