/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVolumeSliceBlend
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeVolumeSliceBlend_H__
#define __albaPipeVolumeSliceBlend_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class albaGUIFloatSlider;
class vtkOutlineCornerFilter;
class vtkPolyData;
class vtkImageData;
class vtkTexture;
class vtkLookupTable;
class vtkPolyDataMapper;
class vtkActor;
class vtkFollower;
class vtkALBAVolumeOrthoSlicer;
class vtkCamera;
class vtkALBAAssembly;
class vtkOutlineSource;
class vtkPlaneSource;
class albaLODActor;

//----------------------------------------------------------------------------
// albaPipeVolumeSliceBlend :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaPipeVolumeSliceBlend : public albaPipe
{
public:
  albaTypeMacro(albaPipeVolumeSliceBlend,albaPipe);

  albaPipeVolumeSliceBlend();
  virtual ~albaPipeVolumeSliceBlend();

  /** process events coming from gui */
  virtual void OnEvent(albaEventBase *alba_event);

  /** IDs for the GUI */
  enum PIPE_VOLUME_SLICE_WIDGET_ID
  {
    ID_LUT_CHOOSER = Superclass::ID_LAST,
    ID_OPACITY_SLIDER,
    ID_LAST
  };

  void InitializeSliceParameters(int direction, bool show_vol_bbox, bool show_bounds=false);
  void InitializeSliceParameters(int direction, double slice_origin0[3],double slice_origin1[3], bool show_vol_bbox,bool show_bounds=false);
 // void InitializeSliceParameters(int direction, double slice_origin0[3],double slice_origin1[3], float slice_xVect[3], float slice_yVect[3], bool show_vol_bbox,bool show_bounds=false);
  virtual void Create(albaSceneNode *n);

  /** Set the range to the lookup table for the slice. */
  void SetLutRange(double low, double high);

  /** Get the range of the slice's lookup table. */
  void GetLutRange(double range[2]);
	  
  /** Set slicer parameter to generate the slice. */
  void SetSlice(int nSlice, double origin[3]);

  /** Get slice parameters: origin and normal */
  void GetSliceOrigin(int nSlice,double origin[3]);
	  
  /** Assign a color lookup table to the slices*/
  void SetColorLookupTable(vtkLookupTable *lut);

  /** Return the color lookup table of the slices*/
  vtkLookupTable *GetColorLookupTable() {return m_ColorLUT;};

  /** Set the opacity of the slice. */
  void SetSliceOpacity(double opacity);

  /** Get the opacity of the slice. */
  float GetSliceOpacity();

  virtual void Select(bool select); 

protected:
  /** Create the slicer pipeline. */
  void CreateSlice(int direction);

  virtual albaGUI  *CreateGui();

  double	m_Origin[2][3];
  double  m_SliceOpacity;

  int		 m_SliceDirection; ///< Store the slicing direction: SLICE_X, SLICE_Y or SLICE_)

  vtkALBAAssembly *m_AssemblyUsed;

  vtkALBAVolumeOrthoSlicer*m_Slicer[2][3];
  vtkImageData					 *m_Image[2][3];
  vtkTexture						 *m_Texture[2][3];
  vtkLookupTable         *m_ColorLUT;
  vtkLookupTable         *m_CustomColorLUT;
  vtkPolyDataMapper			 *m_SliceMapper[2][3];
	vtkPlaneSource				 *m_SlicePlane[2][3];
  vtkActor               *m_SliceActor[2][3];

  vtkOutlineCornerFilter *m_VolumeBox;
  vtkPolyDataMapper			 *m_VolumeBoxMapper;
  vtkActor               *m_VolumeBoxActor;
  vtkActor							 *m_TickActor;

  vtkActor               *m_GhostActor[2];

  vtkOutlineSource       *m_Box;
  vtkPolyDataMapper	     *m_Mapper;
  albaLODActor            *m_Actor;

  bool                    m_SliceParametersInitialized;
  bool                    m_ShowVolumeBox;
  bool										m_ShowBounds;
};
#endif // __albaPipeVolumeSliceBlend_H__
