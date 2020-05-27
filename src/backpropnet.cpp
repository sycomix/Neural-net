#include "backpropnet.h"


double BackpropNet::__dbg_time1 = 0;
double BackpropNet::__dbg_time2 = 0;
double BackpropNet::__dbg_time3 = 0;
double BackpropNet::__dbg_time4 = 0;
double BackpropNet::__dbg_time5 = 0;
double BackpropNet::__dbg_time6 = 0;
double BackpropNet::__dbg_time7 = 0;
double BackpropNet::__dbg_time8 = 0;
double BackpropNet::__dbg_time9 = 0;
double BackpropNet::__dbg_time10 = 0;

BackpropNet::BackpropNet(unsigned int ID
                         ,QObject *parent)
    :   Net(ID,parent)
{
    this->init();
    try {
        this->set_mutationFactor(0.1);
    } catch (std::runtime_error &e) {
        this->addError(Error("BackpropNet",QString(e.what())));
        //error_general("BackpropNet()",e);
    }

}
BackpropNet::BackpropNet(unsigned int ID,
                         unsigned int inputs,
                         unsigned int hiddenX,
                         unsigned int hiddenY,
                         unsigned int outputs,
                         QObject *parent)
    :   Net(ID,inputs,hiddenX,hiddenY,outputs,parent)
{
    this->init();
    try {
        this->set_mutationFactor(0.1);
    } catch (std::runtime_error &e) {
        /*error_general("BackpropNet(unsigned int ["+QString::number(inputs)+
                              "] , unsigned int ["+QString::number(hiddenX)+
                              "] , unsigned int ["+QString::number(hiddenY)+
                              "] , unsigned int ["+QString::number(outputs)+"])",e);*/
        this->addError(Error("BackpropNet(unsigned int ["+QString::number(inputs)+
                             "] , unsigned int ["+QString::number(hiddenX)+
                             "] , unsigned int ["+QString::number(hiddenY)+
                             "] , unsigned int ["+QString::number(outputs)+"]",
                             QString(e.what())));
    }
}
BackpropNet::BackpropNet(unsigned int ID,
                         unsigned int inputs,
                         unsigned int hiddenX,
                         unsigned int hiddenY,
                         unsigned int outputs,
                         bool enableBias,
                         bool enableAverage,
                         Activation func,
                         QObject *parent)
    :   Net(ID,inputs,hiddenX,hiddenY,outputs,enableBias,enableAverage,func,parent)
{
    this->init();
    try {
        this->set_mutationFactor(0.1);
    } catch (std::runtime_error &e) {
       /* error_general("BackpropNet(unsigned int ["+QString::number(inputs)+
                              "] , unsigned int ["+QString::number(hiddenX)+
                              "] , unsigned int ["+QString::number(hiddenY)+
                              "] , unsigned int ["+QString::number(outputs)+
                              "] , bool ["+QString::number(enableBias)+
                              "] , bool ["+QString::number(enableAverage)+
                              "] , Activation ["+QString::number(func)+"])",e);*/
        this->addError(Error("BackpropNet(unsigned int ["+QString::number(inputs)+
                             "] , unsigned int ["+QString::number(hiddenX)+
                             "] , unsigned int ["+QString::number(hiddenY)+
                             "] , unsigned int ["+QString::number(outputs)+
                             "] , bool ["+QString::number(enableBias)+
                             "] , bool ["+QString::number(enableAverage)+
                             "] , Activation ["+QString::number(func)+"])",
                             QString(e.what())));
    }
    if(this->get_activationFunction() == Activation::Binary)
    {
        /*error_general("BackpropNet(unsigned int ["+QString::number(inputs)+
                      "] , unsigned int ["+QString::number(hiddenX)+
                      "] , unsigned int ["+QString::number(hiddenY)+
                      "] , unsigned int ["+QString::number(outputs)+
                      "] , bool ["+QString::number(enableBias)+
                      "] , bool ["+QString::number(enableAverage)+
                      "] , Activation ["+QString::number(func)+"])","you cant use this activation function: Binary for this learn algorithm");
        */
        this->addError(Error("BackpropNet(unsigned int ["+QString::number(inputs)+
                             "] , unsigned int ["+QString::number(hiddenX)+
                             "] , unsigned int ["+QString::number(hiddenY)+
                             "] , unsigned int ["+QString::number(outputs)+
                             "] , bool ["+QString::number(enableBias)+
                             "] , bool ["+QString::number(enableAverage)+
                             "] , Activation ["+QString::number(func)+"])",
                             QString("you cant use this activation function: Binary for this learn algorithm")));
    }
}

