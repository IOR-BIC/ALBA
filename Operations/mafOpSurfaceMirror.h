/*=========================================================================

 Program: MAF2
 Module: mafOpSurfaceMirror
 Authors: Paolo Quadrani - porting  Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpSurfaceMirror_H__
#define __mafOpSurfaceMirror_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;
class mafEvent;
class vtkMAFPolyDataMirror;

//----------------------------------------------------------------------------
// mafOpSurfaceMirror :
//----------------------------------------------------------------------------
/** 
class name: mafOpSurfaceMirror
Operation which permits to create a copy of the surface which is "mirrored" respect  xy,yz or xz plane.
*/
class MAF_EXPORT mafOpSurfaceMirror: public mafOp
{
public:
  /** constructor */
	mafOpSurfaceMirror(wxString label = "Surface Mirror");
  /** destructor */
	~mafOpSurfaceMirror();
  /** handle events which becomes from other classes */
	virtual void OnEvent(mafEventBase *maf_event);
  /** return the copy of the object */
	mafOp* Copy();

  /** RTTI Macro */
	mafTypeMacro(mafOpSurfaceMirror, mafOp);

	/** Return true for the acceptable vme type. */
	bool Accept(mafVME* node);   

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

  vtkMAFPolyDataMirror *m_MirrorFilter;

	vtkPolyData	*m_OutputImafeData;
	vtkPolyData	*m_InputImageData;

	

	int		m_MirrorX;
	int		m_MirrorY;
	int		m_MirrorZ;
	int		m_FlipNormals;
};
#endif
