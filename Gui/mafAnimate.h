/*=========================================================================

 Program: MAF2
 Module: mafAnimate
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafAnimate_H__
#define __mafAnimate_H__

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
	void SetListener(mafObserver *listener);

	/** Return mafAnimate User Interface */
	mafGUI *GetGui();

  /** set the vme that hold the tag-array where the ViewPoint are stored */
	void SetInputVME(mafNode *vme);

  /** delete All entries in listbox.*/
  void ResetKit();

  /** Allow to fly on specified position present into the stored positions. */
  void FlyTo(const char *fly_position);

  /** Return the tag-array containing the stored positions (tag) list.*/
  mafTagArray *GetStoredPositions();

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


  /** Load poses from file. */
  void LoadPoseFromFile(mafString &fileName);

  /** Store poses to file. */
  void StorePoseToFile(mafString &fileName);



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
  mafGUIButton			*m_ImportPositionButton;
  mafGUIButton			*m_ExportPositionButton;
  mafGUIMovieCtrl   *m_AnimatePlayer;
	int						 m_InterpolateFlag;
};
#endif // __mafAnimate_H__
