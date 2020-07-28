/* This root macro was created to carry out full analysis of recieved data for Bismuth 210 */
/* Creator: Artem, lab technician of Low Backgound Measurement Laboatory, PNPI */


/*-------------------------------------------PREAMBLE-------------------------------------------*/

// Manually defined int-to-string macro (root compiler, apparently, doesn't support this feature)
  
  string to_string(int x)
  {
    std::stringstream ss; 
    ss << x;
    std::string str = ss.str();
    return str;
  }


/*The following function  takes a number (representing the hour of measurements) of a data file
 and returns the pointer to the created histogram */

TH1F* read_file(int x)   
{
  //We consider measurement data files with numbers of lenth 3
  int datafilenumberlenth  = 3 ;
  int bins_amount = 16384 ; //inluding 16394=2^14 bins

  //The number of file includes several zeros before x; thus, we need to add a zero-string before x
  std::string xstr = to_string(x);
  int numberofzeros = datafilenumberlenth - xstr.size();
  string zerostring (numberofzeros , '0');
  
  // Setting up the file path
  std::ostringstream filepath;
  filepath<<"/home/artem/Desktop/210Bi/210Bi_alpha_1."<<zerostring<<x;
  
  // Setting up histograms' names
  char *hname = new char*[27];
  sprintf(hname,"abs_hist%i",x);
  
  // Creating pointer to TH1F (T Histogtram 1-Dimensional Float)
	TH1F *hist = new TH1F(hname, "The Spectr for 210Bi", bins_amount, 0, bins_amount); 
        std::string name = filepath.str();
	std::cout<<name<<std::endl;
	std::ifstream inp(name.c_str()); // create input stream

	int i; // bin number
	double v; // bin value

  //Filling up of the histogram
	for (i=0; i<bins_amount; i++)
  {
		inp >> v;
		hist->SetBinContent(i, v);
	}
  return hist;
}

/*-------------------------------------------START-------------------------------------------*/

void rebin_24_full_analysis(void)
{ 

// We have 960 data files
int datafiles_amount = 960;
int bins_amount = 16384 ; //inluding 16394=2^14 bins

// Creating root file
TFile* file = new TFile("fileBi210.root","recreate");

// Creating pointer to TH1F histogams H,M,F for output of results 
TH1F *H = new TH1F("MaxBin","Maximum bin dependence on time",datafiles_amount,1,datafiles_amount);
TH1F *M = new TH1F("FirstMoment","Mean dependence on time",datafiles_amount,1,datafiles_amount);
TH1F *F = new TH1F("Fit","Fit parameter Mean dependence on time",datafiles_amount,1,datafiles_amount);
TH1F *S = new TH1F("Sigma","Sigma dependence on time",datafiles_amount,1,datafiles_amount);



for (j=1; j<datafiles_amount+1 ; j++)
{
  if(j>1)
  {
  TH1F* h_tmp = read_file(j-1);
  TH1F* h1 = read_file(j);
  h1->Add(h_tmp,-1);
    
  }else
      {
         TH1F* h1 = read_file(j);
      }

  h1->SetLineColor(1);
  h1->Write();
  //h1->Rebin(4);

  // Filling up H with maximum bins
  H->SetBinContent(j, (*h1).GetMaximumBin ());
   
  // Filling up M with expectation
  m = 0;
  c = 0;
  for (i=10000; i<= bins_amount; i++) //start from 10000 in order to exclude beta-spectrum from the result
  {
    m +=  i*(*h1).GetBinContent (i);
    c +=(*h1).GetBinContent (i);
  }
  m = m/c;
  M->SetBinContent(j, m);


  // Fit range: 
  a = 15830;
  b = 16000;

  // Creating function to fit our data
   TF1* f = new TF1("f","gaus",a,b);
  f->SetParameters(1e3,15830,25.7,0);

  // Filling up F with fit parameter mean
  h1->Fit(f,"","",a,b);
  F->SetBinContent(j,f->GetParameter(1));
  S->SetBinContent(j,f->GetParameter(2));


  //TH1F* h_tmp = h1;
}

//REBINNED! 24 is the number of hours in a day
H->Rebin(24);
M->Rebin(24);
F->Rebin(24);
S->Rebin(24);

//Write into file
H->Write();
M->Write();
F->Write();
S->Write();

//Draw on the same TCanvas
TCanvas *Canvas1=new TCanvas();

//MaxBin
H->SetLineColor(1);
H->Draw();

//Mean
M->SetLineColor(3);
M->Draw("same");

//Fit
F->SetLineColor(4)
F->Draw("same");

//Draw on the separate TCanvas
TCanvas *Canvas2=new TCanvas();
M->Draw();
TCanvas *Canvas3=new TCanvas();
F->Draw();

//Draw fit parameter sigma
TCanvas *Canvas4=new TCanvas();
S->Draw();

}
