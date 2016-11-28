# -*- coding:utf-8 -*-

# playerType: 0 -> Human VS Human, 1 -> Human VS AI
# difficultLevel: 0 -> easyModeAI, 1 -> mediumModeAI, 2 -> hardModeAI

import logging
from time import sleep

import easyModeAI.calculate
import mediumModeAI.calculate
import hardModeAI.calculate
from graphics import *
from canvas import Button, Canvas

__author__ = 'patrick_psq'

logging.basicConfig(level=logging.INFO)


# 刷新双方棋子信息
def refresh():
    t_white.setText("WHITE:%d" % canvas.piece_num[1])
    t_black.setText("BLACK:%d" % canvas.piece_num[2])

# 一些要用到的常量

restart = False

win = GraphWin("OTHELLO", 800, 600)

t1 = Text(Point(400, 130), "OTHELLO")  # game name
t1.setSize(36)
t2 = Text(Point(400, 230), "Author: patrick_psq")  # author name
t2.setSize(20)
t3 = Text(Point(250, 350), "Human VS Human : ")
t3.setSize(15)
t4 = Text(Point(250, 450), "Human VS AI : ")
t4.setSize(15)

text_row = []
text_column = []
for ch in ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h']:
    text_ch = Text(Point((ord(ch) - ord('a')) * 60 + 90, 40), ch)
    text_ch.setSize(20)
    text_ch.setOutline('dark blue')
    text_row.append(text_ch)
for i in range(1, 9):
    text_num = Text(Point(40, i * 60 + 30), str(i))
    text_num.setSize(20)
    text_num.setOutline('dark blue')
    text_column.append(text_num)

b_start = Button(win, Point(400, 350), 100, 60, "Start")  # start button
b_start_easy = Button(win, Point(400, 450), 100, 60, "Start Easy Mode")
b_start_medium = Button(win, Point(530, 450), 100, 60, "Start Medium Mode")
b_start_hard = Button(win, Point(660, 450), 100, 60, "Start Hard Mode")
b_qt = Button(win, Point(750, 575), 100, 50, "Quit")  # quit button

t_white = Text(Point(700, 200), "WHITE:%d" % 2)  # white count
t_white.setSize(25)
t_black = Text(Point(700, 400), "BLACK:%d" % 2)  # black count
t_black.setSize(25)

b_re = Button(win, Point(700, 50), 190, 90, "Return to Main Menu")
b_undo = Button(win, Point(700, 100), 100, 60, "Undo")


def init_board():

    win.setBackground('white')

    t1.draw(win)
    t2.draw(win)
    t3.draw(win)
    t4.draw(win)

    # b_start.activate()
    # b_start_easy.activate()
    # b_start_medium.activate()
    # b_start_hard.activate()
    b_start.clear()
    b_start.redraw()
    b_start_easy.clear()
    b_start_easy.redraw()
    b_start_medium.clear()
    b_start_medium.redraw()
    b_start_hard.clear()
    b_start_hard.redraw()

    # b_qt.activate()
    b_qt.clear()
    b_qt.redraw()

    b_re.clear()
    b_undo.clear()


