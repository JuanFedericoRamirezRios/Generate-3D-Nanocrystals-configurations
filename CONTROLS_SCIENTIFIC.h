// #include "TF1.h"
// #include "TH2F.h"
// #include "Math/WrappedTF1.h"
// #include "Math/BrentRootFinder.h"
#include "TMath.h"
// #include "stdio.h"
// #include <iomanip>
#include "iostream"
// #include <filesystem>
// #include <sstream>
// #include "fstream"
// #include <unistd.h>
#include "TGSlider.h"
#include "TGNumberEntry.h"
// #include "TGButton.h"
// #include "TGTextEntry.h"
// #include "TCanvas.h"
#include "TGLabel.h"
#include "TGFrame.h"
// #include "TRootCanvas.h"
// #include "TStyle.h"
// #include "TRandom1.h"
// #include "TGraph.h"
// #include "TMultiGraph.h"
// #include "TMatrixD.h"
// #include "TVectorD.h"
// #include "TDecompLU.h"
// #include "TApplication.h"
// #include "TSystemDirectory.h"

using namespace std;

class CONTROLS_SCIENTIFIC : public TGVerticalFrame 
{
public:
    Double_t *value, fac, minFac, maxFac;
    Int_t exp, minExp, maxExp;
    TString name, units;

    TGLabel *valueLabel;

    TGNumberEntry *maxFacNumberEntry, *facNumberEntry, *minFacNumberEntry;
    TGVSlider *facSlider;

    TGNumberEntry *maxExpNumberEntry, *expNumberEntry, *minExpNumberEntry;
    TGVSlider *expSlider;

    CONTROLS_SCIENTIFIC(TGFrame *father, TString name, TString units, Double_t &value1) : TGVerticalFrame(father) {
        value = &value1;

        if (*value == 0.0)
        {
            fac = 0.0;
            minFac = 0.0;
            maxFac = 10.0;
            exp = 0;
            minExp = -5;
            maxExp = 5;
        }
        else
        {
            Double_t log10 = TMath::Log10(TMath::Abs(*value));

            if (log10 < 0.0)
                exp = log10 - 1.0;
            else
                exp = log10;

            if (*value < 0.0)
            {
                minFac = -10.0;
                maxFac = 0.0;
            }
            else
            {
                minFac = 0.0;
                maxFac = 10.0;
            }
            fac = *value / TMath::Power(10.0, exp);

            maxExp = exp + 5;
            minExp = exp - 5;
        }

        this->name = name;
        this->units = units;

        TGLayoutHints *expandHints = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 1, 1, 1, 1);
        TGLayoutHints *topHints = new TGLayoutHints(kLHintsExpandX | kLHintsTop, 1, 1, 1, 1);
        TGLayoutHints *centerHints = new TGLayoutHints(kLHintsExpandX | kLHintsCenterY, 1, 1, 1, 1);
        TGLayoutHints *bottomHints = new TGLayoutHints(kLHintsExpandX | kLHintsBottom, 1, 1, 1, 1);

        //     fmtStr << "This number is " << fixed << setprecision(1) << a << ".\n";
        //     string line = fmtStr.str();
        //     cout << line;

        TString text = name + "=";
        ostringstream oString;
        oString << setprecision(3) << (*value);
        text += oString.str();
        text += units;
        valueLabel = new TGLabel(this, text);
        AddFrame(valueLabel, topHints);

        TGHorizontalFrame *tghf2 = new TGHorizontalFrame(this);
        AddFrame(tghf2, expandHints);

        TGVerticalFrame *tgvfFac = new TGVerticalFrame(tghf2);
        tghf2->AddFrame(tgvfFac, expandHints);

        maxFacNumberEntry = new TGNumberEntry(tgvfFac);
        tgvfFac->AddFrame(maxFacNumberEntry, topHints);
        maxFacNumberEntry->SetNumber(maxFac);
        (maxFacNumberEntry->GetNumberEntry())->Connect("TextChanged(char*)", "CONTROLS_SCIENTIFIC", this, "DoMaxFac()");

