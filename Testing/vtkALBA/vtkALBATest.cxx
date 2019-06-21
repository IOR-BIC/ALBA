#include "vtkALBAExample.h"

int main()
{
  vtkALBAExample* l = vtkALBAExample::New();
  l->Print(cout);
  l->Delete();
  return 0;
}
