/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module:  albaGUIImageViewer
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUIImageViewer_H__
#define __albaGUIImageViewer_H__

#include "albaObserver.h"
#include "albaServiceClient.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class albaDeviceButtonsPadMouse;
class albaGUIButton;
class albaGUIDialog;
class albaRWI;
class albaVME;
class albaVMEGroup;
class albaVMEImage;
class vtkActor;
class vtkImageData;
class vtkPlaneSource;
class vtkPolyDataMapper;
class vtkTexture;

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
	ID_IMAGE_PRINT,
	ID_SETTING_ENABLE_RESOLUTION,
	ID_SETTING_RESOLUTION,
	ID_SETTING_COLOR,
};

/**
 class name: albaGUIImageViewer
*/

class ALBA_EXPORT albaGUIImageViewer : public albaObserver, albaServiceClient
{
public:

	albaGUIImageViewer(albaObserver *Listener, const albaString &label = _("ImageViewer"));
	~albaGUIImageViewer();

	/** Answer to the messages coming from interface. */
	virtual void OnEvent(albaEventBase *alba_event);

	/** Set the listener of the events launched*/
	void SetListener(albaObserver *Listener) { m_Listener = Listener; };

	/** Show ImagePreview dialog*/
	void ShowImageDialog(albaVMEGroup *group, int selection = 0);

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

	void SetMouse(albaDeviceButtonsPadMouse *mouse) { m_Mouse = mouse; };
protected:

	void FillImageList();
	
	int SaveVMEImage(albaVMEImage *image, wxString imageFileName, bool scale = false);
	void SaveImageAs();

	albaObserver *m_Listener;

	albaVMEGroup *m_ImagesGroup;

	std::vector<wxString> m_ImagesList;

	int m_ImageSelection;
	int m_ImageCheck;

	albaGUIDialog			*m_Dialog;
	albaGUIButton			*m_PrevBtn;
	albaGUIButton			*m_NextBtn;
	wxCheckBox				*m_CheckBtn;
	
	wxString m_TitleDialog;
	
	bool m_EnableDeleteButton;
	bool m_EnableSaveImageButton;
	bool m_EnableImageCheckButton;

	albaDeviceButtonsPadMouse *m_Mouse;

	wxBoxSizer				*m_RwiSizer;
	albaRWI						*m_Rwi; //Render window

	vtkPlaneSource		*m_PlaneSource;
	vtkPolyDataMapper	*m_DataMapper;
	vtkTexture				*m_Texture;
	vtkActor					*m_Actor;
};
#endif
