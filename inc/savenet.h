#ifndef SAVENET_H
#define SAVENET_H
//              Autor   Alex Krieg
#define SAVENET_VERSION "02.02.01"
//              Datum   09.02.2020

#include <net.h>
#include <time.h>

#define _DEBUG_READ_TIMING

class SaveNet
{
    public:
        SaveNet();
        ~SaveNet();

        void            set_filename(QString filename);
        QString         get_filename();
        void            set_fileEnding(QString fileEnding);
        QString         get_fileEnding();
        void            set_inputNeurons(unsigned int inputs);
        unsigned int    get_inputNeurons();
        void            set_hiddenNeuronsX(unsigned int hiddenX);
        unsigned int    get_hiddenNeuronsX();
        void            set_hiddenNeuronsY(unsigned int hiddenY);
        unsigned int    get_hiddenNeuronsY();
        void            set_outputNeurons(unsigned int outputs);
        unsigned int    get_neurons();
        unsigned int    get_outputNeurons();
        unsigned int    get_hiddenNeurons();
        unsigned int    get_costumNeurons();
        unsigned int    get_connections();
        unsigned int    get_costumConnections();


        void            set_bias(bool bias);
        bool            get_bias();
        void            set_biasValue(double value);
        double          get_biasValue();
        void            set_enableAverage(bool average);
        bool            get_enableAverage();
        void            set_activationFunction(Activation func);
        Activation      get_activationFunction();
        void            set_ExtraParam(QString name,double value);
        void            get_ExtraParam(QString name, double &value);
        void            set_ExtraParam(std::vector<QString> name,std::vector<double> value);
        void            get_ExtraParam(std::vector<QString> &name,std::vector<double> &value);


        bool            loadFile();
        bool            loadFile(QString filename);
        bool            saveFile();
        bool            saveFile(QString filename);

      //  void setGenom(std::vector<double>   genom);
      //  void setGenom(unsigned int index,std::vector<double>   genom);
      //  void setGenom(std::vector<std::vector<double>   > genomList);
      //  void addGenom(std::vector<double>   genom);
      //  void addGenom(std::vector<std::vector<double>   > genomList);
        std::vector<double>           get_genom(unsigned int ID);
        std::vector<std::vector<double>  >  get_genom();
        unsigned int            get_animals();

        void clear();
        void set(unsigned int inputs,
                 unsigned int hiddenX,
                 unsigned int hiddenY,
                 unsigned int outputs,
                 bool         bias,
                 bool         average,
                 Activation   activationFunction,
                 double        biasValue);
        void            set_ptr_neuronsOfNet(unsigned int ID,std::vector<Neuron*> *neurons);
        std::vector<Neuron*>  get_neuronsOfNet(unsigned int ID);

        std::vector<Connection>           *get_ptr_connectionList(unsigned int animal);
        std::vector<std::vector<Connection> >   *get_ptr_connectionList();


        void clearGenomList();
        void clearExternParam();

        void            clearErrors();
        Error           get_lastError() const;
        Error           get_error(unsigned int index);
        ErrorList       get_errorList() const;
        unsigned int    get_errorAmount() const;

    private:
        void            checkParam();
        unsigned int    get_genomsize();
        void            saveGenomOfNet(unsigned int ID);

        void addError(const Error &e);

     /*   //----------ERROR
        QString error_paramOutOfRange(unsigned int paramPos,QString value,QString min, QString max);
        QString error_paramOutOfRange(unsigned int paramPos,unsigned int value,unsigned int min, unsigned int max);
        QString error_paramOutOfRange(unsigned int paramPos,int value,int min, int max);
        QString error_paramOutOfRange(unsigned int paramPos,double value,double min, double max);
        void        error_general(QString function, QString cause);
        void        error_general(QString function, std::runtime_error &e);
        void        error_general(QString function, QString cause, std::runtime_error &e);
        //---------------
*/
        QString _filename;
        QString _fileEnding;
        unsigned int _saves;
        FILE *_file;

        unsigned int _animals;
        unsigned int _inputs;
        unsigned int _hiddenX;
        unsigned int _hiddenY;
        unsigned int _outputs;
        bool         _bias;
        bool         _average;
        double        _biasValue;
        Activation   _activationFuncton;
        unsigned int _neurons;
        unsigned int  _hiddenNeurons;
        unsigned int  _outputNeurons;
        unsigned int  _costumNeurons;
        unsigned int _connections;
        unsigned int  _costumConnections;
        std::vector<QString> _extraParamName;
        std::vector<double>       _extraParamValue;

        std::vector<std::vector<double>  >_genomList;
        unsigned int _genomsize;
        // std::vector<std::vector<Net>    >_netList;

        bool        _check_inputs;
        bool        _check_hiddenX;
        bool        _check_hiddenY;
        bool        _check_outputs;
        bool        _check_bias;
        bool        _check_biasValue;
        bool        _check_average;
        bool        _check_activationFunction;
        bool        _check_neurons;
        bool        _check_hiddenNeurons;
        bool        _check_outputNeurons;
        bool        _check_costumNeuron;
        bool        _check_connections;
        bool        _check_costumConnections;

        std::vector<std::vector<Neuron*>    > _neuronList;
        std::vector<std::vector<Connection> > _connectionListFromFile;
        std::vector<unsigned int>             _ID_list;


        ErrorList _errorList;

#ifdef _DEBUG_READ_TIMING
        std::chrono::high_resolution_clock::time_point __debug_timer1_start;
        std::chrono::high_resolution_clock::time_point __debug_timer1_end;

        std::chrono::duration<double> __debug_time_span;
        double __debug_run_time;
        const double debugInterval = 0.1; //sec
#endif
};
#endif // SAVENET_H
