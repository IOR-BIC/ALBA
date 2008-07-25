/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputPolyline.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:05:59 $
  Version:   $Revision: 1.5 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEOutputPolyline_h
#define __mafVMEOutputPolyline_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEOutputVTK.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;
class mmaMaterial;

/** NULL output for VME node with a VTK image output data.
  mafVMEOutputPolyline is the output produced by a node generating an output
  with a VTK dataset.
*/
class MAF_EXPORT mafVMEOutputPolyline : public mafVMEOutputVTK
{
public:
  mafVMEOutputPolyline();
  virtual ~mafVMEOutputPolyline();

  mafTypeMacro(mafVMEOutputPolyline,mafVMEOutputVTK);

  /**
    Return a VTK dataset corresponding to the current time. This is
    the output of the DataPipe currently attached to the VME.
    Usually the output is a  "smart copy" of one of the dataset in 
    the DataArray. In some cases it can be NULL, e.g. in case the number
    of stored Items is 0. Also special VME could not support VTK dataset output.
    An event is rised when the output data changes to allow attached classes to 
    update their input.*/
  virtual vtkPolyData *GetPolylineData();

  /** return material attribute of this polyline if present */
  mmaMaterial *GetMaterial();

  /** set the material of the polyline.*/ 
  void SetMaterial(mmaMaterial *material);

  /** Update all the output data structures (data, bounds, matrix and abs matrix).*/
  virtual void Update();

	/** Calculate Length of polyline*/
	double CalculateLength();

protected:
  mmaMaterial *m_Material; ///< material object used to store shading propertied to render the polyline

  mafGUI *CreateGui();

  mafString m_Length;
  mafString m_NumberOfPoints;
private:
  mafVMEOutputPolyline(const mafVMEOutputPolyline&); // Not implemented
  void operator=(const mafVMEOutputPolyline&); // Not implemented
};

#endif
