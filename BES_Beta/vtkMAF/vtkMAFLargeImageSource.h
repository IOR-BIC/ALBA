/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFLargeImageSource.h,v $ 
  Language: C++ 
  Date: $Date: 2011-05-26 08:51:01 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#ifndef __vtkMAFLargeImageSource_h
#define __vtkMAFLargeImageSource_h

#include "vtkMEDConfigure.h"
#include "vtkSource.h"
#include "vtkMAFIdType64.h"

class vtkMAFLargeImageData;

class VTK_vtkMED_EXPORT vtkMAFLargeImageSource : public vtkSource
{
public:
  static vtkMAFLargeImageSource* New();

  vtkTypeRevisionMacro(vtkMAFLargeImageSource,vtkSource);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get the output of this source.
  void SetOutput(vtkMAFLargeImageData *output);
  vtkMAFLargeImageData *GetOutput();
  vtkMAFLargeImageData *GetOutput(int idx);
  
protected:
  vtkMAFLargeImageSource();
  ~vtkMAFLargeImageSource() {};

  void Execute();
  virtual void Execute(vtkMAFLargeImageData *data);

  // a helper method that sets the extent and allocates the output 
  // passed into it and returns it as an image data
  virtual vtkMAFLargeImageData *AllocateOutputData(vtkDataObject *out);

private:
  vtkMAFLargeImageSource(const vtkMAFLargeImageSource&);  // Not implemented.
  void operator=(const vtkMAFLargeImageSource&);  // Not implemented.
};


#endif


