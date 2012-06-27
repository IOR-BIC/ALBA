/*=========================================================================

 Program: MAF2
 Module: mmgFlyPoseList
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mmgFlyPoseList_H__
#define __mmgFlyPoseList_H__

#include "mafEvent.h"
#include "mafObserver.h"

class mafNode;
class mafTagArray;
class mmgButton;
class vtkRenderer;
class mmgGui;

class mafOsgTerrainManipulator;


//----------------------------------------------------------------------------
// mmgFlyPoseList :
//----------------------------------------------------------------------------
/**
This component allow to manage camera animation into a view.
mmgFlyPoseList allow to store and retrieve a particular point of view.
*/
class mmgFlyPoseList : public mafObserver
{
public:
	mmgFlyPoseList( mafOsgTerrainManipulator *manipulator);
	~mmgFlyPoseList(); 
	void OnEvent(mafEventBase *maf_event);
	void SetListener(mafObserver *listener) {m_Listener = listener;};

	/** Return mmgFlyPoseList User Interface */
	mmgGui *GetGui() {return m_Gui;};

  /** delete All entries in listbox.*/
  void ResetKit();

protected:
	/** Move the camera to the target position */
	void FlyTo();

	/** Store a view position into the root's tag array */
	void StoreViewPoint();

	/** Rename the view position */
	void RenameViewPoint();

	/** Delete the view position */
	void DeleteViewPoint();

	/** Create the GUI for the FlyTo animation */
	void CreateGui();

	/** Enable-Disable the GUI's widgets */
	void EnableWidgets();
	mafObserver	*m_Listener;

  mmgGui				*m_Gui;

	wxString			 m_SelectedPosition;
	wxListBox			*m_PositionList;
	mmgButton			*m_StorePositionButton; 
	mmgButton			*m_DeletePositionButton;
	mmgButton			*m_RenamePositionButton;
	int						 m_AnimateFlag;
  double         m_AnimationLenght;

  mafOsgTerrainManipulator *m_manip;

  struct ListElement 
  {
    double pos[3];
    double yaw, pitch, dist;
    wxString name;
  };
  //ListElement m_List[50];
};
#endif

