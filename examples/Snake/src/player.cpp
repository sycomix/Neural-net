#include "player.h"



Player::Player(unsigned int player,QSize mapSize)
{
    _mapSize = mapSize;
    playerIndex(player);
    _presetSize = 5;
    _foodPerTile = 20;
    _maxSteps = 5000;
    _presetFood = _presetSize * _foodPerTile;
    standardColor(QColor(0,200,0));
    globalView(false);

    _death = true;
    revive();
}
Player::~Player()
{

}
void Player::playerIndex(unsigned int index)
{
    _playerIndex = index;
}
unsigned int Player::playerIndex()
{
    return _playerIndex;
}
void Player::mapSize(QSize mapSize)
{
    _mapSize = mapSize;
}
QSize Player::mapsize()
{
    return _mapSize;
}

void Player::sollSize(int size)
{
    if(size >= 2)
        _size = size;
    else
        _size = 2;
}
int Player::sollSize()
{
    return _size;
}
unsigned int Player::size()
{
    return _playerPos.size();
}
void Player::addSize(int increment)
{
    sollSize(sollSize() + increment);
}
void Player::food(long int food)
{
    _food = food;
}
long int Player::food()
{
    return _food;
}
void Player::addFood(int increment)
{
    food(food() + increment);
}

void Player::pos(vector<QPoint> pos)
{
    if(pos.size() >= 2)
    {
        _playerPos = pos;
        sollSize(_playerPos.size());
    }
}
QPoint Player::pos(unsigned int index)
{
    if(_playerPos.size() <= index)
    {
        return QPoint(0,0);
    }
    return _playerPos[index];
}
vector<QPoint> Player::pos()
{
    return _playerPos;
}
void Player::standardColor(QColor standard)
{
    _standardBodyColor = standard;
}
QColor Player::standardColor()
{
    return _standardBodyColor;
}
QColor Player::color(unsigned int index)
{
    if(_playerColor.size() <= index)
    {
        return _standardBodyColor;
    }
    return _playerColor[index];
}
vector<QColor> Player::color()
{
    return _playerColor;
}


void Player::update()
{
    if(_death)
        return;
    if(_playerPos.size() > _size)
    {
        _playerPos.pop_back();
        _playerColor.pop_back();
    }else if(_playerPos.size() < _size)
    {
        _playerPos.push_back(_lastPos);
        _playerColor.push_back(_lastColor);
    }

   /* for(unsigned int a=0; a<_playerPos.size(); a++)
    {
        qDebug() << "index: "<< a << " "<<_playerPos[a];
    }
    qDebug() << "";*/
    //----------Shift

    _lastPos = _playerPos[_playerPos.size()-1];
    _lastColor = _playerColor[_playerColor.size()-1];
    for(int a=_playerPos.size()-2; a>=0; a--)
    {
        _playerPos[a+1] = _playerPos[a];
        _playerColor[a+1] = _playerColor[a];
    }
    _playerColor[0] = _standardBodyColor;
    switch (_direction)
    {
        case Direction::_up:
        {
            _playerPos[0] = QPoint(_playerPos[1].x(),_playerPos[1].y()-1);
            break;
        }
        case Direction::_left:
        {
            _playerPos[0] = QPoint(_playerPos[1].x()-1,_playerPos[1].y());
            break;
        }
        case Direction::_down:
        {
            _playerPos[0] = QPoint(_playerPos[1].x(),_playerPos[1].y()+1);
            break;
        }
        case Direction::_right:
        {
            _playerPos[0] = QPoint(_playerPos[1].x()+1,_playerPos[1].y());
            break;
        }
    }
   // qDebug() << "ges food: " << food();
    int tmpFood = food();
    if(tmpFood < 0)
        tmpFood = 0;
    for(int a=0; a<size(); a++)
    {
        float colorFactor;
        if(tmpFood / _foodPerTile != 0)
        {
            colorFactor = 1;
        }
        else
        {
            colorFactor =  (float)(tmpFood % _foodPerTile)/_foodPerTile;
        }
        //qDebug() << "food index: " << a << " : " << colorFactor << "tmpFood: " << tmpFood;
       // colorFactor = 1.f - colorFactor;
        tmpFood-=_foodPerTile;
        if(tmpFood < 0)
        {
            tmpFood = 0;
        }
        if(colorFactor <= 0.01)
        {
            sollSize(size()-1);
        }
        _playerColor[a] = QColor(255-(float)255*colorFactor,(float)_standardBodyColor.green()*colorFactor,(float)_standardBodyColor.blue()*colorFactor);
        //_playerColor[a] = QColor(0,0,0);
    }

    addFood(-1);
    if(food() <= 0)
    {
        qDebug() << "starved: "<< _playerIndex;
        emit starved(_playerIndex);
    }
    if(food() / _foodPerTile > size()-1)
    {
        addSize(1);
    }
    checkForSelfCollision();
    _steps++;
    if(_steps >= _maxSteps)
    {
        kill();
    }
    //---------------
}
void Player::globalView(bool globalView)
{
    _glovalView = globalView;
}
bool Player::globalView()
{
    return _glovalView;
}
void Player::direction(Direction dir)
{
    if(_glovalView){
        if(_direction == dir)
            return;
        if((_direction+dir)%2 == 0)// check if direction is opposite
            return;

        addFood(-2);
        _direction = dir;
    }
    else {
        if(dir == Direction::_up)
            return;
        addFood(-2);
        _direction = (_direction + dir)%4;
    }
}
Direction Player::direction()
{
    return (Direction)_direction;
}
void Player::checkForSelfCollision()
{
    if(_death)
        return;

    vector<QPoint> collisionPos;
    for(unsigned int a=0; a<size(); a++)
    {
        for(unsigned int b=a+1; b<size(); b++)
        {
            //qDebug() << "check: "<<a<<" to: "<<b;
            if(_playerPos[a].x() == _playerPos[b].x() &&
               _playerPos[a].y() == _playerPos[b].y())
            {
                qDebug() << "collision: "<< _playerPos[a];
                collisionPos.push_back(_playerPos[a]);
            }
        }
    }
    if(collisionPos.size() != 0)
    {
        qDebug() << "collision: "<< _playerIndex;
        emit collision(_playerIndex,collisionPos);
    }
}
void Player::kill()
{
    qDebug() << "Killed Player";
    qDebug() << "Stats:";
    qDebug() << "Steps: " << steps();
    qDebug() << "Food: " << food();
    qDebug() << "Size: " << size();
    _death = true;
    _playerPos.clear();
    _playerColor.clear();
}
void Player::revive()
{
    if(!_death)
        return;

    _death = false;

    sollSize(_presetSize);
    food(_presetFood);
    //qDebug() << "food: " << food() << " " << _presetFood;
    _steps = 0;

    _direction = rand()%4;
    _playerPos = vector<QPoint>(sollSize());
    _playerPos[0] = QPoint(5+rand() %(_mapSize.width()-10),5+rand()%(_mapSize.height()-10));

    _playerColor = vector<QColor>(sollSize());
    _playerColor[0] = _standardBodyColor;


    for(unsigned int a=1; a<size(); a++)
    {
        _playerPos[a] = QPoint(_playerPos[a-1].x()-1,_playerPos[a-1].y());
        _playerColor[a] = _standardBodyColor;
    }
}
bool Player::isAlive()
{
    return !_death;
}
unsigned int Player::steps()
{
    return _steps;
}