/*=========================================================================
This source has no copyright.  It is intended to be copied by users
wishing to create their own VTK classes locally.
=========================================================================*/
// .NAME vtkMAFExample - Example class using VTK.
// .SECTION Description
// vtkMAFExample is a simple class that uses VTK.  This class can be
// copied and modified to produce your own classes.

#ifndef __vtkMAFExample_h
#define __vtkMAFExample_h

#include "vtkMAFConfigure.h" // Include configuration header.
#include "vtkObject.h"

class VTK_vtkMAF_EXPORT vtkMAFExample : public vtkObject
{
public:
  static vtkMAFExample* New();
  vtkTypeMacro(vtkMAFExample, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
protected:
  vtkMAFExample();
  ~vtkMAFExample();
  
private:
  vtkMAFExample(const vtkMAFExample&);  // Not implemented.
  void operator=(const vtkMAFExample&);  // Not implemented.
};

#endif
