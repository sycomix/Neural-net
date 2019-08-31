#include "environment.h"


Environment::Environment(QWidget *parent,QPainter *p,unsigned int player)
{
    _parent = parent;
    _painter = p;
    _playerAmount = player;

    _environmentColor = QColor(250,250,250);
    _snakeColor      = QColor(0,200,0);
    _obsticleColor   = QColor(250,150,0);

    scale(2);
    mapsize(QSize(30,30));
    tileSize(8);
    tileSpace(2);
    drawPos(QPoint(50,50));




    //_player = new Player(mapsize());

    foodAmount(10);
    showInfoText(false);
    drawEnable(true);
    mapInit();

    _player.reserve(_playerAmount);
    _threadList.reserve(_playerAmount);
    _threadData.reserve(_playerAmount);
    _threadExit = false;
    _threadPause = true;
    _threadDelayMicros = 1000000;
    for(unsigned int a=0; a<_playerAmount; a++)
    {


        _player.push_back(new Player(_player.size(),mapsize()));
        _player[_player.size()-1]->standardColor(QColor(0,55+rand()%200,55+rand()%200));
    //    connect(_player[a],SIGNAL(collision(unsigned int,vector<QPoint>)),this,SLOT(snakeCollision(unsigned int,vector<QPoint>)));
    //    connect(_player[a],SIGNAL(starved(unsigned int)),this,SLOT(snakeStarved(unsigned int)));


    }
    for(unsigned int a=0; a<_playerAmount; a++)
    {
        _threadList.push_back(pthread_t());
        _threadData.push_back(thread_data_player());
        _threadData[a].thread_id = a;
        _threadData[a].player = &_player;
        _threadData[a].food = &_food;
        _threadData[a].viewMap = &_viewMap;
        _threadData[a].exit = &_threadExit;
        _threadData[a].pause = &_threadPause;
        _threadData[a].lock = &_threadLock;
        _threadData[a].condition_var = &_thread_condition_var;
        _threadData[a].isPaused = false;
        _threadData[a].delayMicros = &_threadDelayMicros;
        _threadData[a].killPlayer = false;
    }
    _debugCount = 0;




#ifdef __enableEnviromentThread
    int rc;
    for(unsigned int a=0; a<_playerAmount; a++)
    {
        rc = pthread_create(&_threadList[a], NULL, runThread, (void *)&_threadData[a]);
        if (rc)
        {
            qDebug() << "Error:unable to create thread," << rc << endl;
        }
    }
#endif

}
Environment::~Environment()
{

}


void Environment::mapsize(QSize size)
{
    _mapsize = size /* scale()*/;
    for(unsigned int a=0; a<_player.size(); a++)
    {
        _player[a]->mapSize(_mapsize);
    }
}
QSize Environment::mapsize()
{
    return _mapsize;
}
void Environment::tileSize(unsigned int size)
{
    _tileSize = size * scale();
}
unsigned int Environment::tileSize()
{
    return _tileSize;
}
void Environment::tileSpace(unsigned int space)
{
    _tileSpace = space * scale();
}
unsigned int Environment::tileSpace()
{
    return _tileSpace;
}
void Environment::drawPos(QPoint pos)
{
    _drawPos = pos;
}
QPoint Environment::drawPos()
{
    return _drawPos;
}
void Environment::scale(unsigned int scale)
{
    if(scale >= 1)
        _scale = scale;
    else
        _scale = 1;
}
unsigned int Environment::scale()
{
    return _scale;
}
void Environment::foodAmount(unsigned int amount)
{
    _foodAmount = (amount * _mapsize.height() * _mapsize.width())/1000;
}
unsigned int Environment::foodAmount()
{
    return _foodAmount;
}

