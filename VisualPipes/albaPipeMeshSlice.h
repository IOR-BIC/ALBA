/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeMeshSlice
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeMeshSlice_H__B
#define __albaPipeMeshSlice_H__B

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipeSlice.h"
#include "albaPipeGenericPolydata.h"

class vtkALBAMeshCutter;
class vtkPlane; 

//----------------------------------------------------------------------------
// albaPipeMeshSlice :
class ALBA_EXPORT albaPipeMeshSlice : public albaPipeGenericPolydata, public albaPipeSlice
{
public:
  /** RTTI macro*/
	albaTypeMacro(albaPipeMeshSlice,albaPipeGenericPolydata);

  /** constructor */
	albaPipeMeshSlice();
  /** destructor */
	virtual     ~albaPipeMeshSlice();

	virtual vtkAlgorithmOutput* GetPolyDataOutputPort();

  /** Set the origin and normal of the slice.
  Both, Origin and Normal may be NULL, if the current value is to be preserved. */
  /*virtual*/ void SetSlice(double* Origin, double* Normal);  

protected:
  vtkPlane				        *m_Plane;
  vtkALBAMeshCutter		    *m_Cutter;
};  
#endif // __albaPipeMeshSlice_H__B
