//Nicolas Stoian
//CS780 Image Processing
//Project 7 - Chain Code

//This program needs 3 command line arguments
//argv[1] "input" for text file representing the input image
//argv[2] "output1" for the result of the chain-codes of the connected components in the input image
//argv[3] "output2" for pretty print of the input image

#include <fstream>
using namespace std;

class ImageProcessing{ friend class ChainCode;
private:
    int** imgAry;
    int numRows;
    int numCols;
    int minVal;
    int maxVal;

public:
    ImageProcessing();
    ImageProcessing(ifstream& inFile);
    ~ImageProcessing();
    void loadImage(ifstream& inFile);
    void zeroFramed();
    static string mapInt2Char(int theInt);
    void prettyPrint(ofstream& outFile);
    void printHeader(ofstream& outFile);
    int getMaxVal();
};

class ChainCode{
private:
    struct Pixel{
        int rowPosition;
        int colPosition;
    };
    Pixel firstNonZero;
    Pixel currPixel;
    int label;
    int nextDir;
    int chainDir;
    int lastZeroDir;
    int lastZeroTable [8] = {6, 6, 0, 0, 2, 2, 4, 4};
    int neighborTable [2][8];

public:
    ChainCode();
    ~ChainCode();
    Pixel findNextComponent(ImageProcessing* image);
    int findNextNoneZeroNeighbor(ImageProcessing* image);
    void setLabel(int label);
    bool isCurrEqualFirst();
    void chainCodeStep2(ImageProcessing* image, ofstream& outFile);
    void chainCodeStep3(ImageProcessing* image, ofstream& outFile);
};

int main(int argc, char* argv[]){
    ifstream inFile1;
    inFile1.open(argv[1]);
    ImageProcessing* image = new ImageProcessing(inFile1);
    inFile1.close();
    ofstream outFile2;
    outFile2.open(argv[3]);
    image->prettyPrint(outFile2);
    outFile2.close();
    ofstream outFile1;
    outFile1.open(argv[2]);
    image->printHeader(outFile1);
    outFile1 << endl;
    ChainCode* nextCC;
    for(int i = 1; i <= image->getMaxVal(); i++){
        nextCC = new ChainCode();
        nextCC->setLabel(i);
        nextCC->chainCodeStep2(image, outFile1);
        do{
            nextCC->chainCodeStep3(image, outFile1);
        }
        while(!nextCC->isCurrEqualFirst());
        outFile1 << endl << endl;
    }
    outFile1.close();
}

ImageProcessing::ImageProcessing(): imgAry(NULL), numRows(0), numCols(0), minVal(0), maxVal(0){
}

ImageProcessing::ImageProcessing(ifstream& inFile){
    loadImage(inFile);
}

ImageProcessing::~ImageProcessing(){
    if(imgAry != NULL){
        for(int i = 0; i < numRows + 2; i++){
            delete [] imgAry[i];
        }
    }
    delete [] imgAry;
}

void ImageProcessing::loadImage(ifstream& inFile){
    inFile >> numRows;
    inFile >> numCols;
    inFile >> minVal;
    inFile >> maxVal;
    zeroFramed();
    for(int row = 1; row < numRows + 1; row++){
        for(int col = 1; col < numCols + 1; col++){
            int value;
            inFile >> value;
            imgAry[row][col] = value;
        }
    }
}

void ImageProcessing::zeroFramed(){
    imgAry = new int* [numRows + 2];
    for(int i = 0; i < numRows + 2; i++){
        imgAry[i] = new int [numCols + 2];
    }
    for(int row = 0; row < numRows + 2; row++){
        for(int col = 0; col < numCols + 2; col++){
            imgAry[row][col] = 0;
        }
    }
}

string ImageProcessing::mapInt2Char(int theInt){
    char toReturn [33];
    sprintf(toReturn, "%d", theInt);
    //itoa(theInt, toReturn, 10);
    return toReturn;
}

void ImageProcessing::prettyPrint(ofstream& outFile){
    for(int row = 1; row < numRows + 1; row++){
        for(int col = 1; col < numCols + 1; col++){
            if(imgAry[row][col] <= 0){
                outFile << " " << " ";
            }
            else{
                outFile << mapInt2Char(imgAry[row][col]) << " ";
            }
        }
        outFile << endl;
    }
}

void ImageProcessing::printHeader(ofstream& outFile){
    outFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
}

int ImageProcessing::getMaxVal(){
    return maxVal;
}

ChainCode::ChainCode(): label(0), nextDir(0), chainDir(0), lastZeroDir(0){
    firstNonZero.colPosition = 0;
    firstNonZero.rowPosition = 0;
    currPixel.colPosition = 0;
    currPixel.rowPosition = 0;
}

ChainCode::~ChainCode(){
}

ChainCode::Pixel ChainCode::findNextComponent(ImageProcessing* image){
    Pixel toReturn;
    toReturn.rowPosition = 0;
    toReturn.colPosition = 0;
    for(int row = 1; row < image->numRows + 1; row++){
        for(int col = 1; col < image->numCols + 1; col++){
            if(image->imgAry[row][col] == label){
                toReturn.rowPosition = row;
                toReturn.colPosition = col;
                return toReturn;
            }
        }
    }
    return toReturn;
}

int ChainCode::findNextNoneZeroNeighbor(ImageProcessing* image){
    for(int i = 0; i < 8; i++){
        if(i == 0 || i == 4){
            neighborTable[0][i] = currPixel.rowPosition;
        }
        if(i >= 1 && i <= 3){
            neighborTable[0][i] = currPixel.rowPosition - 1;
        }
        if(i >= 5){
            neighborTable[0][i] = currPixel.rowPosition + 1;
        }

        if(i == 2 || i == 6){
            neighborTable[1][i] = currPixel.colPosition;
        }
        if(i >= 3 && i <= 5){
            neighborTable[1][i] = currPixel.colPosition - 1;
        }
        if(i == 7 || i <= 1){
            neighborTable[1][i] = currPixel.colPosition + 1;
        }
    }
    for(int i = nextDir; i < nextDir + 8; i++){
        if(image->imgAry[neighborTable[0][i % 8]][neighborTable[1][i % 8]] > 0){
            return (i % 8);
        }
    }
}

void ChainCode::setLabel(int label){
    this->label = label;
}

bool ChainCode::isCurrEqualFirst(){
    return (currPixel.rowPosition == firstNonZero.rowPosition &&
            currPixel.colPosition == firstNonZero.colPosition);
}

void ChainCode::chainCodeStep2(ImageProcessing* image, ofstream& outFile){
    firstNonZero = findNextComponent(image);
    outFile << firstNonZero.rowPosition << " " << firstNonZero.colPosition << " " << label;
    lastZeroDir = 4;
    currPixel = firstNonZero;
}

void ChainCode::chainCodeStep3(ImageProcessing* image, ofstream& outFile){
    nextDir = lastZeroDir + 1;
    chainDir = findNextNoneZeroNeighbor(image);
    outFile << " " << chainDir;
    lastZeroDir = lastZeroTable[chainDir];
    currPixel.rowPosition = neighborTable[0][chainDir];
    currPixel.colPosition = neighborTable[1][chainDir];
}
