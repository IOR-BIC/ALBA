/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVTKInterpolator.h,v $
  Language:  C++
  Date:      $Date: 2008-06-23 13:01:29 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVTKInterpolator_h
#define __mafVTKInterpolator_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDataInterpolator.h"
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
class MAF_EXPORT mafVTKInterpolator : public mafDataInterpolator
{
public:
  mafTypeMacro(mafVTKInterpolator,mafDataInterpolator);

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
  mafVTKInterpolator();
  virtual ~mafVTKInterpolator();

  virtual void PreExecute();
  virtual void Execute() {}

  vtkMAFDataPipe *m_VTKDataPipe; ///< VTK pipeline bridge component

private:
  mafVTKInterpolator(const mafVTKInterpolator&); // Not implemented
  void operator=(const mafVTKInterpolator&); // Not implemented  
};

#endif /* __mafVTKInterpolator_h */
 
