

#include <iostream>
#include <fstream>
#include <queue>

using namespace std;

double strToDouble(string num){
    
    int i = 0;
    // checking sign.
    if(num[0] == '-')
        i++;

    // converting integer part
    float integer = 0;
    for(; num[i] != '.' && num[i]; i++) {
        integer *= 10.0;
        integer += num[i] - '0';
    }

    //converting decimal part;
    int count = 1; //decimal place. 
    for(; num[i]; i++ ){
        if (num[i] >= '0' && num[i] <= '9'){
        integer += ((num[i] - '0') / pow(10, count));
        count++;
        }
    }

    if(num[0] == '-')
        integer *= -1.0;

    return integer;


}


class weights{
    double** input_layer; //[input no] [Neuron ID of Current process]
    double*** hidden_layers; //[Hiden layerNo], [neuron ID of previos process], [Neuron ID of Current process]
    double* output_layer;

    int numOfN;
    int noInput;
    int numOfHiddenLayers;

    double* inputss ;

public:

    weights(){
        numOfN = 0;
        noInput = 0;
        numOfHiddenLayers = 0;

        input_layer = nullptr;
        hidden_layers = nullptr;
        output_layer = nullptr;
        inputss = new double[2];


    }

    weights(int N, int input, int hiddenLayers){
        
        numOfN = N;
        noInput = input;
        numOfHiddenLayers = hiddenLayers;

        input_layer = new double*[noInput];
        for(int i = 0;  i < input; i++)
            input_layer[i] = new double[numOfN];

        
        hidden_layers =  new double**[numOfHiddenLayers];
        for(int i = 0; i < numOfHiddenLayers; i++) { 
            hidden_layers[i] = new double*[numOfN];
            for(int j = 0; j < numOfN;  j++)
                hidden_layers[i][j] = new double[numOfN];

        }

        output_layer = new double[numOfN];

    }

    void setInput(double* N){
        inputss = new double[2];
        inputss[0] = N[0];
        inputss[1] = N[1];

    }

    void allocateInputLayer(){

        // cout << noInput << numOfN << endl;

         input_layer = new double*[noInput];
            for(int i = 0;  i < noInput; i++)
                input_layer[i] = new double[numOfN];
    }

    void allocateHiddenLayer(){

        // cout << numOfHiddenLayers << numOfN << endl;

        hidden_layers =  new double**[numOfHiddenLayers];
        for(int i = 0; i < numOfHiddenLayers; i++) { 
            hidden_layers[i] = new double*[numOfN];
            for(int j = 0; j < numOfN;  j++)
                hidden_layers[i][j] = new double[numOfN];

        }
    }

    void allocateOutputLayer(){

        // cout << numOfN << endl;

        output_layer = new double[numOfN];

    }

    void setInputLayer(double N[][8]){
        for(int i = 0; i < noInput; i++)
            for(int j = 0; j < numOfN; j++)
                input_layer[i][j] = N[i][j];
    }

    void setHiddenLayers(double H[][8][8]){
        for(int i = 0; i < numOfHiddenLayers; i++)
            for(int j = 0; j < numOfN; j++)
                for(int k = 0; k < numOfN; k++)
                    hidden_layers[i][j][k] = H[i][j][k];
    }

    void setOutputLayer(double* O){
        for(int j = 0; j < numOfN; j++)
            output_layer[j] = O[j];

    }

    int getNoofNeurons()
    {
        return numOfN;
    }

    int getnumOfHiddenLayers()
    {
        return numOfHiddenLayers;
    }

    double** getInputLayer(){
        return input_layer;

    }

    double* getInputValues(){
        return inputss;

    }

    double getInputLayerVal(int N, int nueronID){
        return input_layer[N % noInput][nueronID % numOfN];

    }

    double** getHiddenLayer (int pid){
        return hidden_layers[pid];
    }

    double getHiddenLayerVal (int HLN, int PPNID, int CPNID){
        return hidden_layers[HLN % numOfHiddenLayers][PPNID % numOfN][CPNID % numOfN];
    }

    double getOutlayerVal (int N){
        return output_layer[N % numOfN];
    }


