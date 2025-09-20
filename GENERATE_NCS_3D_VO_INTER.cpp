/*
Generate NCs structure with the size and position especified a plain text. In the interfaces there are Vo.
*/

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
    Int_t sample[100][100][1000]; // Array of this size need to be local variable
                                    //sample[x][y][z]
    
    Double_t nanoCrysts[1000][4];   // nanoCrysts[][0]: x center
                                    // nanoCrysts[][1]: y center
                                    // nanoCrysts[][2]: z center
                                    // nanoCrysts[][3]: radius
};
void GENERATE_NCS_3D_VO_INTER()
{
    new MAIN_FRAME();
}

MAIN_FRAME::MAIN_FRAME()/*  : TGMainFrame(gClient->GetRoot()) */ {
    TString nameInput = "3D-3BL.txt";
    TString nameOutput = "3BL-3D-structure.txt";
    
    
    Char_t chars[10000];
    TString numText;

    Int_t Xs, Ys, Zs, column;
    
    Int_t numNanoCrysts = 0;
    Double_t xCoor, yCoor, zCoor, radius;

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
        } else if (chars[0] == 'Y') { // If it is the number of Ys
            for (Int_t n = 3; n < 10000; n++) {
                if (chars[n] == '\0') {
                    Ys = numText.Atof();
                    break;
                } else
                    numText += chars[n];
            }
        } else if (chars[0] == 'Z') { // If it is the number of Ys
            for (Int_t n = 3; n < 10000; n++) {
                if (chars[n] == '\0') {
                    Zs = numText.Atof();
                    break;
                } else
                    numText += chars[n];
            }
        }
    }
    inputStructure.close();

    cout << "Xs = " << Xs << endl;
    cout << "Ys = " << Ys << endl;
    cout << "Zs = " << Zs << endl;

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
                } else if (column == 3) {
                    zCoor = numText.Atof();
                }
                numText = "";
                column++;
            } else if (column == 4 && chars[n] == '\0') { //If achieve the column 4
                radius = numText.Atof();
                nanoCrysts[numNanoCrysts][0] = xCoor;
                nanoCrysts[numNanoCrysts][1] = yCoor;
                nanoCrysts[numNanoCrysts][2] = zCoor;
                nanoCrysts[numNanoCrysts][3] = radius;
                numNanoCrysts++;
                break;
            } 
            else {
                numText += chars[n];
            }
        }
    }
    inputStructure2.close();

    cout << "x\ty\tz\tradius" << endl;
    for(Int_t n = 0; n < numNanoCrysts; n++)
        cout << nanoCrysts[n][0] << "\t" << nanoCrysts[n][1] << "\t" << nanoCrysts[n][2] << "\t" << nanoCrysts[n][3] << endl;
    
    Double_t xx, yy, zz, rr, xCent, yCent, zCent;
    for(Int_t x = 0; x < Xs; x++) {
        for(Int_t y = 0; y < Ys; y++) {
            for(Int_t z = 0; z < Zs; z++) {
                sample[x][y][z] = 0;
            }
        }
    }
    for(Int_t n = 0; n < numNanoCrysts; n++) {
        for(Int_t x = 0; x < Xs; x++) {
            for(Int_t y = 0; y < Ys; y++) {
                for(Int_t z = 0; z < Zs; z++) {
                    xx = x+0.5; // Convert to double, each site is in the middle of coordinate
                    yy = y+0.5; // Convert to double, each site is in the middle of coordinate
                    zz = z+0.5; // Convert to double, each site is in the middle of coordinate

                    xCent = nanoCrysts[n][0]; // Convert to double
                    yCent = nanoCrysts[n][1]; // Convert to double
                    zCent = nanoCrysts[n][2]; // Convert to double
                    rr = nanoCrysts[n][3]; // Convert to double
                    
                    if(
                        TMath::Sqrt(
                            TMath::Power(xx-xCent, 2.0) + TMath::Power(yy-yCent, 2.0)  + TMath::Power(zz-zCent, 2.0)
                        ) - rr <= -0.3333333
                    ) {
                        sample[x][y][z] = 2;
                    }
                }
            }
        }
    }
    for(Int_t x = 0; x < Xs; x++) {
        for(Int_t y = 0; y < Ys; y++) {
            for(Int_t z = 0; z < Zs; z++) {
                if(sample[x][y][z] == 2) continue;

                if(x > 0) {
                    if(sample[x-1][y][z] == 2) {
                        sample[x][y][z] = 1;
                        continue;
                    }
                }
                if(x < (Xs-1)) {
                    if(sample[x+1][y][z] == 2) {
                        sample[x][y][z] = 1;
                        continue;
                    }
                }
                if(y > 0) {
                    if(sample[x][y-1][z] == 2) {
                        sample[x][y][z] = 1;
                        continue;
                    }
                }
                if(y < (Ys-1)) {
                    if(sample[x][y+1][z] == 2) {
                        sample[x][y][z] = 1;
                        continue;
                    }
                }
                if(x > 0 && y > 0) {
                    if(sample[x-1][y-1][z] == 2){
                        sample[x][y][z] = 1;
                        continue;
                    }
                }
                if(x > 0 && y < (Ys-1)) {
                    if(sample[x-1][y+1][z] == 2) {
                        sample[x][y][z] = 1;
                        continue;
                    }
                }
                if(x < (Xs-1) && y > 0) {
                    if(sample[x+1][y-1][z] == 2) {
                        sample[x][y][z] = 1;
                        continue;
                    }
                }
                if(x < (Xs-1) && y < (Ys-1)) {
                    if(sample[x+1][y+1][z] == 2) {
                        sample[x][y][z] = 1;
                        continue;
                    }
                }

                if(z > 0) {
                    if(sample[x][y][z-1] == 2) {
                        sample[x][y][z] = 1;
                        continue;
                    }
                    if(x > 0) {
                        if(sample[x-1][y][z-1] == 2) {
                            sample[x][y][z] = 1;
                            continue;
                        }
                    }
                    if(x < (Xs-1)) {
                        if(sample[x+1][y][z-1] == 2) {
                            sample[x][y][z] = 1;
                            continue;
                        }
                    }
                    if(y > 0) {
                        if(sample[x][y-1][z-1] == 2) {
                            sample[x][y][z] = 1;
                            continue;
                        }
                    }
                    if(y < (Ys-1)) {
                        if(sample[x][y+1][z-1] == 2) {
                            sample[x][y][z] = 1;
                            continue;
                        }
                    }
                    if(x > 0 && y > 0) {
                        if(sample[x-1][y-1][z-1] == 2){
                            sample[x][y][z] = 1;
                            continue;
                        }
                    }
                    if(x > 0 && y < (Ys-1)) {
                        if(sample[x-1][y+1][z-1] == 2) {
                            sample[x][y][z] = 1;
                            continue;
                        }
                    }
                    if(x < (Xs-1) && y > 0) {
                        if(sample[x+1][y-1][z-1] == 2) {
                            sample[x][y][z] = 1;
                            continue;
                        }
                    }
                    if(x < (Xs-1) && y < (Ys-1)) {
                        if(sample[x+1][y+1][z-1] == 2) {
                            sample[x][y][z] = 1;
                            continue;
                        }
                    }
                }

                if(z < (Zs-1)) {
                    if(sample[x][y][z+1] == 2) {
                        sample[x][y][z] = 1;
                        continue;
                    }
                    if(x > 0) {
                        if(sample[x-1][y][z+1] == 2) {
                            sample[x][y][z] = 1;
                            continue;
                        }
                    }
                    if(x < (Xs-1)) {
                        if(sample[x+1][y][z+1] == 2) {
                            sample[x][y][z] = 1;
                            continue;
                        }
                    }
                    if(y > 0) {
                        if(sample[x][y-1][z+1] == 2) {
                            sample[x][y][z] = 1;
                            continue;
                        }
                    }
                    if(y < (Ys-1)) {
                        if(sample[x][y+1][z+1] == 2) {
                            sample[x][y][z] = 1;
                            continue;
                        }
                    }
                    if(x > 0 && y > 0) {
                        if(sample[x-1][y-1][z+1] == 2){
                            sample[x][y][z] = 1;
                            continue;
                        }
                    }
                    if(x > 0 && y < (Ys-1)) {
                        if(sample[x-1][y+1][z+1] == 2) {
                            sample[x][y][z] = 1;
                            continue;
                        }
                    }
                    if(x < (Xs-1) && y > 0) {
                        if(sample[x+1][y-1][z+1] == 2) {
                            sample[x][y][z] = 1;
                            continue;
                        }
                    }
                    if(x < (Xs-1) && y < (Ys-1)) {
                        if(sample[x+1][y+1][z+1] == 2) {
                            sample[x][y][z] = 1;
                            continue;
                        }
                    }
                }
            }
        }
    }

    // for(Int_t z = 10; z <= 26; z++) {
    //     cout << "z = " << z << endl;
    //     for(Int_t y = 0; y < Ys; y++) {
    //         for(Int_t x = 0; x < Xs; x++) {
    //             cout << sample[x][y][z];            
    //         }
    //         cout << endl;
    //     }
    //     cout << endl;
    // }

    ofstream outFile(nameOutput);
    outFile << Xs << " " << Ys << " " << Zs << endl;
    for(Int_t z = 0; z < Zs; z++) {
        for(Int_t y = 0; y < Ys; y++) {
            for(Int_t x = 0; x < Xs; x++) {
                outFile << sample[x][y][z] << endl;
            }
        }
    }
    
    outFile.close();

    

    gApplication->Terminate(0);
}