void Environment::mapInit()
{
    foodAmount(foodAmount());
    _map.clear();
    _labelMap.clear();
    _viewMap = vector<vector<int>   >(mapsize().width(),vector<int>(mapsize().height()));


    for(unsigned int x=0; x<mapsize().width(); x++)
    {
        vector<Rect*> tmpMapVec;
        vector<QLabel*> tmpLabelVec;
        for(unsigned int y=0; y<mapsize().height(); y++)
        {
            Rect *tmpRect = new Rect(_painter);
            QLabel *tmpLabel = new QLabel(_parent);
            QFont textFont;
            textFont.setPointSize(mapsize().height()/2);
            tmpLabel->setFont(textFont);
            tmpLabel->setGeometry(drawPos().x()+(tileSize()+_tileSpace)*x-tileSize()/2,drawPos().y()+(tileSize()+_tileSpace)*y-tileSize()/2,tileSize(),tileSize());
            tmpLabel->hide();
            tmpLabel->setText("");

            tmpRect->begin(QPoint(0,0));
            tmpRect->end(QPoint(tileSize(),tileSize()));

            tmpRect->drawPos(QPoint(drawPos().x()+(tileSize()+_tileSpace)*x,drawPos().y()+(tileSize()+_tileSpace)*y));

            tmpRect->frame(false);
            tmpRect->frameSize(5);
            tmpRect->frameColor(_environmentColor);
            tmpRect->color(_environmentColor);

            tmpMapVec.push_back(tmpRect);
            tmpLabelVec.push_back(tmpLabel);
        }
        _map.push_back(tmpMapVec);
        _labelMap.push_back(tmpLabelVec);
    }
    for(unsigned int a=0; a<_player.size(); a++)
    {
        _player[a]->kill();
        _player[a]->revive();
    }
    placeObsticle();

}
void Environment::placeObsticle()
{
    // -1 because of the ever existing boarder
    for(unsigned int x=1; x<_mapsize.width()-1; x++)
    {
        for(unsigned int y=1; y<_mapsize.height()-1; y++)
        {
            if(_viewMap[x][y] == MapData::obsticle)
            {
                _viewMap[x][y] = MapData::nothing;
            }
        }
    }
    unsigned int wallAmount = mapsize().width()*mapsize().height()/100;
   // qDebug() << "wallamount: "<< wallAmount;
    for(unsigned int walls = 0; walls < wallAmount; walls++)
    {

        int randDirection = rand() % 4;
        int randWallSize  = rand() % 5;
        QPoint randPos    = QPoint(1+rand() % (mapsize().width()-2),1+rand() % (mapsize().height()-2));

    //    qDebug() << "randDirection: "<< randDirection;
    //    qDebug() << "randWallSize: "<< randWallSize;
    //    qDebug() << "randPos: "<< randPos;
        QPoint obsticlePos(-1,-1);
        for(int wallTile=0; wallTile<randWallSize; wallTile++)
        {
            switch(randDirection)
            {
                case 0:
                {
                    if(randPos.y()-wallTile < 0)
                        break;
                    obsticlePos = QPoint(randPos.x(),randPos.y()-wallTile);
                    //_viewMap[randPos.x()][] = MapData::obsticle;
                    break;
                }
                case 1:
                {
                    if(randPos.x()+wallTile >= _viewMap.size())
                        break;
                    obsticlePos = QPoint(randPos.x()+wallTile,randPos.y());
                   // _viewMap[randPos.x()+wallTile][randPos.y()] = MapData::obsticle;
                    break;
                }
                case 2:
                {
                    if(randPos.y()+wallTile >= _viewMap[randPos.x()].size())
                        break;
                    obsticlePos = QPoint(randPos.x(),randPos.y()+wallTile);
                   // _viewMap[randPos.x()][randPos.y()+wallTile] = MapData::obsticle;
                    break;
                }
                case 3:
                {
                    if(randPos.x()-wallTile < 0)
                        break;
                    obsticlePos = QPoint(randPos.x()-wallTile,randPos.y());
                  //  _viewMap[randPos.x()-wallTile][randPos.y()] = MapData::obsticle;
                    break;
                }
            }
            if(obsticlePos.x() != -1 && obsticlePos.y() != -1)
            {
               // if(_viewMap[obsticlePos.x()][obsticlePos.y()] == MapData::nothing)
                {
                 //   qDebug()<< "set: "<<obsticlePos;
                    _viewMap[obsticlePos.x()][obsticlePos.y()] = MapData::obsticle;
                }
            }
        }
    }
}
void Environment::setSnakeOnMap(unsigned int player)
{
    if(_player.size() <= player)
    {
        qDebug() << "out of Range error: void Environment::setSnakeOnMap(unsigned int player) : " << player;
        return;
    }
    if(_player[player]->pos()->size() == 0)
    {
        qDebug() << "Player: "<<player<<" size == 0";
    }
    for(unsigned int a=0; a<_player[player]->size(); a++)
    {
        if(_player[player]->pos(a)->rx() >= _mapsize.width() ||
           _player[player]->pos(a)->rx() < 0 ||
           _player[player]->pos(a)->ry() >= _mapsize.height() ||
           _player[player]->pos(a)->rx() < 0)
        {
            qDebug() << "ERROR: snake: "<<player<<" out of Boudry";
            qDebug() << _player[player]->pos(a);
            controlSnakeDeath(player,true);
        }else
        {
           // qDebug() <<"a"<<a;
            //crashes for no reason
            if(_viewMap[_player[player]->pos(a)->rx()][_player[player]->pos(a)->ry()] == MapData::obsticle)
            {
                controlSnakeDeath(player,true);
                return;
            }
            _viewMap[_player[player]->pos(a)->rx()][_player[player]->pos(a)->ry()] = MapData::snake;
            //_map[_player[player]->pos()[a].rx()][_player[player]->pos()[a].y()]->frameSize(5);
            //_map[_player[player]->pos()[a].rx()][_player[player]->pos()[a].y()]->frameColor(_player[player]->color(a));
            _map[_player[player]->pos(a)->rx()][_player[player]->pos(a)->ry()]->frame(false);
            _map[_player[player]->pos(a)->rx()][_player[player]->pos(a)->ry()]->color(_player[player]->color(a));

            _map[_player[player]->pos(a)->rx()][_player[player]->pos(a)->ry()]->end(QPoint(tileSize()*1.3,tileSize()*1.3));
        }
    }
}
void Environment::drawMap()
{
    if(!_drawEnable)
        return;

    //qDebug() << "------------------------------------";
    for(unsigned int y=0; y<mapsize().height(); y++)
    {
       // QString mapLine;
        for(unsigned int x=0; x<mapsize().width(); x++)
        {
            _map[x][y]->draw();
           // mapLine += QString::number(_viewMap[x][y]);
        }
       // qDebug() << mapLine;
    }
   // qDebug() << "------------------------------------";
}

