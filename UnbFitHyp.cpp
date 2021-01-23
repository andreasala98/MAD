
#include "RooWorkspace.h"
#include "RooAbsPdf.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooRandom.h"
#include "RooArgList.h"

#include "RooStats/ModelConfig.h"

using namespace RooFit; 



void GausBkgModel(int nsig = 100,    // number of signal events 
                  int nbkg = 1000 )  // number of background events
{ 

   RooWorkspace w("w"); 
   w.factory("Polynomial:bkg_pdf(x[0,10], c[0])");
   w.factory("Gaussian:sig_pdf(x[0,10], mu[5,0,10], sigma[0.3])");

   w.factory("SUM:model(nsig[0,10000]*sig_pdf, nbkg[0,10000]*bkg_pdf)");  // for extended model

   RooAbsPdf * pdf = w.pdf("model");
   RooRealVar * x = w.var("x");  // the observable

   // set the desired value of signal and background events
   w.var("nsig")->setVal(nsig);
   w.var("nbkg")->setVal(nbkg);

   // generate the data

   // use fixed random numbers for reproducibility (use 0 for changing every time)
   RooRandom::randomGenerator()->SetSeed(111);

   // fix number of bins to 50 to plot or to generate data (default is 100 bins) 
   x->setBins(50);

   RooDataSet * data = pdf->generate( *x);  // will generate accordint to total S+B events
   //RooDataSet * data = pdf->generate( *x, AllBinned());  // will generate accordint to total S+B events
   data->SetName("data");
   w.import(*data);

   data->Print(); 


   RooPlot * plot2 = x->frame(Title("Flat background + Gaussian signal"));
   data->plotOn(plot2);
   plot2->Draw();

   RooFitResult * r = pdf->fitTo(*data, RooFit::Save(true), RooFit::Minimizer("Minuit","Migrad"));
   r->Print();

   pdf->plotOn(plot2);
   //draw the two separate pdf's
   pdf->plotOn(plot2, RooFit::Components("bkg_pdf"), RooFit::LineStyle(kDashed) );
   pdf->plotOn(plot2, RooFit::Components("sig_pdf"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed) );

   pdf->paramOn(plot2,Layout(0.5,0.9,0.85));

  plot2->Draw();


   RooStats::ModelConfig mc("ModelConfig",&w);
   mc.SetPdf(*pdf);
   mc.SetParametersOfInterest(RooArgList(*w.var("nsig"),*w.var("mu")));
   mc.SetObservables(*w.var("x"));
   // define set of nuisance parameters
//   w.defineSet("nuisParams","nbkg");

//   mc.SetNuisanceParameters(*w.set("nuisParams"));

   // import model in the workspace 
   w.import(mc);

   // write the workspace in the file
   TString fileName = "GausFlatModel.root";
   w.writeToFile(fileName,true);
   cout << "model written to file " << fileName << endl;

//    ******+
    
    RooStats::ModelConfig*  sbModel = (RooStats::ModelConfig*) w.obj("ModelConfig");
    RooRealVar* poi = (RooRealVar*) sbModel->GetParametersOfInterest()->first();
    // define the S+B snapshot (this is used for computing the expected significance)
    poi->setVal(50);
    sbModel->SetSnapshot(*poi);
    // create B only model
    RooStats::ModelConfig* bModel = (RooStats::ModelConfig*) sbModel->Clone();
    bModel->SetName("B_only_model");      
    poi->setVal(0);
    bModel->SetSnapshot( *poi  );

    
    
    RooStats::FrequentistCalculator   fc(*data, *sbModel, *bModel);
    fc.SetToys(2000,1000);    // 2000 for H0 (B) and 1000 for H1 (S+B)
    
    std::cout << "Ciao " << std::endl;

    // create the test statistics
    RooStats::ProfileLikelihoodTestStat profll(*sbModel->GetPdf());
//    RooStats::RatioOfProfiledLikelihoodsTestStat profll(*bModel->GetPdf(), *sbModel->GetPdf());
//    profll.SetSubtractMLE(false);
    // use one-sided profile likelihood
    profll.SetOneSidedDiscovery(true);

    // configure  ToyMCSampler and set the test statistics
    RooStats::ToyMCSampler *toymcs = (RooStats::ToyMCSampler*)fc.GetTestStatSampler();
    toymcs->SetTestStatistic(&profll);
      
    if (!sbModel->GetPdf()->canBeExtended())
       toymcs->SetNEventsPerToy(1);
    
    RooStats::HypoTestResult * fqResult = fc.GetHypoTest();
    fqResult->Print();
    
    // plot test statistic distributions
    RooStats::HypoTestPlot * plot = new RooStats::HypoTestPlot(*fqResult);
//    plot->SetLogYaxis();
    plot->Draw();
    
  
}
