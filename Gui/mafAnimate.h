/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAnimate.h,v $
  Language:  C++
  Date:      $Date: 2008-07-17 16:00:24 $
  Version:   $Revision: 1.9 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafAnimate_H__
#define __mafAnimate_H__

#include "mafEvent.h"
#include "mafObserver.h"

class mafNode;
class mafTagArray;
class mmgButton;
class vtkRenderer;
class mmgGui;
class mmgMovieCtrl;

//----------------------------------------------------------------------------
// mafAnimate :
//----------------------------------------------------------------------------
/**
This component allow to manage camera animation into a view.
mafAnimate allow to store and retrieve a particular point of view.
*/
class mafAnimate : public mafObserver
{
public:
	mafAnimate(vtkRenderer *renderer, mafNode *vme, mafObserver *listener = NULL);
	~mafAnimate(); 
	void OnEvent(mafEventBase *maf_event);
	void SetListener(mafObserver *listener) {m_Listener = listener;};

	/** Return mafAnimate User Interface */
	mmgGui *GetGui() {return m_Gui;};

  /** set the vme that hold the tag-array where the ViewPoint are stored */
	void SetInputVME(mafNode *vme);

  /** delete All entries in listbox.*/
  void ResetKit();

  /** Allow to fly on specified position present into the stored positions. */
  void FlyTo(const char *fly_position);

  /** Return the tag-array containing the stored positions (tag) list.*/
  mafTagArray *GetStoredPositions() {RetrieveStoredPositions(false); return m_StoredPositions;};

  /** Set the list of stored position given by the user.*/
  void SetStoredPositions(mafTagArray *positions);

protected:
	/** Move the camera to the target position */
	void FlyTo();

  /** Read root node's tags to retrieve previous stored camera positions.  */
  void RetrieveStoredPositions(bool update_listbox = true);

	/** Store a view position into the root's tag array */
	void StoreViewPoint();

	/** Rename the view position */
	void RenameViewPoint();

	/** Delete the view position */
	void DeleteViewPoint(int pos = 0);

	/** Create the GUI for the FlyTo animation */
	void CreateGui();

	/** Enable-Disable the GUI's widgets */
	void EnableWidgets();
	mafObserver	*m_Listener;

	mafTagArray		*m_Tags;
  mafTagArray		*m_StoredPositions;
	vtkRenderer		*m_Renderer;
	mmgGui				*m_Gui;

	wxString			 m_SelectedPosition;
	wxListBox			*m_PositionList;
	mmgButton			*m_StorePositionButton; 
	mmgButton			*m_DeletePositionButton;
	mmgButton			*m_RenamePositionButton;
  mmgMovieCtrl  *m_AnimatePlayer;
	int						 m_InterpolateFlag;
};
#endif
