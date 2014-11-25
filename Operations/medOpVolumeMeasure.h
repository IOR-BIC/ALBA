/*=========================================================================

 Program: MAF2Medical
 Module: medOpVolumeMeasure
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpVolumeMeasure_H__
#define __medOpVolumeMeasure_H__

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
// medOpVolumeMeasure :
//----------------------------------------------------------------------------
/** 
Measure a Volume of a PolyData

This Operation compute a volume of a surface. 
The algorithm implemented here is based on the discrete form 
of the divergence theorem. The general assumption here is 
that the model is of closed surface.
*/

class MAF_EXPORT medOpVolumeMeasure: public mafOp
{
public:
             
            	 medOpVolumeMeasure(wxString label = "Volume Measure");
	virtual     ~medOpVolumeMeasure();
	virtual void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(medOpVolumeMeasure, mafOp);

  mafOp* Copy();

	bool Accept(mafNode* Node);
	void OpRun();	
	
  /**
  Compute the Volume */
  void VolumeCompute(mafVME *vme);

	mafString GetVolume(){return m_VolumeMeasure;};

  void OpDo();
  void OpStop(int result);	

  static bool SurfaceAccept(mafNode* node) {return(node != NULL && (node->IsMAFType(mafVMESurface) || node->IsMAFType(mafVMESurfaceParametric) ));};

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

