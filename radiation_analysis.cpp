/* This root macro was created to carry out radiation analysis of recieved data for Bismuth 210 */
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
  string dir = "/home/artem/Desktop/alpha_1203_M2";
  int datafilenumberlenth  = 3 ;
  int bins_amount = 16384 ; //inluding 16394=2^14 bins

  if (x>960) 
  {
    int n=x+1000;
    cout<<n;
    std::ostringstream filepath;
     filepath<<"/home/artem/Desktop/Analysis/Bismuth/[DATA]21nov/210Bi_alpha_1."<<x;
  }else
  {

  //The number of file includes several zeros before x; thus, we need to add a zero-string before x
  std::string xstr = to_string(x);
  int numberofzeros = datafilenumberlenth - xstr.size();
  string zerostring (numberofzeros , '0');
  
  // Setting up the file path
  int n = x;
  std::ostringstream filepath;
   filepath<<"/home/artem/Desktop/Analysis/Bismuth/[DATA]21nov/210Bi_alpha_1."<<zerostring<<x;
  }
  
  // Setting up histograms' names
  char *hname = new char[27];
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

void radiation_analysis(void)
{

// We have 960 data files
int datafiles_amount = 960;
int bins_amount = 16384 ; //inluding 16394=2^14 bins

// Creating root file
TFile* file = new TFile("fileBi210.root","recreate");


// Creating pointer to TH1F histogams H,F,I for output of results 
TH1F *H = new TH1F("Max","Maximum dependence on time",datafiles_amount,1,datafiles_amount);
TH1F *F = new TH1F("FitMax","Fit parameter maximum dependence on time",datafiles_amount,1,datafiles_amount);
TH1F *I = new TH1F("Int","Events in a peak",datafiles_amount,1,datafiles_amount);

int startnumber = 1;
TH1F* hists[10000];

for (int j=startnumber; j<=datafiles_amount ; j++)
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

// Filling up H with values of maximum bins
int maxbin = (*h1).GetMaximumBin();
H->SetBinContent(j, (*h1).GetBinContent(maxbin));

// Creating function to fit our data
TF1* f = new TF1("f","gaus+[3]",0,16000);
f->SetParameters(1e3,15850,25.7,0);

// Fit range
int left_fit_margin = (*h1).GetMaximumBin()-30;
int right_fit_margin = (*h1).GetMaximumBin()+100

// Fit range: 15850 - 15920
h1->Fit(f,"","",left_fit_margin,right_fit_margin);

// Filling up F with values of maximum bins (now we find maximum bin using fit!)
int maxfitbin= f->GetParameter(1);
F->SetBinContent(j,(*h1).GetBinContent(maxfitbin));

//Integrating the alpha peak. It exists between 14000 and 16000
int left_margin = 10000;
int right_margin = 16000;
int sum = 0;

for (int k=left_margin; k<right_margin ; k++)
{
sum += (*h1).GetBinContent(k);
}
// Filling up I with the sum of bins' values between margins
 I->SetBinContent(j, sum);
}

//Write into file
H->Write();
F->Write();
I->Write();

// Draw results
TCanvas *Canvas1=new TCanvas();
H->Draw();
TCanvas *Canvas3=new TCanvas();
F->Draw();
TCanvas *Canvas3=new TCanvas();
I->Draw();

}
