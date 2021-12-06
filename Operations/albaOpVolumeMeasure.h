/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpVolumeMeasure
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpVolumeMeasure_H__
#define __albaOpVolumeMeasure_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"
#include "albaVMESurface.h"
#include "albaVMESurfaceParametric.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class albaVME;
class albaEvent;
class albaEventListener;
class vtkTriangleFilter;
class vtkMassProperties;
class vtkPolyData;

//----------------------------------------------------------------------------
// albaOpVolumeMeasure :
//----------------------------------------------------------------------------
/** 
Measure a Volume of a PolyData

This Operation compute a volume of a surface. 
The algorithm implemented here is based on the discrete form 
of the divergence theorem. The general assumption here is 
that the model is of closed surface.
*/

class ALBA_EXPORT albaOpVolumeMeasure: public albaOp
{
public:
             
            	 albaOpVolumeMeasure(wxString label = "Volume Measure");
	virtual     ~albaOpVolumeMeasure();
	virtual void OnEvent(albaEventBase *alba_event);

  albaTypeMacro(albaOpVolumeMeasure, albaOp);

  albaOp* Copy();

	void OpRun();	
	
  /**
  Compute the Volume */
  void VolumeCompute(albaVME *vme);

	albaString GetVolume(){return m_VolumeMeasure;};

  void OpDo();
  void OpStop(int result);	

  static bool SurfaceAccept(albaVME* node) {return(node != NULL && (node->IsALBAType(albaVMESurface) || node->IsALBAType(albaVMESurfaceParametric) ));};

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);  

  albaVME *m_VmeSurface;
  
  albaString m_MeasureText;
  albaString m_NormalizedShapeIndex;
	albaString m_SurfaceArea;
	albaString m_VolumeMeasure;
  wxListBox *m_MeasureList;
 
  vtkTriangleFilter *m_TriangleFilter;
  vtkMassProperties *m_MassProperties;

};


#endif

