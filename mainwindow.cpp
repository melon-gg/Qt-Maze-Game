#include "mainwindow.h"
#include <QMessageBox>
#include <QStyleFactory>
#include <QPalette>
#include <QPixmap>
#include <QTime>
#pragma execution_character_set("utf-8")
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    initialWindow_Layout();
}

MainWindow::~MainWindow()
{
}
void MainWindow::initialWindow_Layout()
{
    gLayout_Map=new QGridLayout;
    hLayout=new QHBoxLayout;
    gLayout_Map->setContentsMargins(0,0,0,0);//让布局紧贴主窗口，不留空白
    gLayout_Map->setSpacing(0);//让两个控件之间的间隔为0
    hLayout->setContentsMargins(0,0,0,0);
    hLayout->setSpacing(0);
    //初始化迷宫砖块
    for(int i=0; i <60 ;i++)
    {
        for(int j=0; j<60; j++)
        {
            MazeWidget[i][j]=new QWidget(this);
            gLayout_Map->addWidget(MazeWidget[i][j],i,j);
            MazeWidget[i][j]->hide();
        }
    }
    //初始化角色
    People = new QWidget(this);
    People->hide();

    QWidget *Mapwidget=new QWidget(this);

    surface=new interface(this);
    surface->setMinimumSize(840,680);
    connect(surface->timer,SIGNAL(timeout()),this,SLOT(ShowWidget()));//显示控制界面()));//传递过来的参数有问题，会一直调用槽函数
    gLayout_Map->addWidget(surface,0,0);
    Mapwidget->setMinimumSize(840,680);
    Mapwidget->setLayout(gLayout_Map);

    BASIC_WIDTH=0;
    BASIC_HEIGHT=0;

    iNum=0;
    lastheight=0;
    lastwidth=0;
    isPlay=false;
    isShow=false;
    issurface=true;
    isAIAnimationButton=false;
    isAutoMoveButton=false;


    initialControlWidget();


    MainWidget=new QWidget(this);
    hLayout->addWidget(Mapwidget);
    hLayout->addWidget(Controlwidget);


    MainWidget->setLayout(hLayout);
    MainWidget->setFocusPolicy(Qt::StrongFocus);


    //设置widget为Mainwindow的中心窗口
    this->setCentralWidget(MainWidget);
    group=new QSequentialAnimationGroup;
    timer=new QTimer(this);
    iNum=1;

    connect(timer,SIGNAL(timeout()),this,SLOT(moveCharacter()));
    connect(timer,SIGNAL(timeout()),this,SLOT(ShowPath()));

	ShowWidget();
}

