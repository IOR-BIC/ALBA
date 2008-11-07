/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAnimate.h,v $
  Language:  C++
  Date:      $Date: 2008-11-07 13:13:33 $
  Version:   $Revision: 1.12.2.1 $
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
class mafVME;
class mafTagArray;
class mafGUIButton;
class vtkRenderer;
class mafGUI;
class mafGUIMovieCtrl;

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
	mafGUI *GetGui() {return m_Gui;};

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
	/** Select the item into the listbox and update the validator of the widget.*/
  void SetCurrentSelection(int pos);

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

  mafNode *m_Vme;
  mafVME *m_Root;

	mafTagArray		*m_Tags;
  mafTagArray		*m_StoredPositions;
	vtkRenderer		*m_Renderer;
	mafGUI				*m_Gui;

	wxString			 m_SelectedPosition;
	wxListBox			*m_PositionList;
	mafGUIButton			*m_StorePositionButton; 
	mafGUIButton			*m_DeletePositionButton;
	mafGUIButton			*m_RenamePositionButton;
  mafGUIButton			*m_RefreshPositionButton;
  mafGUIMovieCtrl  *m_AnimatePlayer;
	int						 m_InterpolateFlag;
};
#endif