        facNumberEntry = new TGNumberEntry(tgvfFac);
        tgvfFac->AddFrame(facNumberEntry, centerHints);
        facNumberEntry->SetNumber(fac);
        (facNumberEntry->GetNumberEntry())->Connect("TextChanged(char*)", "CONTROLS_SCIENTIFIC", this, "DoFac()");

        minFacNumberEntry = new TGNumberEntry(tgvfFac);
        tgvfFac->AddFrame(minFacNumberEntry, bottomHints);
        minFacNumberEntry->SetNumber(minFac);
        (minFacNumberEntry->GetNumberEntry())->Connect("TextChanged(char*)", "CONTROLS_SCIENTIFIC", this, "DoMinFac()");

        facSlider = new TGVSlider(tghf2, 150, kSlider1);
        tghf2->AddFrame(facSlider, expandHints);
        facSlider->SetRange(-100, 0);
        facSlider->SetPosition(-((fac - minFac) * 100.0 / (maxFac - minFac)));
        //	facSlider->Connect("Released()", "MAIN_FRAME", this, "SolveLightDrawSolution()");
        facSlider->Connect("PositionChanged(Int_t)", "CONTROLS_SCIENTIFIC", this, "MoveFacSlider(Int_t)");

        TGVerticalFrame *tgvfExp = new TGVerticalFrame(tghf2);
        tghf2->AddFrame(tgvfExp, expandHints);

        maxExpNumberEntry = new TGNumberEntry(tgvfExp);
        maxExpNumberEntry->SetNumStyle(TGNumberFormat::kNESInteger);
        tgvfExp->AddFrame(maxExpNumberEntry, topHints);
        maxExpNumberEntry->SetNumber(maxExp);
        (maxExpNumberEntry->GetNumberEntry())->Connect("TextChanged(char*)", "CONTROLS_SCIENTIFIC", this, "DoMaxExp()");

        expNumberEntry = new TGNumberEntry(tgvfExp);
        expNumberEntry->SetNumStyle(TGNumberFormat::kNESInteger);
        tgvfExp->AddFrame(expNumberEntry, centerHints);
        expNumberEntry->SetNumber(exp);
        (expNumberEntry->GetNumberEntry())->Connect("TextChanged(char*)", "CONTROLS_SCIENTIFIC", this, "DoExp()");

        minExpNumberEntry = new TGNumberEntry(tgvfExp);
        minExpNumberEntry->SetNumStyle(TGNumberFormat::kNESInteger);
        tgvfExp->AddFrame(minExpNumberEntry, bottomHints);
        minExpNumberEntry->SetNumber(minExp);
        (minExpNumberEntry->GetNumberEntry())->Connect("TextChanged(char*)", "CONTROLS_SCIENTIFIC", this, "DoMinExp()");

        expSlider = new TGVSlider(tghf2, 150, kSlider1);
        tghf2->AddFrame(expSlider, expandHints);
        expSlider->SetRange(-100, 0);
        expSlider->SetPosition(-((exp - minExp) * 100.0 / (maxExp - minExp)));
        //	expSlider->Connect("Released()", "MAIN_FRAME", this, "SolveLightDrawSolution()");
        expSlider->Connect("PositionChanged(Int_t)", "CONTROLS_SCIENTIFIC", this, "MoveExpSlider(Int_t)");