void        BackpropNet::set(unsigned int inputs,
                             unsigned int hiddenX,
                             unsigned int hiddenY,
                             unsigned int outputs,
                             bool enableBias,
                             bool enableAverage,
                             Activation func)
{
    this->set_inputNeurons(inputs);
    this->set_hiddenNeuronsX(hiddenX);
    this->set_hiddenNeuronsY(hiddenY);
    this->set_outputNeurons(outputs);
    this->set_bias(enableBias);
    this->set_enableAverage(enableAverage);
    this->set_activationFunction(func);
}
void                    BackpropNet::set_netFileName(QString filename)
{
    _saveNet.set_filename(filename);
}
QString                 BackpropNet::get_netFileName()
{
    return _saveNet.get_filename();
}
void                    BackpropNet::set_netFileEnding(QString fileEnding)
{
    _saveNet.set_fileEnding(fileEnding);
}
QString                 BackpropNet::get_netFileEnding()
{
    return _saveNet.get_fileEnding();
}
void                    BackpropNet::loadFromNetFile()
{
    unsigned int genomSelection = 0;
    bool loadSucceed = false;
    try {
        loadSucceed = _saveNet.loadFile();
    } catch (std::runtime_error &e) {
       __DEBUG_BACKPROPNET(this,"loadFromNetFile()"," Warning: "+QString(e.what()));
        return;
    }
    if(loadSucceed)
    {
        try {
            this->set(_saveNet.get_inputNeurons(),
                      _saveNet.get_hiddenNeuronsX(),
                      _saveNet.get_hiddenNeuronsY(),
                      _saveNet.get_outputNeurons(),
                      _saveNet.get_bias(),
                      _saveNet.get_enableAverage(),
                      _saveNet.get_activationFunction());
            this->set_biasValue(_saveNet.get_biasValue());
            this->set_connectionList(*_saveNet.get_ptr_connectionList(genomSelection));
            /*this->set_neurons(_saveNet.get_neurons(),
                          _saveNet.get_hiddenNeurons(),
                          _saveNet.get_outputNeurons(),
                          _saveNet.get_costumNeurons());*/

            //this->updateNetConfiguration();
            //this->genom(_saveNet.genom(genomSelection));
        } catch (std::runtime_error &e) {
            addError(Error("loadFromNetFile(QString ["+_saveNet.get_filename()+"] , QString ["+_saveNet.get_fileEnding()+"] )",
                          {"unable to apply the settings. Maybe the file is damaged.",
                           e.what()}));
        }
    }
}
void                    BackpropNet::loadFromNetFile(QString filename)
{
    this->set_netFileName(filename);
    this->loadFromNetFile();
}
void                    BackpropNet::loadFromNetFile(QString filename,QString fileEnding)
{
    this->set_netFileName(filename);
    this->set_netFileEnding(fileEnding);
    this->loadFromNetFile();
}
void                    BackpropNet::saveToNetFile()
{
    try {
        _saveNet.set(this->get_inputNeurons(),
                     this->get_hiddenNeuronsX(),
                     this->get_hiddenNeuronsY(),
                     this->get_outputNeurons(),
                     this->get_bias(),
                     this->get_enableAverage(),
                     this->get_activationFunction(),
                     this->get_biasValue());
        //_saveNet.setGenom(this->genom());
        _saveNet.set_ptr_neuronsOfNet(this->get_ID(),this->get_ptr_allNeurons());
        _saveNet.saveFile();
    } catch (std::runtime_error &e) {
        addError(Error("saveToNetFile()",QString(e.what())));
        //error_general("saveToNetFile()",e);
    }
}
void                    BackpropNet::saveToNetFile(QString filename)
{
    this->set_netFileName(filename);
    this->saveToNetFile();
}
void                    BackpropNet::saveToNetFile(QString filename,QString fileEnding)
{
    this->set_netFileName(filename);
    this->set_netFileEnding(fileEnding);
    this->saveToNetFile();
}



