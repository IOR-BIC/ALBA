/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIMaterialChooser.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:54 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGUIMaterialChooser_H__
#define __mafGUIMaterialChooser_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
#include <vector>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafGUIListCtrlBitmap;
class mmaMaterial;
class vtkProperty;
class vtkSphereSource;
class vtkPolyDataMapper;
class vtkProperty;
class mafRWI;
class mafGUI;
class mafGUIButton;
class vtkWindowToImageFilter;
class mafGUIBitmaps;
class vtkImageData;
class mmaMaterial;
class vtkActor;
class mafVME;

//----------------------------------------------------------------------------
// mafGUIMaterialChooser :
//----------------------------------------------------------------------------
/** mafMaterialChooser let the user Choose one of the predefined materials,
    but also edit material settings, or create a new one. */  
class mafGUIMaterialChooser : public mafObserver
{

public:
	mafGUIMaterialChooser(wxString dialog_title = "Material Chooser");  // reads materials from file
 ~mafGUIMaterialChooser();  // store materials to file

  /** Show in modal configuration the material editor. */
	bool ShowChooserDialog(mafVME *vme, bool remember_last_material = false);

  void OnEvent(mafEventBase *maf_event);
	
  /** Get the material tag from vme and if it is not present, create it. */
  void LoadLibraryFromVme(mafVME *vme);

  /** Store the material library into the vme as a tag. */
  void StoreLibraryToVme (mafVME *vme);

  /** Create the default material library. */
  void CreateDefaultLibrary();

protected:  
  /** Create the dialog that show the interface for materials. */
  void CreateGUI();

  /** Create the pipeline showing the colored spheres. */
  void CreatePipe();

  /** Delete the material list. */
  void ClearList();

  /** Load material library from file. */
  void LoadLibraryFromFile();

  /** Store material library to file. */
  void StoreLibraryToFile();

  void LoadMaterials_old();
  void SaveMaterials_old();

  /** Store the current settings as a new material. */
	void AddMaterial();

  /** remove choosed material from the list. */
	void RemoveMaterial();

  /** Copy choosed material on vme properties. */
  void SelectMaterial(mmaMaterial *m);

  wxDialog					*m_Dialog;
  mafGUIListCtrlBitmap	*m_ListCtrlMaterial;
	mafGUI						*m_Gui;
	mafRWI						*m_RWI;

  std::vector<mmaMaterial	*> m_List;
	mmaMaterial				*m_ChoosedMaterial;
	mmaMaterial       *m_VmeMaterial;
  mafVME            *m_Vme;
	vtkSphereSource		*m_Sphere;
	vtkProperty				*m_Property;
	vtkPolyDataMapper	*m_Mapper;
	vtkActor					*m_Actor;
  mafGUIButton *m_OkButton;
  mafGUIButton *m_CancelButton;
  mafGUIButton *m_ApplyButton;

	mafString	m_MaterialName;
	wxColour	m_AmbientColor;
	double		m_AmbientIntensity;
	wxColour	m_DiffuseColor;
	double		m_DiffuseIntensity;
	wxColour	m_SpecularColor;
	double		m_SpecularIntensity;
	int				m_SpecularPower;
	double		m_Opacity;
	int				m_Wire;

	mafString m_Filename;
};
#endif // _mafGUIMaterialChooser_H_
