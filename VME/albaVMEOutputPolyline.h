/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputPolyline
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEOutputPolyline_h
#define __albaVMEOutputPolyline_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEOutputVTK.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;
class mmaMaterial;

/** NULL output for VME node with a VTK image output data.
  albaVMEOutputPolyline is the output produced by a node generating an output
  with a VTK dataset.
*/
class ALBA_EXPORT albaVMEOutputPolyline : public albaVMEOutputVTK
{
public:
  albaVMEOutputPolyline();
  virtual ~albaVMEOutputPolyline();

  albaTypeMacro(albaVMEOutputPolyline,albaVMEOutputVTK);

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

  /** Return the Length*/
  albaString GetLengthSTR(){return m_Length;};

  /** Return the Number Of Points*/
  albaString GetNumberOfPointsSTR(){return m_NumberOfPoints;};

protected:
  mmaMaterial *m_Material; ///< material object used to store shading propertied to render the polyline

  albaGUI *CreateGui();

	albaString m_VtkDataType;
  albaString m_Length;
  albaString m_NumberOfPoints;
private:
  albaVMEOutputPolyline(const albaVMEOutputPolyline&); // Not implemented
  void operator=(const albaVMEOutputPolyline&); // Not implemented
};

#endif
