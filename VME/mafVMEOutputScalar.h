/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputScalar.h,v $
  Language:  C++
  Date:      $Date: 2009-03-24 13:53:00 $
  Version:   $Revision: 1.7.2.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEOutputScalar_h
#define __mafVMEOutputScalar_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEOutput.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMEGeneric;
#ifdef MAF_USE_VTK
class vtkPolyData;
#endif //MAF_USE_VTK

/** NULL output for VME node with a scalar output data.
  mafVMEOutputScalar is the output produced by a node generating an output
  with a double value representing scalar data.
*/
class MAF_EXPORT mafVMEOutputScalar : public mafVMEOutput
{
public:
  mafTypeMacro(mafVMEOutputScalar,mafVMEOutput);

  /**
    Return a VNL matrix corresponding to the current time. This is
    the output of the DataPipe currently attached to the VME Scalar.
    Usually the output is a  "smart copy" of one of the vnl matrix in 
    the DataArray. An event is rise when the output data changes to allow attached classes to 
    update their input.*/
  virtual double GetScalarData();

#ifdef MAF_USE_VTK
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
  mafVMEOutputScalar(); // to be allocated with New()
  virtual ~mafVMEOutputScalar(); // to be deleted with Delete()

#ifdef MAF_USE_VTK
  vtkPolyData *m_Polydata;
#endif

  mafString m_ScalarDataString;
  mafGUI *CreateGui();

private:
  mafVMEOutputScalar(const mafVMEOutputScalar&); // Not implemented
  void operator=(const mafVMEOutputScalar&); // Not implemented
};
#endif
