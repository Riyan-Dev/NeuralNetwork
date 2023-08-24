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
#include <X11/Xlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <atomic>

#include "weights.h"

using namespace std;





int main(){

    int FP = 2;
    int Layers = 5;
    int Neurons = 6; 

    



    cout << "Enter Number of Hidden Layers: ";
    cin >> Layers;
    cout << "Enter Number of Nuerons: ";
    cin >> Neurons;
    
    char layer[5];
     snprintf(layer, sizeof(layer), "%Id", Layers);

    char neurons[5];
     snprintf(neurons, sizeof(neurons), "%Id", Neurons);

    int n = mkfifo("IPC", 0666);

    

    // cout << "Enter Number of Forward Pass: ";


    int layersno = 0;
    char layerNo[5];
    snprintf(layerNo, sizeof(layerNo), "%Id", layersno);

    double inputVals[2] = {0.0};


    
    for (int i = 0; i < FP; i++) {
        int id = mkfifo("wait0", 0666);
        
        char fp[5];    
        snprintf(fp, sizeof(fp), "%Id", i);
        
        
        if (fork() == 0){
            
            cout<< inputVals[0] << " & " << inputVals[1] << endl;

            system("g++ -o hl1 helper.cpp -lpthread");
            execlp("./hl1", "hl1", layerNo, layer, neurons,fp, NULL);
            perror("execlp failed");
            
        }
        else {

                if(i != 0){
                    int fifo = open("IPC", O_WRONLY);
                    write(fifo, inputVals, sizeof(double)*2);
                    close(fifo);
                }
            // while(reading from wait0 is equals = 0)
            // cout << "Hello" << endl;
            // while(wait1 == 0){
                int fifo = open("wait0", O_RDONLY);
                read(fifo, &inputVals, sizeof(double)* 2);
                close(fifo);

                system("rm wait0");
            // }

            //wait(NULL);
            cout << "Input Layer Reached Back and Updating Input Values to " << inputVals[0] << " & " << inputVals[1]  
            << endl << "-------------------------------------------------------------------------------"<< endl; 
        }
    }
     

    system("rm IPC");

    // ifstream fin;
    // fin.open("Configuration.docx");
    // char ch;
    // if(fin.is_open())
    //     while(fin.get(ch)){

    //         cout << ch;

    //     }   
    // else 
    //     cout << "File not opened" << endl;



}