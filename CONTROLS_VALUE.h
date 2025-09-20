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

class CONTROLS_VALUE : public TGVerticalFrame
{
public:
    Double_t *value, min, max;
    TString name, units;

    TGLabel *valueLabel;

    TGNumberEntry *maxNumberEntry, *valueNumberEntry, *minNumberEntry;
    TGVSlider *valueSlider;

    CONTROLS_VALUE(TGFrame *father, TString name, TString units, Double_t &value1) : TGVerticalFrame(father)
    {

        this->value = &value1;
        this->max = MaxDoubleValue(*value);
        this->min = MinDoubleValue(*value);
        this->name = name;
        this->units = units;

        TGLayoutHints *expandHints = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 1, 1, 1, 1);
        TGLayoutHints *topHints = new TGLayoutHints(kLHintsExpandX | kLHintsTop, 1, 1, 1, 1);
        TGLayoutHints *centerHints = new TGLayoutHints(kLHintsExpandX | kLHintsCenterY, 1, 1, 1, 1);
        TGLayoutHints *bottomHints = new TGLayoutHints(kLHintsExpandX | kLHintsBottom, 1, 1, 1, 1);

        TString text = name + "=";
        ostringstream oString;
        oString << setprecision(3) << (*value);
        text += oString.str();
        text += units;
        valueLabel = new TGLabel(this, text);
        AddFrame(valueLabel, topHints);

        TGHorizontalFrame *tghf2 = new TGHorizontalFrame(this);
        AddFrame(tghf2, expandHints);

        TGVerticalFrame *tgvfValue = new TGVerticalFrame(tghf2);
        tghf2->AddFrame(tgvfValue, expandHints);

        maxNumberEntry = new TGNumberEntry(tgvfValue);
        tgvfValue->AddFrame(maxNumberEntry, topHints);
        maxNumberEntry->SetNumber(max);
        (maxNumberEntry->GetNumberEntry())->Connect("TextChanged(char*)", "CONTROLS_VALUE", this, "DoMax()");

        valueNumberEntry = new TGNumberEntry(tgvfValue);
        tgvfValue->AddFrame(valueNumberEntry, centerHints);
        valueNumberEntry->SetNumber(*value);
        (valueNumberEntry->GetNumberEntry())->Connect("TextChanged(char*)", "CONTROLS_VALUE", this, "DoValue()");

        minNumberEntry = new TGNumberEntry(tgvfValue);
        tgvfValue->AddFrame(minNumberEntry, bottomHints);
        minNumberEntry->SetNumber(min);
        (minNumberEntry->GetNumberEntry())->Connect("TextChanged(char*)", "CONTROLS_VALUE", this, "DoMin()");

        valueSlider = new TGVSlider(tghf2, 150, kSlider1);
        tghf2->AddFrame(valueSlider, expandHints);
        valueSlider->SetRange(-100, 0);
        valueSlider->SetPosition(-((*value - min) * 100.0 / (max - min)));
        //	valueSlider->Connect("Released()", "MAIN_FRAME", this, "SolveLightDrawSolution()");
        valueSlider->Connect("PositionChanged(Int_t)", "CONTROLS_VALUE", this, "MoveSlider(Int_t)");

        ChangeOptions(/*kRaisedFrame |*/ kSunkenFrame);
    }
    Double_t MinDoubleValue(Double_t doubleValue)
    {
        if (doubleValue == 0.0)
            return -1.0;
        if (doubleValue > 0.0)
            return 0.0;

        // if(doubleValue < 0.0)

        doubleValue *= -1.0; // Absolute value.

        Double_t log10 = TMath::Log10(doubleValue);

        Int_t exp10;
        if (log10 < 0.0)
            exp10 = log10;
        else
            exp10 = log10 + 1.0;
        Double_t minVal = -TMath::Power(10.0, exp10);

        if (doubleValue < minVal)
            return -doubleValue * 10.0;

        return minVal;

        return 0.0;
    }
    Double_t MaxDoubleValue(Double_t doubleValue)
    {
        if (doubleValue == 0.0)
            return 1.0;
        if (doubleValue < 0.0)
            return 0.0;

        // if(doubleValue > 0.0)

        Double_t log10 = TMath::Log10(doubleValue);

        Int_t exp10;
        if (log10 < 0.0)
            exp10 = log10;
        else
            exp10 = log10 + 1.0;
        Double_t maxVal = TMath::Power(10.0, exp10);

        if (doubleValue > maxVal)
            return doubleValue * 10.0;

        return maxVal;
    }
    void DoMax()
    {
        if (maxNumberEntry->GetNumber() >= *value)
        {
            max = maxNumberEntry->GetNumber();
        }
        else
        {
            max = *value;
            maxNumberEntry->SetNumber(*value);
        }
        // Change position of slider
        valueSlider->SetPosition(-((*value - min) * 100.0 / (max - min)));
    }
    void DoValue()
    {
        if ((min <= valueNumberEntry->GetNumber()) && (valueNumberEntry->GetNumber() <= max))
        {
            *value = valueNumberEntry->GetNumber();
        }
        else if (min > valueNumberEntry->GetNumber())
        {
            *value = min;
            valueNumberEntry->SetNumber(min);
        }
        else if (valueNumberEntry->GetNumber() > max)
        {
            *value = max;
            valueNumberEntry->SetNumber(max);
        }

        TString text = name + "=";
        ostringstream oString;
        oString << setprecision(3) << (*value);
        text += oString.str();
        text += units;
        valueLabel->SetText(text);
        // Change position of slider
        valueSlider->SetPosition(-((*value - min) * 100.0 / (max - min)));
    }
    void MoveSlider(Int_t param)
    {
        *value = (-1.0 * param / 100.0) * (max - min) + min;
        valueNumberEntry->SetNumber(*value);

        // 	TString text = name + "="; text += *value; text += units;
        // 	valueLabel->SetText(text);
        //	cout << *value << endl;
    }
    void DoMin()
    {
        if (minNumberEntry->GetNumber() <= *value)
        {
            min = minNumberEntry->GetNumber();
        }
        else
        {
            min = *value;
            minNumberEntry->SetNumber(*value);
        }
        // Change position of slider
        valueSlider->SetPosition(-((*value - min) * 100.0 / (max - min)));
    }

    
};