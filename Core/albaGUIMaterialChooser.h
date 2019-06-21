/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIMaterialChooser
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUIMaterialChooser_H__
#define __albaGUIMaterialChooser_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaObserver.h"
#include "albaStorable.h"
#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"
#include <vector>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaGUIListCtrlBitmap;
class mmaMaterial;
class vtkProperty;
class vtkSphereSource;
class vtkPolyDataMapper;
class vtkProperty;
class albaRWI;
class albaGUI;
class albaGUIButton;
class vtkWindowToImageFilter;
class albaGUIBitmaps;
class vtkImageData;
class mmaMaterial;
class vtkActor;
class albaVME;

//----------------------------------------------------------------------------
// albaGUIMaterialChooser :
//----------------------------------------------------------------------------
/** albaMaterialChooser let the user Choose one of the predefined materials,
    but also edit material settings, or create a new one. */  
class ALBA_EXPORT albaGUIMaterialChooser : public albaObserver, public albaServiceClient
{

public:
	albaGUIMaterialChooser(wxString dialog_title = "Material Chooser");  // reads materials from file
 ~albaGUIMaterialChooser();  // store materials to file

  /** Show in modal configuration the material editor. */
	bool ShowChooserDialog(albaVME *vme, bool remember_last_material = false);

	mmaMaterial *GetMaterial(int id);

  void OnEvent(albaEventBase *alba_event);
	  
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

  /** Store the current settings as a new material. */
	void AddMaterial();

  /** remove choosed material from the list. */
	void RemoveMaterial();

  /** Copy choosed material on vme properties. */
  void SelectMaterial(long matPos);

	void SetFromMat(mmaMaterial * mat);

  wxDialog					*m_Dialog;
  albaGUIListCtrlBitmap	*m_ListCtrlMaterial;
	albaGUI						*m_Gui;
	albaRWI						*m_RWI;

  std::vector<mmaMaterial	*> m_List;
	mmaMaterial				*m_ChoosedMaterial;
	long							 m_ChoosedMaterialId;
	mmaMaterial       *m_VmeMaterial;
  albaVME            *m_Vme;
	vtkSphereSource		*m_Sphere;
	vtkProperty				*m_Property;
	vtkPolyDataMapper	*m_Mapper;
	vtkActor					*m_Actor;
  albaGUIButton *m_OkButton;
  albaGUIButton *m_CancelButton;
  albaGUIButton *m_ApplyButton;

	albaString	m_MaterialName;
	wxColour	m_AmbientColor;
	double		m_AmbientIntensity;
	wxColour	m_DiffuseColor;
	double		m_DiffuseIntensity;
	wxColour	m_SpecularColor;
	double		m_SpecularIntensity;
	int				m_SpecularPower;
	double		m_Opacity;
	int				m_Wire;

	albaString m_Filename;
};
//------------------------------------------------------------------------------
class ALBA_EXPORT albaStorableMaterialLibrary: public albaObject, public albaStorable
//------------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaStorableMaterialLibrary,albaObject);
  albaStorableMaterialLibrary(){};
  albaStorableMaterialLibrary(std::vector<mmaMaterial *> *mat_list);
  ~albaStorableMaterialLibrary(){};
  int InternalStore(albaStorageElement *parent);
  int InternalRestore(albaStorageElement *node);
  std::vector<mmaMaterial *> *m_MaterialList;
};
#endif // _albaGUIMaterialChooser_H_