double                  BackpropNet::get_netError()
{

    try {
        calc_netError();
    } catch (std::runtime_error &e) {
        addError(Error("get_netError()",QString(e.what())));
    }
    return _netError;
}
std::vector<double>     BackpropNet::get_outputError()
{
    try {
        calc_netError();
    } catch (std::runtime_error &e) {
        addError(Error("get_outputError()",QString(e.what())));
    }
    return _outputError;
}
double                  BackpropNet::get_outputError(unsigned int output)
{
    if(output >= this->get_outputNeurons())
    {
        addError(Error("get_outputError(unsigned int ["+QString::number(output)+"] )",
                       ErrorMessage::outOfRange('[',static_cast<unsigned int>(0),output,this->get_outputNeurons()-1,']')));
        //error_general("outputError(unsigned int ["+QString::number(output)+"] )",error_paramOutOfRange((unsigned int)0,output,(unsigned int)0,this->outputNeurons()-1));
    }
    try {
        calc_netError();
    } catch (std::runtime_error &e) {
        addError(Error("get_outputError(unsigned int ["+QString::number(output)+"] )",
                       QString(e.what())));
        //error_general("outputError(unsigned int ["+QString::number(output)+"] )",e);
    }
    return _outputError[output];
}
void                    BackpropNet::set_mutationFactor(double mutationFactor)
{
    if(mutationFactor <= 0)
    {
        addError(Error("set_mutationFactor(double ["+QString::number(mutationFactor)+"] )",
                       QString("parameter 0 is out of range. MutationFactor has to be greather than 0")));
        //error_general("mutationFactor(double ["+QString::number(mutationFactor)+"] )","parameter 0 is out of range. MutationFactor has to be greather than 0");
    }
    _mutationFactor = mutationFactor;
}
double                  BackpropNet::get_mutationFactor()
{
    return _mutationFactor;
}
void                    BackpropNet::set_expected(std::vector<double> expected)
{
    if(static_cast<unsigned int>(expected.size()) != this->get_outputNeurons())
    {
        addError(Error("get_outputNeurons()",
                       QString("parameter 0 has the wrong size: "+
                               QString::number(expected.size())+" std::vector.size() should be: "+
                               QString::number(this->get_outputNeurons()))));
        //error_general("expected(std::vector<double>)","parameter 0 has the wrong size: "+QString::number(expected.size())+" std::vector.size() should be: "+QString::number(this->outputNeurons()));
    }
    _needsCalculationUpdate   = true;
    _expected = expected;
    try {
        calc_netError();
    } catch (std::runtime_error &e) {
        addError(Error("get_outputNeurons()",
                       QString(e.what())));
        //error_general("expected(std::vector<double>)",e);
    }
}
void                    BackpropNet::learn()
{    
    double filter = 0.999;
    std::chrono::high_resolution_clock::time_point __clock_end;
    std::chrono::duration<double> __clock_time_span;
    std::chrono::high_resolution_clock::time_point __clock_start = std::chrono::high_resolution_clock::now();

   /* if(__output_error.size() == 0)
    {
        __output_error = std::vector<double>(this->get_outputNeurons());
    }
    //std::vector<double>  __output_error(this->get_outputNeurons());

    if(__hidden_error.size() == 0)
    {
        __hidden_error = std::vector<std::vector<double>  >(this->get_hiddenNeuronsX(),std::vector<double>(this->get_hiddenNeuronsY(),0));
    }*/
    //std::vector<std::vector<double>  > __hidden_error(this->get_hiddenNeuronsX(),std::vector<double>(this->get_hiddenNeuronsY(),0));

    for(unsigned int y=0; y<this->get_outputNeurons(); y++)
    {
        __output_error[y] = derivative(this->get_ptr_outputNeuron(y)->get_netInput()) * _outputError[y];
    }
    double output   = 0;
    __clock_end     = std::chrono::high_resolution_clock::now();
    __clock_time_span = std::chrono::duration_cast<std::chrono::microseconds>(__clock_end - __clock_start);
    __dbg_time1     = __dbg_time1*filter+(1-filter)*__clock_time_span.count()*1000;
    __clock_start   = std::chrono::high_resolution_clock::now();
    if(this->hasHiddenLayer())
    {
        for(unsigned int x=this->get_hiddenNeuronsX(); x>0; x--)
        {
            for(unsigned int y=0; y<this->get_hiddenNeuronsY(); y++)
            {

                double weightError = 0;
                if(x == this->get_hiddenNeuronsX())
                {
                    for(unsigned int y2=0; y2<this->get_outputNeurons(); y2++)
                    {
                        weightError += __output_error[y2] * this->get_ptr_outputNeuron(y2)->get_weight(y);
                    }
                }
                else
                {
                    for(unsigned int y2=0; y2<this->get_hiddenNeuronsY(); y2++)
                    {
                        weightError += __hidden_error[x][y2] * this->get_ptr_hiddenNeuron(x,y2)->get_weight(y);
                    }
                }
                __hidden_error[x-1][y] = derivative(this->get_ptr_hiddenNeuron(x-1,y)->get_netInput()) * weightError;
            }
        }
        __clock_end = std::chrono::high_resolution_clock::now();
        __clock_time_span           = std::chrono::duration_cast<std::chrono::microseconds>(__clock_end - __clock_start);
        __dbg_time2 = __dbg_time2*filter+(1-filter)*__clock_time_span.count()*1000;
        __clock_start = std::chrono::high_resolution_clock::now();
    //----------INPUT CONNECTIONS
        for(unsigned int y=0; y<this->get_inputNeurons() + this->get_bias(); y++)
        {
            for(unsigned int y2=0; y2<this->get_hiddenNeuronsY(); y2++)
            {

                if(y == this->get_inputNeurons() + this->get_bias() -1)
                {
                    output = this->get_biasValue();
                }
                else
                {
                    output = this->get_input(y);
                }
                if( std::isnan(this->get_ptr_hiddenNeuron(0,y2)->get_weight(y) + (__hidden_error[0][y2] * _mutationFactor * output)))
                {
                    qDebug() << "nan";
                }
                this->get_ptr_hiddenNeuron(0,y2)->set_weight(y,this->get_ptr_hiddenNeuron(0,y2)->get_weight(y) + (__hidden_error[0][y2] * _mutationFactor * output));
            }
        }
        __clock_end = std::chrono::high_resolution_clock::now();
        __clock_time_span           = std::chrono::duration_cast<std::chrono::microseconds>(__clock_end - __clock_start);
        __dbg_time3 = __dbg_time3*filter+(1-filter)*__clock_time_span.count()*1000;
        __clock_start = std::chrono::high_resolution_clock::now();
    //}
    //----------HIDDEN CONNECTIONS
    //if(this->hasHiddenLayer())
    //{
        for(unsigned int x=1; x<this->get_hiddenNeuronsX(); x++)
        {
            for(unsigned int y=0; y<this->get_hiddenNeuronsY() + this->get_bias(); y++)
            {
                for(unsigned int y2=0; y2<this->get_hiddenNeuronsY(); y2++)
                {
                    if(y == this->get_hiddenNeuronsY() + this->get_bias() -1)
                    {
                        output = this->get_biasValue();
                    }
                    else
                    {
                        output = this->get_ptr_hiddenNeuron(x-1,y)->get_output();
                    }
                    this->get_ptr_hiddenNeuron(x,y2)->set_weight(y,this->get_ptr_hiddenNeuron(x,y2)->get_weight(y) + (__hidden_error[x][y2] * _mutationFactor * output));
                }
            }
        }
        __clock_end = std::chrono::high_resolution_clock::now();
        __clock_time_span           = std::chrono::duration_cast<std::chrono::microseconds>(__clock_end - __clock_start);
        __dbg_time4 = __dbg_time4*filter+(1-filter)*__clock_time_span.count()*1000;
        __clock_start = std::chrono::high_resolution_clock::now();
    }
    //----------OUTPUT CONNECTIONS
    if(!this->hasHiddenLayer())
    {
        for(unsigned int y=0; y<this->get_inputNeurons() + this->get_bias(); y++)
        {
            for(unsigned int y2=0; y2<this->get_outputNeurons(); y2++)
            {
                if(y == this->get_inputNeurons() + this->get_bias() -1)
                {
                    output = this->get_biasValue();
                }
                else
                {
                    output = this->get_input(y);
                }
                this->get_ptr_outputNeuron(y2)->set_weight(y,this->get_ptr_outputNeuron(y2)->get_weight(y) + (__output_error[y2] * _mutationFactor * output));
            }
        }
        __clock_end = std::chrono::high_resolution_clock::now();
        __clock_time_span           = std::chrono::duration_cast<std::chrono::microseconds>(__clock_end - __clock_start);
        __dbg_time5 = __dbg_time5*filter+(1-filter)*__clock_time_span.count()*1000;
        __clock_start = std::chrono::high_resolution_clock::now();
    }
    else
    {
        for(unsigned int y=0; y<this->get_hiddenNeuronsY() + this->get_bias(); y++)
        {
            for(unsigned int y2=0; y2<this->get_outputNeurons(); y2++)
            {
                double output = 0;
                if(y == this->get_hiddenNeuronsY() + this->get_bias() -1)
                {
                    output = this->get_biasValue();
                }
                else
                {
                    output = this->get_ptr_hiddenNeuron(this->get_hiddenNeuronsX()-1,y2)->get_output();
                }
                this->get_ptr_outputNeuron(y2)->set_weight(y,this->get_ptr_outputNeuron(y2)->get_weight(y) + (__output_error[y2] * _mutationFactor * output));
            }
        }
        __clock_end = std::chrono::high_resolution_clock::now();
        __clock_time_span           = std::chrono::duration_cast<std::chrono::microseconds>(__clock_end - __clock_start);
        __dbg_time6 = __dbg_time6*filter+(1-filter)*__clock_time_span.count()*1000;
        __clock_start = std::chrono::high_resolution_clock::now();
    }
}
void                    BackpropNet::learn(std::vector<double> expected)
{
    this->set_expected(expected);
    this->learn();
}

