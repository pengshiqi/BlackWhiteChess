# -*- coding:utf-8 -*-

__author__ = 'patrick_psq'


from graphics import *
from time import sleep
global num
global cells
global fx
global white_turn
global black_turn
global kz_fx
global step


class Button:
    def __init__(self,win1,center,width,height,label):
        w,h=width/2.0,height/2.0
        x,y=center.getX(),center.getY()
        self.xmax,self.xmin=x+w,x-w
        self.ymax,self.ymin=y+h,y-h
        p1=Point(self.xmin,self.ymin)
        p2=Point(self.xmax,self.ymax)
        self.rect=Rectangle(p1,p2)
        self.rect.setFill('lightgray')
        self.rect.draw(win1)
        self.label=Text(center,label)
        self.label.draw(win1)
        self.deactivate()
        self.win=win1
    def activate(self):
        self.label.setFill('black')
        self.active=True
    def deactivate(self):
        self.label.setFill('white')
        self.active=False
    def clicked(self,p):
        return self.active and self.xmin<=p.getX()<=self.xmax and self.ymin<=p.getY()<=self.ymax
    def clear(self):
        self.rect.undraw()
        self.label.undraw()
        self.deactivate()
    def redraw(self):
        self.rect.draw(self.win)
        try:self.label.draw(self.win)
        except:pass
        self.activate()

class Cell:
    def __init__(self,color,x,y,win):
        self.win=win
        self.x = x
        self.y = y
        self.color=color
        self.rect=Rectangle(Point((self.x)*60+60,(self.y)*60+60),Point(self.x*60+120,self.y*60+120))
        self.rect.draw(win)
        self.circle=Circle(Point((self.x)*60+60+30,(self.y)*60+60+30),27)
        self.draw_cir(self.color)
    def draw_cir(self,color):
        self.color=color
        if self.color==1:
            self.circle.setFill('white')
            self.circle.draw(win)
        elif self.color==2:
            self.circle.setFill('black')
            self.circle.draw(win)
    def clicked(self,p):
        return self.color==0 and (self.x)*60+60<p.getX()<self.x*60+120 and (self.y)*60+60<p.getY()<self.y*60+120
    def change(self):
        global num
        if self.color==2:
            self.circle.undraw()
            self.color=1
            self.draw_cir(1)
        elif self.color==1:
            self.circle.undraw()
            self.color=2
            self.draw_cir(2)
    def check(self,color):
        global fx
        self.color=color
        flag=False
        for i in range(8):
            temp=0
            xx=self.x+fx[i][0]
            yy=self.y+fx[i][1]
            while 0<=xx<8 and 0<=yy<8 and cells[xx][yy].color==3-self.color:
                xx=xx+fx[i][0]
                yy=yy+fx[i][1]
                temp+=1
            if 0<=xx<8 and 0<=yy<8 and cells[xx][yy].color==self.color and temp>0:
                flag=True
                break
        self.color=0
        return flag
    def click(self,color):
        global fx
        global cells
        global num
        self.color=color
        self.draw_cir(self.color)
        kz_fx=[]
        for i in range(8):
            xx=self.x+fx[i][0]
            yy=self.y+fx[i][1]
            flag=False
            while  0<=xx<8 and 0<=yy<8 and cells[xx][yy].color==3-self.color :
                flag=True
                xx=xx+fx[i][0]
                yy=yy+fx[i][1]
            if 0<=xx<8 and 0<=yy<8 and cells[xx][yy].color==self.color and flag:
                kz_fx.append(i)
        if kz_fx!=[]:
            for i in kz_fx:
                xx=self.x+fx[i][0]
                yy=self.y+fx[i][1]
                while not (cells[xx][yy].color==self.color):
                    cells[xx][yy].change()
                    num[self.color]+=1
                    num[3-self.color]-=1
                    xx=xx+fx[i][0]
                    yy=yy+fx[i][1]
    def imaginary_change(self):
        if self.color==2:
            self.color=1
        elif self.color==1:
            self.color=2
    def imaginary_click(self):
        global fx
        global cells
        global kz_fx
        global step
        kz_fx=[]
        step=[0,0,0,0,0,0,0,0]
        for i in range(8):
            xx=self.x+fx[i][0]
            yy=self.y+fx[i][1]
            flag=False
            temp_step=0
            while  0<=xx<8 and 0<=yy<8 and cells[xx][yy].color==3-self.color :
                flag=True
                temp_step+=1
                xx=xx+fx[i][0]
                yy=yy+fx[i][1]
            if 0<=xx<8 and 0<=yy<8 and cells[xx][yy].color==self.color and flag:
                kz_fx.append(i)
                step[i]=temp_step
        if kz_fx!=[]:
            for i in kz_fx:
                xx=self.x+fx[i][0]
                yy=self.y+fx[i][1]
                while not (cells[xx][yy].color==self.color):
                    cells[xx][yy].imaginary_change()
                    xx=xx+fx[i][0]
                    yy=yy+fx[i][1]
    def recall(self):
        global fx
        global cells
        global kz_fx
        global step
        for i in kz_fx:
            xx=self.x+fx[i][0]
            yy=self.y+fx[i][1]
            while step[i]>0:
                cells[xx][yy].imaginary_change()
                xx=xx+fx[i][0]
                yy=yy+fx[i][1]
                step[i]-=1
    def find(self,color):
        global fx
        global cells
        global num
        self.color=color
        kz_fx=[]
        for i in range(8):
            xx=self.x+fx[i][0]
            yy=self.y+fx[i][1]
            flag=False
            while  0<=xx<8 and 0<=yy<8 and cells[xx][yy].color==3-self.color :
                flag=True
                xx=xx+fx[i][0]
                yy=yy+fx[i][1]
            if 0<=xx<8 and 0<=yy<8 and cells[xx][yy].color==color and flag:
                kz_fx.append(i)
        temp=0
        for i in kz_fx:
            xx=self.x+fx[i][0]
            yy=self.y+fx[i][1]
            while not (cells[xx][yy].color==self.color):
                temp+=1
                xx=xx+fx[i][0]
                yy=yy+fx[i][1]
        self.color=0
        return temp
    def clear(self):
        self.rect.undraw()
        self.circle.undraw()