void Environment::update()
{
    if(_food.size() != _foodAmount)
    {
        for(int a=0; a<30; a++)
        {
            if(_food.size() > _foodAmount)
            {
                _food.pop_back();
            }
            if(_food.size() < _foodAmount)
            {
                _food.push_back(new Food(_mapsize));
            }
        }
#ifdef __enableEnviromentThread
        for(unsigned int a=0; a<_player.size(); a++)
        {
            _threadData[a].food = &_food;
        }
#endif
    }
    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    //clock_t startTime = clock();
    //qDebug() << "sEnv: ";
#ifdef __enableEnviromentThread
//Thread begin
    unsigned int playerSize = _player.size();
    pthread_mutex_lock(&_threadLock);
    //_threadDelayMicros = 1000000;

    _threadPause = false;
    for(unsigned int a=0; a<playerSize; a++)
    {
        _threadData[a].isPaused = false;
    }
    pthread_cond_broadcast( &_thread_condition_var);
    pthread_mutex_unlock(&_threadLock);
    int pause = 0;
    bool _pause;

   // Sleep(10);
    vector<bool> restartCheckList(playerSize,false);

    do{
        pause = 0;
        for(unsigned int a=0; a<playerSize; a++)
        {
            if(restartCheckList[a])
            {
                pause++;
                continue;
            }
            pthread_mutex_lock(&_threadLock);
            _pause = _threadData[a].isPaused;
            pthread_mutex_unlock(&_threadLock);
            if(_pause)
            {
               restartCheckList[a] = true;
            }

            //pthread_join(_threadList[a], NULL);
        }
    }while(pause != playerSize);
  //  Sleep(10);
    pthread_mutex_lock(&_threadLock);
    _threadPause = true;
    pthread_mutex_unlock(&_threadLock);
    for(unsigned int player=0; player<_player.size(); player++)
    {
        if(_threadData[player].killPlayer == true)
        {
            controlSnakeDeath(player,true);
            _threadData[player].killPlayer = false;
        }
    }

//Thread end
#else
   // Sleep(10);
    for(unsigned int playerOne=0; playerOne<_player.size(); playerOne++)
    {
        if(!_player[playerOne]->isAlive())
            continue;
    //---------Check Snake Food collision
        //for(unsigned int a=0; a<_player[c]->pos().size(); a++)        //optimise
                                                                        //only check if the head has a collision with a food

         if(_player[playerOne]->size() != 0)
         {
            for(unsigned int b=0; b<_food.size(); b++)
            {
                if(_player[playerOne]->pos(0)->x() == _food[b]->pos().x() &&   //optimise
                   _player[playerOne]->pos(0)->y() == _food[b]->pos().y())     //optimise
                {
                //    qDebug() << "Food eaten. Amount: " << _food[b]->amount();
                    _player[playerOne]->addFood(_food[b]->amount());
                    _food[b]->eaten();
                    _food[b]->respawn();
                }
            }
        }
        for(unsigned int snakeTwo=0; snakeTwo<_player.size(); snakeTwo++)
        {
            if(!_player[snakeTwo]->isAlive() || !_player[playerOne]->isAlive())
                continue;
            if(playerOne == snakeTwo)
                continue;
            for(unsigned int b=0; b<_player[snakeTwo]->size(); b++)
            {
                if(!_player[snakeTwo]->isAlive() || !_player[playerOne]->isAlive())
                    continue;
                if(_player[playerOne]->pos(0)->x() == _player[snakeTwo]->pos(b)->x() &&
                   _player[playerOne]->pos(0)->y() == _player[snakeTwo]->pos(b)->y())
                {
                 //   qDebug() << "Collision between snakes: " << c << " : " << a;
                    if(b == 0)
                    {
                        if(_player[playerOne]->size() > _player[snakeTwo]->size())
                        {
                            controlSnakeDeath(snakeTwo,true);
                            //emit playerKill(playerOne,snakeTwo);
                        }
                        else {
                            controlSnakeDeath(playerOne,true);
                            //emit playerKill(snakeTwo,playerOne);
                        }
                    }else
                    {
                        //reward for snakeTwo
                        if(_player[snakeTwo]->killreward())
                        {
                            _player[snakeTwo] ->addFood(_player[playerOne]->food()/2);
                        }
                        controlSnakeDeath(playerOne,true);
                        //emit playerKill(snakeTwo,playerOne);
                    }
                    continue;
                }
            }
        }
    }
#endif
    for(unsigned int player=0; player<_player.size(); player++)
    {
        _player[player]->update();
    }

    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    //qDebug() << "fEnv: " << _timeinterval;
    //-----------------------------------
    _timeinterval = 0.9*_timeinterval + 0.1*time_span.count();
#ifdef __DEBUG_TIMEINTERVAL
    if(_debugCount > 1000)
    {
        qDebug() << "e: "<<_timeinterval;
        _debugCount = 0;
    }
    _debugCount++;
#endif
    for(unsigned int a=0; a<_food.size(); a++)
    {
        _food[a]->update();
    }
    for(unsigned int y=0; y<mapsize().height(); y++)
    {
        for(unsigned int x=0; x<mapsize().width(); x++)
        {
            if(_viewMap[x][y] != MapData::obsticle)
            {
                _viewMap[x][y] = MapData::nothing;
                if(_drawEnable)
                {
                    _map[x][y]->frame(false);
                    _map[x][y]->color(_environmentColor);
                    _map[x][y]->end(QPoint(tileSize(),tileSize()));
                }
                if(!_showInfoText)
                {
                    _labelMap[x][y]->hide();
                }
            }
            else
            {
                if(_drawEnable)
                {
                    _map[x][y]->color(_obsticleColor);
                }
            }
        }
    }
    for(unsigned int a=0; a<mapsize().height(); a++)
    {
        _viewMap[0][a] = MapData::obsticle;
        _viewMap[_viewMap.size()-1][a] = MapData::obsticle;
        if(_drawEnable)
        {
            _map[0][a]->color(_obsticleColor);
            _map[_viewMap.size()-1][a]->color(_obsticleColor);
        }
    }
    for(unsigned int a=0; a<mapsize().width(); a++)
    {
        _viewMap[a][0] = MapData::obsticle;
        _viewMap[a][_viewMap[a].size()-1] = MapData::obsticle;
        if(_drawEnable)
        {
            _map[a][0]->color(_obsticleColor);
            _map[a][_viewMap[a].size()-1]->color(_obsticleColor);
        }
    }

    for(unsigned int a=0; a<_food.size(); a++)
    {
        if(_food[a]->pos().x() >= _mapsize.width() ||
           _food[a]->pos().x() < 0 ||
           _food[a]->pos().y() >= _mapsize.height() ||
           _food[a]->pos().y() < 0)
        {
            qDebug() << "ERROR: food out of Boudry";
            qDebug() << _food[a]->pos();

        }else
        {
            _viewMap[_food[a]->pos().x()][_food[a]->pos().y()] = MapData::food;
            if(_drawEnable)
            {
                _map[_food[a]->pos().x()][_food[a]->pos().y()]->frameSize(2);
                _map[_food[a]->pos().x()][_food[a]->pos().y()]->frameColor(QColor((_food[a]->color().red()*80)/100,(_food[a]->color().green()*80)/100,(_food[a]->color().blue()*80)/100));
                _map[_food[a]->pos().x()][_food[a]->pos().y()]->frame(true);
                _map[_food[a]->pos().x()][_food[a]->pos().y()]->color(_food[a]->color());

                _map[_food[a]->pos().x()][_food[a]->pos().y()]->end(QPoint(tileSize()*0.8,tileSize()*0.8));
                if(_showInfoText)
                {
                    _labelMap[_food[a]->pos().x()][_food[a]->pos().y()]->setText(QString::number(_food[a]->amount()));
                    _labelMap[_food[a]->pos().x()][_food[a]->pos().y()]->show();
                }
            }
        }
    }
    //-----------Set Snake
    for(unsigned int a=0; a<_player.size(); a++)
    {
        if(_player[a]->isAlive())
        {
            try{
                setSnakeOnMap(a);
            }
            catch(std::runtime_error &e)
            {
                //qDebug() << "ERROR  setSnakeOnMap("<<a<<") "<< e.what();
                std::string errorMessage = "ERROR: setSnakeOnMap("+std::to_string(a)+")\n";
                errorMessage+= std::string(e.what())+"\n";
                errorMessage+= "playersize: "+std::to_string(_player[a]->size()) +"\n";
                qDebug() << QString::fromStdString(errorMessage);
                FILE *file = fopen("error_e.txt","a");
                if(file)
                {
                    fprintf(file,"%s\n",errorMessage.c_str());
                    fclose(file);
                }
            }catch(...)
            {
                qDebug() << "ERROR setSnakeOnMap("<<a<<")"<< "alife? : "<<_player[a]->isAlive();
            }
        }
    }
    //----------Set Food
   /* test++;
    for(unsigned int x=0; x<_map.size(); x++)
    {
        for(unsigned int y=0; y<_map[x].size(); y++)
        {
            //_map[x][y]->middlePoint(QPointF(0,0));
            //_map[x][y]->rotate(5);
            QPoint beginPos = QPoint(drawPos().x()+(tileSize()+_tileSpace)*x,drawPos().y()+(tileSize()+_tileSpace)*y);
            QPointF tmpPos = geometry::rotate(QPointF((float)beginPos.x(),(float)beginPos.y()),QPointF(300,300),test);
            _map[x][y]->drawPos(tmpPos.toPoint());
        }
    }*/
}


