/*=========================================================================
This source has no copyright.  It is intended to be copied by users
wishing to create their own VTK classes locally.
=========================================================================*/
#include "vtkMAFExample.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkMAFExample);

//----------------------------------------------------------------------------
vtkMAFExample::vtkMAFExample()
{
}

//----------------------------------------------------------------------------
vtkMAFExample::~vtkMAFExample()
{
}

//----------------------------------------------------------------------------
void vtkMAFExample::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