void                    BackpropNet::calc_netError()
{
    if(_expected.size() == 0)
    {
        addError(Error("calc_netError()",QString("no expected output values defined. first set them by BackpropNet::set_expected(std::vector<double>)")));
        return;
        //error_general("calc_netError()","no expected output values defined. first set them by BackpropNet::expected(std::vector<double>)");
    }
    _outputError = std::vector<double> (this->get_outputNeurons(),0);
    _netError    = 0;

    for(unsigned int y=0; y<this->get_outputNeurons(); y++)
    {
        _outputError[y] =_expected[y] - this->get_output(y);
        _netError      += _outputError[y];
    }
}
double                  BackpropNet::derivative(double netinput)
{
    double derivative = 0;
    switch(this->get_activationFunction())
    {
        case Activation::Binary:
        {
            addError(Error("derivative(double ["+QString::number(netinput)+"])",
                           "you cant use this activation function: Binary for this learn algorithm"));
            return 0;
            //error_general("derivative(double ["+QString::number(netinput)+"] )","you cant use this activation function: Binary for this learn algorithm");
        }
        case Activation::Gaussian:
        {
            derivative = Neuron::deriv_activation_Gaussian(netinput);
            break;
        }
        case Activation::Linear:
        {
            derivative = Neuron::deriv_activation_Linear(netinput);
            break;
        }
        case Activation::ReLu:
        {
            derivative = Neuron::deriv_activation_ReLu(netinput);
            break;
        }
        case Activation::Sigmoid:
        {
            derivative = Neuron::deriv_activation_Sigmoid(netinput);
            break;
        }
    }
    return derivative;
}