#-------------------------------main-------------------------------------
cells=[]
steps=4
num=[0,2,2]
fx=[[1,1],[1,0],[1,-1],[0,-1],[-1,-1],[-1,0],[-1,1],[0,1]]
def refresh():
    t_white.setText("WHITE:%d"%num[1])
    t_black.setText("BLACK:%d"%num[2])

win=GraphWin("BLACK WHITE CHESS",800,600)

win.setBackground('white')

t1=Text(Point(400,200),"BLACK WHITE CHESS")
t1.setSize(35)
t1.draw(win)
b_start=Button(win,Point(400,400),140,80,"Start")
b_start.activate()
b_qt=Button(win,Point(750,575),100,50,"Quit")
b_qt.activate()


t_white=Text(Point(700,200),"WHITE:%d"%num[1])
t_white.setSize(25)
t_black=Text(Point(700,400),"BLACK:%d"%num[2])
t_black.setSize(25)

white_win=Text(Point(700,300),"WHITE WIN!")
white_win.setSize(30)
white_win.setOutline('red')
black_win=Text(Point(700,300),"BLACK WIN!")
black_win.setSize(30)
black_win.setOutline('red')
tie=Text(Point(700,300),"TIE!")
tie.setSize(30)
tie.setOutline('red')



white_turn=Circle(Point(600,200),10)
white_turn.setFill('white')
black_turn=Circle(Point(600,400),10)
black_turn.setFill('black')

b_re=Button(win,Point(700,50),190,90,"Restart")
b_re.clear()

def test():
    global cells
    global num
    for j in range(8):
        for i in range(8):
            print cells[i][j].color,
        print
    print num
    print
