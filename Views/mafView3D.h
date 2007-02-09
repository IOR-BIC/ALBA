/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafView3D.h,v $
  Language:  C++
  Date:      $Date: 2007-02-09 16:41:10 $
  Version:   $Revision: 1.2 $
  Authors:   Matteo Giacomoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafView3D_H__
#define __mafView3D_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewVTK.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafSceneNode;
class mmgGui;

//----------------------------------------------------------------------------
// mafViewRX :
//----------------------------------------------------------------------------
/** 
mafViewRX is a View that visualize volume as projection along x or y axis and 
initialize the visual pipe according to the camera position that is passed through constructor
\sa mafViewVTK
*/
class mafView3D: public mafViewVTK
{
public:
  mafView3D(wxString label = "3D", int camera_position = CAMERA_RX_FRONT, bool show_axes = false, bool show_grid = false, bool show_ruler = false, int stereo = 0);
  virtual ~mafView3D(); 

  mafTypeMacro(mafView3D, mafViewVTK);

  virtual mafView*  Copy(mafObserver *Listener);
	virtual void			OnEvent(mafEventBase *maf_event);

	void Create();


  /** IDs for the GUI */
  enum VIEW_RX_WIDGET_ID
  { 
    ID_NONE = Superclass::ID_LAST,
		ID_COMBO_PIPE,
    ID_LAST
  };

	/** 
  Create the visual pipe for the node passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  virtual void VmeCreatePipe(mafNode *vme);

	virtual void VmeSelect(mafNode *vme, bool select);

protected:

	virtual mmgGui *CreateGui();

	int m_Choose;

};
#endif
