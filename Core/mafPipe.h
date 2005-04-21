/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipe.h,v $
  Language:  C++
  Date:      $Date: 2005-04-21 16:37:43 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafPipe_H__
#define __mafPipe_H__

#ifdef __GNUG__
    #pragma interface "mafPipe.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVME;
class mafSceneNode;
class mafSceneGraph;
class vtkMAFAssembly;
class vtkRenderer;
//----------------------------------------------------------------------------
// mafPipe :
//----------------------------------------------------------------------------
class mafPipe
{
public:
							 mafPipe(mafSceneNode *n);
	virtual			~mafPipe();

	/** Change the visibility of the bounding box actor representing the selection for the vme. */
	virtual	void Select(bool select)										{};

	 //SIL. 21-4-2005: -- SHOW is to be removed -- when a vme is hidden the pipe is destroyed
  /** Change the visibility of the actor representing the vme. */
	// virtual void Show(bool show)												{};

	/** Update the properties according to the vme's tags. */
	virtual	void UpdateProperty(bool fromTag = false)		{};

	mafNode        *m_mafnode;
  mafVME         *m_vme;
	vtkMAFAssembly *m_asm1;
	vtkMAFAssembly *m_asm2;
	mafSceneGraph  *m_sg;
  bool            m_selected; //we need to remeber if this is selected when show(true) is called
  vtkRenderer    *m_ren1;     //used by the axis
  vtkRenderer    *m_ren2;
};
#endif

























