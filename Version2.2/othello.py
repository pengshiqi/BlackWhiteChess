#!/usr/bin/env python
# -*- coding:utf-8 -*-

# playerType: 0 -> Human VS Human, 1 -> Human VS AI
# difficultLevel: 0 -> easyModeAI, 1 -> hardModeAI

import copy
import logging
from time import sleep

import easyModeAI.calculate
import hardModeAI.calculate
from graphics import *
from canvas import Button, Cell, Canvas

__author__ = 'patrick_psq'

logging.basicConfig(level=logging.INFO)

max_num = 99999999999
max_depth = 3


# 复制canvas对象
def duplicate(c):
    new_c = copy.copy(c)
    new_c.board = copy.copy(c.board)
    for i in range(8):
        new_c.board[i] = copy.copy(c.board[i])
        for j in range(8):
            new_c.board[i][j] = copy.copy(c.board[i][j])
    new_c.piece_num = copy.copy(c.piece_num)
    return new_c


def find_the_best(canvas):
    best_x = best_y = -1
    min_value = max_num
    for i in range(8):
        for j in range(8):
            if canvas.board[i][j].color == 0 and canvas.check(i, j, 1):
                new_canvas = duplicate(canvas)
                new_canvas.imaginary_click(i, j, 1)
                # debug(new_canvas)
                value = minimax(new_canvas, max_depth, -max_num, max_num, True)
                if value < min_value:
                    min_value = value
                    best_x = i
                    best_y = j
    return [best_x, best_y]


def minimax(can, depth, alpha, beta, MaxmizingPlayer):
    if MaxmizingPlayer:
        is_terminal = not can.check_black()
    else:
        is_terminal = not can.check_white()
    if depth == 0 or is_terminal:
        # debug(can)
        return can.evaluate()

    if MaxmizingPlayer:
        bestValue = -max_num
        for i in range(8):
            for j in range(8):
                if can.board[i][j].color == 0 and can.check(i, j, 2):
                    new_canvas = duplicate(can)
                    new_canvas.imaginary_click(i, j, 2)
                    # debug(new_canvas)
                    v = minimax(new_canvas, depth - 1, alpha, beta, False)
                    bestValue = max(bestValue, v)
                    alpha = max(alpha, bestValue)
                    if beta <= alpha:
                        break
        return bestValue
    else:
        bestValue = max_num
        for i in range(8):
            for j in range(8):
                if can.board[i][j].color == 0 and can.check(i, j, 1):
                    new_canvas = duplicate(can)
                    new_canvas.imaginary_click(i, j, 1)
                    # debug(new_canvas)
                    v = minimax(new_canvas, depth - 1, alpha, beta, True)
                    bestValue = min(bestValue, v)
                    beta = min(beta, bestValue)
                    if beta <= alpha:
                        break
        return bestValue


# 刷新双方棋子信息
def refresh():
    t_white.setText("WHITE:%d" % canvas.piece_num[1])
    t_black.setText("BLACK:%d" % canvas.piece_num[2])


