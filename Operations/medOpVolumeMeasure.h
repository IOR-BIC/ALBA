/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpVolumeMeasure.h,v $
  Language:  C++
  Date:      $Date: 2012-04-06 09:24:52 $
  Version:   $Revision: 1.1.2.4 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpVolumeMeasure_H__
#define __medOpVolumeMeasure_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medOperationsDefines.h"
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

class MED_OPERATION_EXPORT medOpVolumeMeasure: public mafOp
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

