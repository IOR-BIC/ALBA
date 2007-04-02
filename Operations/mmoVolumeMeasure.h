/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoVolumeMeasure.h,v $
  Language:  C++
  Date:      $Date: 2007-04-02 10:08:57 $
  Version:   $Revision: 1.4 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoVolumeMeasure_H__
#define __mmoVolumeMeasure_H__

#include "mafOp.h"
#include "mmgVMEChooserAccept.h"
#include "mafNode.h"
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
// mmoVolumeMeasure :
//----------------------------------------------------------------------------
/** 
Measure a Volume of a PolyData

This Operation compute a volume of a surface. 
The algorithm implemented here is based on the discrete form 
of the divergence theorem. The general assumption here is 
that the model is of closed surface.
*/

class mmoVolumeMeasure: public mafOp
{
public:
             
            	 mmoVolumeMeasure(wxString label = "Volume Measure");
	virtual     ~mmoVolumeMeasure();
	virtual void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(mmoVolumeMeasure, mafOp);

  mafOp* Copy();

	bool Accept(mafNode* Node) {return true;};
	void OpRun();	
	
  /**
  Compute the Volume */
  void VolumeCompute(mafVME *vme);

  void OpDo();

  class mafSurfaceAccept : public mmgVMEChooserAccept
	{
		public:
			
			mafSurfaceAccept() {};
		 ~mafSurfaceAccept() {};

		bool Validate(mafNode* node) {return(node != NULL && (node->IsMAFType(mafVMESurface) || node->IsMAFType(mafVMESurfaceParametric) ));};
	};
	mafSurfaceAccept *m_SurfaceAccept;	
  
protected:
  void OpStop(int result);	

  mafVMESurface  *m_VmeSurface;
  
  mafString m_MeasureText;
  mafString m_NormalizedShapeIndex;
	mafString m_SurfaceArea;
	mafString m_VolumeMeasure;
  wxListBox *m_MeasureList;
 
  vtkTriangleFilter *m_TriangleFilter;
  vtkMassProperties *m_MassProperties;

};


#endif

