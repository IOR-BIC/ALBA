#include "vtkMAFExample.h"

int main()
{
  vtkMAFExample* l = vtkMAFExample::New();
  l->Print(cout);
  l->Delete();
  return 0;
}
