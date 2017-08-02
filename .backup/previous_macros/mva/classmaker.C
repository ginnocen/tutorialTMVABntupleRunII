void classmaker(TString filename)
{
  ofstream myfile("header.h");
  myfile<<"#include "<<" \""<<filename<< "\"";
}