        ChangeOptions(/*kRaisedFrame |*/ kSunkenFrame);
    }
    void DoMaxFac()
    {
        if (maxFacNumberEntry->GetNumber() >= fac)
        {
            maxFac = maxFacNumberEntry->GetNumber();
        }
        else
        {
            maxFac = fac;
            maxFacNumberEntry->SetNumber(fac);
        }
        // Change position of slider
        facSlider->SetPosition(-((fac - minFac) * 100.0 / (maxFac - minFac)));
    }
    void DoFac()
    {
        if ((minFac <= facNumberEntry->GetNumber()) && (facNumberEntry->GetNumber() <= maxFac))
        {
            fac = facNumberEntry->GetNumber();
        }
        else if (minFac > facNumberEntry->GetNumber())
        {
            fac = minFac;
            facNumberEntry->SetNumber(minFac);
        }
        else if (facNumberEntry->GetNumber() > maxFac)
        {
            fac = maxFac;
            facNumberEntry->SetNumber(maxFac);
        }
        //*exp = expNumberEntry->GetNumber(); //Actually value of exp is double. I do not konw why...
        *value = fac * TMath::Power(10.0, exp);
        TString text = name + "=";
        ostringstream oString;
        oString << setprecision(3) << (*value);
        text += oString.str();
        text += units;
        valueLabel->SetText(text);
        // Change position of slider
        facSlider->SetPosition(-((fac - minFac) * 100.0 / (maxFac - minFac)));
    }
    void MoveFacSlider(Int_t param)
    {
        fac = (-1.0 * param / 100.0) * (maxFac - minFac) + minFac;
        facNumberEntry->SetNumber(fac); // Change of NumberEntry call to TextChanged.
        //*value = fac*TMath::Power(10.0, exp);
        // TString text = name + "="; text += *value; text += units;
        // valueLabel->SetText(text);
        //	cout << *value << endl;
    }
    void DoMinFac()
    {
        if (minFacNumberEntry->GetNumber() <= fac)
        {
            minFac = minFacNumberEntry->GetNumber();
        }
        else
        {
            minFac = fac;
            minFacNumberEntry->SetNumber(fac);
        }
        // Change position of slider
        facSlider->SetPosition(-((fac - minFac) * 100.0 / (maxFac - minFac)));
    }
    void DoMaxExp()
    {
        if (maxExpNumberEntry->GetNumber() >= exp)
        {
            maxExp = maxExpNumberEntry->GetNumber();
        }
        else
        {
            maxExp = exp;
            maxExpNumberEntry->SetNumber(exp);
        }
        // Change position of slider
        expSlider->SetPosition(-((exp - minExp) * 100.0 / (maxExp - minExp)));
    }
    void DoExp()
    {
        if ((minExp <= expNumberEntry->GetNumber()) && (expNumberEntry->GetNumber() <= maxExp))
        {
            exp = expNumberEntry->GetNumber();
        }
        else if (minExp > expNumberEntry->GetNumber())
        {

            exp = minExp;
            expNumberEntry->SetNumber(minExp);
        }
        else if (expNumberEntry->GetNumber() > maxExp)
        {
            exp = maxExp;
            expNumberEntry->SetNumber(maxExp);
        }
        *value = fac * TMath::Power(10.0, exp);
        TString text = name + "=";
        ostringstream oString;
        oString << setprecision(3) << (*value);
        text += oString.str();
        text += units;
        valueLabel->SetText(text);
        // Change position of slider
        expSlider->SetPosition(-((exp - minExp) * 100.0 / (maxExp - minExp)));
    }
    void MoveExpSlider(Int_t param)
    {
        exp = (-1.0 * param / 100.0) * (maxExp - minExp) + minExp;
        expNumberEntry->SetNumber(exp);

        // 	*value = fac*TMath::Power(10.0, exp);
        // 	TString text = name + "="; text += *value; text += units;
        // 	valueLabel->SetText(text);
        //	cout << *value << endl;
    }
    void DoMinExp()
    {
        if (minExpNumberEntry->GetNumber() <= exp)
        {
            minExp = minExpNumberEntry->GetNumber();
        }
        else
        {
            minExp = exp;
            minExpNumberEntry->SetNumber(exp);
        }
        // Change position of slider
        expSlider->SetPosition(-((exp - minExp) * 100.0 / (maxExp - minExp)));
    }
};
