/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputScalar
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEOutputScalar_h
#define __albaVMEOutputScalar_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEOutput.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVMEGeneric;
#ifdef ALBA_USE_VTK
class vtkPolyData;
#endif //ALBA_USE_VTK

/** NULL output for VME node with a scalar output data.
  albaVMEOutputScalar is the output produced by a node generating an output
  with a double value representing scalar data.
*/
class ALBA_EXPORT albaVMEOutputScalar : public albaVMEOutput
{
public:
  albaTypeMacro(albaVMEOutputScalar,albaVMEOutput);

  /**
    Return a VNL matrix corresponding to the current time. This is
    the output of the DataPipe currently attached to the VME Scalar.
    Usually the output is a  "smart copy" of one of the vnl matrix in 
    the DataArray. An event is rise when the output data changes to allow attached classes to 
    update their input.*/
  virtual double GetScalarData();

#ifdef ALBA_USE_VTK
  /**
  Return a VTK dataset with the choose scalar information corresponding to the current time. This is
  the output of the DataPipe currently attached to the VME.
  Usually the output is a  "smart copy" of one of the dataset in 
  the DataArray. In some cases it can be NULL, e.g. in case the number
  of stored Items is 0. Also special VME could not support VTK dataset output.
  An event rise when the output data changes to allow attached classes to 
  update their input.*/
  virtual vtkDataSet *GetVTKData();

  /** Function to update VTK representation of double value representing the scalar data.*/
  void UpdateVTKRepresentation();
#endif

  /** Update all the output data structures (data, bounds, matrix and abs matrix).*/
  virtual void Update();

	/** Retrieve scalar data written in a string*/
	const char* GetScalarDataString(){return m_ScalarDataString.GetCStr();};
    
protected:
  albaVMEOutputScalar(); // to be allocated with New()
  virtual ~albaVMEOutputScalar(); // to be deleted with Delete()

#ifdef ALBA_USE_VTK
  vtkPolyData *m_Polydata;
#endif

  albaString m_ScalarDataString;
  albaGUI *CreateGui();

private:
  albaVMEOutputScalar(const albaVMEOutputScalar&); // Not implemented
  void operator=(const albaVMEOutputScalar&); // Not implemented
};
#endif