while 1:
    try:p1 = win.getMouse()
    except:break
    if b_qt.clicked(p1):break
    elif b_start.clicked(p1):
        t1.undraw()
        t_white.draw(win)
        t_black.draw(win)
        black_turn.draw(win)
        win.setBackground('light blue')
        b_start.clear()
        for i in range(8):
                line=[]
                for j in range(8):
                    if ((i==3) and (j==3)): line.append(Cell(2,i,j,win))
                    elif ((i==4) and (j==4)): line.append(Cell(2,i,j,win))
                    elif ((i==3) and (j==4)): line.append(Cell(1,i,j,win))
                    elif ((i==4) and (j==3)): line.append(Cell(1,i,j,win))
                    else :line.append(Cell(0,i,j,win))
                cells.append(line)
        while 1:
            whether_black=False
            for lst in cells:
                for cell in lst:
                    if cell.color==0 and cell.check(2):
                        whether_black=True
                        break
            if whether_black:
                try:p2 = win.getMouse()
                except:break
                whether_clicked=False
                if b_qt.clicked(p2):break
                for i in range(8):
                    for j in range(8):
                        if (cells[i][j].clicked(p2) and cells[i][j].check(1+(steps+1)%2)):
                            cells[i][j].click(1+(steps+1)%2)
                            num[1+(steps+1)%2]+=1
                            if 1+(steps+1)%2==1:
                                white_turn.undraw()
                                black_turn.draw(win)
                            elif 1+(steps+1)%2==2:
                                black_turn.undraw()
                                white_turn.draw(win)
                            steps+=1
                            refresh()
                            whether_clicked=True
            if not whether_black:
                black_turn.undraw()
                white_turn.draw(win)
                steps+=1
                whether_clicked=True


            if whether_clicked:
                sleep(0.8)
                #AI--------------------------------------------------
                max_change=0
                max_x=-1
                max_y=-1
                #------don't let the black occupy the corner-------
                whether_placed=True
                for i in range(8):
                    for j in range(8):
                        if cells[i][j].color==0 and cells[i][j].check(1):
                            if cells[i][j].find(1)>max_change:
                                '''print 'before imaginary'
                                test()'''
                                cells[i][j].color=1
                                cells[i][j].imaginary_click()
                                '''print 'after imaginary'
                                test()'''
                                if not ((cells[0][0].color==0 and cells[0][0].check(2))):
                                    max_change=cells[i][j].find(1)
                                    cells[i][j].color=1
                                    max_x=i
                                    max_y=j
                                    whether_placed=False
                                elif not (cells[7][0].color==0 and cells[7][0].check(2)):
                                    max_change=cells[i][j].find(1)
                                    cells[i][j].color=1
                                    max_x=i
                                    max_y=j
                                    whether_placed=False
                                elif not (cells[0][7].color==0 and cells[0][7].check(2)):
                                    max_change=cells[i][j].find(1)
                                    cells[i][j].color=1
                                    max_x=i
                                    max_y=j
                                    whether_placed=False
                                elif not (cells[7][7].color==0 and cells[7][7].check(2)):
                                    max_change=cells[i][j].find(1)
                                    cells[i][j].color=1
                                    max_x=i
                                    max_y=j
                                    whether_placed=False
                                cells[i][j].recall()
                                cells[i][j].color=0
                                '''print 'after recall'
                                test()'''
                #----------------

                if max_change==0:
                    for i in range(8):
                        for j in range(8):
                            if cells[i][j].color==0 and cells[i][j].check(1):
                                if cells[i][j].find(1)>max_change:
                                    max_change=cells[i][j].find(1)
                                    max_x=i
                                    max_y=j


                if whether_placed:
                    for i in range(8):
                        if cells[0][i].color==0 and cells[0][i].check(1):
                            max_change=cells[0][i].find(1)
                            max_x=0
                            max_y=i
                        elif cells[7][i].color==0 and cells[7][i].check(1):
                            max_change=cells[7][i].find(1)
                            max_x=7
                            max_y=i
                        elif cells[i][0].color==0 and cells[i][0].check(1):
                            max_change=cells[i][0].find(1)
                            max_x=i
                            max_y=0
                        elif cells[i][7].color==0 and cells[i][7].check(1):
                            max_change=cells[i][7].find(1)
                            max_x=i
                            max_y=7

                #--------check whether can occupy the corner----------
                if cells[0][0].color==0 and cells[0][0].check(1):
                    max_change=cells[0][0].find(1)
                    max_x=0
                    max_y=0
                elif  cells[0][7].color==0 and cells[0][7].check(1):
                    max_change=cells[0][7].find(1)
                    max_x=0
                    max_y=7
                elif  cells[7][0].color==0 and cells[7][0].check(1):
                    max_change=cells[7][0].find(1)
                    max_x=7
                    max_y=0
                elif  cells[7][7].color==0 and cells[7][7].check(1):
                    max_change=cells[7][7].find(1)
                    max_x=7
                    max_y=7

                if max_change>0:
                    cells[max_x][max_y].click(1)
                    num[1]+=1
                    steps+=1
                    white_turn.undraw()
                    black_turn.draw(win)
                    refresh()



                #test()
            #----------------------------------------------------
            #check whether someone wins
            who_win=0
            game_over=False

            if num[1]+num[2]<64:
                whether_white=False
                whether_black=False
                for lst in cells:
                    for cell in lst:
                        if cell.color==0 and cell.check(1):
                            whether_white=True
                        if cell.color==0 and cell.check(2):
                            whether_black=True
                if (not whether_white) and (not whether_black):
                    b_re.redraw()
                    if num[1]>num[2]:
                        who_win=1
                        white_win.draw(win)
                    elif num[1]<num[2]:
                        who_win=2
                        black_win.draw(win)
                    else:
                        who_win=0
                        tie.draw(win)
                    game_over=True
                elif (not whether_white) or (not whether_black):
                    if 1+(steps+1)%2==1:
                            white_turn.undraw()
                            black_turn.draw(win)
                    elif 1+(steps+1)%2==2:
                            black_turn.undraw()
                            white_turn.draw(win)
                    steps+=1
            elif num[1]==0:
                who_win=2
                b_re.redraw()
                black_win.draw(win)
                game_over=True
            elif num[2]==0:
                who_win=1
                b_re.redraw()
                white_win.draw(win)
                game_over=True
            elif num[1]+num[2]==64:
                b_re.redraw()
                if num[1]>num[2]:
                    who_win=1
                    white_win.draw(win)
                elif num[1]<num[2]:
                    who_win=2
                    black_win.draw(win)
                else:
                    who_win=0
                    tie.draw(win)
                game_over=True

            if game_over:
                while 1:
                    try:p3=win.getMouse()
                    except:break
                    if b_re.clicked(p3):
                        for i in cells:
                             for j in i:
                                j.clear()
                        if 1+(steps+1)%2==1:    white_turn.undraw()
                        elif 1+(steps+1)%2==2:  black_turn.undraw()
                        black_turn.draw(win)
                        num=[0,2,2]
                        steps=4
                        cells=[]
                        refresh()
                        b_re.clear()
                        if who_win==0:tie.undraw()
                        elif who_win==1:white_win.undraw()
                        else :black_win.undraw()
                        for i in range(8):
                            line=[]
                            for j in range(8):
                                if ((i==3) and (j==3)): line.append(Cell(2,i,j,win))
                                elif ((i==4) and (j==4)): line.append(Cell(2,i,j,win))
                                elif ((i==3) and (j==4)): line.append(Cell(1,i,j,win))
                                elif ((i==4) and (j==3)): line.append(Cell(1,i,j,win))
                                else :line.append(Cell(0,i,j,win))
                            cells.append(line)
                        break
                    elif b_qt.clicked(p3):break
            try:
                if b_qt.clicked(p3):break
            except:pass
        if b_qt.clicked(p2):break
        try:
            if b_qt.clicked(p3):break
        except:pass
win.close()
