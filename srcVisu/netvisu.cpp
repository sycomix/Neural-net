#include "netvisu.h"
#include "neuron.h"

NetVisu::NetVisu(Net* _net,QObject *parent):
    QThread(parent)
{
    netList.push_back(_net);
    setupNetVisu();

    //db();
}

NetVisu::NetVisu(vector<Net*> _net,QObject *parent):
    QThread(parent)
{
    netList=_net;
    setupNetVisu();
}

void NetVisu::setupNetVisu(){


    if(netList.size() == 0)
    {
        qDebug() << "Error: NetVisu::setupQml(): netList.size() == 0 | No Nets defined";
        return;
    }
    if(netList[0]->needsUpdate())
    {
        qDebug() << "Error: NetVisu::setupQml(): nets aren't updated, please call \"Net::updateNetConfiguration()\" before you create a Netvisu object";
        return;
    }
    netWidget = nullptr;

    disableNeuronSignalUpdateEvent  = vector<bool>(netList.size(),false);
    disableNetGenomUpdateEvent      = vector<bool>(netList.size(),false);
    disableNetBiasValueUpdateEvent  = vector<bool>(netList.size(),false);
    //Reserve the space for all nets
    inputValueList  = vector<vector<double>  >(netList.size(),vector<double>(0,0));
    hiddenValueList = vector<vector<double>  >(netList.size(),vector<double>(0,0));
    outputValueList = vector<vector<double>  >(netList.size(),vector<double>(0,0));
    genomList       = vector<vector<double>  >(netList.size(),vector<double>(0,0));
    biasValueList   = vector<double>          (netList.size(),0);

    for(unsigned i=0;i<netList.size();++i){
        connect(netList[i],SIGNAL(accessLock()),this,SLOT(stopUpdateSlot()));
        connect(netList[i],SIGNAL(accessUnlock()),this,SLOT(startUpdateSlot()));
    }
    startUpdateSlot();
    //for(unsigned long long i=0;i<netList.size();i++){
    db.saveNet(netList[0]);
    //}
    Net* testNet;
    testNet=db.loadNet(0);
}

void NetVisu::setUpdateTime(unsigned int upDateTime){
    emit setUpdateTimeSignal(upDateTime);
}

void NetVisu::showWindow(){
    engine = new QQmlApplicationEngine;
    context= new QQmlContext(engine);
    context=engine->rootContext();
    context->setContextProperty (QmlRootContext, this);
    engine->load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
}

void NetVisu::loadNetInUi(QQuickWidget* widget){
    widget->rootContext()->setContextProperty(QmlRootContext,this);
    widget->setSource((QUrl(QStringLiteral("qrc:/qml/UiIntegratableNet.qml"))));
    netWidget=widget;
}

void NetVisu::displayUpdatNetTimer(const int &netId){
    if(netId >= netList.size())
    {
        qDebug() << "Error: NetVisu::displayUpdatNetTimer(const int &netId ["<<netId<<"]) : netId out of range";
        return;
    }
    disableNeuronSignalUpdateEvent[unsigned(netId)]  = false;
    disableNetGenomUpdateEvent[unsigned(netId)]      = false;
    disableNetBiasValueUpdateEvent[unsigned(netId)]  = false;
}

int NetVisu::getHiddenX(const int &netId) {
    if(unsigned(netId)<netList.size()&&access){
        return int(netList[unsigned(netId)]->get_hiddenNeuronsX());
    }
    return 0;
}
int NetVisu::getHiddenY(const int &netId) {
    if(unsigned(netId)<netList.size()&&access){
        return int(netList[unsigned(netId)]->get_hiddenNeuronsY());
    }
    else{
        return 0;
    }
}
int NetVisu::getInputs(const int &netId) {
    if(unsigned(netId)<netList.size()&&access){
        return int(netList[unsigned(netId)]->get_inputNeurons());
    }
    else{
        return 0;
    }
}
int NetVisu::getOutputs(const int &netId) {
    if(unsigned(netId)<netList.size()&&access){
        return int(netList[unsigned(netId)]->get_outputNeurons());
    }
    else{
        return 0;
    }
}

QVector<qreal> NetVisu::getHiddenValue(const int &netId) {
    QVector<qreal> vect;
    if(unsigned(netId)<netList.size()&&access){
        std::vector<Neuron*> hiddenNeuronList = *netList[unsigned(netId)]->get_ptr_allNeurons();
        for(unsigned  int j=0;j<hiddenNeuronList.size();j++){
            if(hiddenNeuronList[j]->get_ID().TYPE == NeuronType::hidden)
            {
                vect.push_back(hiddenNeuronList[j]->get_output());
            }
            //vect.push_back(int(net->get_hiddenNeuronY_ptr(0)[j]->get_ID().ID));
        }
    }
    return vect;
}
QVector<qreal> NetVisu::getInputsValue(const int &netId) {
    QVector<qreal> vect;
    if(unsigned(netId)<netList.size()&&access){
        vect =QVector<qreal>::fromStdVector(netList[unsigned(netId)]->get_input());
    }
    return vect;
}
QVector<qreal> NetVisu::getOutputsValue(const int &netId) {
    QVector<qreal> vect;
    if(unsigned(netId)<netList.size()&&access){
        vect =QVector<qreal>::fromStdVector(netList[unsigned(netId)]->get_output());
    }
    return vect;
}

