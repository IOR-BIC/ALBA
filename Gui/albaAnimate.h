/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAnimate
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaAnimate_H__
#define __albaAnimate_H__

#include "albaObserver.h"
#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"

class albaVME;
class albaTagArray;
class albaGUIButton;
class vtkRenderer;
class albaGUI;
class albaGUIMovieCtrl;

//----------------------------------------------------------------------------
// albaAnimate :
//----------------------------------------------------------------------------
/**
This component allow to manage camera animation into a view.
albaAnimate allow to store and retrieve a particular point of view.
*/
class albaAnimate : public albaObserver, public albaServiceClient
{
public:

	albaAnimate(vtkRenderer *renderer, albaVME *vme, albaObserver *listener = NULL);
	~albaAnimate(); 
	void OnEvent(albaEventBase *alba_event);
	void SetListener(albaObserver *listener);

	/** Return albaAnimate User Interface */
	albaGUI *GetGui();

  /** set the vme that hold the tag-array where the ViewPoint are stored */
	void SetInputVME(albaVME *vme);

  /** delete All entries in listbox.*/
  void ResetKit();

  /** Allow to fly on specified position present into the stored positions. */
  void FlyTo(const char *fly_position);

  /** Return the tag-array containing the stored positions (tag) list.*/
  albaTagArray *GetStoredPositions();

  /** Set the list of stored position given by the user.*/
  void SetStoredPositions(albaTagArray *positions);

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
  void LoadPoseFromFile(albaString &fileName);

  /** Store poses to file. */
  void StorePoseToFile(albaString &fileName);



	albaObserver	*m_Listener;

  albaVME *m_Vme;
  albaVME *m_Root;

	albaTagArray		*m_Tags;
  albaTagArray		*m_StoredPositions;
	vtkRenderer		*m_Renderer;
	albaGUI				*m_Gui;

	wxString			 m_SelectedPosition;
	wxListBox			*m_PositionList;
	albaGUIButton			*m_StorePositionButton; 
	albaGUIButton			*m_DeletePositionButton;
	albaGUIButton			*m_RenamePositionButton;
  albaGUIButton			*m_RefreshPositionButton;
  albaGUIButton			*m_ImportPositionButton;
  albaGUIButton			*m_ExportPositionButton;
  albaGUIMovieCtrl   *m_AnimatePlayer;
	int						 m_InterpolateFlag;
};
#endif // __albaAnimate_H__