void MainWindow::initialControlWidget()
{
	QVBoxLayout * v_layout = new QVBoxLayout;
	v_layout->setSpacing(20);

    Controlwidget = new QWidget(this);
    Controlwidget->setStyleSheet("background-color:lightGray");
    QString button_style="QPushButton{background-color:white; color: black;border-radius:1px;border:2px groove gray;border-style:outset;}"
                         "QPushButton:hover{background-color:lightGray; color: black;}"
                         "QPushButton:pressed{background-color:gray;border-style:inset;}";
	QHBoxLayout *h0 = new QHBoxLayout;
	QHBoxLayout *h1 = new QHBoxLayout;
    QLabel *label_w = new QLabel;
    QLabel *label_h = new QLabel;
    label_w->setStyleSheet("background-color:transparent");
    label_h->setStyleSheet("background-color:transparent");
    QLabel *label_blank[10];
    for(int i=0;i<10;i++)
    {
        label_blank[i]=new QLabel(this);
        label_blank[i]->setStyleSheet("background-color:transparent");
    }

    label_Stytle=new QLabel;
    StytleNum=2;
    label_Stytle->setStyleSheet(m.MapStytle[StytleNum][15]);
    QLabel *label_select=new QLabel;
    label_w->setText("迷宫宽度");
    label_h->setText("迷宫高度");
    label_select->setText("选择地图风格");
    sp_w=new QSpinBox(this);
    sp_h=new QSpinBox(this);
    sp_w->setStyleSheet("background-color:transparent");
    sp_h->setStyleSheet("background-color:transparent");

	h0->addWidget(label_w);
	h0->addWidget(sp_w);
	h1->addWidget(label_h);
	h1->addWidget(sp_h);

	label_Stytle->setStyleSheet(m.MapStytle[0][15]);
    QPushButton *GenerateButton=new QPushButton(this);
    quitButton=new QPushButton();
    AIAnimationButton=new QPushButton(this);
    AutoMoveButton=new QPushButton(this);
    AIAnimationButton->setEnabled(false);
    AutoMoveButton->setEnabled(false);
    GenerateButton->setText("生成迷宫");
    AutoMoveButton->setText("自动寻路");
    AIAnimationButton->setText("AI操作");

    connect(GenerateButton,SIGNAL(clicked()),this,SLOT(CreateMaze_Layout()));
    connect(AutoMoveButton,SIGNAL(clicked()),this,SLOT(timeStart()));
    connect(AIAnimationButton,SIGNAL(clicked()),this,SLOT(ShowAnimation()));
    connect(quitButton,SIGNAL(clicked()),this,SLOT(quit()));

	v_layout->addLayout(h0);
	v_layout->addLayout(h1);
	v_layout->addWidget(GenerateButton);
	v_layout->addWidget(AutoMoveButton);
	//v_layout->addWidget(AIAnimationButton);
	v_layout->addWidget(new QWidget(), 1);
    Controlwidget->setLayout(v_layout);
    Controlwidget->hide();
}
void MainWindow::resetMaze()
{
    AIAnimationButton->setEnabled(false);
    AutoMoveButton->setEnabled(false);
    quitButton->setEnabled(false);
    sp_h->setValue(0);
    sp_w->setValue(0);
    SelectMapStytle->setCurrentIndex(2);
}

QString intToQString(int num)
{
    QString result="";
    result.sprintf("%d",num);
    return result;
}

bool isOdd(int num)
{
    return !(num%2==0);
}
void MainWindow::CreateMaze_Layout()
{
    //停止之前可能的工作
    timer->stop();
    group->stop();
    iNum=1;

    if((sp_h->value()<6||sp_h->value()>58)||(sp_w->value()<6||sp_w->value()>58))
    {
        QMessageBox message(QMessageBox::NoIcon, tr("警告! "), QObject::tr("输入的数据范围超出6~58 "));
        message.setIconPixmap(QPixmap(":/info/image/information/warning.png"));
        message.exec();
        return;
    }
    else
    {
        m.height=sp_h->value();
        m.width=sp_w->value();
    }
    //当h和w都为偶数时，迷宫的出口（height-2，width-2）会被墙给封住
	if (!isOdd(m.height) && !isOdd(m.width))
	{
		if (m.height > m.width)
		{
			m.height += 1;
			m.width = m.height;
		}
		else
		{
			m.width += 1;
			m.height = m.width;
		}
	}
    BASIC_WIDTH = 840 / m.width;//52
    BASIC_HEIGHT = 680 / m.height;//56

    if(issurface)
    {
        surface->hide();
        gLayout_Map->removeWidget(surface);
        issurface=false;
    }

    People->setMinimumSize(BASIC_WIDTH,BASIC_HEIGHT);

    int temph=m.height;
    int tempw=m.width;
    m.initialMaze(temph,tempw);
    m.CreateMaze();
    m.setCharacterPos();
    m.setExitPos();
    m.autoFindPath();
    StytleNum = 0;
    ShowMaze_Layout();
    lastheight=temph;
    lastwidth=tempw;
    m.last_height=m.height;
    AIAnimationButton->setEnabled(true);
    AutoMoveButton->setEnabled(true);
    quitButton->setEnabled(true);
    isPlay=true;
}

