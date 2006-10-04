/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewGlobalSlice.h,v $
  Language:  C++
  Date:      $Date: 2006-10-04 15:10:56 $
  Version:   $Revision: 1.2 $
  Authors:   Matteo Giacomoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafViewGlobalSlice_H__
#define __mafViewGlobalSlice_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewVTK.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafView;
class mafObserver;
class mafSceneNode;
class mafEventBase;
class mafString;
class vtkActor2D;
class vtkTextMapper;

//----------------------------------------------------------------------------
// mafViewGlobalSlice :
//----------------------------------------------------------------------------

class mafViewGlobalSlice: public mafViewVTK
{
public:
  mafViewGlobalSlice(wxString label = "Global Slice", int camera_position = CAMERA_PERSPECTIVE, bool show_axes = false, bool show_grid = false, bool show_ruler = false, int stereo = 0);
  virtual ~mafViewGlobalSlice(); 

  mafTypeMacro(mafViewGlobalSlice, mafViewVTK);

	/** IDs for the view GUI */
  enum VIEW_SLICE_WIDGET_ID
  {
    ID_POS_SLIDER = Superclass::ID_LAST,
		ID_CHANGE_VIEW,
		ID_OPACITY_SLIDER,
    ID_LAST
  };

	virtual mafView*  Copy(mafObserver *Listener);

	virtual void Create();

	/** 
  Create the visual pipe for the node passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  virtual void VmeCreatePipe(mafNode *vme);

	/** 
  Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  virtual void VmeDeletePipe(mafNode *vme);

	virtual void OnEvent(mafEventBase *maf_event);

	virtual void VmeSelect(mafNode *node, bool select);

	/** 
	Set slicer parameter to generate the slice. */
  void SetSlice(double origin[3], float xVect[3], float yVect[3]);
	
	/** 
	Set slicer parameter to generate the slice. */
	void SetSlice(double origin[3], int dn = 0);

protected:

	void UpdateSliceParameters();
	void UpdateSlice();
	void UpdateText();

	virtual mmgGui  *CreateGui();

	double		m_SliceOrigin[3];
	float			m_SliceXVector[3];
	float			m_SliceYVector[3];
	float			m_SliceNormal[3];
	double		m_GlobalBounds[6];
	double		m_SliderOldOrigin;
	double		m_SliderOrigin;
	double		m_Opacity;
	int				m_IDPlane;
	int				m_SliceMode;
	int				m_ViewIndex;
	int				m_Dn;
	bool			m_GlobalBoundsInitialized;

	mafSceneNode		*m_SelectedVolume;
	mmgFloatSlider	*m_GlobalSlider;

	vtkActor2D					*m_TextActor;
	vtkTextMapper	*m_TextMapper;
	mafString		m_Text;
	double			m_TextColor[3];
};
#endif