void Environment::controlSnakeDirection(unsigned int player,Direction dir)
{
    if(_player.size() <= player)
        player = 0;
    _player[player]->direction(dir);
}
void Environment::controlSnakeAddSize(unsigned int player,int increment)
{
    if(_player.size() <= player)
        player = 0;
    _player[player]->addSize(increment);
}
void Environment::controlSnakeAddFood(unsigned int player,int increment)
{
    if(_player.size() <= player)
        player = 0;
    //qDebug() << "Added food: "<< increment;
    _player[player]->addFood(increment);
}
void Environment::controlSnakeDeath(unsigned int player,bool death)
{
    if(_player.size() <= player)
        player = 0;
    if(death)
        _player[player]->kill();
    else
        _player[player]->revive();
}

void Environment::snakeCollision(unsigned int player,vector<QPoint> pos)
{
    if(_player.size() <= player)
        player = 0;
    //qDebug() << "<<<collision: "<<pos;
    controlSnakeDeath(player,true);
}
void Environment::snakeStarved(unsigned int player)
{
    if(_player.size() <= player)
        player = 0;
   // qDebug() << "<<<starved: ";
    controlSnakeDeath(player,true);
}
void Environment::obsticleReplace()
{

    placeObsticle();
}

Player *Environment::player(unsigned int player)
{
    if(_player.size() <= player)
        player = 0;
    return _player[player];
}
unsigned int Environment::playerAmount()
{
    return _player.size();
}
vector<vector<float>    >Environment::AI_mapData(unsigned int player,vector<QPoint>   fieldOfView)
{

    vector<int>   rawData;
    vector<vector<float>    >outputData(3,vector<float>());
    //vector<QPoint> coordView;
    if(fieldOfView.size() == 0 || player >= _player.size())
        return outputData;


    geometry::Angle rotAnge = 0;
    switch(_player[player]->direction()){
        case Direction::_up:
        {
            rotAnge = 0;
            fieldOfView = rotate_90(fieldOfView,QPoint(0,0),4);
            break;
        }
        case Direction::_left:
        {
            rotAnge = 270;
            fieldOfView = rotate_90(fieldOfView,QPoint(0,0),3);
            break;
        }
        case Direction::_down:
        {
            rotAnge = 180;
            fieldOfView = rotate_90(fieldOfView,QPoint(0,0),2);

            break;
        }
        case Direction::_right:
        {
            rotAnge = 90;
            fieldOfView = rotate_90(fieldOfView,QPoint(0,0),1);
            break;
        }
    }

  //  fieldOfView = geometry::rotate(fieldOfView,QPoint(0,0),rotAnge);
    int c = 0;

    vector<QPointF> fieldOfFiew_F;
    for(unsigned int a=0; a<fieldOfView.size(); a++)
    {
        fieldOfFiew_F.push_back(QPointF((float)fieldOfView[a].x(),(float)fieldOfView[a].y()));
    }
    fieldOfFiew_F = geometry::rotate(fieldOfFiew_F,QPoint(0,0),rotAnge);
  /*  if(_player[player]->direction() == Direction::_up)
    {
        fieldOfFiew_F = geometry::rotate(fieldOfFiew_F,QPoint(0,0),geometry::Angle(180));
        fieldOfFiew_F = geometry::rotate(fieldOfFiew_F,QPoint(0,0),geometry::Angle(180));
        fieldOfView = geometry::rotate(fieldOfView,QPoint(0,0),geometry::Angle(180));
        fieldOfView = geometry::rotate(fieldOfView,QPoint(0,0),geometry::Angle(180));
    }
    for(unsigned int a=0; a<fieldOfView.size(); a++)
    {
       fieldOfView[a] = QPoint((int)round(fieldOfFiew_F[a].x()),(int)round(fieldOfFiew_F[a].y()));
    }

    qDebug() << "fieldOfView_";

    for(unsigned int a=0; a<sqrt(fieldOfFiew_F.size()); a++)
    {
        string txt = "";
        for(unsigned int b=0; b<sqrt(fieldOfFiew_F.size()); b++)
        {
            txt+= to_string(fieldOfFiew_F[c].x())+"|"+to_string(fieldOfFiew_F[c].y())+"  ";
            c++;
        }
        qDebug() << QString::fromStdString(txt);
    }
    c = 0;
    for(unsigned int a=0; a<sqrt(fieldOfFiew_F.size()); a++)
    {
        string txt = "";
        for(unsigned int b=0; b<sqrt(fieldOfFiew_F.size()); b++)
        {
            txt+= to_string((int)round(fieldOfFiew_F[c].x()))+"|"+to_string((int)round(fieldOfFiew_F[c].y()))+"  ";
            c++;
        }
        qDebug() << QString::fromStdString(txt);
    }*/

    for(unsigned int a=0; a<fieldOfView.size(); a++)
    {

        outputData[food_layer].push_back(0);
        outputData[snake_layer].push_back(0);
        outputData[obsticle_layer].push_back(0);
        if(_player[player]->pos(0)->x()+fieldOfView[a].x() >= _viewMap.size() || _player[player]->pos(0)->x()+fieldOfView[a].x() < 0)
        {
            rawData.push_back(MapData::nothing);
        }else if(_player[player]->pos(0)->y()+fieldOfView[a].y() >= _viewMap[_player[player]->pos(0)->x()+fieldOfView[a].x()].size() || _player[player]->pos(0)->y()+fieldOfView[a].y() < 0)
        {
            rawData.push_back(MapData::nothing);
        }else
        {
            QPoint pos = QPoint(_player[player]->pos(0)->x()+fieldOfView[a].x(),_player[player]->pos(0)->y()+fieldOfView[a].y());
            rawData.push_back(_viewMap[pos.x()][pos.y()]);
            switch(_viewMap[pos.x()][pos.y()])
            {
                case MapData::food:
                {
                    for(unsigned int b=0; b<_food.size(); b++)
                    {
                        if(_food[b]->pos().x() == pos.x() && _food[b]->pos().y() == pos.y())
                        {
                            outputData[food_layer][outputData[food_layer].size()-1] = (float)_food[b]->amount()/100;
                            //outputData[food_layer][outputData[food_layer].size()-1] = 1;
                        }
                    }
                    break;
                }
                case MapData::snake:
                {
                    outputData[snake_layer][outputData[snake_layer].size()-1] = -1.0f; //bad snake
                    for(unsigned int b=0; b<_player[player]->size(); b++)
                    {
                       // qDebug() << "b: " << b << "pos: "<<_player[player]->pos(b) << " | " << pos;
                        if(_player[player]->pos(b)->x() == pos.x() && _player[player]->pos(b)->y() == pos.y())
                        {
                            outputData[snake_layer][outputData[snake_layer].size()-1] = 1.0f; //good snake
                            break;
                        }
                    }
                    break;
                }
                case MapData::obsticle:
                {
                    outputData[obsticle_layer][outputData[obsticle_layer].size()-1] = 1;
                    break;
                }
                case MapData::nothing:
                {

                    break;
                }
            }
        }
    }

    /*if(player == 0)
    {
       qDebug() << "AI_mapData: "<<outputData;
       //qDebug() << "AI_mapData";
        c = 0;
        for(unsigned int a=0; a<sqrt(rawData.size()); a++)
        {
            QString raw = "";
            QString food = "";
            QString snake = "";
            QString obsticle = "";
            for(unsigned int b=0; b<sqrt(rawData.size()); b++)
            {
                raw+=QString::number(rawData[c]);
                food+=QString::number(outputData[food_layer][c]);
                snake+=QString::number((int)outputData[snake_layer][c]);
                obsticle+=QString::number((int)outputData[obsticle_layer][c]);
                c++;
            }
            qDebug() << raw << "  |  " << food << "  |  " << snake << "  |  " << obsticle;
        }
    }*/
    return outputData;

    /*for(unsigned int x=0; x<fieldOfView.size(); x++)
    {
        vector<QPoint> tmpLine;
        for(unsigned int y=0; y<fieldOfView[x].size(); y++)
        {
            //tmpLine.push_back(geometry::rotate(_map[pointOfView.x()+fieldOfView[x][y].x()][pointOfView.y()+fieldOfView[x][y].y()],))
            tmpLine.push_back(QPoint(pointOfView.x()fieldOfView[x].,pointOfView.y()));
        }
    }*/
}
vector<vector<float>    >  Environment::AI_mapData_simple(unsigned int player)
{
    vector<vector<float>    > outputData(3,vector<float>());
    int viewLength = 5;
    float value = 0;
    vector<QPoint> viewDirection;
    viewDirection.push_back(QPoint(-1,0));
    viewDirection.push_back(QPoint(0,-1));
    viewDirection.push_back(QPoint(1,0));

    viewDirection.push_back(QPoint(-1,-1));
    viewDirection.push_back(QPoint(1,-1));

    geometry::Angle rotAnge = 0;
    switch(_player[player]->direction()){
        case Direction::_up:
        {
            rotAnge = 0;
            viewDirection = rotate_90(viewDirection,QPoint(0,0),4);
            break;
        }
        case Direction::_left:
        {
            rotAnge = 270;
            viewDirection = rotate_90(viewDirection,QPoint(0,0),3);
            break;
        }
        case Direction::_down:
        {
            rotAnge = 180;
            viewDirection = rotate_90(viewDirection,QPoint(0,0),2);

            break;
        }
        case Direction::_right:
        {
            rotAnge = 90;
            viewDirection = rotate_90(viewDirection,QPoint(0,0),1);
            break;
        }
    }

    if(_player.size() <= player)
        return outputData;

    for(int x=0; x<viewDirection.size(); x++)
    {
        outputData[food_layer].push_back(0);
        outputData[snake_layer].push_back(0);
        outputData[obsticle_layer].push_back(0);
        //for(int a=-1; a>viewDirection[0].x()*viewLength; a--)
        for(int a=1; a<viewLength; a++)
        {
            QPoint pos = QPoint(_player[player]->pos(0)->x()+a*viewDirection[x].x(),_player[player]->pos(0)->y()+a*viewDirection[x].y());
            if(pos.x() >= _viewMap.size() || pos.x() < 0)
            {
                continue;
            }else if(pos.y() >= _viewMap[pos.x()].size() || pos.y() < 0)
            {
                continue;
            }else if(_viewMap[pos.x()][pos.y()] == MapData::nothing)
            {
                continue;
            }


            switch(_viewMap[pos.x()][pos.y()])
            {
                case MapData::food:
                {
                    for(unsigned int b=0; b<_food.size(); b++)
                    {
                        if(_food[b]->pos().x() == pos.x() && _food[b]->pos().y() == pos.y())
                        {
                            outputData[food_layer][outputData[food_layer].size()-1] = (float)_food[b]->amount()/100;
                            //outputData[food_layer][outputData[food_layer].size()-1] = 1;
                        }
                    }
                    break;
                }
                case MapData::snake:
                {
                    outputData[snake_layer][outputData[snake_layer].size()-1] = 1.0f * ((float)viewLength-(float)a)/(float)viewLength; //bad snake
                    for(unsigned int b=0; b<_player[player]->size(); b++)
                    {
                       // qDebug() << "b: " << b << "pos: "<<_player[player]->pos(b) << " | " << pos;
                        if(_player[player]->pos(b)->x() == pos.x() && _player[player]->pos(b)->y() == pos.y())
                        {
                            outputData[snake_layer][outputData[snake_layer].size()-1] = 1.0f * ((float)viewLength-(float)a)/(float)viewLength; //good snake
                            break;
                        }
                    }
                    break;
                }
                case MapData::obsticle:
                {
                    outputData[obsticle_layer][outputData[obsticle_layer].size()-1] = 1.f * ((float)viewLength-(float)a)/(float)viewLength;
                    break;
                }
            }
            a = viewLength;

        }
    }
   /* if(player == 0)
    {
        qDebug() << outputData;
    }*/
    return outputData;

}
vector<QPoint> Environment::rotate_90(vector<QPoint> data,QPoint rotPoint, int amount)
{
    if(amount == 0)
        return data;
    if(data.size() == 0)
        return data;
    QPoint tmpPoint;
    for(unsigned int b=0; b<amount; b++)
    {
        for(unsigned int a=0; a<data.size(); a++)
        {
            tmpPoint.setX(-(data[a].y()+rotPoint.x()));
            tmpPoint.setY(data[a].x()-rotPoint.y());
            data[a] = tmpPoint;
        }
    }
    return data;
}
void Environment::showInfoText(bool enable)
{
    _showInfoText = enable;
    if(!_showInfoText)
    {
        for(unsigned int x=0; x<_labelMap.size(); x++)
        {
            for(unsigned int y=0; y<_labelMap[x].size(); y++)
            {
                _labelMap[x][y]->hide();
            }
        }
    }
}
bool Environment::showInfoText()
{
    return _showInfoText;
}
void Environment::drawEnable(bool enable)
{
    _drawEnable = enable;
}
bool Environment::drawEnable()
{
    return _drawEnable;
}
double Environment::cycleTime()
{
    return _timeinterval;
}

