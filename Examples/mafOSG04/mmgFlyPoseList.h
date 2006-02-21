/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgFlyPoseList.h,v $
  Language:  C++
  Date:      $Date: 2006-02-21 16:12:41 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
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