if __name__ == '__main__':

    win = GraphWin("OTHELLO", 800, 600)

    win.setBackground('white')

    t1 = Text(Point(400, 130), "OTHELLO")  # game name
    t1.setSize(36)
    t1.draw(win)
    t2 = Text(Point(400, 230), "Author: patrick_psq")  # author name
    t2.setSize(20)
    t2.draw(win)

    t3 = Text(Point(300, 350), "Human VS Human : ")
    t3.setSize(15)
    t3.draw(win)

    t4 = Text(Point(300, 450), "Human VS AI : ")
    t4.setSize(15)
    t4.draw(win)

    b_start = Button(win, Point(450, 350), 100, 60, "Start")  # start button
    b_start.activate()

    b_start_easy = Button(win, Point(450, 450), 100, 60, "Start Easy Mode")
    b_start_easy.activate()
    b_start_hard = Button(win, Point(580, 450), 100, 60, "Start Hard Mode")
    b_start_hard.activate()

    b_qt = Button(win, Point(750, 575), 100, 50, "Quit")  # quit button
    b_qt.activate()

    t_white = Text(Point(700, 200), "WHITE:%d" % 2)  # white count
    t_white.setSize(25)
    t_black = Text(Point(700, 400), "BLACK:%d" % 2)  # black count
    t_black.setSize(25)

    white_win = Text(Point(700, 300), "WHITE WIN!")
    white_win.setSize(30)
    white_win.setOutline('red')
    black_win = Text(Point(700, 300), "BLACK WIN!")
    black_win.setSize(30)
    black_win.setOutline('red')
    tie = Text(Point(700, 300), "TIE!")
    tie.setSize(30)
    tie.setOutline('red')

    white_turn = Circle(Point(600, 200), 10)
    white_turn.setFill('white')
    black_turn = Circle(Point(600, 400), 10)
    black_turn.setFill('black')

    b_re = Button(win, Point(700, 50), 190, 90, "Restart")
    b_re.clear()
    b_undo = Button(win, Point(700, 300), 100, 60, "Undo")
    b_undo.clear()

    while 1:
        p1 = win.getMouse()
        if b_qt.clicked(p1):
            break
        elif b_start.clicked(p1) or b_start_easy.clicked(p1) or b_start_hard.clicked(p1):
            if b_start.clicked(p1):
                playerType = 0
            else:
                playerType = 1

            if b_start_easy.clicked(p1):
                difficultLevel = 0
            if b_start_hard.clicked(p1):
                difficultLevel = 1

            t1.undraw()
            t2.undraw()  # Un-draw game name and author name
            t3.undraw()
            t4.undraw()

            b_start.clear()  # clear start button
            b_start_easy.clear()
            b_start_hard.clear()

            t_white.draw(win)
            t_black.draw(win)
            black_turn.draw(win)
            win.setBackground('light blue')

            canvas = Canvas(win)  # init the canvas

            logging.info('A new game starts!')

            b_undo.redraw()
            undoList = [[0, 0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 1, 2, 0, 0, 0],
                        [0, 0, 0, 2, 1, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0, 0]
                        ]
            lastMove = [[0, 0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 1, 2, 0, 0, 0],
                        [0, 0, 0, 2, 1, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0, 0]
                        ]

            while 1:
                # 黑子是否可以落子
                whether_black = canvas.check_black()
                whether_clicked = False
                if whether_black:
                    p2 = win.getMouse()

                    whether_clicked = False  # 黑子方是否落子
                    if b_qt.clicked(p2):  # 如果点击quit按钮
                        break
                    if b_undo.clicked(p2):  # 如果点击悔棋
                        canvas.undo(undoList, win)
                        refresh()
                        continue

                    for i in range(8):
                        for j in range(8):
                            if canvas.board[i][j].clicked(p2) and canvas.check(i, j, 2):
                                canvas.click(i, j, 2)
                                logging.info('BLACK places at (%s, %s)' % (i, j))
                                canvas.piece_num[2] += 1  # 更新棋盘上两种颜色的棋子数

                                # 黑色落子,光标移动,轮到白色
                                black_turn.undraw()
                                white_turn.draw(win)

                                canvas.total_num += 1
                                refresh()
                                whether_clicked = True  # 标记为已经落子

                if not whether_black:
                    # 黑色不能落子
                    black_turn.undraw()
                    white_turn.draw(win)
                    canvas.total_num += 1
                    whether_clicked = True

                if whether_clicked:
                    # 如果黑色已经落子，则轮到白色方落子
                    # 如果白色可以落子
                    whether_white = canvas.check_white()

                    if whether_white:

                        # Human VS Human
                        if playerType == 0:
                            quitFlag = False
                            white_clicked = False
                            while 1:
                                if white_clicked:
                                    break

                                p4 = win.getMouse()

                                if b_qt.clicked(p4):  # 如果点击quit按钮
                                    quitFlag = True
                                    break

                                for i in range(8):
                                    for j in range(8):
                                        if canvas.board[i][j].clicked(p4) and canvas.check(i, j, 1):
                                            canvas.click(i, j, 1)
                                            logging.info('WHITE places at (%s, %s)' % (i, j))
                                            canvas.piece_num[1] += 1
                                            canvas.total_num += 1
                                            refresh()
                                            white_clicked = True

                            if quitFlag:
                                break
                        # Human VS AI
                        elif playerType == 1:
                            # easyMode
                            if difficultLevel == 0:
                                sleep(0.2)
                                a = easyModeAI.calculate.intArray(64)
                                for i in xrange(8):
                                    for j in xrange(8):
                                        a[i * 8 + j] = canvas.board[i][j].color
                                best_place = easyModeAI.calculate.find_the_best(a)

                            # hardMode
                            elif difficultLevel == 1:
                                sleep(0.2)
                                # AI--------------------------------------------------
                                # 找出白色的最佳落子坐标
                                # [best_x, best_y] = find_the_best(canvas)
                                a = hardModeAI.calculate.intArray(64)
                                for i in xrange(8):
                                    for j in xrange(8):
                                        a[i * 8 + j] = canvas.board[i][j].color
                                best_place = hardModeAI.calculate.find_the_best(a)

                            best_x = best_place / 10
                            best_y = best_place % 10
                            logging.info('WHITE places at (%s, %s)' % (best_x, best_y))

                            canvas.click(best_x, best_y, 1)
                            canvas.piece_num[1] += 1
                            canvas.total_num += 1
                            refresh()

                    white_turn.undraw()
                    black_turn.draw(win)

                    if canvas.check_black():
                        # 更新undoList
                        for i in xrange(8):
                            for j in xrange(8):
                                undoList[i][j] = lastMove[i][j]
                        for i in xrange(8):
                            for j in xrange(8):
                                lastMove[i][j] = canvas.board[i][j].color

                # check whether someone wins
                who_win = 0
                game_over = False

                if canvas.piece_num[1] + canvas.piece_num[2] < 64:
                    # 白色和黑色是否可以落子
                    whether_white = canvas.check_white()
                    whether_black = canvas.check_black()

                    if (not whether_white) and (not whether_black):
                        b_re.redraw()  # 显示restart按钮
                        if canvas.piece_num[1] > canvas.piece_num[2]:
                            who_win = 1  # 白色获胜
                            white_win.draw(win)
                        elif canvas.piece_num[1] < canvas.piece_num[2]:
                            who_win = 2  # 黑色获胜
                            black_win.draw(win)
                        else:
                            who_win = 0  # 平局
                            tie.draw(win)
                        game_over = True

                if canvas.piece_num[1] == 0:
                    # 无白子， 黑色获胜
                    who_win = 2
                    b_re.redraw()
                    black_win.draw(win)
                    game_over = True

                elif canvas.piece_num[2] == 0:
                    # 无黑子， 白色获胜
                    who_win = 1
                    b_re.redraw()
                    white_win.draw(win)
                    game_over = True

                elif canvas.piece_num[1] + canvas.piece_num[2] == 64:
                    # 棋盘下满
                    b_re.redraw()
                    if canvas.piece_num[1] > canvas.piece_num[2]:
                        who_win = 1
                        white_win.draw(win)
                    elif canvas.piece_num[1] < canvas.piece_num[2]:
                        who_win = 2
                        black_win.draw(win)
                    else:
                        who_win = 0
                        tie.draw(win)
                    game_over = True

                if game_over:
                    while 1:
                        p3 = win.getMouse()

                        if b_re.clicked(p3):
                            for i in canvas.board:
                                 for j in i:
                                    j.clear()
                            try:
                                white_turn.undraw()
                            except:
                                pass
                            try:
                                black_turn.undraw()
                            except:
                                pass

                            black_turn.draw(win)
                            canvas.restart(win)
                            refresh()

                            b_re.clear()

                            if who_win == 0:
                                tie.undraw()
                            elif who_win == 1:
                                white_win.undraw()
                            else:
                                black_win.undraw()

                            logging.info('A new game starts!')

                            undoList = [[0, 0, 0, 0, 0, 0, 0, 0],
                                         [0, 0, 0, 0, 0, 0, 0, 0],
                                         [0, 0, 0, 0, 0, 0, 0, 0],
                                         [0, 0, 0, 1, 2, 0, 0, 0],
                                         [0, 0, 0, 2, 1, 0, 0, 0],
                                         [0, 0, 0, 0, 0, 0, 0, 0],
                                         [0, 0, 0, 0, 0, 0, 0, 0],
                                         [0, 0, 0, 0, 0, 0, 0, 0]
                                         ]
                            break

                        elif b_qt.clicked(p3):
                            break
                try:
                    if b_qt.clicked(p3):
                        break
                except:
                    pass
            if b_qt.clicked(p2):
                break
            try:
                if b_qt.clicked(p3):
                    break
            except:
                pass

    win.close()
