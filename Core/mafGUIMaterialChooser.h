/*=========================================================================

 Program: MAF2
 Module: mafGUIMaterialChooser
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUIMaterialChooser_H__
#define __mafGUIMaterialChooser_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
#include "mafStorable.h"
#include "mafServiceClient.h"
#include "mafAbsLogicManager.h"
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
class MAF_EXPORT mafGUIMaterialChooser : public mafObserver, public mafServiceClient
{

public:
	mafGUIMaterialChooser(wxString dialog_title = "Material Chooser");  // reads materials from file
 ~mafGUIMaterialChooser();  // store materials to file

  /** Show in modal configuration the material editor. */
	bool ShowChooserDialog(mafVME *vme, bool remember_last_material = false);

	mmaMaterial *GetMaterial(int id);

  void OnEvent(mafEventBase *maf_event);
	  
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
  mafGUIListCtrlBitmap	*m_ListCtrlMaterial;
	mafGUI						*m_Gui;
	mafRWI						*m_RWI;

  std::vector<mmaMaterial	*> m_List;
	mmaMaterial				*m_ChoosedMaterial;
	long							 m_ChoosedMaterialId;
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
//------------------------------------------------------------------------------
class MAF_EXPORT mafStorableMaterialLibrary: public mafObject, public mafStorable
//------------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafStorableMaterialLibrary,mafObject);
  mafStorableMaterialLibrary(){};
  mafStorableMaterialLibrary(std::vector<mmaMaterial *> *mat_list);
  ~mafStorableMaterialLibrary(){};
  int InternalStore(mafStorageElement *parent);
  int InternalRestore(mafStorageElement *node);
  std::vector<mmaMaterial *> *m_MaterialList;
};
#endif // _mafGUIMaterialChooser_H_
