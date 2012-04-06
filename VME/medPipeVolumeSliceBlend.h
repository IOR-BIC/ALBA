/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medPipeVolumeSliceBlend.h,v $
Language:  C++
Date:      $Date: 2012-04-06 10:08:46 $
Version:   $Revision: 1.1.2.2 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medPipeVolumeSliceBlend_H__
#define __medPipeVolumeSliceBlend_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medVMEDefines.h"
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class mafGUIFloatSlider;
class vtkOutlineCornerFilter;
class vtkPolyData;
class vtkImageData;
class vtkTexture;
class vtkLookupTable;
class vtkPolyDataMapper;
class vtkActor;
class vtkFollower;
class vtkMAFVolumeSlicer;
class vtkCamera;
class vtkMAFAssembly;
class vtkOutlineSource;
class mafLODActor;

//----------------------------------------------------------------------------
// medPipeVolumeSliceBlend :
//----------------------------------------------------------------------------
class MED_VME_EXPORT medPipeVolumeSliceBlend : public mafPipe
{
public:
  mafTypeMacro(medPipeVolumeSliceBlend,mafPipe);

  medPipeVolumeSliceBlend();
  virtual ~medPipeVolumeSliceBlend();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  /** IDs for the GUI */
  enum PIPE_VOLUME_SLICE_WIDGET_ID
  {
    ID_LUT_CHOOSER = Superclass::ID_LAST,
    ID_SLICE_SLIDER_X,
    ID_SLICE_SLIDER_Y,
    ID_SLICE_SLIDER_Z,
    ID_OPACITY_SLIDER,
    ID_LAST
  };

  void InitializeSliceParameters(int direction, bool show_vol_bbox, bool show_bounds=false);
  void InitializeSliceParameters(int direction, double slice_origin0[3],double slice_origin1[3], bool show_vol_bbox,bool show_bounds=false);
  void InitializeSliceParameters(int direction, double slice_origin0[3],double slice_origin1[3], float slice_xVect[3], float slice_yVect[3], bool show_vol_bbox,bool show_bounds=false);
  virtual void Create(mafSceneNode *n);

  /** Set the range to the lookup table for the slice. */
  void SetLutRange(double low, double high);

  /** Get the range of the slice's lookup table. */
  void GetLutRange(double range[2]);

  /** Set slicer parameter to generate the slice. */
  void SetSlice(int nSlice,double origin[3], float xVect[3], float yVect[3]);

  /** Set slicer parameter to generate the slice. */
  void SetSlice(int nSlice, double origin[3]);

  /** Get slice parameters: origin and normal */
  void GetSliceOrigin(int nSlice,double origin[3]);

  /** Get slice parameters: origin and normal */
  void GetSliceNormal(double normal[3]);

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

  virtual mafGUI  *CreateGui();

  double	m_Origin[2][3];
  float		m_Normal[3][3];
  float		m_XVector[3][3];
  float		m_YVector[3][3];
  double  m_SliceOpacity;

  int		 m_TextureRes;
  int		 m_SliceDirection; ///< Store the slicing direction: SLICE_X, SLICE_Y or SLICE_)

  vtkMAFAssembly *m_AssemblyUsed;

  vtkMAFVolumeSlicer				 *m_SlicerImage[2][3];
  vtkMAFVolumeSlicer				 *m_SlicerPolygonal[2][3];
  vtkImageData					 *m_Image[2][3];
  vtkTexture						 *m_Texture[2][3];
  vtkLookupTable         *m_ColorLUT;
  vtkLookupTable         *m_CustomColorLUT;
  vtkPolyDataMapper			 *m_SliceMapper[2][3];
  vtkPolyData						 *m_SlicePolydata[2][3];
  vtkActor               *m_SliceActor[2][3];

  vtkOutlineCornerFilter *m_VolumeBox;
  vtkPolyDataMapper			 *m_VolumeBoxMapper;
  vtkActor               *m_VolumeBoxActor;
  vtkActor							 *m_TickActor;

  vtkActor               *m_GhostActor[2];

  vtkOutlineSource       *m_Box;
  vtkPolyDataMapper	     *m_Mapper;
  mafLODActor            *m_Actor;

  bool                    m_SliceParametersInitialized;
  bool                    m_ShowVolumeBox;
  bool										m_ShowBounds;
};
#endif // __medPipeVolumeSliceBlend_H__
