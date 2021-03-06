#ifndef GENETICNET_H
#define GENETICNET_H
//                      Autor   Alex Krieg
#define    GENETICNET_VERSION "02.07.02"
//                      Datum  04.10.2020

#include "net.h"
#include "savenet.h"

//#include <QDebug>


#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include <ctime>
#include <ratio>
#include <chrono>

#ifdef QT_APP
  #include <QDebug>
  #include <QObject>
#endif

#ifdef QDEBUG_H
#ifndef CONSOLE
#define CONSOLE qDebug()
#endif
#else
#include <iostream>
#include <stdio.h>
#ifndef CONSOLE
#define CONSOLE std::cout
#endif
#endif

#define GENETICNET_MIN_ANIMALS 2
#define GENETICNET_MAX_ANIMALS 1000

#define __enableGeneticNetThread
//#define __DEBUG_TIMEINTERVAL
//#define __DEBUG_TIMEINTERVAL_IN_THREAD
#define __DEBUG_GENETICNET

struct thread_data_geneticNet {
   int  thread_id;
   Net *net;
   bool *exit;
   bool *pause;
   pthread_mutex_t *lock;
   pthread_cond_t *condition_var;
   bool isPaused;
   //long *delayMicros;
   unsigned int debugParam;
};
#ifdef QT_APP
class GeneticNet    : public QObject
#else
class GeneticNet
#endif
{
        #ifdef QT_APP
            Q_OBJECT
        #endif
    public:
#ifdef QT_APP
        GeneticNet(QObject *parent = nullptr);
        GeneticNet(unsigned int animals,
                   QObject *parent = nullptr);
        GeneticNet(unsigned int animals,
                   unsigned int inputs,
                   unsigned int hiddenX,
                   unsigned int hiddenY,
                   unsigned int outputs,
                   QObject *parent = nullptr);
        GeneticNet(unsigned int animals,
                   unsigned int inputs,
                   unsigned int hiddenX,
                   unsigned int hiddenY,
                   unsigned int outputs,
                   bool enableBias,
                   bool enableAverage,
                   Activation func,
                   QObject *parent = nullptr);
#else
        GeneticNet();
        GeneticNet(unsigned int animals);
        GeneticNet(unsigned int animals,
                   unsigned int inputs,
                   unsigned int hiddenX,
                   unsigned int hiddenY,
                   unsigned int outputs);
        GeneticNet(unsigned int animals,
                   unsigned int inputs,
                   unsigned int hiddenX,
                   unsigned int hiddenY,
                   unsigned int outputs,
                   bool enableBias,
                   bool enableAverage,
                   Activation func);
#endif
        ~GeneticNet();

        void                    set(unsigned int animals,
                                    unsigned int inputs,
                                    unsigned int hiddenX,
                                    unsigned int hiddenY,
                                    unsigned int outputs,
                                    bool enableBias,
                                    bool enableAverage,
                                    Activation func);

        void                    set_netFileName(std::string filename);
        std::string             get_netFileName();
        void                    set_netFileEnding(std::string fileEnding);
        std::string             get_netFileEnding();
        void                    loadFromNetFile();
        void                    loadFromNetFile(std::string filename);
        void                    loadFromNetFile(std::string filename,std::string fileEnding);
        void                    saveToNetFile();
        void                    saveToNetFile(std::string filename);
        void                    saveToNetFile(std::string filename,std::string fileEnding);

        void                    set_animals(unsigned int animals);
        unsigned int            get_animals();
        void                    set_inputNeurons(unsigned int inputs);
        unsigned int            get_inputNeurons();
        void                    set_hiddenNeuronsX(unsigned int hiddenX);
        unsigned int            get_hiddenNeuronsX();
        void                    set_hiddenNeuronsY(unsigned int hiddenY);
        unsigned int            get_hiddenNeuronsY();
        void                    set_outputNeurons(unsigned int outputs);
        unsigned int            get_outputNeurons();
        unsigned int            get_costumNeurons();


        void                    set_bias(bool enableBias);
        bool                    get_bias();
        void                    set_enableAverage(bool enableAverage);
        bool                    get_enableAverage();
        void                    set_biasValue(double value);
        double                  get_biasValue();
        void                    set_activationFunction(Activation func);
        Activation              get_activationFunction();
        bool                    hasHiddenLayer();

        void                    set_randomGenom();
        void                    set_genom(std::vector<std::vector<double>    > genomList);
        void                    set_genom(unsigned int animal, std::vector<double> genom);
        std::vector<double>      get_genom(unsigned int animal);
        std::vector<std::vector<double>  >get_genom();
        unsigned int            get_genomsize();
        void                    set_genomFromNetFile();

        void                    set_input(unsigned int animal, unsigned int input, double signal);
        double                   get_input(unsigned int animal, unsigned int input);
        void                    set_input(unsigned int animal, std::vector<double> inputList);
        std::vector<double>      get_input(unsigned int animal);
        void                    set_input(std::vector<std::vector<double>    > input);
        std::vector<std::vector<double>  >get_input();

        double                   get_hidden(unsigned int animal, unsigned int hiddenX, unsigned int hiddenY);
        std::vector<double>      get_hidden(unsigned int hiddenX, unsigned int hiddenY);
        std::vector<double>      get_hiddenX(unsigned int animal, unsigned int hiddenX);  // |    Alle in einer Spalte
        std::vector<std::vector<double>  >get_hiddenX(unsigned int hiddenX);
        std::vector<double>      get_hiddenY(unsigned int animal, unsigned int hiddenY);  // --   Alle in einer Reihe
        std::vector<std::vector<double>  >get_hiddenY(unsigned int hiddenY);

        Neuron                 *get_ptr_hiddenNeuron(unsigned int animal, unsigned int hiddenX, unsigned int hiddenY);
        std::vector<Neuron*>     get_hiddenNeuronX_ptr(unsigned int animal, unsigned int hiddenX);    // |    Alle in einer Spalte
        std::vector<Neuron*>     get_hiddenNeuronY_ptr(unsigned int animal, unsigned int hiddenY);    // --   Alle in einer Reihe
        std::vector<std::vector<Neuron*> > get_hiddenNeuron_ptr(unsigned int animal);
        Neuron                 *get_ptr_outputNeuron(unsigned int animal, unsigned int output);
        std::vector<Neuron*>   *get_ptr_outputNeuron(unsigned int animal);


        double                   get_output(unsigned int animal, unsigned int output);
        std::vector<double>      get_output(unsigned int animal);
        std::vector<std::vector<double>  >get_output();


        void                    run();
        void                    run(unsigned int animal);



        Net *operator[](unsigned int animal);
        std::vector<Net*>   get_netList_ptr();
        std::vector<Net*>   *get_ptr_netList_ptr();


        void                set_mutationFactor(double factor);
        void                set_mutationChangeWeight(double weight);

        void                set_score(unsigned int animal, double score);
        double               get_score(unsigned int animal);
        void                set_score(std::vector<double>   scoreList);
        std::vector<double>  get_score();

        void                learn(std::vector<double>   scoreList);
        void                learn();

        void                updateNetConfiguration();
        /*  Needed after every change in the Net-structure like
         *  inputNeurons()
         *  sins V02.03.00
         */
        void                addConnection(NeuronID fromNeuron,NeuronID toNeuron,ConnectionDirection direction = ConnectionDirection::forward);
        void                addConnection(Connection connection);
        void                addConnection(std::vector<Connection> connections);
        void                set_connectionList(std::vector<std::vector<Connection> >connections);

        NeuronID            addNeuron();
        NeuronID            addNeuron(Neuron *neuron);

        std::string             toString();
        std::vector<std::string>         toStringList();



        SaveNet            *get_ptr_saveNet();

#if defined(_DEBUG_NET_TIMING)
        double get_runtime();
        /* Retruns the time, it takes to update the net.

          ERROR:
           | none
         */
#endif
        void                clearErrors();
        Error               get_lastError() const;
        Error               get_error(unsigned int index);
        ErrorList           get_errorList() const;
        unsigned int        get_errorAmount() const;
#ifdef QT_APP
    signals:
        void errorOccured(Error &e);
        void netConfigurationUpdateNeeded(Net *net); //Trigger, for updating the netConfiguration
        void netConfigurationUpdateStarted(Net *net); //Trigger, when the updating function gets called
        void netConfigurationUpdated(Net *net);      //Infosignal when the updating is finished
        void accessLock(Net *net);                    //do not access functions like: get_input() ... otherwise this error will be shown: "Update required: call updateNetConfiguration() first!"
        void accessUnlock(Net *net);                  //from now on you can access all functions again

        void runDone(Net *net);
        void biasValueChanged(Net *net);
        void weightValuesChanged(Net *net);
    private slots:
        void onNetError(unsigned int netID,Error &e);

        void onNetConfigurationUpdateNeeded(Net *net);
        void onNetConfigurationUpdateStarted(Net *net);
        void onNetConfigurationUpdated(Net *net);
        void onAccessLock(Net *net);
        void onAccessUnlock(Net *net);

        void onRunDone(Net *net);
        void onBiasValueChanged(Net *net);
        void onWeightValuesChanged(Net *net);

#endif
    private:

        void                init(unsigned int animals,
                                 unsigned int inputs,
                                 unsigned int hiddenX,
                                 unsigned int hiddenY,
                                 unsigned int outputs,
                                 bool enableBias,
                                 bool enableAverage,
                                 Activation func);
#ifdef QT_APP
        void connectSignalsFromNet(Net *net);
#endif

        void learn_selectAnimal(double gesScore,unsigned int &selection1,unsigned int &selection2);
        void learn_crossover(unsigned int selection1,unsigned int selection2,std::vector<double> &newGen1,std::vector<double> &newGen2);
        void learn_mutate(std::vector<double> &genom);

        //Threads
        static void *runThread(void *threadarg);
        static void *runThread_setupNet(void *threadarg);
        void addError(const Error &e);
       /* //----------ERROR
        std::string error_paramOutOfRange(unsigned int paramPos,std::string value,std::string min, std::string max);
        std::string error_paramOutOfRange(unsigned int paramPos,unsigned int value,unsigned int min, unsigned int max);
        std::string error_paramOutOfRange(unsigned int paramPos,int value,int min, int max);
        std::string error_paramOutOfRange(unsigned int paramPos,double value,double min, double max);
        void        error_general(std::string function, std::string cause);
        void        error_general(std::string function, std::runtime_error &e);
        void        error_general(std::string function, std::string cause, std::runtime_error &e);
        //---------------
*/
        bool         _needsCalculationUpdate;

        unsigned int _animals;
        unsigned int _currentAnimal;

        double        _mutationFactor;
        double        _mutationChangeWeight;

        std::vector<Net*>    _netList;
        std::vector<double>  _scoreList;

        std::default_random_engine          _randEngine;

        SaveNet _saveNet;

        //Threads
        pthread_mutex_t _threadLock                     =PTHREAD_MUTEX_INITIALIZER;
        pthread_cond_t  _thread_condition_var           = PTHREAD_COND_INITIALIZER;

        pthread_mutex_t _threadLock_setupNet            =PTHREAD_MUTEX_INITIALIZER;
        pthread_cond_t  _thread_condition_var_setupNet  = PTHREAD_COND_INITIALIZER;
        std::vector<thread_data_geneticNet> _threadData;
        std::vector<pthread_t>  _threadList;
        std::vector<thread_data_geneticNet> _threadData_setupNet;
        std::vector<pthread_t>  _threadList_setupNet;
        bool                    _threadExit;
        bool                    _threadPause;
    //  long                    _threadDelayMicros;

        unsigned int _debugCount;

        ErrorList _errorList;

#ifdef _DEBUG_NET_TIMING
        std::chrono::high_resolution_clock::time_point __debug_timer1_start;
        std::chrono::high_resolution_clock::time_point __debug_timer1_end;

        std::chrono::high_resolution_clock::time_point __debug_timer2_start;
        std::chrono::high_resolution_clock::time_point __debug_timer2_end;

        std::chrono::duration<double> __debug_time_span;
        double __debug_run_time;
#endif
};
#endif // GENETICNET_H