void                    BackpropNet::set_costumNeurons(unsigned int costum)
{
    printIllegalFunctionMessage("set_costumNeurons(unsigned int ["+QString::number(costum)+"])");
}
unsigned int            BackpropNet::get_costumNeurons()
{
    printIllegalFunctionMessage("get_costumNeurons()");
    return 0;
}
unsigned int            BackpropNet::get_costumConnections()
{
    printIllegalFunctionMessage("get_costumConnections()");
    return 0;
}
void                    BackpropNet::set_costumConnections(unsigned int connections)
{
    printIllegalFunctionMessage("set_costumConnections(unsigned int ["+QString::number(connections)+"])");
}
/*void                 BackpropNet::set_neurons(unsigned int neurons,unsigned int hiddenNeurons,unsigned int outputNeurons,unsigned int costumNeurons)
{
    printIllegalFunctionMessage("set_neurons(unsigned int ["+QString::number(neurons)+"] ,unsigned int ["+QString::number(hiddenNeurons)+"] ,unsigned int ["+QString::number(outputNeurons)+"] ,unsigned int ["+QString::number(costumNeurons)+"] )");
}*/
Neuron              *BackpropNet::get_ptr_costumNeuron(NeuronID ID)
{
    printIllegalFunctionMessage("get_ptr_costumNeuron(NeuronID ["+Neuron::toIDString(ID)+"])");
    return nullptr;
}
std::vector<Neuron*>      *BackpropNet::get_ptr_costumNeuron()
{
    printIllegalFunctionMessage("get_ptr_costumNeuron()");
    return nullptr;
}
void                 BackpropNet::addConnection(NeuronID fromNeuron,NeuronID toNeuron,ConnectionDirection direction)
{
    printIllegalFunctionMessage("addConnection(NeuronID ["+Neuron::toIDString(fromNeuron)+"],NeuronID ["+Neuron::toIDString(toNeuron)+"],ConnectionDirection ["+Neuron::toDirectionString(direction)+"])");
}
void                 BackpropNet::addConnection(Connection connection)
{
    printIllegalFunctionMessage("addConnection(Connection connection)");
}
void                 BackpropNet::addConnection(std::vector<Connection> connections)
{
    printIllegalFunctionMessage("addConnection(std::vector<Connection> connections)");
}
/*void                 BackpropNet::set_ptr_connectionList(std::vector<Connection> *connections)
{
    printIllegalFunctionMessage("set_ptr_connectionList(std::vector<Connection> *connections)");
}*/
/*std::vector<Connection>  *BackpropNet::get_ptr_connectionList()
{
    printIllegalFunctionMessage("get_ptr_connectionList()");
    return nullptr;
}*/
void                BackpropNet::clearConnectionList()
{
    printIllegalFunctionMessage("clearConnectionList()");
}
NeuronID            BackpropNet::addNeuron()
{
    printIllegalFunctionMessage("addNeuron()");
    return NeuronID();
}
NeuronID            BackpropNet::addNeuron(Neuron *neuron)
{
    printIllegalFunctionMessage("addNeuron(Neuron *neuron)");
    return NeuronID();
}
NeuronID            BackpropNet::addNeuron(Connection connection)
{
    printIllegalFunctionMessage("addNeuron(Connection ["+Neuron::toConnectionString(connection)+"])");
    return NeuronID();
}
NeuronID            BackpropNet::addNeuron(std::vector<Connection> inputConnections)
{
    printIllegalFunctionMessage("addNeuron(std::vector<Connection> inputConnections)");
    return NeuronID();
}
void                BackpropNet::clearErrors()
{
    _errorList.clear();
}
Error               BackpropNet::get_lastError() const
{
    if(static_cast<unsigned int>(_errorList.size()) == 0)
    {
        return Error("get_lastError",ErrorMessage::listIsEmpty("_errorList"));
    }
    return _errorList[_errorList.size()-1];
}
Error               BackpropNet::get_error(unsigned int index)
{
    if(static_cast<unsigned int>(_errorList.size()) == 0)
    {
        return Error("get_error",ErrorMessage::listIsEmpty("_errorList"));
    }
    if(index >= static_cast<unsigned int>(_errorList.size()))
    {
        return Error("get_error",ErrorMessage::outOfRange<unsigned int>('[',0,index,unsigned(_errorList.size()-1),']'));
    }
    return _errorList[signed(index)];
}
ErrorList           BackpropNet::get_errorList() const
{
    return _errorList;
}
unsigned int        BackpropNet::get_errorAmount() const
{
    return unsigned(_errorList.size());
}

