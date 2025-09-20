/*
Generate random NCs with a fixed size. In the interfaces there are Vo. Some NCs are joined.
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
// #include <filesystem>
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
    void PutNC();
    Int_t ObtainVolCryst();
    Int_t ObtainTnpnp();
    

    Int_t sample[100][100][1000]; // Array of this size need to be local variable
                                    //sample[x][y][z]
    Int_t sampleCenters[100][100][1000]; // It is a sample with a fixed radius of 2 pixels and it is used to calc the mean distance between centers in the z lines.
    
    Double_t nanoCrysts[100000][4];   // nanoCrysts[][0]: x center
                                    // nanoCrysts[][1]: y center
                                    // nanoCrysts[][2]: z center
                                    // nanoCrysts[][3]: radius
    TRandom1 *randObj;
    Int_t cryst;
    Int_t Xs, Ys, Zs;
    Double_t meanRadius;
    Double_t rCenter = 2.0; // Radius of sampleCenters

};
void GENERATE_RANDOMLY_NCS_3D_VO_INTER()
{
    new MAIN_FRAME();
}

MAIN_FRAME::MAIN_FRAME()/*  : TGMainFrame(gClient->GetRoot()) */ {
    TString nameOutput = "Zs11_3D_structure_VoInter.txt";
    meanRadius = 7;
    Int_t seed = 9876;
    Xs = 40;
    Ys = 40;
    Zs = 216;
    Int_t thicknessMatNC = 92; //92 pixels is the tickness of silicon.

    Char_t chars[10000];
    TString numText;
    Int_t numInitNCs;
    Int_t volNeedCrystal = Xs*Ys*thicknessMatNC;
    cout << "Xs = " << Xs << endl;
    cout << "Ys = " << Ys << endl;
    cout << "Zs = " << Zs << endl;
    cout << "thickness of material NC = " << thicknessMatNC << endl;
    cout << "vol need crystal = " << volNeedCrystal << endl;

    

    numInitNCs = 6*Xs*Ys*thicknessMatNC/(TMath::Pi()*TMath::Power(2.0*meanRadius, 3.0)); 
    
    cout << "radius = " << meanRadius << endl;
    cout << "initial num NCs = " << numInitNCs << endl;
    

    // randObj = new TRandom1();
    randObj = new TRandom1(seed);
    cout << "Seed of random numbers = " << randObj->GetSeed() << endl;
    cout << endl;
    
    cryst = 0;

    for(Int_t x = 0; x < Xs; x++) {
        for(Int_t y = 0; y < Ys; y++) {
            for(Int_t z = 0; z < Zs; z++) {
                sample[x][y][z] = 0;
                sampleCenters[x][y][z] = 0;
            }
        }
    }

    while(cryst < numInitNCs) {
        PutNC();
    }
    
    cout << "initial Vol crystals = " << ObtainVolCryst() << endl;
    cout << endl;

    while(ObtainVolCryst() < volNeedCrystal) {
        PutNC();
    }
    cout << "final num NCs = " << cryst << endl;
    cout << "final Vol crystals = " << ObtainVolCryst() << endl;
    cout << endl;

    // cout << "crys\tx\ty\tz\tradius" << endl;
    // for(Int_t n = 0; n < cryst; n++)
    //     cout << n << "\t" << nanoCrysts[n][0] << "\t" << nanoCrysts[n][1] << "\t" << nanoCrysts[n][2] << "\t" << nanoCrysts[n][3] << endl;
    // cout << endl;

    Int_t sumTnpnp = 0;
    Int_t Tnpnp;
    Int_t numTnpnp = 0;
    Bool_t neighbour;
    
    for(Int_t x = 0; x < Xs; x++) {
        for(Int_t y = 0; y < Ys; y++) {
            neighbour = kFALSE;
            Tnpnp = 0;

            for(Int_t z = 0; z < (Zs-4); z++) {
                if(sampleCenters[x][y][z] == 2) {
                    for(Int_t nearZ = z + 1; nearZ < Zs; nearZ++) {
                        if(sampleCenters[x][y][nearZ] == 0) Tnpnp++;
                        else if(sampleCenters[x][y][nearZ] == 2 && Tnpnp > 0) {
                            numTnpnp++;
                            neighbour = kTRUE;
                            sumTnpnp += Tnpnp;
                            break;
                        }
                    }
                }
                if(neighbour) { 
                    break;
                }
            }

            // if(neighbour) cout << "Tnpnp = " << Tnpnp << ", numTnpnp = " << numTnpnp << ", sumTnpnp = " << sumTnpnp << ", mean Tnpnp = " << sumTnpnp/numTnpnp <<  endl;
        }
    }
    cout << endl;
    cout << "numTnpnp = " << numTnpnp << ", sumTnpnp = " << sumTnpnp << ", meanTnpnp = " << sumTnpnp/numTnpnp << ", meanTnpnp - 2r = " << sumTnpnp/numTnpnp - 2*meanRadius <<  endl;
    cout << endl;

    // for(Int_t y = 0; y < Ys; y++) {
    //     for(Int_t x = 0; x < Xs; x++) {
    //         cout << sample[x][y][18];            
    //     }
    //     cout << endl;
    // }


    // Put Vo in the interface matrix|nanocristals:
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
void MAIN_FRAME::PutNC(){
    while(kTRUE) {
        Bool_t isRepeat = kFALSE;
        Int_t rndX, rndY, rndZ;
        rndX = (Int_t)Xs * randObj->Rndm();
        rndY = (Int_t)Ys * randObj->Rndm();
        rndZ = (Int_t)Zs * randObj->Rndm();

        for(Int_t n = 0; n < cryst; n++) {
            if(nanoCrysts[n][0] == rndX && nanoCrysts[n][1] == rndY && nanoCrysts[n][2] == rndX) {
                isRepeat = kTRUE;
                break;
            }
        }
        if(!isRepeat) {
            nanoCrysts[cryst][0] = rndX;
            nanoCrysts[cryst][1] = rndY;
            nanoCrysts[cryst][2] = rndZ;
            nanoCrysts[cryst][3] = meanRadius;
            break;
        } else {
            cout << "Is repeated" << endl;
        }
    }
    Double_t xx, yy, zz, rr, xCent, yCent, zCent;
    
    for(Int_t x = 0; x < Xs; x++) {
        for(Int_t y = 0; y < Ys; y++) {
            for(Int_t z = 0; z < Zs; z++) {
                xx = x+0.5; // Convert to double, each site is in the middle of coordinate
                yy = y+0.5; // Convert to double, each site is in the middle of coordinate
                zz = z+0.5; // Convert to double, each site is in the middle of coordinate

                xCent = nanoCrysts[cryst][0]; // Convert to double
                yCent = nanoCrysts[cryst][1]; // Convert to double
                zCent = nanoCrysts[cryst][2]; // Convert to double
                rr = nanoCrysts[cryst][3]; // Convert to double

                
                if(
                    TMath::Sqrt(
                        TMath::Power(xx-xCent, 2.0) + TMath::Power(yy-yCent, 2.0)  + TMath::Power(zz-zCent, 2.0)
                    ) - rr <= -0.3333333
                ) {
                    sample[x][y][z] = 2;
                }

                if(
                    TMath::Sqrt(
                        TMath::Power(xx-xCent, 2.0) + TMath::Power(yy-yCent, 2.0)  + TMath::Power(zz-zCent, 2.0)
                    ) - rCenter <= -0.3333333
                ) {
                    sampleCenters[x][y][z] = 2;
                }

            }
        }
    }
    cryst++;
    // cout << "cryst = " << cryst << endl;


}
Int_t MAIN_FRAME::ObtainVolCryst() {
    Int_t numFixVo = 0;
    for(Int_t x = 0; x < Xs; x++) {
        for(Int_t y = 0; y < Ys; y++) {
            for(Int_t z = 0; z < Zs; z++) {
                if(sample[x][y][z] == 2) numFixVo++;
            }
        }
    }
    // cout << numFixVo << endl;
    return numFixVo;
}
Int_t MAIN_FRAME::ObtainTnpnp() {
    return 0;
}