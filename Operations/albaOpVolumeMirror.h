/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpVolumeMirror
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpVolumeMirror_H__
#define __albaOpVolumeMirror_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkImageData;
class vtkImageFlip;

//----------------------------------------------------------------------------
// albaOpVolumeMirror :
//----------------------------------------------------------------------------
/** 
class name: albaOpVolumeMirror
Operation which permits to create a copy of the surface which is "mirrored" respect  xy,yz or xz plane.
*/
class ALBA_EXPORT albaOpVolumeMirror: public albaOp
{
public:
  /** constructor */
	albaOpVolumeMirror(wxString label = "Volume Mirror");
  /** destructor */
	~albaOpVolumeMirror();
  /** handle events which becomes from other classes */
	virtual void OnEvent(albaEventBase *alba_event);
  /** return the copy of the object */
	albaOp* Copy();

  /** RTTI Macro */
	albaTypeMacro(albaOpVolumeMirror, albaOp);

	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();
	
	/** Returns MirrorX */
	int GetMirrorX() const { return m_MirrorX; }

	/** Sets MirrorX */
	void SetMirrorX(int mirrorX) { m_MirrorX = mirrorX; }
	
	/** Returns MirrorY */
	int GetMirrorY() const { return m_MirrorY; }

	/** Sets MirrorY */
	void SetMirrorY(int mirrorY) { m_MirrorY = mirrorY; }
	
	/** Returns MirrorZ */
	int GetMirrorZ() const { return m_MirrorZ; }

	/** Sets MirrorZ */
	void SetMirrorZ(int mirrorZ) { m_MirrorZ = mirrorZ; }

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);

	/** Makes the Preview for the mirror. */
	void Preview();  
	
	vtkImageData	*m_OutputImageData;
	vtkImageData	*m_InputImageData;
	
	int		m_MirrorX;
	int		m_MirrorY;
	int		m_MirrorZ;
	int		m_FlipNormals;
};
#endif
