/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testView.h,v $
  Language:  C++
  Date:      $Date: 2005-04-21 13:20:04 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __testView_H__
#define __testView_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewVTK.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// testView :
//----------------------------------------------------------------------------
/** 
*/
class testView: public mafViewVTK
{
public:
  testView(wxString label);
  virtual ~testView(); 
  virtual mafView*  Copy(mafEventListener *Listener);
  virtual void      Create();

  /*
  virtual void			VmeAdd(mafNode *vme);
  virtual void			VmeRemove(mafNode *vme);
  virtual void			VmeSelect(mafNode *vme, bool select);
  virtual void			VmeShow(mafNode *vme, bool show);
	virtual void      VmeUpdateProperty(mafNode *vme, bool fromTag = false);
  virtual int 	    GetNodeStatus(mafNode *vme);
  */

	virtual void			CameraReset();	 
  virtual void			CameraUpdate(); 

  virtual mafSceneGraph *GetSceneGraph()	  {return m_sg;}; 
  virtual mafRWIBase    *GetRWI()           {return m_rwi->m_rwi;}; 
protected:
  mafRWI        *m_rwi; 
  mafSceneGraph *m_sg; 
};
#endif