//布局管理器方式，大小可以随着窗口改变
void MainWindow::ShowMaze_Layout()
{
    if(isShow)
    {
        for(int i=0;i<lastheight;i++)
        {
            for(int j=0;j<lastwidth;j++)
            {
                MazeWidget[i][j]->hide();
            }
        }
    }
    gLayout_Map->addWidget(People,m.x,m.y);
    People->setStyleSheet(m.MapStytle[StytleNum][0]);
    People->show();//用就显示
    for(int i=0;i<m.height;i++)
    {
        for(int j=0;j<m.width;j++)
        {
            MazeWidget[i][j]->setMinimumSize(BASIC_WIDTH,BASIC_HEIGHT);
            MazeWidget[i][j]->show();//要用的窗口必须要显示出来
            if(m.Matrix[i][j].state == 0)//状态0 代表 墙 障碍
                MazeWidget[i][j]->setStyleSheet(m.MapStytle[StytleNum][12]);
            if(m.Matrix[i][j].state == 1)//状态1 代表 草地 非障碍
                MazeWidget[i][j]->setStyleSheet(m.MapStytle[StytleNum][13]);
        }
    }
    MazeWidget[m.exit_x][m.exit_y]->setStyleSheet(m.MapStytle[StytleNum][14]);
    isShow=true;
}
void MainWindow::hideMaze()
{

    for(int i=0;i<lastheight;i++)
    {
        for(int j=0;j<lastwidth;j++)
        {
            MazeWidget[i][j]->hide();
        }
    }
    People->hide();
    isShow=false;
    surface->classicalisok=false;
}
void MainWindow::quit()
{
    Controlwidget->hide();
    hideMaze();
    resetMaze();
    surface->isok=false;

    surface->show();//开始界面显示后，主窗口大小还是不变
    this->resize(840,680);//需要重新设置主窗口大小

    issurface=true;//标识主界面显示过
}



#include <QMediaPlayer>

void MainWindow::playDuduMusic() {
	stopMusic->setMedia(QUrl("qrc:/music/music/stop.mp3"));
	stopMusic->setVolume(80);
	stopMusic->play();
	QTimer::singleShot(700, this, &MainWindow::stopDuduMusic);
}
//角色跳跃方式移动(结合布局管理器)
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(surface->classicalisok)
    {
        if(!isPlay)
        {
            return;
        }
        int step=0;
        //可以作为调节速度的接口
        //while(step<100000000)
        while(step<1000)
        {
           step++;
        }
        switch(event->key())
        {
        case Qt::Key_W://上
            //上面为墙，则什么也不执行
            if(m.Matrix[m.x-1][m.y].state==0)
            {
				playDuduMusic();
                return;
            }
            m.x-=1;
            gLayout_Map->addWidget(People,m.x,m.y);
            People->setStyleSheet(m.MapStytle[StytleNum][3]);//显示向上移动图片
            break;
        case Qt::Key_S://下
            if(m.Matrix[m.x+1][m.y].state==0)
            {
				playDuduMusic();
               return;
            }
            m.x+=1;
            gLayout_Map->addWidget(People,m.x,m.y);
            People->setStyleSheet(m.MapStytle[StytleNum][0]);
            break;
        case Qt::Key_A://左
            if(m.Matrix[m.x][m.y-1].state==0)
            {
				playDuduMusic();
                return;
            }
            m.y-=1;
            gLayout_Map->addWidget(People,m.x,m.y);
            People->setStyleSheet(m.MapStytle[StytleNum][6]);
            //qDebug()<<"width "<<People->width()<<"height "<<People->height();
            break;
        case Qt::Key_D://右
            if(m.Matrix[m.x][m.y+1].state==0)
            {
				playDuduMusic();
                return;
            }
            m.y+=1;
            gLayout_Map->addWidget(People,m.x,m.y);
            People->setStyleSheet(m.MapStytle[StytleNum][9]);
            break;
        }
        if(m.x==m.exit_x&&m.y==m.exit_y)
        {
            QMessageBox message(QMessageBox::NoIcon, "胜利！", "进入迷宫下一层！");
            message.setIconPixmap(QPixmap(":/new/image/congratulation.png"));
            message.exec();//不加这个对话框会一闪而过
            CreateMaze_Layout();
            return;
        }
    }
}

