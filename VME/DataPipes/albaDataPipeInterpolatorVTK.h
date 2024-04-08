/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeInterpolatorVTK
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVTKInterpolator_h
#define __albaVTKInterpolator_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDataPipeInterpolator.h"
//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaVMEItemVTK;
class vtkALBADataPipe;

/** data interpolator specialized for albaVMEGenericVTK (for VTK data).
  This interpolator is specialized for VTK datasets. By default selects the 
  right VMEItem, extracts the inner VTK dataset and set it as input of the 
  vtkALBADataPipe .

  @sa vtkALBADataPipe albaVMEGenericVTK
  
  @todo
  -
*/
class ALBA_EXPORT albaDataPipeInterpolatorVTK : public albaDataPipeInterpolator
{
public:
  albaTypeMacro(albaDataPipeInterpolatorVTK,albaDataPipeInterpolator);

  /** This DataPipe accepts only VME's with internal DataArray. */
  virtual bool Accept(albaVME *vme);

  /**
    Get the MTime: this is the bit of magic that makes everything work.*/
  //virtual vtkMTimeType GetMTime();

  /** process events coming from vtkALBADataPipe bridge component */
  void OnEvent(albaEventBase *e);

   /**  Get the output of the interpolator item*/
  albaVMEItemVTK *GetCurrentItem() {return (albaVMEItemVTK *)m_CurrentItem;}

  /** return the VTK dataset generated as output to this data pipe */
  virtual vtkDataSet *GetVTKData();

  /** update the data pipe output */
  virtual void Update();

  /** retrieve vtk data pipe*/
  vtkALBADataPipe *GetVTKDataPipe(){return m_VTKDataPipe;};

protected:
  albaDataPipeInterpolatorVTK();
  virtual ~albaDataPipeInterpolatorVTK();

  virtual void PreExecute();
  virtual void Execute() {}

  vtkALBADataPipe *m_VTKDataPipe; ///< VTK pipeline bridge component

private:
  albaDataPipeInterpolatorVTK(const albaDataPipeInterpolatorVTK&); // Not implemented
  void operator=(const albaDataPipeInterpolatorVTK&); // Not implemented  
};

#endif /* __albaVTKInterpolator_h */
 
