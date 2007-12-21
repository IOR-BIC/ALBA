/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpSurfaceMirror.h,v $
  Language:  C++
  Date:      $Date: 2007-12-21 12:01:37 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani - porting  Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpSurfaceMirror_H__
#define __medOpSurfaceMirror_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;
class mafEvent;
class vtkMEDPolyDataMirror;

//----------------------------------------------------------------------------
// medOpSurfaceMirror :
//----------------------------------------------------------------------------
/** */
class medOpSurfaceMirror: public mafOp
{
public:
	medOpSurfaceMirror(wxString label);
	~medOpSurfaceMirror(); 
	virtual void OnEvent(mafEventBase *maf_event);
	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node);   


	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);

	/** Makes the Preview for the mirror. */
	void Preview();  

    vtkMEDPolyDataMirror *m_MirrorFilter;

	vtkPolyData	*m_OutputPolydata;
	vtkPolyData	*m_InputPolydata;

	

	int		m_MirrorX;
	int		m_MirrorY;
	int		m_MirrorZ;
	int		m_FlipNormals;
};
#endif
