/*=========================================================================

 Program: MAF2
 Module: vtkMAFDataPipe
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __vtkMAFDataPipe_h
#define __vtkMAFDataPipe_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafObserver.h"
#include "vtkDataSetToDataSetFilter.h"
//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafDataPipe;
class vtkDataSet;

/** bridge class linking VTK pipeline to VME data pipe update mechanism.
  This object is a bridge between VTK pipeline update mechanism and MAF
  VME.
  @sa mflInterpolator
 
  @todo
  -
*/
class MAF_EXPORT vtkMAFDataPipe : public vtkDataSetToDataSetFilter
{
public:
  vtkTypeMacro(vtkMAFDataPipe,vtkDataSetToDataSetFilter);

  static vtkMAFDataPipe *New();

  /** Set the dataset to be reported as output of the VTK data pipe */
  virtual void SetNthInput(int num, vtkDataSet *input);

  virtual vtkDataSet *GetOutput(int idx);
  virtual vtkDataSet *GetOutput();

  /** A bit of magic making this filter to take into consideration VME data pipe MTime */
  virtual unsigned long GetMTime();

  /** return the modification time for internally stored information */
  virtual unsigned long GetInformationTime();

  /** the data pipe it is linked to */
  void SetDataPipe(mafDataPipe *dpipe);

  /** return the data pipe connected to this object */
  mafDataPipe *GetDataPipe() {return m_DataPipe;}

  /** 
    Overridden to attempt doing something before the pipeline is checked for the
    MTime (i.e. change the inputs) */
  virtual void UpdateInformation();

protected:
  vtkMAFDataPipe();
  virtual ~vtkMAFDataPipe();

  virtual void ExecuteInformation();
  virtual void Execute();

  mafDataPipe *m_DataPipe; ///< the data pipe this object is linked to

private:
  vtkMAFDataPipe(const vtkMAFDataPipe&); // Not implemented
  void operator=(const vtkMAFDataPipe&); // Not implemented
  
};

#endif /* __vtkMAFDataPipe_h */
 
