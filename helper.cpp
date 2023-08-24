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

#define read_end 0
#define write_end 1

using namespace std;

int chartoInt(char* num){
    
    int ans = 0; 
    for(int i = 0; num[i]; i++){
        
        ans *= 10;
        ans += (num[i] - '0');
    }

    return ans;
}
std::atomic<int> threads_complete = 0;

struct Lock{
    sem_t mutex;

    Lock(){
        sem_init(&mutex, 0, 1);
    }
};


Lock L;
// weights w(8, 2, 5);
weights w;

struct argThreads{
    double* prevNuerons;
    double* currentNeurons;
    int curlayerNo;
    int pid;
    int sizeOfNeurons;   
    int Tlayers;

    argThreads(double* PN, double* CN, int CLN, int pid, int SZ, int TL){
        sizeOfNeurons = SZ;
        this->pid = pid;
        curlayerNo = CLN;
        prevNuerons = PN;
        currentNeurons = CN;
        Tlayers = TL;

    }    
};
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

        // cout << "Executing Neuron with pid: " << temp->pid << endl;

        if(temp->layerNo < 0)
        {
            double** weights_of_inputs = w.getInputLayer();
            double* input_vals = w.getInputValues();
            
            

            
                for(int j = 0; j < 2; j++) {
                    
                    temp->newNeuron[temp->pid] += input_vals[j] *  weights_of_inputs[j][temp->pid%w.getNoofNeurons()];
                }
            
        }
        else
        {
            double** weights_of_prev;
            weights_of_prev = w.getHiddenLayer(temp->layerNo%w.getnumOfHiddenLayers());
            for(int i = 0; i < temp->size_of_neurons; i++)
            {
                temp->newNeuron[temp->pid] += temp->prevNeuron[i] *  weights_of_prev[i%w.getNoofNeurons()][temp->pid%w.getNoofNeurons()];
            }
            // cout << "DONEEEEEEEEE LAYER" << endl;
        }

        cout << "Value of Neuron at layer "  <<  temp->pid << " : " <<  temp->newNeuron[temp->pid] << endl;
        
        threads_complete++;
        sem_post(&L.mutex);
        
        pthread_exit(NULL);
    
}

void wait(string pipename, string pipename1, int layerNo){

            double wait1[2];
            double fifo1 = open(pipename.c_str(), O_RDONLY);
            read(fifo1, &wait1, sizeof(double)*2);
            close(fifo1);
            string command = "rm " + pipename;
            system(command.c_str());
 
           // wait(NULL);
            cout << "back propagating through layer" << layerNo << endl;

            
            int fifo2 = open(pipename1.c_str(), O_WRONLY);
            write(fifo2, &wait1, sizeof(double)*2);
            close(fifo2);

}


int main(int argc, char *argv[]){

    // w.setHiddenLayers(hidden);
    // w.setInput(inputs);
    // w.setInputLayer(input);
    // w.setOutputLayer(output);
    w.fileReading();
    // cout << w.getInputValues()[0] << endl;

    cout << "Helloo guys this is the new file" << endl;
    int layerNo = chartoInt(argv[1]);

    
    layerNo++;
    cout << layerNo << endl;

    char layerno[5];
    snprintf(layerno, sizeof(layerno), "%Id", layerNo);
    pthread_t* threads = new pthread_t[chartoInt(argv[3])];
    inputs_of_threads* abc = new inputs_of_threads[chartoInt(argv[3])];

    string pipename = "wait" + to_string(layerNo);
    int n = mkfifo(pipename.c_str(), 0666);

    string pipename1 = "wait" + to_string(layerNo - 1);

    


    if(layerNo <= chartoInt(argv[2])){

        int fd[2];

        pipe(fd);

        if(fork() == 0){

            close(fd[read_end]);

            

            //Initialization of Arrays
            double* prevNuerons = new double[chartoInt(argv[3])]{0};
            double* currentNeurons = new double[chartoInt(argv[3])]{0};
            double* inputVals = new double[2];
            cout << "Execing new hidden Layer" << endl;

            //Reading new Input Values for 
            if (layerNo == 1 && chartoInt(argv[4]) != 0){
                int fifo = open("IPC", O_RDONLY);
                read(fifo, inputVals, sizeof(double)*2);
                close(fifo);
                cout << "new Input Values: " << inputVals[0] << " & " << inputVals[1] << endl;
            w.setInput(inputVals);

            }

            //reading previous neurons Value
            if (layerNo > 1){
                int fifo = open("IPC", O_RDONLY);
                read(fifo, prevNuerons, sizeof(double)*chartoInt(argv[3]));
                close(fifo);
            }


            //implement threads here and calculating neuron value
            //int pid[chartoInt(argv[3])];
            for(int i = 0; i < chartoInt(argv[3]); i++) {
                abc[i].layerNo = layerNo-2;
                abc[i].prevNeuron = prevNuerons;
                abc[i].newNeuron = currentNeurons;
                abc[i].pid = i;
                abc[i].size_of_neurons = chartoInt(argv[3]);
                pthread_create(&threads[i], NULL, calculations, &abc[i]);
            }

            
            //PthreadJoin My version
            while(threads_complete < chartoInt(argv[3]));


            //writing value of current nerons to the IPC with Parent
            write(fd[write_end], currentNeurons, sizeof(double)*chartoInt(argv[3]));
            close(fd[write_end]);
            
            
            string command = "g++ -o hl1 helper.cpp -lpthread";
            system(command.c_str());
            execlp("./hl1", "h11", layerno, argv[2], argv[3], NULL);
            pthread_exit(NULL);
            perror("execlp failed");
        } else {
            int* currentNeurons = new int[chartoInt(argv[3])]{0};
            
            //Wait for the child process to calculate current values of neurons
            close(fd[write_end]);
            read(fd[read_end], currentNeurons, sizeof(double)*chartoInt(argv[3]));
            close(fd[read_end]);
            
            //writing current value of neurons to main pipe for child process child to access
            int fifo = open("IPC", O_WRONLY);
            write(fifo, currentNeurons, sizeof(double)*chartoInt(argv[3]));
            close(fifo);
            
            cout << "Forward pass done" << layerNo << endl;
            
            //waiting for child process to complete it's execution
            wait(pipename, pipename1, layerNo);
            
        }

    } else {
            double* prevNuerons = new double[chartoInt(argv[3])]{0};

            double fifo = open("IPC", O_RDONLY);
            read(fifo, prevNuerons, sizeof(double)*chartoInt(argv[3]));
            close(fifo);

            double X = 0;

            for(int i = 0; i < chartoInt(argv[3]); i++)
                X += prevNuerons[i] * w.getOutlayerVal(i);

            
            
            
            string command = "rm " + pipename;
            system(command.c_str());
        // this is outlayer which is layer 6
        cout << "output Layer with Value of X calculated: " << X << endl;

            double wait1[2];

            wait1[1] = ((X*X)- X)/2; 
            wait1[0] = ((X*X) + X + 1)/2;
            double fifo2 = open(pipename1.c_str(), O_WRONLY);
            write(fifo2, &wait1, sizeof(double)*2);
            close(fifo2);

    }

}