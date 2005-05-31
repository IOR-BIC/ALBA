/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgMaterialChooser.h,v $
  Language:  C++
  Date:      $Date: 2005-05-31 23:46:22 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgMaterialChooser_H__
#define __mmgMaterialChooser_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
#include <vector>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mmgListCtrlBitmap;
class mmaMaterial;
class vtkProperty;
class vtkSphereSource;
class vtkPolyDataMapper;
class vtkProperty;
class mafRWI;
class mmgGui;
class vtkWindowToImageFilter;
class mmgBitmaps;
class vtkImageData;
class mmaMaterial;
class vtkActor;
class mafVME;

//----------------------------------------------------------------------------
// mmgMaterialChooser :
//----------------------------------------------------------------------------
/** mafMaterialChooser let the user Choose one of the predefined materials,
    but also edit material settings, or create a new one. */  
class mmgMaterialChooser : public mafObserver
{

public:
	mmgMaterialChooser(wxString dialog_title = "Material Chooser");  // reads materials from file
 ~mmgMaterialChooser();  // store materials to file

  /** Show in modal configuration the material editor. */
	bool ShowChooserDialog(mafVME *vme);

  void OnEvent(mafEventBase *event);
	
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
  mmgListCtrlBitmap	*m_ListCtrlMaterial;
	mmgGui						*m_Gui;
	mafRWI						*m_RWI;

  std::vector<mmaMaterial	*> m_List;
	mmaMaterial				*m_ChoosedMaterial;
	mmaMaterial       *m_VmeMaterial;
	vtkSphereSource		*m_Sphere;
	vtkProperty				*m_Property;
	vtkPolyDataMapper	*m_Mapper;
	vtkActor					*m_Actor;

	mafString	m_MaterialName;
	wxColour	m_AmbientColor;
	float			m_AmbientIntensity;
	wxColour	m_DiffuseColor;
	float			m_DiffuseIntensity;
	wxColour	m_SpecularColor;
	float			m_SpecularIntensity;
	int				m_SpecularPower;
	float			m_Opacity;
	int				m_Wire;

	mafString  m_Filename;
};
#endif // _mmgMaterialChooser_H_
