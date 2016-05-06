/*=========================================================================

 Program: MAF2
 Module: mafOpVolumeMeasure
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpVolumeMeasure_H__
#define __mafOpVolumeMeasure_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"
#include "mafVMESurface.h"
#include "mafVMESurfaceParametric.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafVME;
class mafEvent;
class mafEventListener;
class vtkTriangleFilter;
class vtkMassProperties;
class vtkPolyData;

//----------------------------------------------------------------------------
// mafOpVolumeMeasure :
//----------------------------------------------------------------------------
/** 
Measure a Volume of a PolyData

This Operation compute a volume of a surface. 
The algorithm implemented here is based on the discrete form 
of the divergence theorem. The general assumption here is 
that the model is of closed surface.
*/

class MAF_EXPORT mafOpVolumeMeasure: public mafOp
{
public:
             
            	 mafOpVolumeMeasure(wxString label = "Volume Measure");
	virtual     ~mafOpVolumeMeasure();
	virtual void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(mafOpVolumeMeasure, mafOp);

  mafOp* Copy();

	bool Accept(mafVME* Node);
	void OpRun();	
	
  /**
  Compute the Volume */
  void VolumeCompute(mafVME *vme);

	mafString GetVolume(){return m_VolumeMeasure;};

  void OpDo();
  void OpStop(int result);	

  static bool SurfaceAccept(mafVME* node) {return(node != NULL && (node->IsMAFType(mafVMESurface) || node->IsMAFType(mafVMESurfaceParametric) ));};

protected:

  

  mafVME *m_VmeSurface;
  
  mafString m_MeasureText;
  mafString m_NormalizedShapeIndex;
	mafString m_SurfaceArea;
	mafString m_VolumeMeasure;
  wxListBox *m_MeasureList;
 
  vtkTriangleFilter *m_TriangleFilter;
  vtkMassProperties *m_MassProperties;

};


#endif

