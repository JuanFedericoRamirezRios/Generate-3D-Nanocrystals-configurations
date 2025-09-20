/*
Generate random NCs structure in MLs, the size have a Gaussian (normal) distribution. Some NCs are joined.
*/
#include "CONTROLS_SCIENTIFIC.h"
#include "CONTROLS_VALUE.h"

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
    Bool_t PutNC(Int_t zPosition, Double_t meanRadii, Double_t desvRadii);
    Int_t ObtainVolCryst();
    Int_t ObtainTnpnp();
    void CloseWindow();
    

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
    Double_t meanRadius, desvRadius;
    Double_t rCenter = 2.0; // Radius of sampleCenters
    Int_t maxAttempts;

};
void GENERATE_MLs_NORMAL_DIST_NCS_NOT_SEPARATED_3D()
{
    new MAIN_FRAME();
}

MAIN_FRAME::MAIN_FRAME() : TGMainFrame(gClient->GetRoot()) {   
    TString nameOutput = "BLs_3D_structure.txt";
    Xs = 50;
    Ys = 50;
    Zs = 108;
    meanRadius = 7.69;
    desvRadius = 3.08;
    Int_t seed = 4;
    Double_t zCenterRadiusMLs[3][3] = {    // zCenterRadiusMLs[][0]: z coordenate of centers 
                                        // zCenterRadiusMLs[][1]: mean radius
                                        // zCenterRadiusMLs[][2]: Desv. standar of radius
        {23, meanRadius, desvRadius},
        {54, meanRadius, desvRadius},
        {85, meanRadius, desvRadius}
    };

    /*********************************GUI************************************/

    TGLayoutHints *expandHints = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 1, 1, 1, 1);
    TGLayoutHints *topHints = new TGLayoutHints(kLHintsExpandX | kLHintsTop, 1, 1, 1, 1);
    TGLayoutHints *centerHints = new TGLayoutHints(kLHintsExpandX | kLHintsCenterY, 1, 1, 1, 1);
    TGLayoutHints *bottomHints = new TGLayoutHints(kLHintsExpandX | kLHintsBottom, 1, 1, 1, 1);

    /*************************Model controls******************************/

    TGHorizontalFrame *hFrame1 = new TGHorizontalFrame(this, 0, 0, 0);
    AddFrame(hFrame1, expandHints);

    TGVerticalFrame *vFrame1 = new TGVerticalFrame(hFrame1);
    hFrame1->AddFrame(vFrame1, expandHints);

    TGHorizontalFrame *hFrame2 = new TGHeaderFrame(vFrame1);
    vFrame1->AddFrame(hFrame2, centerHints);
    TGLabel *XsLabel = new TGLabel(hFrame2, "Xs:");
    hFrame2->AddFrame(XsLabel, expandHints);
    TGNumberEntry *XsNumberEntry = new TGNumberEntry(hFrame2);
    XsNumberEntry->SetNumStyle(TGNumberFormat::kNESInteger);
    XsNumberEntry->SetNumAttr(TGNumberFormat::kNEAPositive); //Only 1, 2, 3, .... -> The seed must be 1, 2, 3..., i.e. w/o 0
    hFrame2->AddFrame(XsNumberEntry, expandHints);
    XsNumberEntry->SetNumber(Xs);

    TGHorizontalFrame *hFrame3 = new TGHeaderFrame(vFrame1);
    vFrame1->AddFrame(hFrame3, centerHints);
    TGLabel *YsLabel = new TGLabel(hFrame3, "Ys:");
    hFrame3->AddFrame(YsLabel, expandHints);
    TGNumberEntry *YsNumberEntry = new TGNumberEntry(hFrame3);
    YsNumberEntry->SetNumStyle(TGNumberFormat::kNESInteger);
    YsNumberEntry->SetNumAttr(TGNumberFormat::kNEAPositive); //Only 1, 2, 3, .... -> The seed must be 1, 2, 3..., i.e. w/o 0
    hFrame3->AddFrame(YsNumberEntry, expandHints);
    YsNumberEntry->SetNumber(Ys);

    TGHorizontalFrame *hFrame4 = new TGHeaderFrame(vFrame1);
    vFrame1->AddFrame(hFrame4, centerHints);
    TGLabel *ZsLabel = new TGLabel(hFrame4, "Zs:");
    hFrame4->AddFrame(ZsLabel, expandHints);
    TGNumberEntry *ZsNumberEntry = new TGNumberEntry(hFrame4);
    ZsNumberEntry->SetNumStyle(TGNumberFormat::kNESInteger);
    ZsNumberEntry->SetNumAttr(TGNumberFormat::kNEAPositive); //Only 1, 2, 3, .... -> The seed must be 1, 2, 3..., i.e. w/o 0
    hFrame4->AddFrame(ZsNumberEntry, expandHints);
    ZsNumberEntry->SetNumber(Zs);
    



    maxAttempts = Xs*Ys;

    TString numText;
    Int_t numNCsByLayer;

    // randObj = new TRandom1(seed);

    // for(Int_t n = 0; n < 100; n++) {
    //     cout << randObj->Gaus(6.7, 0.8) << endl;
    // }

    cout << endl << endl;

    cout << "Xs = " << Xs << endl;
    cout << "Ys = " << Ys << endl;
    cout << "Zs = " << Zs << endl;
    cout << "Vertical positions of NCs layers: " << endl;
    for(Int_t n = 0; n < (sizeof(zCenterRadiusMLs)/sizeof(zCenterRadiusMLs[0])); n++) {
        cout << "zCenters=" << zCenterRadiusMLs[n][0] << " radius=" << zCenterRadiusMLs[n][1] << endl;
    }
    cout << endl;

    numNCsByLayer = (Xs/(2*meanRadius)+1) * (Ys/(2*meanRadius)+1); // Number of spheres if these are join and rectangular arranged .
    
    cout << "expected number of NCs for layer = " << numNCsByLayer << endl;
    

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

    for(Int_t zCenter = 0; zCenter < (sizeof(zCenterRadiusMLs)/sizeof(zCenterRadiusMLs[0])); zCenter++) {
        for(Int_t n = 0; n < numNCsByLayer; n++) {
            // if(!PutNC(ZsCenterMLs[zCenter], radius)) break;
            PutNC((Int_t)zCenterRadiusMLs[zCenter][0], zCenterRadiusMLs[zCenter][1], zCenterRadiusMLs[zCenter][2]);
        }
    }

    cout << "final num NCs = " << cryst << endl;
    cout << "final Vol crystals = " << ObtainVolCryst() << endl;
    cout << endl;

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

    MapSubwindows();
    Resize(1700, 500);
    SetWMPosition(100, 50);
    MapWindow();

    // gApplication->Terminate(0);
}
Bool_t MAIN_FRAME::PutNC(Int_t zPosition, Double_t meanRadii, Double_t desvRadii){
    Int_t attempt = 0;
    Double_t xx, yy, zz, rr, xCent, yCent, zCent;
    Double_t radii;
    Int_t rndX, rndY;

    while(kTRUE) {
        Bool_t isRepeat = kFALSE;
        radii = randObj->Gaus(meanRadii, desvRadii);
        rndX = (Int_t)Xs * randObj->Rndm();
        rndY = (Int_t)Ys * randObj->Rndm();

        if(attempt >= maxAttempts) {
            // cout << "hello" << endl;
            return kFALSE;
        }
        
        for(Int_t n = 0; n < cryst; n++) {
            if(nanoCrysts[n][0] == rndX && nanoCrysts[n][1] == rndY && nanoCrysts[n][2] == rndX) {
                isRepeat = kTRUE;
                break;
            }
        }
        

        // for(Int_t n = 0; n < cryst; n++) {
        //     xx = rndX+0.5; // Convert to double, each site is in the middle of coordinate
        //     yy = rndY+0.5; // Convert to double, each site is in the middle of coordinate
        //     zz = zPosition+0.5; // Convert to double, each site is in the middle of coordinate

        //     xCent = nanoCrysts[n][0]; // Convert to double
        //     yCent = nanoCrysts[n][1]; // Convert to double
        //     zCent = nanoCrysts[n][2]; // Convert to double
        //     rr = nanoCrysts[n][3]; // Convert to double

        //     if(
        //         TMath::Sqrt(
        //             TMath::Power(xx-xCent, 2.0) + TMath::Power(yy-yCent, 2.0)  + TMath::Power(zz-zCent, 2.0)
        //         ) -(radii + rr) <= -0.3333333
        //     ) {
        //         isRepeat = kTRUE;
        //         attempt++;
        //         break;
        //     }
        // }
        
        if(!isRepeat) {
            nanoCrysts[cryst][0] = rndX;
            nanoCrysts[cryst][1] = rndY;
            nanoCrysts[cryst][2] = zPosition;
            nanoCrysts[cryst][3] = radii;
            break;
        } 
        attempt++;
        // else {
        //     cout << "Is repeated" << endl;
        // }
    }
    
    
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
    return kTRUE;


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
    return numFixVo;
}
Int_t MAIN_FRAME::ObtainTnpnp() {
    return 0;
}
/*************************Close application********************/
void MAIN_FRAME::CloseWindow()
{
    gApplication->Terminate(0);
}