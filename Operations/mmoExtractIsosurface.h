/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoExtractIsosurface.h,v $
  Language:  C++
  Date:      $Date: 2005-07-18 10:12:58 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani     Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoExtractIsosurface_H__
#define __mmoExtractIsosurface_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmgDialog;
class mmgFloatSlider;
class mafVMESurface;
class mafNode;
class mafRWI;

class vtkActor;
class vtkVolume;
class vtkContourVolumeMapper;
class vtkPolyDataMapper;
class vtkOutlineCornerFilter;
class vtkRenderer;

class mmiPicker;
class vtkImageData;
class vtkTexture;
class vtkPolyData;
class vtkVolumeSlicer;
class vtkFixedCutter;
class vtkPlane;

//----------------------------------------------------------------------------
// mmoExtractIsosurface :
//----------------------------------------------------------------------------
/** */
class mmoExtractIsosurface: public mafOp
{
public:
	mmoExtractIsosurface(wxString label);
 ~mmoExtractIsosurface(); 
	mafOp* Copy();
  void OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode* vme);

	/** Builds operation's interface by calling CreateOpDialog() method. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();

protected:
  mafVMESurface *m_IsosurfaceVme;

  mmgDialog		*m_Dialog;
	mafRWI      *m_Rwi;
  vtkRenderer *m_PIPRen;
	double       m_IsoValue;
	double       m_max;
	double       m_min;
	double       m_step;
  double       m_Slice;
  double       m_SliceMin;
  double       m_SliceMax;
	double       m_SliceStep;
  int          m_ShowSlice;
  int          m_Clean;
  double       m_bbox[6];
  double       m_origin[3];
  float        m_xVect[3];
  float        m_yVect[3];

  mmgFloatSlider *m_IsoSlider;
  mmgFloatSlider *m_SliceSlider;
  
  vtkVolume               *v_volume;
  vtkPolyDataMapper       *m_contour_volume_mapper;
  vtkActor                *m_contour_volume_actor;
  vtkActor                *v_box;
  vtkContourVolumeMapper  *v_contour_mapper; 
  vtkOutlineCornerFilter  *v_outline_filter;
  vtkPolyDataMapper       *v_outline_mapper;

  vtkVolumeSlicer   *m_slicer;
  vtkVolumeSlicer   *m_pslicer;
  vtkImageData      *m_image_slice;
  vtkTexture        *m_texture;
  vtkPolyData       *m_polydata;
  vtkPolyDataMapper *m_smapper;
  vtkActor          *m_actorSlice;
  vtkPolyDataMapper *m_mapper;
  vtkActor          *m_actor;
  vtkPlane          *m_plane;
  vtkFixedCutter    *m_cutter;

  mmiPicker         *m_DensityPicker;

	/** 
  Builds operation's interface and visualization pipeline. */
  void CreateOpDialog();

	/** 
  Remove operation's interface. */
  void DeleteOpDialog();

	/** 
  Create the pipeline to generate the isosurface of the volume. */
  void CreateVolumePipeline();

	/** 
  Create the pipeline to generate the slice of the volume. */
  void CreateSlicePipeline();

	/** 
  Extract the isosurface and build the related vme. */
  void ExtractSurface(bool clean=true);

	/** 
  Re-generate the surface according to the new threshold value. */
  void UpdateSurface(bool use_lod = false);

	/** 
  Re-slice the volume according to the new coordinate value. */
  void UpdateSlice();
};
#endif
