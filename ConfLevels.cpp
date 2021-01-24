
#include "RooWorkspace.h"
#include "RooAbsPdf.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooRandom.h"

#include "RooStats/ModelConfig.h"
#include "RooStats/ProfileLikelihoodCalculator.h"
#include "RooStats/LikelihoodInterval.h"



using namespace RooFit; 
using namespace RooStats;


void ConfLevels (void)  
{ 
    TFile *f = new TFile("GausFlatModel.root");
    RooWorkspace *w = (RooWorkspace *)f->Get("w");
    RooRealVar *x = w->var("x");
    RooAbsPdf *model = w->pdf("model");
    
    RooStats::ModelConfig * mc = (RooStats::ModelConfig *) w->obj("ModelConfig");
    RooAbsData *data = w->data("data");
    
    std::cout<< data << std::endl;
        
    
    RooStats::ProfileLikelihoodCalculator pl(*data,*mc);
    pl.SetConfidenceLevel(0.683); // 68.3% interval
    RooStats::LikelihoodInterval* interval = pl.GetInterval();

    // find the interval on the first Parameter of Interest (nsig)
    RooRealVar* firstPOI = (RooRealVar*) mc->GetParametersOfInterest()->first();
    double firstlowerLimit = interval->LowerLimit(*firstPOI);
    double firstupperLimit = interval->UpperLimit(*firstPOI);
    
    // find the interval on the second Parameter of Interest (mu)
    RooRealVar* secondPOI = (RooRealVar*) mc->GetParametersOfInterest()->find("mu");
    double secondlowerLimit = interval->LowerLimit(*secondPOI);
    double secondupperLimit = interval->UpperLimit(*secondPOI);
    
    cout << "\n68% interval on " <<firstPOI->GetName()<<" is : ["<< firstlowerLimit << ", "<< firstupperLimit <<"] "<<endl;
    
    cout << "\n68% interval on " <<secondPOI->GetName()<<" is : ["<< secondlowerLimit << ", "<< secondupperLimit <<"] "<<endl;
    
    
    
    
    
    pl.SetConfidenceLevel(0.90); // 90% interval
    RooStats::LikelihoodInterval* interval90 = pl.GetInterval();

    
    double firstlowerLimit90 = interval90->LowerLimit(*firstPOI);
    double firstupperLimit90 = interval90->UpperLimit(*firstPOI);
    
    double secondlowerLimit90 = interval90->LowerLimit(*secondPOI);
    double secondupperLimit90 = interval90->UpperLimit(*secondPOI);
    
    cout << "\n90% interval on " <<firstPOI->GetName()<<" is : ["<< firstlowerLimit90 << ", "<< firstupperLimit90 <<"] "<<endl;
    
    cout << "90% interval on " <<secondPOI->GetName()<<" is : ["<< secondlowerLimit90 << ", "<< secondupperLimit90 <<"] "<<endl;

    
    
    pl.SetConfidenceLevel(0.95); // 90% interval
    RooStats::LikelihoodInterval* interval95 = pl.GetInterval();

    
    double firstlowerLimit95 = interval95->LowerLimit(*firstPOI);
    double firstupperLimit95 = interval95->UpperLimit(*firstPOI);
    
    double secondlowerLimit95 = interval95->LowerLimit(*secondPOI);
    double secondupperLimit95 = interval95->UpperLimit(*secondPOI);
    
    cout << "\n95% interval on " <<firstPOI->GetName()<<" is : ["<< firstlowerLimit95 << ", "<< firstupperLimit95 <<"] "<<endl;
    
    cout << "95% interval on " <<secondPOI->GetName()<<" is : ["<< secondlowerLimit95 << ", "<< secondupperLimit95 <<"] "<<endl;

    
    
    LikelihoodIntervalPlot * plot = new LikelihoodIntervalPlot(interval);
     plot->SetNPoints(500); 
     plot->SetRange(40,140);
     plot->Draw(""); 
    
    LikelihoodIntervalPlot * plot90 = new LikelihoodIntervalPlot(interval90);
     plot90->SetNPoints(500);  
     plot90->Draw("same");  
    
    LikelihoodIntervalPlot * plot95 = new LikelihoodIntervalPlot(interval95);
     plot95->SetNPoints(500); 
     plot95->Draw("same");  
    
  
}



