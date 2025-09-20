#include "TF1.h"
#include "TH2F.h"
#include "Math/WrappedTF1.h"
#include "Math/BrentRootFinder.h"
#include "TMath.h"
#include "TRandom.h"
#include "stdio.h"
#include "TGFrame.h"
#include <iomanip>
#include "iostream"
#include <filesystem>
#include <sstream>
#include "fstream"
// #include <unistd.h>
#include "TGSlider.h"
#include "TGNumberEntry.h"
#include "TGButton.h"
#include "TGTextEntry.h"
#include "TCanvas.h"
#include "TGLabel.h"
#include "TGFrame.h"
#include "TRootCanvas.h"
#include "TStyle.h"
#include "TRandom1.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TDecompLU.h"
#include "TApplication.h"

using namespace std;

class MAIN_FRAME : public TGMainFrame
{
public:
    MAIN_FRAME();
    Int_t sample[1000][1000]; // Array of this size need to be local variable
    Double_t nanoCrysts[1000][3];   // nanoCrysts[][0]: x center
                                    // nanoCrysts[][1]: y center
                                    // nanoCrysts[][2]: radius
};
void GENERATE_NCS_2D_VO_INTER()
{
    new MAIN_FRAME();
}

MAIN_FRAME::MAIN_FRAME()/*  : TGMainFrame(gClient->GetRoot()) */ {
    TString nameInput = "2D-1BL.txt";
    TString nameOutput = "1BL-2D-structure.txt";
    
    
    Char_t chars[10000];
    TString numText;
    Double_t V, I;
    Int_t numInt;

    Int_t Xs, Ys, column;
    
    Int_t numNanoCrysts = 0;
    Double_t xCoor, yCoor, radius;

    // V = -0.1499;
    // cout << TMath::Fl
    


    ifstream inputStructure(nameInput);

    

    while (!inputStructure.eof()) { // Obtain the number of rows and columns

        inputStructure.getline(chars, 10000);

        numText = "";

        if (chars[0] == '\0')
            continue;

        if (chars[0] == 'X') { // If it is the number of Xs
            for (Int_t n = 3; n < 10000; n++) {
                if (chars[n] == '\0') {
                    Xs = numText.Atof();
                    break;
                } else
                    numText += chars[n];
            }
        }
        else if (chars[0] == 'Y') { // If it is the number of Ys
            for (Int_t n = 3; n < 10000; n++) {
                if (chars[n] == '\0') {
                    Ys = numText.Atof();
                    break;
                } else
                    numText += chars[n];
            }
        }
    }
    inputStructure.close();

    cout << "Xs = " << Xs << endl;
    cout << "Ys = " << Ys << endl;

    ifstream inputStructure2(nameInput);

    while (!inputStructure2.eof()) { //Obtain centers and radius of NCs.
        inputStructure2.getline(chars, 10000);
    
        if (chars[0] == '\0')
            continue;
        
        numText = "";
        column = 1;

        for (Int_t n = 0; n < 10000; n++) {
            if (chars[n] == '\t' || chars[n] == ' ') {
                if (!numText.IsFloat())
                    break;
                if (column == 1) {
                    xCoor = numText.Atof();
                } else if (column == 2) {
                    yCoor = numText.Atof();
                } 
                numText = "";
                column++;
            } else if (column == 3 && chars[n] == '\0') { //If achieve the column 3
                radius = numText.Atof();
                nanoCrysts[numNanoCrysts][0] = xCoor;
                nanoCrysts[numNanoCrysts][1] = yCoor;
                nanoCrysts[numNanoCrysts][2] = radius;
                numNanoCrysts++;
                break;
            } 
            else {
                numText += chars[n];
            }
        }
    }
    inputStructure2.close();

    cout << "x\ty\tradius" << endl;
    for(Int_t n = 0; n < numNanoCrysts; n++)
        cout << nanoCrysts[n][0] << "\t" << nanoCrysts[n][1] << "\t" << nanoCrysts[n][2] << endl;

    Double_t xx, yy, rr, xCent, yCent;
    for(Int_t x = 0; x < Xs; x++) {
        for(Int_t y = 0; y < Ys; y++) {
            sample[x][y] = 0;
            //cout << sample[x][y];            
        }
        // cout << endl;
    }
    for(Int_t n = 0; n < numNanoCrysts; n++) {
        for(Int_t x = 0; x < Xs; x++) {
            for(Int_t y = 0; y < Ys; y++) {
                xx = x+0.5; // Convert to double, each site is in the middle of coordinate
                yy = y+0.5; // Convert to double, each site is in the middle of coordinate

                xCent = nanoCrysts[n][0]; // Convert to double
                yCent = nanoCrysts[n][1]; // Convert to double
                rr = nanoCrysts[n][2]; // Convert to double
                
                if(
                    TMath::Sqrt(
                        TMath::Power(xx-xCent, 2.0) + TMath::Power(yy-yCent, 2.0)
                    ) - rr <= -0.3333333
                ) {
                    sample[x][y] = 2;
                }
                
            }
        }
    }

    for(Int_t y = 0; y < Ys; y++) {
        for(Int_t x = 0; x < Xs; x++) {
            if(sample[x][y] == 2) continue;
            if(x > 0) 
                if(sample[x-1][y] == 2) 
                    sample[x][y] = 1;
            if(x < (Xs-1)) 
                if(sample[x+1][y] == 2) 
                    sample[x][y] = 1;
            if(y > 0) 
                if(sample[x][y-1] == 2) 
                    sample[x][y] = 1;
            if(y < (Ys-1)) 
                if(sample[x][y+1] == 2) 
                    sample[x][y] = 1;
            if(x > 0 && y > 0)
                if(sample[x-1][y-1] == 2)
                    sample[x][y] = 1;
            if(x > 0 && y < (Ys-1))
                if(sample[x-1][y+1] == 2)
                    sample[x][y] = 1;
            if(x < (Xs-1) && y > 0)
                if(sample[x+1][y-1] == 2)
                    sample[x][y] = 1;
            if(x < (Xs-1) && y < (Ys-1))
                if(sample[x+1][y+1] == 2)
                    sample[x][y] = 1;
        }
    }

    ofstream outFile(nameOutput);
    for(Int_t y = 0; y < Ys; y++) {
        for(Int_t x = 0; x < Xs; x++) {
            outFile << sample[x][y];
            cout << sample[x][y];            
        }
        outFile << endl;
        cout << endl;
    }
    
    outFile.close();

    

    gApplication->Terminate(0);
}