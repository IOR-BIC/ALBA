/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputVolume.h,v $
  Language:  C++
  Date:      $Date: 2005-07-22 13:50:44 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEOutputVolume_h
#define __mafVMEOutputVolume_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEOutputVTK.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkImageData;
class vtkRectilinearGrid;

/** NULL output for VME node with a VTK image output data.
  mafVMEOutputVolume is the output produced by a node generating an output
  with a VTK dataset. */
class MAF_EXPORT mafVMEOutputVolume : public mafVMEOutputVTK
{
public:
  mafVMEOutputVolume();
  virtual ~mafVMEOutputVolume();

  mafTypeMacro(mafVMEOutputVolume,mafVMEOutputVTK);

  /** return the output data casting to vtkImageData */
  virtual vtkImageData *GetStructuredData();
  
  /** return the output data casting to vtkRectilinearGrid */
  virtual vtkRectilinearGrid *GetRectilinearData();

protected:
  mafOBB m_VolumeBounds;
  mmgGui *CreateGui();

private:
  mafVMEOutputVolume(const mafVMEOutputVolume&); // Not implemented
  void operator=(const mafVMEOutputVolume&); // Not implemented
};
#endif