    void fileReading();

    // ~weights(){
    //     if (input)
    //         delete[] input;

    //     if (output_layer)
    //         delete[] output_layer;

    //     if (hidden_layers) {
    //         for(int i = 0; i < numOfHiddenLayers; i++) 
    //             for(int j = 0; j < numOfN;  j++)
    //                 delete[] hidden_layers[i][j];
            
    //         for(int i = 0; i < numOfHiddenLayers; i++)
    //             delete[] hidden_layers[i];
            
    //         delete[] hidden_layers;
    //     }

    //     if (input_layer) {
    //         for(int i = 0;  i < noInput; i++)
    //             delete[] input_layer[i];
            
    //         delete[] input_layer;
    //     }

    // }

};



void weights::fileReading(){
        noInput = 2;
        queue<double> Q;

        char ch;

        ifstream fin("file.txt");
        string layerType;
        int h = 0, o = 0;
        int input = 0;
        int countEndl;

        int neurons = 0;

        if(fin.is_open()){
            
            noInput == 0;

            while(fin.get(ch)){

                if (ch == 10)
                    layerType = "";



                
                if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')){
                    layerType += ch;
                    while(fin.get(ch)){
                        if (ch == ' ')
                            break;
                        layerType += ch;   
                    }
                    while(fin.get(ch))
                        if(ch == 10)
                            break;

                    if (layerType == "Hidden")
                        h++;
                    else if (layerType == "Input")
                        input++;
                    else if (layerType == "Output")
                        o++;
                    
                    
                }
                string db;
                if (ch == '-' || (ch >= '0' && ch <= '9')){
                    db += ch;
                    while(fin.get(ch)){

                        if (ch == ',' || ch == 10)
                            break;
                        db += ch;
                    }
                    Q.push(strToDouble(db));
                    neurons++;
                }

                if (ch == 10 && !Q.empty()) {

                    if (layerType == "Hidden" && h == 1 && input == 1) {
                        
                        if (input == 1){
                            numOfN = neurons/noInput;
                            allocateInputLayer();
                        }
                        for(int i = 0; i < noInput; i++){
                            for(int j = 0; j < numOfN; j++) {
                                input_layer[i][j] = Q.front();
                                // cout << Q.front() << " ";
                                Q.pop();
                            }
                            // cout << endl;
                        }
                    
                    
                        input--;
                    } 
                    if (layerType == "Input" && h > 0 && input == 1) {
                        allocateOutputLayer();
                        for(int i = 0; i < numOfN; i++){
                            output_layer[i] = Q.front() ;
                                // cout << Q.front() << " ";

                            Q.pop();
                        }
                            // cout << endl;

                        input--;
                    }

                    if (layerType == "Output" && o == 1 ){
                        numOfHiddenLayers = h;
                        allocateHiddenLayer();
                        for(int i = 0; i < numOfHiddenLayers; i++){
                            for(int j = 0; j < numOfN; j++){
                                for(int k = 0; k < numOfN; k++) {
                                    hidden_layers[i][j][k] =  Q.front();
                                // cout << Q.front() << " ";

                                    Q.pop();
                                }
                            // cout << endl;

                            }
                            // cout << endl;
                            // cout << endl;

                        }
                        o--;
                    }
                }
            }

        } else {
            cout << "File Not found or opened" << endl;
        }

        inputss[0] = Q.front();
        Q.pop();
        inputss[1] = Q.front();
        Q.pop();

       cout << inputss[0] << endl;
       //<< noInput << numOfHiddenLayers << numOfN <<endl;
        fin.close();
       

    }


    double input[2][8] = {{0.1, -0.2, 0.3, 0.1, -0.2, 0.3, 0.1, -0.2} ,
                    {-0.4, 0.5, 0.6, -0.4, 0.5, 0.6, -0.4, 0.5}};

