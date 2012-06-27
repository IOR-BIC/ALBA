/*=========================================================================

 Program: MAF2
 Module: mafDataPipeInterpolatorVTK
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVTKInterpolator_h
#define __mafVTKInterpolator_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDataPipeInterpolator.h"
//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafVMEItemVTK;
class vtkMAFDataPipe;

/** data interpolator specialized for mafVMEGenericVTK (for VTK data).
  This interpolator is specialized for VTK datasets. By default selects the 
  right VMEItem, extracts the inner VTK dataset and set it as input of the 
  vtkMAFDataPipe .

  @sa vtkMAFDataPipe mafVMEGenericVTK
  
  @todo
  -
*/
class MAF_EXPORT mafDataPipeInterpolatorVTK : public mafDataPipeInterpolator
{
public:
  mafTypeMacro(mafDataPipeInterpolatorVTK,mafDataPipeInterpolator);

  /** This DataPipe accepts only VME's with internal DataArray. */
  virtual bool Accept(mafVME *vme);

  /**
    Get the MTime: this is the bit of magic that makes everything work.*/
  //virtual unsigned long GetMTime();

  /** process events coming from vtkMAFDataPipe bridge component */
  void OnEvent(mafEventBase *e);

   /**  Get the output of the interpolator item*/
  mafVMEItemVTK *GetCurrentItem() {return (mafVMEItemVTK *)m_CurrentItem;}

  /** return the VTK dataset generated as output to this data pipe */
  virtual vtkDataSet *GetVTKData();

  /** update the data pipe output */
  virtual void Update();

  /** retrieve vtk data pipe*/
  vtkMAFDataPipe *GetVTKDataPipe(){return m_VTKDataPipe;};

protected:
  mafDataPipeInterpolatorVTK();
  virtual ~mafDataPipeInterpolatorVTK();

  virtual void PreExecute();
  virtual void Execute() {}

  vtkMAFDataPipe *m_VTKDataPipe; ///< VTK pipeline bridge component

private:
  mafDataPipeInterpolatorVTK(const mafDataPipeInterpolatorVTK&); // Not implemented
  void operator=(const mafDataPipeInterpolatorVTK&); // Not implemented  
};

#endif /* __mafVTKInterpolator_h */
 
