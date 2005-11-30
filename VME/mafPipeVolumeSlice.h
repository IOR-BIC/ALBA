/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeVolumeSlice.h,v $
  Language:  C++
  Date:      $Date: 2005-11-30 11:35:14 $
  Version:   $Revision: 1.10 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef _mafPipeVolumeSlice_H_
#define _mafPipeVolumeSlice_H_

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class mmgFloatSlider;
class vtkOutlineCornerFilter;
class vtkPolyData;
class vtkImageData;
class vtkTexture;
class vtkLookupTable;
class vtkPolyDataMapper;
class vtkActor;
class vtkFollower;
class vtkVolumeSlicer;
class vtkCamera;
class vtkMAFAssembly;

//----------------------------------------------------------------------------
// constant:
//----------------------------------------------------------------------------
enum SLICE_DIRECTION_ID
{
  SLICE_X = 0,
  SLICE_Y, 
  SLICE_Z, 
  SLICE_ORTHO,
  SLICE_ARB
};

//----------------------------------------------------------------------------
// mafPipeVolumeSlice :
//----------------------------------------------------------------------------
class mafPipeVolumeSlice : public mafPipe
{
public:
  mafTypeMacro(mafPipeVolumeSlice,mafPipe);

	mafPipeVolumeSlice();
  virtual ~mafPipeVolumeSlice();
	
  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  /** IDs for the GUI */
  enum PIPE_VOLUME_SLICE_WIDGET_ID
  {
    ID_RGB_LUT = Superclass::ID_LAST,
    ID_SLICE_SLIDER_X,
    ID_SLICE_SLIDER_Y,
    ID_SLICE_SLIDER_Z,
    ID_LAST
  };

  void InitializeSliceParameters(int mode, bool show_vol_bbox);
  void InitializeSliceParameters(int mode, double slice_origin[3], bool show_vol_bbox);
  void InitializeSliceParameters(int mode, double slice_origin[3], float slice_xVect[3], float slice_yVect[3], bool show_vol_bbox);
  virtual void Create(mafSceneNode *n);

	/** Show the slice bounding box actor. */
	void Select     (bool select); 
	
	/** Set the range to the lookup table for the slice. */
	void SetLutRange(double low, double hi);
	
	/** Get the range of the slice's lookup table. */
	void GetLutRange(double range[2]);
	
	/** Set slicer parameter to generate the slice. */
	void SetSlice(double origin[3], float xVect[3], float yVect[3]);
	
  /** Set slicer parameter to generate the slice. */
	void SetSlice(double origin[3]);
	
	/** Get slice parameters: origin and normal */
	void GetSliceOrigin(double origin[3]);
	
  /** Get slice parameters: origin and normal */
	void GetSliceNormal(double normal[3]);

  /** 
  Enable/Disable color lookupTable for volume slice. */
  void ColorLookupTable(bool enable);
					
  /** 
  return true/false if color lookupTable for volume slice is Enabled/Disabled. */
  bool IsColorLookupTable();

  /** 
  Set the opacity of the slice. */
  void SetSliceOpacity(float opacity);
  
  /** 
  Get the opacity of the slice. */
  float GetSliceOpacity();

  /** 
  Allow to show/hide the cursor unit with length 1 cm. 
  Passing the camera as second parameter, allow the cursor to follow the camera orientation.*/
  void ShowUnit(bool show_unit, vtkCamera *camera_to_follow = NULL);
					
protected:
	/** Create the slicer pipeline. */
	void CreateSlice(int mode);

  /** 
  Draw unit segment */
  void DrawUnit();

  virtual mmgGui  *CreateGui();

	double m_Origin[3];
	float  m_Normal[3][3];
	float  m_XVector[3][3];
	float  m_YVector[3][3];
  float  m_SliceOpacity;

	int		  m_TextureRes;
	int		  m_SliceMode;
  mmgFloatSlider *m_SliceSlider[3];

  vtkMAFAssembly *m_AssemblyUsed;

	vtkVolumeSlicer				 *m_SlicerImage[3];
	vtkVolumeSlicer				 *m_SlicerPolygonal[3];
	vtkImageData					 *m_Image[3];
	vtkTexture						 *m_Texture[3];
  vtkLookupTable         *m_ColorLUT[3];
  vtkPolyDataMapper			 *m_SliceMapper[3];
	vtkPolyData						 *m_SlicePolydata[3];
  vtkActor               *m_SliceActor[3];
	
	vtkOutlineCornerFilter *m_VolumeBox;
  vtkPolyDataMapper			 *m_VolumeBoxMapper;
  vtkActor               *m_VolumeBoxActor;

  int                     m_UnitLength;
  vtkFollower            *m_UnitCubeActor;
  vtkCamera              *m_CameraToFollow;

  vtkActor               *m_GhostActor;

  int                     m_ColorLUTEnabled;
  bool                    m_SliceParametersInitialized;
  bool                    m_ShowVolumeBox;
  bool                    m_ShowUnit;
};
#endif // _mafPipeVolumeSlice_H_
