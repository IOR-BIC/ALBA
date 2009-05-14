/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFDataArrayLayout.h,v $ 
  Language: C++ 
  Date: $Date: 2009-05-14 15:03:31 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
  vtkMAFDataArrayLayout describes the physical layout of data array described
  by vtkMAFDataArrayDescriptor, e.g., interleaved, non-interleaved modes,
  offsets, gaps between components, etc.
  =========================================================================
*/
#ifndef vtkMAFDataArrayLayout_h__
#define vtkMAFDataArrayLayout_h__

#include "vtkObject.h"
#include "vtkMAFIdType64.h"

class VTK_COMMON_EXPORT vtkMAFDataArrayLayout : public vtkObject 
{
public:
  vtkTypeRevisionMacro(vtkMAFDataArrayLayout,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  static vtkMAFDataArrayLayout* New();
public:
  // Description:
  // Set/Get the mode: interleaved or non-interleaved. 
  vtkSetMacro(NonInterleaved, int);
  inline int GetNonInterleaved() {
    return this->NonInterleaved;
  }  

  // Description:
  // Set the start offset, i.e., the number of bytes to be skipped
  // from the beginning of the physical media to reach the first element
  inline void SetStartOffset(vtkIdType64 nOffsetInBytes) 
  {
    if (this->StartOffset != nOffsetInBytes) 
    {
      this->StartOffset = nOffsetInBytes;
      this->Modified();
    }
  }

  // Get the start offset, i.e., the number of bytes to be skipped
  // from the beginning of the physical media to reach the first element
  inline vtkIdType64 GetStartOffset() {
    return this->StartOffset;
  }

  // Description:
  // Set the number of elements in one non-interleaved block
  // useful, if the data is stored, e.g., slices by slice in non-interleaved mode  
  inline void SetNonInterleavedSize(vtkIdType64 nSizeInBytes) 
  {
    if (this->NonInterleavedSize != nSizeInBytes) 
    {
      this->NonInterleavedSize = nSizeInBytes;
      this->Modified();
    }
  }

  // Get the number of elements in one non-interleaved block
  // useful, if the data is stored, e.g., slices by slice in non-interleaved mode
  inline vtkIdType64 GetNonInterleavedSize() {
    return this->NonInterleavedSize;
  }

  // Description:
  // Set number of bytes to be skipped after every tuple (or block - non-interleaved)
  // i.e., C1,C2,C3...Cn|GAP|C1,C2,...Cn|GAP ... or C1,C1,...C1|GAP|C2,C2,...C2|GAP|  
  inline void SetTupleGap(vtkIdType64 nSizeInBytes) 
  {
    if (this->TupleGap != nSizeInBytes) 
    {
      this->TupleGap = nSizeInBytes;
      this->Modified();
    }
  }

  // Get number of bytes to be skipped after every tuple (or block - non-interleaved)
  // i.e., C1,C2,C3...Cn|GAP|C1,C2,...Cn|GAP ... or C1,C1,...C1|GAP|C2,C2,...C2|GAP|
  inline vtkIdType64 GetTupleGap() {
    return this->TupleGap;
  }

  // Description:
  // Set/Get the dimension (n) of the components. Must be >= 1. 
  void SetNumberOfComponents(int nComps);
  inline int GetNumberOfComponents() {
    return this->NumberOfComponents;
  };

  // Description:
  // Set/Get the gap (in bytes) after the component 
  void SetComponentGap(int iComp, vtkIdType64 nGapBytes);
  inline vtkIdType64 GetComponentGap(int iComp) 
  {
    if (this->ComponentGaps == NULL || iComp >= this->NumberOfComponents)
      return 0; //no gap after the specified component

    return this->ComponentGaps[iComp];
  };

  // Gets the array with gaps after components
  // it returns NULL, if there is no gap
  inline vtkIdType64* GetComponentGaps() {
    return this->ComponentGaps;
  }

  //Deep copy. 
  void DeepCopy(vtkMAFDataArrayLayout *src);

protected:
  // Construct object with default physical layout, which is
  // interleaved mode, no gaps, starting offset = 0
  vtkMAFDataArrayLayout();
  ~vtkMAFDataArrayLayout();

protected:
  int NonInterleaved;               //<non-zero if the data is stored in non-interleaved fashion
  vtkIdType64 StartOffset;          //<the number of bytes on the physical medium before the first element   
  vtkIdType64 NonInterleavedSize;   //<number of elements in one block before the next component chain, ignored for interleaved layout                                     
                                    //it allows: C1,C1,...C1,C2,C2...C2, ... Cn,Cn,..Cn,C1,C1,...C1, ....
  int NumberOfComponents;           //<number of components - should be the same as in vtkMAFDataArrayDescriptor
  vtkIdType64 TupleGap;             //<number of bytes to be skipped after every tuple (or block - non-interleaved)  
  vtkIdType64* ComponentGaps;       //<number of bytes to be skipped after i-th component 

private:
  vtkMAFDataArrayLayout(const vtkMAFDataArrayLayout&);  // Not implemented.
  void operator=(const vtkMAFDataArrayLayout&);  // Not implemented.
};


#endif // vtkMAFDataArrayLayout_h__