void *Environment::runThread(void *threadarg)
{
    pthread_detach(pthread_self());
    struct thread_data_player *my_data;
    bool ret = false;
    bool pause = false;
    bool enableLoop = false;
    //bool lastPauseState = false;
    my_data = (struct thread_data_player *) threadarg;
    struct timespec time, timestart;
    time.tv_sec = 0;
    pthread_mutex_lock(my_data->lock);
    my_data->isPaused = true;
    time.tv_nsec = *my_data->delayMicros;
    pthread_mutex_unlock(my_data->lock);
    qDebug() << "thread start: "<<my_data->thread_id << " "<<(*my_data->player)[my_data->thread_id];


    unsigned int plyer_size;
    QPoint  player_head_pos;
    unsigned int foodSize;
    unsigned int players;
#ifdef __DEBUG_TIMEINTERVAL_IN_THREAD
    high_resolution_clock::time_point t1;
    high_resolution_clock::time_point t2;
    duration<double> time_span;
    double _timeInterval = 0;
    unsigned int _debugCount = 0;
#endif
    while(!ret)
    {

        if(enableLoop)
        {
             try{
                //begin
                bool playerIsAlive;
                pthread_mutex_lock(my_data->lock);
                playerIsAlive = (*my_data->player)[my_data->thread_id]->isAlive();
                pthread_mutex_unlock(my_data->lock);
                if(!playerIsAlive)
                {
                    enableLoop = false;
                    continue;
                }

                //if(enableLoop)
                {
                    pthread_mutex_lock(my_data->lock);
                    plyer_size          = (*my_data->player)[my_data->thread_id]->size();
                    pthread_mutex_unlock(my_data->lock);
                    if(plyer_size == 0)
                    {
                        enableLoop = false;
                        bool ___alive;
                        pthread_mutex_lock(my_data->lock);
                        ___alive = (*my_data->player)[my_data->thread_id]->isAlive();
                        pthread_mutex_unlock(my_data->lock);
                        continue;
                    }
                    pthread_mutex_lock(my_data->lock);
                    player_head_pos     = *(*my_data->player)[my_data->thread_id]->pos(0);
                    foodSize            = my_data->food->size();
                    players             = (*my_data->player).size();
                    pthread_mutex_unlock(my_data->lock);

                    vector<Food> foodList;
                    foodList.reserve(foodSize);
                    for(unsigned int a=0; a<foodSize; a++)
                    {
                        Food tmpFood(QSize(10,10));
                        foodList.push_back(tmpFood);
                        pthread_mutex_lock(my_data->lock);
                        foodList[a] = *(*my_data->food)[a];
                        pthread_mutex_unlock(my_data->lock);
                    }

                    if(plyer_size != 0)
                    {
                        // check collision with food
                        bool foodIsAlive;
                        QPoint foodPos;
                        for(unsigned int food=0; food<foodSize; food++)
                        {
                            /*pthread_mutex_lock(my_data->lock);
                            foodIsAlive = (*my_data->food)[food]->isAlive();
                            foodPos = (*my_data->food)[food]->pos();
                            pthread_mutex_unlock(my_data->lock);*/
                            if(!foodList[food].isAlive())
                                continue;

                            //check collsison between snake and foods
                            if(player_head_pos == foodList[food].pos())
                            {
                                int foodAmount;
                               /* pthread_mutex_lock(my_data->lock);
                                foodAmount = (*my_data->food)[food]->amount();
                                pthread_mutex_unlock(my_data->lock);*/
                                pthread_mutex_lock(my_data->lock);
                                (*my_data->player)[my_data->thread_id]->addFood(foodList[food].amount());
                                (*my_data->food)[food]->eaten();
                                pthread_mutex_unlock(my_data->lock);
                            }
                        }

                        // check collision with snakes
                        unsigned int snakeTwo_size;
                        for(unsigned int snakeTwo=0; snakeTwo<players; snakeTwo++)
                        {
                            if(snakeTwo == my_data->thread_id)
                                continue;

                            pthread_mutex_lock(my_data->lock);
                            snakeTwo_size = (*my_data->player)[snakeTwo]->size();
                            pthread_mutex_unlock(my_data->lock);

                            for(unsigned int snakeTwoTiles=0; snakeTwoTiles<snakeTwo_size; snakeTwoTiles++)
                            {
                                QPoint snakeTwoTile_pos;
                                pthread_mutex_lock(my_data->lock);
                                snakeTwoTile_pos = *(*my_data->player)[snakeTwo]->pos(snakeTwoTiles);
                                pthread_mutex_unlock(my_data->lock);

                                if(snakeTwoTile_pos == player_head_pos)
                                {
                                    if(snakeTwoTiles == 0) // head a head collision
                                    {
                                        if(plyer_size > snakeTwo_size)
                                        {
                                            //controlSnakeDeath(snakeTwo,true);
                                         /*   pthread_mutex_lock(my_data->lock);
                                            (*my_data->player)[snakeTwo]->kill();
                                            pthread_mutex_unlock(my_data->lock);*/
                                        }
                                        else
                                        {
                                            pthread_mutex_lock(my_data->lock);
                                            my_data->killPlayer = true;
                                            //(*my_data->player)[my_data->thread_id]->kill();
                                            pthread_mutex_unlock(my_data->lock);
                                        }
                                    }else
                                    {
                                        //only if you use Killreward
                                       /* bool killreward;
                                        pthread_mutex_lock(my_data->lock);
                                        killreward = (*my_data->player)[snakeTwo]->killreward();
                                        pthread_mutex_unlock(my_data->lock);
                                        if(killreward)
                                        {
                                            (*my_data->player)[snakeTwo]->addFood((*my_data->player)[my_data->thread_id]->food()/2);
                                        }*/
                                        pthread_mutex_lock(my_data->lock);
                                        my_data->killPlayer = true;
                                        //(*my_data->player)[my_data->thread_id]->kill();
                                        pthread_mutex_unlock(my_data->lock);
                                    }
                                }
                            }
                        }
                       // if(my_data->thread_id == 0)
                       //    qDebug() << "update:     "<<my_data->thread_id << " ptr: " <<(*my_data->player)[my_data->thread_id];
            /*           pthread_mutex_lock(my_data->lock);
                        (*my_data->player)[my_data->thread_id]->update();
                       pthread_mutex_unlock(my_data->lock);*/


                    }
                    //qDebug() << "run: "<<my_data->thread_id;
                }
            }
            catch(std::runtime_error &e)
            {
                qDebug() << "ERROR  main "<< e.what();
                FILE *file = fopen("error_e.txt","a");
                if(file)
                {
                    fprintf(file,"%s\n",(std::string("ERROR  enviroment ")+std::to_string(my_data->thread_id)+e.what()).c_str());
                    fclose(file);
                }
            }
            //end
          //  if(my_data->thread_id == 0)
          //      qDebug() << "sleep     : "<<my_data->thread_id << " ptr: " <<(*my_data->player)[my_data->thread_id];
            enableLoop = false;
        }else
        {
            //qDebug() << "thread: "<<my_data->thread_id << " sleeping";
            //usleep(1);
            //nanosleep(&time, NULL);
#ifdef __DEBUG_TIMEINTERVAL_IN_THREAD
            t2 = high_resolution_clock::now();
            time_span = duration_cast<duration<double>>(t2 - t1);
            _timeInterval = 0.9*_timeInterval + 0.1*time_span.count();
            if(_debugCount > 1000)
            {
                if(my_data->thread_id == 0)
                    qDebug() << "e_: "<<my_data->thread_id << " " <<_timeInterval;
                _debugCount = 0;
            }
            _debugCount++;
#endif

            pthread_mutex_lock(my_data->lock);
            my_data->isPaused = true;
            pthread_mutex_unlock(my_data->lock);
            pthread_mutex_lock(my_data->lock);
            pthread_cond_wait( my_data->condition_var, my_data->lock );
            ret = *my_data->exit;
            //pause = *my_data->pause;
            //pause = my_data->isPaused;
            //time.tv_nsec = *my_data->delayMicros;
            pthread_mutex_unlock(my_data->lock);
#ifdef __DEBUG_TIMEINTERVAL_IN_THREAD
            t1 = high_resolution_clock::now();
#endif


            //if(!pause)
            {
               // if(my_data->thread_id == 0)
               //     qDebug() << "awake     : "<<my_data->thread_id << " ptr: " <<(*my_data->player)[my_data->thread_id];

                enableLoop = true;
                //pauseToggle = false;
                /*pthread_mutex_lock(my_data->lock);
                my_data->isPaused = false;
                pthread_mutex_unlock(my_data->lock);*/
            }
            /*if(pause && !pauseToggle)
            {
                pauseToggle = true;
            }*/

        }
    }
    qDebug() << "thread stop: "<<my_data->thread_id << " "<<(*my_data->player)[my_data->thread_id];
    pthread_exit(NULL);
}

