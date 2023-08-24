#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <chrono>
#include <atomic>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#include "weights.h"

using namespace std;

weights w(8, 2, 5);
struct Lock{
    sem_t mutex;

    Lock(){
        sem_init(&mutex, 0, 1);
    }
};


Lock L;
int layerNo;
std::atomic<int> threads_complete;

struct inputs_of_threads{
int layerNo;
int pid;
int size_of_neurons;
double *prevNeuron;
double *newNeuron;

};


void* calculations(void* arg){
    
    
    // use sempahore 

    sem_wait(&L.mutex);

    
        
        inputs_of_threads* temp = (inputs_of_threads*) arg;

        cout << "Executing Neuron with pid: " << temp->pid << endl;

        if(temp->layerNo <= 1)
        {
            double** weights_of_inputs;
            double* input_vals = w.getInputValues();
            weights_of_inputs = w.getInputLayer();

            cout << "hello2" << endl;
            for(int i = 0; i < temp->size_of_neurons; i++)
            {
                for(int j = 0; j < 2; j++)
                temp->newNeuron[temp->pid] += input_vals[j] *  weights_of_inputs[j][i%w.getnumOfHiddenLayers()];
            }
            cout << "DONEEEEEEE LAYER 1" << endl;
        }
        else
        {
            double** weights_of_prev;
            weights_of_prev = w.getHiddenLayer(temp->layerNo%w.getnumOfHiddenLayers());
            cout << "hello4" << endl;
            for(int i = 0; i < temp->size_of_neurons; i++)
            {
                temp->newNeuron[temp->pid] += temp->prevNeuron[i] *  weights_of_prev[i%w.getNoofNeurons()][temp->pid%w.getNoofNeurons()];
            }
            cout << "DONEEEEEEEEE LAYER" << endl;
        }
        
        threads_complete++;
        sem_post(&L.mutex);
        
        pthread_exit(NULL);
    
}

int main()
{
    w.setHiddenLayers(hidden);
    w.setInput(inputs);
    w.setInputLayer(input);
    w.setOutputLayer(output);

    pthread_t tid[8];
    inputs_of_threads abc[8];
    threads_complete = 0;
    double* prevNeuron;

    for(int i = 0; i<8; i++)
    {
        layerNo++;
        if(layerNo <= 1)
        {
            prevNeuron = new double[2];
            prevNeuron[0] = 0.1;
            prevNeuron[1] = 0.2;
        }
        else{
            prevNeuron = new double[8];
            prevNeuron[0] = 0.1;
            prevNeuron[1] = 0.2;
            prevNeuron[2] = 0.1;
            prevNeuron[3] = 0.2;
            prevNeuron[4] = 0.1;
            prevNeuron[5] = 0.2;
            prevNeuron[6] = 0.1;
            prevNeuron[7] = 0.2;
        }

        double* newNeuron = new double[8];
        newNeuron[0] = 0.0;
        newNeuron[1] = 0.0;
        newNeuron[2] = 0.0;
        newNeuron[3] = 0.0;
        newNeuron[4] = 0.0;
        newNeuron[5] = 0.0;
        newNeuron[6] = 0.0;
        newNeuron[7] = 0.0;

        abc[i].layerNo = layerNo;
        abc[i].prevNeuron = prevNeuron;
        abc[i].newNeuron = newNeuron;
        abc[i].pid = i;
        abc[i].size_of_neurons = 8;

        pthread_create(&tid[i],NULL,calculations,&abc[i%5]);
        
    }

    while(threads_complete < 8);
}
