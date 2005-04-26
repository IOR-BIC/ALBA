/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewVTK.h,v $
  Language:  C++
  Date:      $Date: 2005-04-26 10:27:04 $
  Version:   $Revision: 1.5 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafViewVTK_H__
#define __mafViewVTK_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafView.h"
#include "mafRWIBase.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h" //used in subclasses
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mafViewVTK :
//----------------------------------------------------------------------------
/** 
mafViewVTK is a View that got a RenderWindow and a SceneGraph
\sa mafSceneNode mafRWIBase mafRWI
*/
class mafViewVTK: public mafView
{
public:
  mafViewVTK(wxString label);
  virtual ~mafViewVTK(); 
  virtual mafView*  Copy(mafEventListener *Listener);
  virtual void      Create();

  virtual void			VmeAdd(mafNode *vme);
  virtual void			VmeRemove(mafNode *vme);
  virtual void			VmeSelect(mafNode *vme, bool select);
  virtual void			VmeShow(mafNode *vme, bool show);
	virtual void      VmeUpdateProperty(mafNode *vme, bool fromTag = false);
  virtual int 	    GetNodeStatus(mafNode *vme);
  virtual mafPipe*  GetNodePipe(mafNode *vme);

	virtual void			CameraReset();	 
  virtual void			CameraUpdate(); 

  virtual mafSceneGraph *GetSceneGraph()	  {return m_Sg;}; 
  virtual mafRWIBase    *GetRWI()           {return m_Rwi->m_Rwi;}; 
protected:
  mafRWI        *m_Rwi; 
  mafSceneGraph *m_Sg; 
};
#endif