double hidden[5][8][8] = {
{{-0.2, 0.3, -0.4, 0.5, 0.6, -0.7, 0.8, -0.9},
{0.1, 0.9, -0.3, 0.2, -0.5, 0.4, 0.6, -0.8},
{-0.7, 0.5, 0.8, -0.2, -0.3, -0.6, 0.1, 0.4},
{-0.2, 0.3, -0.4, 0.5, 0.6, -0.7, 0.8, -0.9},
{0.1, 0.9, -0.3, 0.2, -0.5, 0.4, 0.6, -0.8},
{-0.7, 0.5, 0.8, -0.2, -0.3, -0.6, 0.1, 0.4},
{-0.2, 0.3, -0.4, 0.5, 0.6, -0.7, 0.8, -0.9},
{0.1, 0.9, -0.3, 0.2, -0.5, 0.4, 0.6, -0.8}},

{{0.2, -0.3, 0.4, -0.5, -0.6, 0.7, -0.8, 0.9},
{-0.1, -0.9, 0.3, -0.2, 0.5, -0.4, -0.6, 0.8},
{0.7, -0.5, -0.8, 0.2, 0.3, 0.6, -0.1, -0.4},
{0.2, -0.3, 0.4, -0.5, -0.6, 0.7, -0.8, 0.9},
{-0.1, -0.9, 0.3, -0.2, 0.5, -0.4, -0.6, 0.8},
{0.7, -0.5, -0.8, 0.2, 0.3, 0.6, -0.1, -0.4},
{0.2, -0.3, 0.4, -0.5, -0.6, 0.7, -0.8, 0.9},
{-0.1, -0.9, 0.3, -0.2, 0.5, -0.4, -0.6, 0.8}},

{{0.3, -0.4, 0.5, -0.6, -0.7, 0.8, -0.9, 0.1},
{-0.2, -0.9, 0.4, -0.3, 0.5, -0.6, -0.8, 0.1},
{0.6, -0.5, -0.7, 0.2, 0.4, 0.8, -0.1, -0.3},
{0.3, -0.4, 0.5, -0.6, -0.7, 0.8, -0.9, 0.1},
{-0.2, -0.9, 0.4, -0.3, 0.5, -0.6, -0.8, 0.1},
{0.6, -0.5, -0.7, 0.2, 0.4, 0.8, -0.1, -0.3},
{0.3, -0.4, 0.5, -0.6, -0.7, 0.8, -0.9, 0.1},
{-0.2, -0.9, 0.4, -0.3, 0.5, -0.6, -0.8, 0.1}},

{{0.4, -0.5, 0.6, -0.7, -0.8, 0.9, -0.1, 0.2},
{-0.3, -0.8, 0.5, -0.4, 0.6, -0.7, -0.9, 0.2},
{0.5, -0.4, -0.6, 0.3, 0.2, 0.8, -0.2, -0.1},
{0.4, -0.5, 0.6, -0.7, -0.8, 0.9, -0.1, 0.2},
{-0.3, -0.8, 0.5, -0.4, 0.6, -0.7, -0.9, 0.2},
{0.5, -0.4, -0.6, 0.3, 0.2, 0.8, -0.2, -0.1},
{0.4, -0.5, 0.6, -0.7, -0.8, 0.9, -0.1, 0.2},
{-0.3, -0.8, 0.5, -0.4, 0.6, -0.7, -0.9, 0.2}},

{{0.5, -0.6, 0.7, -0.8, -0.9, 0.1, -0.2, 0.3},
{-0.4, -0.7, 0.6, -0.5, 0.8, -0.6, -0.2, 0.1},
{0.4, -0.3, -0.5, 0.1, 0.6, 0.7, -0.3, -0.2},
{0.5, -0.6, 0.7, -0.8, -0.9, 0.1, -0.2, 0.3},
{-0.4, -0.7, 0.6, -0.5, 0.8, -0.6, -0.2, 0.1},
{0.4, -0.3, -0.5, 0.1, 0.6, 0.7, -0.3, -0.2},
{0.5, -0.6, 0.7, -0.8, -0.9, 0.1, -0.2, 0.3},
{-0.4, -0.7, 0.6, -0.5, 0.8, -0.6, -0.2, 0.1}}};


double output[8] = {-0.1,0.2,0.3,0.4,0.5,-0.6,-0.7,0.8};

double inputs[2] = {0.1, 0.2};