/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSurfaceMirror
 Authors: Paolo Quadrani - porting  Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpSurfaceMirror_H__
#define __albaOpSurfaceMirror_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;
class albaEvent;
class vtkALBAPolyDataMirror;

//----------------------------------------------------------------------------
// albaOpSurfaceMirror :
//----------------------------------------------------------------------------
/** 
class name: albaOpSurfaceMirror
Operation which permits to create a copy of the surface which is "mirrored" respect  xy,yz or xz plane.
*/
class ALBA_EXPORT albaOpSurfaceMirror: public albaOp
{
public:
  /** constructor */
	albaOpSurfaceMirror(wxString label = "Surface Mirror");
  /** destructor */
	~albaOpSurfaceMirror();
  /** handle events which becomes from other classes */
	virtual void OnEvent(albaEventBase *alba_event);
  /** return the copy of the object */
	albaOp* Copy();

  /** RTTI Macro */
	albaTypeMacro(albaOpSurfaceMirror, albaOp);

	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);

	/** Makes the Preview for the mirror. */
	void Preview();  

  vtkALBAPolyDataMirror *m_MirrorFilter;

	vtkPolyData	*m_OutputPolydata;
	vtkPolyData	*m_InputPolydata;

	

	int		m_MirrorX;
	int		m_MirrorY;
	int		m_MirrorZ;
	int		m_FlipNormals;
};
#endif