QVector<int> NetVisu::getHiddenID(const int &netId) {
    //qDebug()<<"get vect "<<net->get_hiddenNeuronY_ptr(1).size();
    QVector<int> vect;
    if(unsigned(netId)<netList.size()&&access){
        std::vector<Neuron*> hiddenNeuronList = *netList[unsigned(netId)]->get_ptr_allNeurons();
        for(unsigned  int j=0;j<hiddenNeuronList.size();j++){
            if(hiddenNeuronList[j]->get_ID().TYPE == NeuronType::hidden)
            {
                vect.push_back(hiddenNeuronList[j]->get_ID().ID);
            }
            //vect.push_back(int(net->get_hiddenNeuronY_ptr(0)[j]->get_ID().ID));
        }
    }
    return vect;
}
QVector<int> NetVisu::getOutputID(const int &netId) {
    QVector<int> vect;
    if(unsigned(netId)<netList.size()&&access){
        for(unsigned  int j=0;j<netList[unsigned(netId)]->get_outputNeurons();j++){
            vect.push_back(netList[unsigned(netId)]->get_ptr_outputNeuron(j)->get_ID().ID);
        }
    }
    return vect;
}

QVector<int> NetVisu::getConSourceID(const int &netId) {
    QVector<int> vect;
    if(unsigned(netId)<netList.size()&&access){
        for(unsigned  int i=0;i<netList[unsigned(netId)]->get_connections();i++){
            vect.push_back(netList[unsigned(netId)]->get_connectionList()[i].source_ID.ID);
        }
    }
    //qDebug()<<"Source \t"<<vect;
    return vect;
}

QVector<int> NetVisu::getConDestinationID(const int &netId) {
    QVector<int> vect;
    if(unsigned(netId)<netList.size()&&access){
        for(unsigned  int i=0;i<netList[unsigned(netId)]->get_connections();i++){
            vect.push_back(netList[unsigned(netId)]->get_connectionList()[i].destination_ID.ID);
        }
    }
    //qDebug()<<"\t"<<vect;
    return vect;
}

QVector<int> NetVisu::getConSourceType(const int &netId) {
    QVector<int> vect;
    if(unsigned(netId)<netList.size()&&access){
        for(unsigned  int i=0;i<netList[unsigned(netId)]->get_connections();i++){
            vect.push_back(netList[unsigned(netId)]->get_connectionList()[i].source_ID.TYPE);
        }
    }
    return vect;
}

QVector<int> NetVisu::getConDestinationType(const int &netId) {
    QVector<int> vect;
    if(unsigned(netId)<netList.size()&&access){
        for(unsigned  int i=0;i<netList[unsigned(netId)]->get_connections();i++){
            vect.push_back(netList[unsigned(netId)]->get_connectionList()[i].destination_ID.TYPE);
        }
    }
    return vect;
}
QVector<qreal> NetVisu::getConWeight(const int &netId) {
    QVector<qreal> vect;
    if(unsigned(netId)<netList.size()&&access){
        vect= QVector<qreal>::fromStdVector(netList[unsigned(netId)]->get_genom());
    }
    return vect;
}


bool NetVisu::getBias(const int &netId) {
    if(unsigned(netId)<netList.size()&&access){
    return netList[unsigned(netId)]->get_bias();
    }
    else{
        return false;
    }
}

qreal NetVisu::getBiasValue(const int &netId) {
    if(unsigned(netId)<netList.size()&&access){
    return netList[unsigned(netId)]->get_biasValue();
    }
    else{
        return 0;
    }
}

int NetVisu::getNetCount(){
    return netList.size();
}
void NetVisu::stopUpdateSlot(){
    qDebug()<<"stop Update";
    access=false;
    emit stopUpdateSignal();
}
void NetVisu::startUpdateSlot(){
    qDebug()<<"start Update";
    access=true;
    emit startUpdateSignal();
}

void NetVisu::onRunDone(Net *p_net)
{
    if(!access || disableNeuronSignalUpdateEvent[p_net->get_ID()])
        return;
    unsigned int netID              = p_net->get_ID();
    inputValueList[netID]           = p_net->get_input();
    hiddenValueList[netID]          = p_net->get_hidden();
    outputValueList[netID]          = p_net->get_output();
    disableNeuronSignalUpdateEvent[p_net->get_ID()]  = true;
}
void NetVisu::onBiasValueChanged(Net *p_net)
{
    if(!access || disableNetBiasValueUpdateEvent[p_net->get_ID()])
        return;
    unsigned int netID              = p_net->get_ID();
    biasValueList[netID]            = p_net->get_biasValue();
    disableNetBiasValueUpdateEvent[p_net->get_ID()]  = true;
}
void NetVisu::onWeightsChanged(Net *p_net)
{
    if(!access || disableNetGenomUpdateEvent[p_net->get_ID()])
        return;
    unsigned int netID          = p_net->get_ID();
    genomList[netID]            = p_net->get_genom();
    disableNetGenomUpdateEvent[p_net->get_ID()]  = true;
}
void NetVisu::onNetConfigurationChanged()
{

}

