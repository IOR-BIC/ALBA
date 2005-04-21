/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipe.h,v $
  Language:  C++
  Date:      $Date: 2005-04-21 13:18:01 $
  Version:   $Revision: 1.1 $
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

	/** Change the visibility of the actor representing the vme. */
	virtual void Show(bool show)												{};

	/** Update the properties according to the vme's tags. */
	virtual	void UpdateProperty(bool fromTag = false)		{};

	mafNode         *m_vme;
	vtkMAFAssembly    *m_asm1;
	vtkMAFAssembly    *m_asm2;
	mafSceneGraph  *m_sg;
  bool            m_selected; //we need to remeber if this is selected when show(true) is called

  vtkRenderer    *m_ren1; // used by the axis
  vtkRenderer    *m_ren2;

  //--------------------------------------------------------
  //SIL. 19-5-2004: new methods added to support PipeManager
  //--------------------------------------------------------
  
  /** new constructor whitout arguments */
  mafPipe();

  /**  return a copy of itself */
  virtual mafPipe* Copy()               {return NULL;};

  /**  create a visualization pipeline connected to this SceneNode */
  virtual void Create(mafSceneNode *n);

  /**  return true if this pipe can be connected to this vme */
  virtual bool Accept(mafNode *vme)      {return false;};

  /** ignore it, is used by mafView */
  mafPipe     *m_next; 

};
#endif

