//重写鼠标点击和移动事件，使没有边框的窗口可以通过鼠标移动
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    this->windowPos = this->pos();                // 获得部件当前位置
    this->mousePos = event->globalPos();     // 获得鼠标位置
    this->dPos = mousePos - windowPos;       // 移动后部件所在的位置
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - this->dPos);
}

void MainWindow::ShowAnimation()
{
    group->clear();//动画组清空
    isAIAnimationButton=true;
    isAutoMoveButton=false;
    AutoMoveButton->setEnabled(false);
    for(int i=0; i<m.PathStack.size()-1;i++)
    {
        QPropertyAnimation* animation = new QPropertyAnimation(People, "pos");//动作初始化,People是操作对象
        animation->setDuration(200);//设置动作间隔                              //"pos"是操作属性，与QPoint 对应
                                                                              //"geometry" 与QRect/QRectF 对应
        animation->setStartValue(QPoint(m.PathStack[i].j*BASIC_WIDTH,m.PathStack[i].i*BASIC_HEIGHT));
        animation->setEndValue(QPoint(m.PathStack[i+1].j*BASIC_WIDTH,m.PathStack[i+1].i*BASIC_HEIGHT));
        animation->setEasingCurve(QEasingCurve::Linear);
        group->addAnimation(animation);//向动画组中添加动作
    }
    timer->start(200);//设置计时间隔，必须与动作间隔大小一样，才会同步
    group->start();//动画组启用
}

void MainWindow::timeStart()
{
    isAutoMoveButton=true;
    isAIAnimationButton=false;
    AIAnimationButton->setEnabled(false);
    timer->start(50);
}
void MainWindow::MapStytleSet()
{
   label_Stytle->setStyleSheet(m.MapStytle[SelectMapStytle->currentIndex()][15]);

}
void MainWindow::stopDuduMusic()
{
	stopMusic->stop();
}

void MainWindow::ShowWidget()
{
	Controlwidget->show();
}

void MainWindow::ShowPath()
{
    if(!isAIAnimationButton)
    {
        point temp=m.PathStack[iNum];
        MazeWidget[temp.i][temp.j]->setStyleSheet(m.MapStytle[StytleNum][0]);
        iNum++;
        if(iNum == m.PathStack.size()-1)
        {
            iNum = 0;
            for(int i=0;i<m.PathStack.size();i++)
            {
                point temp=m.PathStack[i];
                MazeWidget[temp.i][temp.j]->setStyleSheet(m.MapStytle[StytleNum][13]);
                MazeWidget[m.height-2][m.width-2]->setStyleSheet(m.MapStytle[StytleNum][14]);
            }
            timer->stop();
            AIAnimationButton->setEnabled(true);
            isAutoMoveButton=false;
            isAIAnimationButton=true;
            return;
        }

    }
}

void MainWindow::moveCharacter()//设置移动时的图片
{
    if(!isAutoMoveButton)
    {
        if(m.PathStack[iNum+1].i < m.PathStack[iNum].i)//up
        {
            People->setStyleSheet(m.MapStytle[StytleNum][3+iNum%3]);
        }
        if(m.PathStack[iNum+1].i > m.PathStack[iNum].i)//down
        {
            People->setStyleSheet(m.MapStytle[StytleNum][0+iNum%3]);
        }
        if(m.PathStack[iNum+1].j > m.PathStack[iNum].j)//right
        {
            People->setStyleSheet(m.MapStytle[StytleNum][9+iNum%3]);
        }
        if(m.PathStack[iNum+1].j < m.PathStack[iNum].j)//left
        {
            People->setStyleSheet(m.MapStytle[StytleNum][6+iNum%3]);
        }
        iNum++;
        if(iNum == m.PathStack.size()-1)
        {
            timer->stop();
            iNum = 0;
            AutoMoveButton->setEnabled(true);
            return;
        }
    }

}