void BackpropNet::onNetConfigurationUpdate()
{
    __hidden_error.clear();
    __output_error.clear();
}
void BackpropNet::onNetConfigurationUpdateFinished()
{
    __output_error = std::vector<double>(this->get_outputNeurons());
    __hidden_error = std::vector<std::vector<double>  >(this->get_hiddenNeuronsX(),std::vector<double>(this->get_hiddenNeuronsY(),0));
}


void BackpropNet::printIllegalFunctionMessage(QString func)
{
    addError(Error(func,QString("THIS FUNCTION IS NOT SUPPORTED IN THE BACKPROBNET")));
    __DEBUG_BACKPROPNET(this,func,"THIS FUNCTION IS NOT SUPPORTED IN THE BACKPROBNET");
}
void        BackpropNet::init()
{
    _netError = 0;
    _needsCalculationUpdate = true;
    _needsConfigurationUpdate = true;
    this->set_netFileName("netFile");
    this->set_netFileEnding("bnet");
    connect(this,SIGNAL(netConfigurationUpdateStarted()),this,SLOT(onNetConfigurationUpdate()));
    connect(this,SIGNAL(netConfigurationUpdated()),this,SLOT(onNetConfigurationUpdateFinished()));
}
void        BackpropNet::addError(const Error &e)
{
    _errorList.push_back(e);
    _errorList[_errorList.size()-1].setNamespace("BackpropNet::"+_errorList[_errorList.size()-1].getNamespace());
    Error::print(_errorList[_errorList.size()-1]);
    emit errorOccured(this->get_ID(),_errorList[_errorList.size()-1]);
}
/*

QString BackpropNet::error_paramOutOfRange(unsigned int paramPos,QString value,QString min, QString max)
{
    return " parameter "+QString::number(paramPos)+" is out of range: "+value+"     minimum is: "+min+"     maximum is: "+max;
}
QString BackpropNet::error_paramOutOfRange(unsigned int paramPos,unsigned int value,unsigned int min, unsigned int max)
{
    return error_paramOutOfRange(paramPos,QString::number(value),QString::number(min),QString::number(max));
}
QString BackpropNet::error_paramOutOfRange(unsigned int paramPos,int value,int min, int max)
{
    return error_paramOutOfRange(paramPos,QString::number(value),QString::number(min),QString::number(max));
}
QString BackpropNet::error_paramOutOfRange(unsigned int paramPos,double value,double min, double max)
{
    return error_paramOutOfRange(paramPos,QString::number(value),QString::number(min),QString::number(max));
}
void        BackpropNet::error_general(QString function, QString cause)
{
    throw std::runtime_error("ERROR: BackpropNet::" + function + "     " + cause);
}
void        BackpropNet::error_general(QString function, std::runtime_error &e)
{
    error_general(function,"",e);
}
void        BackpropNet::error_general(QString function, QString cause, std::runtime_error &e)
{
    QString error = "ERROR: BackpropNet::" + function + "     " + cause;
    error += "      --> "+QString(e.what());
    throw std::runtime_error(error);
}
*/
