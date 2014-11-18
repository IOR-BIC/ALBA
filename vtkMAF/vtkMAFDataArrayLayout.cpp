/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFDataArrayLayout.cpp,v $ 
  Language: C++ 
  Date: $Date: 2009-05-14 15:03:31 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "vtkMAFDataArrayLayout.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkMAFDataArrayLayout, "$Revision: 1.1.2.1 $");
vtkStandardNewMacro(vtkMAFDataArrayLayout);

// Construct object with default physical layout, which is
// interleaved mode, no gaps, starting offset = 0
vtkMAFDataArrayLayout::vtkMAFDataArrayLayout()
{
  this->NonInterleaved = 0;
  this->NonInterleavedSize = 1;
  this->NumberOfComponents = 1;
  this->StartOffset = 0;
  this->TupleGap = 0;
  this->ComponentGaps = NULL;
}

vtkMAFDataArrayLayout::~vtkMAFDataArrayLayout()
{
  if (this->ComponentGaps != NULL)
    delete[] this->ComponentGaps;
}

// Description:
// Set/Get the dimension (n) of the components. Must be >= 1. 
void vtkMAFDataArrayLayout::SetNumberOfComponents(int nComps)
{
  if (nComps < 0)
    nComps = 1;   //minimum is 1

  if (this->NumberOfComponents != nComps)
  {
    //change in the number of components
    if (this->ComponentGaps != NULL)
    {
      vtkIdType64* pNewGaps = new vtkIdType64[nComps];
      
      int i, nToCopy = nComps;
      if (this->NumberOfComponents < nToCopy)
        nToCopy = this->NumberOfComponents;

      for (i = 0; i < nToCopy; i++) {
        pNewGaps[i] = this->ComponentGaps[i];
      }
      
      for (; i < nComps; i++) {
        pNewGaps[i] = 0;
      }

      delete[] this->ComponentGaps;
      this->ComponentGaps = pNewGaps;
    }

    this->NumberOfComponents = nComps;
    this->Modified();
  }
}

// Set/Get the gap (in bytes) after the component 
void vtkMAFDataArrayLayout::SetComponentGap(int iComp, vtkIdType64 nGapBytes)
{
  if (nGapBytes == 0)
  {
    //check if there is any non-zero gap
    if (this->ComponentGaps == NULL)
      return; //makes no sense to have an array of zeros

    bool bAllZeros = true;    
    for (int i = 0; i < this->NumberOfComponents; i++) 
    {
      if (i != iComp && this->ComponentGaps[i] != 0) {
        bAllZeros = false; break; //there is some non-zero item
      }
    }

    if (bAllZeros)
    {
      //there was just one non-zero gap at iComp and we want to set it to 0
      delete[] this->ComponentGaps;
      this->ComponentGaps = NULL;
      this->Modified();
      return;
    }
  }

  if (this->ComponentGaps == NULL)
  {
    //new array
    if (iComp >= this->NumberOfComponents)
      this->NumberOfComponents = iComp + 1;

    this->ComponentGaps = new vtkIdType64[this->NumberOfComponents];
    memset(this->ComponentGaps, 0, this->NumberOfComponents*sizeof(vtkIdType64));
  }
  else if (iComp >= this->NumberOfComponents)
  {
    //array alreay exists but it is too small
    vtkIdType64* pNewGaps = new vtkIdType64[iComp + 1];

    int i;
    for (i = 0; i < this->NumberOfComponents; i++) {
      pNewGaps[i] = this->ComponentGaps[i];
    }

    this->NumberOfComponents = iComp + 1;
    for (; i < this->NumberOfComponents; i++) {
      pNewGaps[i] = 0;
    }

    delete[] this->ComponentGaps;
    this->ComponentGaps = pNewGaps;    
  }

  this->ComponentGaps[iComp] = nGapBytes;
  this->Modified();
}

void vtkMAFDataArrayLayout::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  
  os << indent << "StartOffset: " << this->GetStartOffset() << "\n";
  if (NonInterleaved != 0)
  {
    os << indent << "NonInterleaved BS: " << this->GetNonInterleavedSize() << "\n";
    os << indent << "TupleGap: " << this->GetTupleGap() << "\n";
    os << indent << "GetNumberOfComponents: " << this->GetNumberOfComponents() << "\n";
    if (ComponentGaps != NULL)
    {
      os << indent << "ComponentGaps: ";
      for (int i = 0; i < NumberOfComponents; i++)
      {
        os << this->ComponentGaps[i] << ", ";
      }
        
      os << "\n";
    }
  }
}

//Deep copy. 
void vtkMAFDataArrayLayout::DeepCopy(vtkMAFDataArrayLayout *src)
{
  this->NonInterleaved = src->NonInterleaved; 
  this->StartOffset = src->StartOffset; 
  this->NonInterleavedSize = src->NonInterleavedSize; 

  this->NumberOfComponents = src->NumberOfComponents; 
  this->TupleGap = src->TupleGap; 
 
  delete[] this->ComponentGaps;
  this->ComponentGaps = NULL;
  if (src->ComponentGaps != NULL) 
  {
    this->ComponentGaps = new vtkIdType64[this->NumberOfComponents];
    memcpy(this->ComponentGaps, src->ComponentGaps, this->NumberOfComponents*sizeof(vtkIdType64));
  }
}