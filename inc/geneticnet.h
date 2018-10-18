#ifndef GENETICNET_H
#define GENETICNET_H
//                      Autor   Alex Krieg
#define    BACKPROPNET_VERSION "02.01.00"
//                      Datum   18.10.2018

#include "net.h"
#include <QDebug>
#define GENETICNET_MIN_ANIMALS 2
#define GENETICNET_MAX_ANIMALS 500

class GeneticNet
{
    public:
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


        void                    animals(unsigned int animals);
        unsigned int            animals();
        void                    inputNeurons(unsigned int inputs);
        unsigned int            inputNeurons();
        void                    hiddenNeuronsX(unsigned int hiddenX);
        unsigned int            hiddenNeuronsX();
        void                    hiddenNeuronsY(unsigned int hiddenY);
        unsigned int            hiddenNeuronsY();
        void                    outputNeurons(unsigned int outputs);
        unsigned int            outputNeurons();

        void                    bias(bool enableBias);
        bool                    bias();
        void                    enableAverage(bool enableAverage);
        bool                    enableAverage();
        void                    biasValue(float value);
        float                   biasValue();
        void                    activationFunction(Activation func);
        Activation              activationFunction();
        bool                    noHiddenLayer();

        void                    randomGenom();
        void                    genom(std::vector<std::vector<float>    > genomList);
        void                    genom(unsigned int animal, std::vector<float> genom);
        std::vector<float>      genom(unsigned int animal);
        std::vector<std::vector<float>  >genom();
        unsigned int            genomsize();

        void                    input(unsigned int animal, unsigned int input, float signal);
        float                   input(unsigned int animal, unsigned int input);
        void                    input(unsigned int animal, std::vector<float> inputList);
        std::vector<float>      input(unsigned int animal);
        void                    input(std::vector<std::vector<float>    > input);
        std::vector<std::vector<float>  >input();

        float                   hidden(unsigned int animal, unsigned int hiddenX, unsigned int hiddenY);
        std::vector<float>      hidden(unsigned int hiddenX, unsigned int hiddenY);
        std::vector<float>      hiddenX(unsigned int animal, unsigned int hiddenX);  // |    Alle in einer Spalte
        std::vector<std::vector<float>  >hiddenX(unsigned int hiddenX);
        std::vector<float>      hiddenY(unsigned int animal, unsigned int hiddenY);  // --   Alle in einer Reihe
        std::vector<std::vector<float>  >hiddenY(unsigned int hiddenY);

        Neuron                 *hiddenNeuron(unsigned int animal, unsigned int hiddenX, unsigned int hiddenY);
        std::vector<Neuron*>     hiddenNeuronX(unsigned int animal, unsigned int hiddenX);    // |    Alle in einer Spalte
        std::vector<Neuron*>     hiddenNeuronY(unsigned int animal, unsigned int hiddenY);    // --   Alle in einer Reihe
        std::vector<std::vector<Neuron*> > *hiddenNeuron(unsigned int animal);
        Neuron                 *outputNeuron(unsigned int animal, unsigned int output);
        std::vector<Neuron*>   *outputNeuron(unsigned int animal);


        float                   output(unsigned int animal, unsigned int output);
        std::vector<float>      output(unsigned int animal);
        std::vector<std::vector<float>  >output();


        void                    run();
        void                    run(unsigned int animal);



        Net *operator[](unsigned int animal);

        void                mutationFactor(float factor);
        void                mutationChangeWeight(float weight);

        void                score(unsigned int animal, float score);
        float               score(unsigned int animal);
        void                score(std::vector<float>   scoreList);
        std::vector<float>  score();

        void                learn(std::vector<float>   scoreList);
        void                learn();
    private:

        void                    init(unsigned int animals,
                                     unsigned int inputs,
                                     unsigned int hiddenX,
                                     unsigned int hiddenY,
                                     unsigned int outputs,
                                     bool enableBias,
                                     bool enableAverage,
                                     Activation func);

        void learn_selectAnimal(float gesScore,unsigned int &selection1,unsigned int &selection2);
        void learn_crossover(std::vector<float> oldGen1,std::vector<float> oldGen2,std::vector<float> &newGen1,std::vector<float> &newGen2);
        void learn_mutate(std::vector<float> &genom);

        //----------ERROR
        std::string error_paramOutOfRange(unsigned int paramPos,std::string value,std::string min, std::string max);
        std::string error_paramOutOfRange(unsigned int paramPos,unsigned int value,unsigned int min, unsigned int max);
        std::string error_paramOutOfRange(unsigned int paramPos,int value,int min, int max);
        std::string error_paramOutOfRange(unsigned int paramPos,float value,float min, float max);
        void        error_general(std::string function, std::string cause);
        void        error_general(std::string function, std::runtime_error &e);
        void        error_general(std::string function, std::string cause, std::runtime_error &e);
        //---------------

        bool         _update;

        unsigned int _animals;
        unsigned int _currentAnimal;

        float        _mutationFactor;
        float        _mutationChangeWeight;

        std::vector<Net*>    _netList;
        std::vector<float>  _scoreList;

        std::default_random_engine          _randEngine;


};
#endif // GENETICNET_H