if __name__ == '__main__':

    t_skip_white = Text(Point(700, 300), "White SKIP a turn!")
    t_skip_white.setSize(20)
    t_skip_white.setOutline('red')

    t_skip_black = Text(Point(700, 300), "Black SKIP a turn!")
    t_skip_black.setSize(20)
    t_skip_black.setOutline('red')

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

    init_board()

    while 1:
        p1 = win.getMouse()
        if b_qt.clicked(p1):
            break
        elif b_start.clicked(p1) or b_start_easy.clicked(p1) or b_start_medium.clicked(p1) or b_start_hard.clicked(p1):
            if b_start.clicked(p1):
                playerType = 0
            else:
                playerType = 1

            if b_start_easy.clicked(p1):
                difficultLevel = 0
            if b_start_medium.clicked(p1):
                difficultLevel = 1
            if b_start_hard.clicked(p1):
                difficultLevel = 2

            t1.undraw()
            t2.undraw()  # Un-draw game name and author name
            t3.undraw()
            t4.undraw()

            for t in text_row:
                t.draw(win)
            for t in text_column:
                t.draw(win)

            b_start.clear()  # clear start button
            b_start_easy.clear()
            b_start_medium.clear()
            b_start_hard.clear()

            t_white.draw(win)
            t_black.draw(win)
            black_turn.draw(win)
            win.setBackground('light blue')

            canvas = Canvas(win)  # init the canvas
            refresh()

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

            restart = False

            # Game Starts!
            while 1:
                # 黑子是否可以落子
                blackHasValidMove = canvas.check_black()
                blackClicked = False
                if blackHasValidMove:
                    p2 = win.getMouse()

                    blackClicked = False  # 黑子方是否落子
                    if b_qt.clicked(p2):  # 如果点击quit按钮
                        break
                    if b_undo.clicked(p2):  # 如果点击悔棋
                        canvas.undo(undoList, win)
                        refresh()

                    for i in range(8):
                        for j in range(8):
                            if canvas.board[i][j].clicked(p2) and canvas.check(i, j, 2):
                                # update undoList
                                for x in xrange(8):
                                    for y in xrange(8):
                                        undoList[x][y] = canvas.board[x][y].color

                                canvas.click(i, j, 2)
                                logging.info('BLACK places at (%s, %s)' % (i, j))
                                canvas.piece_num[2] += 1  # 更新棋盘上两种颜色的棋子数

                                # canvas.show()

                                # 黑色落子,光标移动,轮到白色
                                black_turn.undraw()
                                white_turn.draw(win)

                                canvas.total_num += 1
                                refresh()
                                blackClicked = True  # 标记为已经落子

                if not blackHasValidMove:
                    # 黑色不能落子
                    t_skip_black.draw(win)
                    sleep(1)
                    t_skip_black.undraw()

                    black_turn.undraw()
                    white_turn.draw(win)
                    # canvas.total_num += 1
                    blackClicked = True

                # 黑色已落子
                if blackClicked:
                    # 如果黑色已经落子，则轮到白色方落子
                    # 如果白色可以落子
                    whiteHasValidMove = canvas.check_white()

                    if whiteHasValidMove:
                        # Human VS Human
                        if playerType == 0:
                            quitFlag = False
                            whiteClicked = False
                            while 1:
                                if whiteClicked:
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
                                            whiteClicked = True

                            if quitFlag:
                                break
                        # Human VS AI
                        elif playerType == 1:
                            # easyMode
                            if difficultLevel == 0:
                                sleep(0.2)
                                # AI--------------------------------------------------
                                # 找出白色的最佳落子坐标
                                # [best_x, best_y] = find_the_best(canvas)
                                a = easyModeAI.calculate.intArray(64)
                                for i in xrange(8):
                                    for j in xrange(8):
                                        a[i * 8 + j] = canvas.board[i][j].color
                                best_place = easyModeAI.calculate.find_the_best(a)

                            # mediumMode
                            elif difficultLevel == 2:
                                sleep(0.2)
                                a = mediumModeAI.calculate.intArray(64)
                                for i in xrange(8):
                                    for j in xrange(8):
                                        a[i * 8 + j] = canvas.board[i][j].color
                                best_place = mediumModeAI.calculate.find_the_best(a)

                            # hardMode
                            elif difficultLevel == 1:
                                # canvas.show()
                                a = hardModeAI.calculate.intArray(64)
                                for i in xrange(8):
                                    for j in xrange(8):
                                        if canvas.board[i][j].color == 1:  # white
                                            a[i * 8 + j] = 0
                                        elif canvas.board[i][j].color == 2:  # black
                                            a[i * 8 + j] = 1
                                        else:
                                            a[i * 8 + j] = -1
                                best_place = hardModeAI.calculate.find_the_best(a, 0)
                                # print "best place: ", best_place

                            best_x = best_place / 10
                            best_y = best_place % 10
                            logging.info('WHITE places at (%s, %s)' % (best_x, best_y))

                            canvas.click(best_x, best_y, 1)

                            canvas.piece_num[1] += 1
                            canvas.total_num += 1

                            # canvas.show()
                            refresh()
                    else:  # white doesn't have valid move
                        t_skip_white.draw(win)
                        sleep(1)
                        t_skip_white.undraw()

                    white_turn.undraw()
                    black_turn.draw(win)

                # check whether someone wins
                who_win = 0
                game_over = False

                if canvas.piece_num[1] + canvas.piece_num[2] < 64:
                    # 白色和黑色是否可以落子
                    whiteHasValidMove = canvas.check_white()
                    blackHasValidMove = canvas.check_black()

                    if (not whiteHasValidMove) and (not blackHasValidMove):
                        b_undo.clear()
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
                    b_undo.clear()
                    b_re.redraw()
                    black_win.draw(win)
                    game_over = True

                elif canvas.piece_num[2] == 0:
                    # 无黑子， 白色获胜
                    who_win = 1
                    b_undo.clear()
                    b_re.redraw()
                    white_win.draw(win)
                    game_over = True

                elif canvas.piece_num[1] + canvas.piece_num[2] == 64:
                    # 棋盘下满
                    b_undo.clear()
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
                            except Exception:
                                pass
                            try:
                                black_turn.undraw()
                            except Exception:
                                pass

                            # black_turn.draw(win)
                            # canvas.restart(win)
                            # refresh()

                            t_black.undraw()
                            t_white.undraw()

                            for item in text_column:
                                item.undraw()
                            for item in text_row:
                                item.undraw()

                            b_re.clear()
                            # b_undo.redraw()

                            if who_win == 0:
                                tie.undraw()
                            elif who_win == 1:
                                white_win.undraw()
                            else:
                                black_win.undraw()

                            # logging.info('A new game starts!')

                            # undoList = [[0, 0, 0, 0, 0, 0, 0, 0],
                            #             [0, 0, 0, 0, 0, 0, 0, 0],
                            #             [0, 0, 0, 0, 0, 0, 0, 0],
                            #             [0, 0, 0, 1, 2, 0, 0, 0],
                            #             [0, 0, 0, 2, 1, 0, 0, 0],
                            #             [0, 0, 0, 0, 0, 0, 0, 0],
                            #             [0, 0, 0, 0, 0, 0, 0, 0],
                            #             [0, 0, 0, 0, 0, 0, 0, 0]
                            #             ]
                            init_board()
                            restart = True
                            break

                        elif b_qt.clicked(p3):
                            break
                try:
                    if restart:
                        break
                except:
                    pass

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
