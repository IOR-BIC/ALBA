/*=========================================================================

 Program: MAF2
 Module:  mafGUIImageViewer
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUIImageViewer_H__
#define __mafGUIImageViewer_H__

#include "mafObserver.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafGUIButton;
class mafGUIDialog;
class mafGUIPicButton;
class mafGUISettingsDialog;
class mafVME;
class mafVMEGroup;
class mafVMEImage;
class vtkImageData;

//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum ImageViewerEvents
{
	ID_IMAGE = MINID,
	ID_IMAGE_PREV,
	ID_IMAGE_NEXT,
	ID_IMAGE_SELECT,
	ID_IMAGE_SAVE,
	ID_IMAGE_DELETE,
};

/**
 class name: mafGUIImageViewer
*/

class MAF_EXPORT mafGUIImageViewer : public mafObserver
{
public:

	mafGUIImageViewer(mafObserver *Listener, const mafString &label = _("ImageViewer"));
	~mafGUIImageViewer();

	/** Answer to the messages coming from interface. */
	virtual void OnEvent(mafEventBase *maf_event);

	/** Set the listener of the events launched*/
	void SetListener(mafObserver *Listener) { m_Listener = Listener; };

	/** Show a Preview of Snapshots*/
	void ShowImageDialog(mafVMEGroup *group, bool reloadList = false);

	void Reset();

	wxString GetSelectedImageName();
	int GetSelection() { return m_ImageSelection; };

	void UpdateSelectionDialog(int selection);

	void SetTitle(wxString title) { m_TitleDialog = title; };
	void EnableDeleteButton(bool enable) { m_EnableDeleteButton = enable; };
	void EnableSaveButton(bool enable) { m_EnableSaveImageButton = enable; };
	void EnableCheckButton(bool enable) { m_EnableImageCheckButton = enable; };

	void SetCheck(int check) { m_ImageCheck = check; };
	int GetCheck() { return m_ImageCheck; };
	
	void AddImage(wxBitmap &bitmap, wxString name);
	void DeleteImage(int index);
	void DeleteSelectedImage();

	int GetImagesNum() { return m_ImagesList.size(); };
protected:

	void FillImageList();
	
	int SaveVMEImage(mafVMEImage *image, wxString imageFileName, bool scale = false);
	void SaveImageAs();

	mafObserver *m_Listener;

	mafVMEGroup *m_ImagesGroup;

	std::vector<wxString> m_ImagesList;

	int m_ImageSelection;
	int m_ImageCheck;

	mafGUIDialog			*m_Dialog;
	mafGUIPicButton		*m_PreviewImageButton;
	wxBoxSizer				*m_ImageBoxSizer;
	mafGUIButton			*m_PrevBtn;
	mafGUIButton			*m_NextBtn;
	wxCheckBox				*m_CheckBtn;

	wxString m_ImagesPath;

	wxString m_TitleDialog;

	bool m_EnableDeleteButton;
	bool m_EnableSaveImageButton;
	bool m_EnableImageCheckButton;
};
#endif
