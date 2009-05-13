/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeVolumeSlice_BES.h,v $
  Language:  C++
  Date:      $Date: 2009-05-13 15:48:46 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipeVolumeSlice_H__B
#define __mafPipeVolumeSlice_H__B

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafPipeSlice.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class mafGUIFloatSlider;
class vtkPolyData;
class vtkImageData;
class vtkTexture;
class vtkLookupTable;
class vtkPolyDataMapper;
class vtkActor;
class vtkFollower;
class vtkMAFVolumeSlicer_BES;
class vtkCamera;
class vtkMAFAssembly;
class vtkOutlineSource;
class mafLODActor;
class mafVMEOutputVolume;

//----------------------------------------------------------------------------
// mafPipeVolumeSlice_BES :
//----------------------------------------------------------------------------
/** This visual pipe allows to represent a volume data as a slice according to the 
position of a plane representing the cutter. The default position of the slice is the center 
of the volume data. The visual pipe can also render 3 different slices centered in a origin point
and oriented along 3 different axes.*/
class mafPipeVolumeSlice_BES : public mafPipeSlice
{
public:
  mafTypeMacro(mafPipeVolumeSlice_BES,mafPipeSlice);

	mafPipeVolumeSlice_BES();
  virtual ~mafPipeVolumeSlice_BES();
	
  /** process events coming from Gui */
  virtual void OnEvent(mafEventBase *maf_event);

  /** IDs for the GUI */
  enum PIPE_VOLUME_SLICE_WIDGET_ID
  {
    ID_LUT_CHOOSER = Superclass::ID_LAST,
    ID_SLICE_SLIDER_X,
    ID_SLICE_SLIDER_Y,
    ID_SLICE_SLIDER_Z,
		ID_OPACITY_SLIDER,
    ID_ENABLE_GPU,
    ID_LAST
  };

  enum SLICE_DIRECTION_ID
  {
    SLICE_X = 0,
    SLICE_Y, 
    SLICE_Z, 
    SLICE_ORTHO,
    SLICE_ARB
  };
  /** Initialize the slicing parameter to show the volume box and cut direction.*/
  void InitializeSliceParameters(int direction, bool show_vol_bbox, bool show_bounds=false);

  /** Initialize the slicing parameter to show the volume box, cut direction and the cut origin.*/
  void InitializeSliceParameters(int direction, double slice_origin[3], bool show_vol_bbox,bool show_bounds=false);

  /** Initialize the slicing parameter to show the volume box, cut origin and orientation.*/
  void InitializeSliceParameters(int direction, double slice_origin[3], float slice_xVect[3], float slice_yVect[3], bool show_vol_bbox,bool show_bounds=false);
  
  /** Create the VTK rendering pipeline*/
  virtual void Create(mafSceneNode *n);

	/** Set the range to the lookup table for the slice. */
	void SetLutRange(double low, double high);
	
	/** Get the range of the slice's lookup table. */
	void GetLutRange(double range[2]);
	
	/** Set slicer parameter to generate the slice. */
	void SetSlice(double origin[3], float xVect[3], float yVect[3]);
	
  /** Set the origin and normal of the slice.
  Both, Origin and Normal may be NULL, if the current value is to be preserved. */
  /*virtual*/ void SetSlice(double* Origin, double* Normal);  
	
  /** Get the slice origin coordinates and normal.
  Both, Origin and Normal may be NULL, if the value is not to be retrieved.*/
  /*virtual*/ void GetSlice(double* Origin, double* Normal);  

  /** Assign a color lookup table to the slices*/
  void SetColorLookupTable(vtkLookupTable *lut);

  /** Return the color lookup table of the slices*/
  vtkLookupTable *GetColorLookupTable() {return m_ColorLUT;};

  /** Set the opacity of the slice. */
  void SetSliceOpacity(double opacity);
  
  /** Get the opacity of the slice. */
  float GetSliceOpacity();

	/** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
  virtual void Select(bool select); 

	void ShowTICKsOn();
	void ShowTICKsOff();

	void HideSlider();
	void ShowSlider();

protected:
	/** Create the slicer pipeline. */
	void CreateSlice(int direction);

	void CreateTICKs();

  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
  virtual mafGUI  *CreateGui();

  /** Updates VTK slicers. It also sets GPUEnabled flag.  */
  void UpdateSlice();

protected:		
	float		m_XVector[3][3]; ///< X Vector director of the cutting plane
	float		m_YVector[3][3]; ///< Y Vector director of the cutting plane
  float		m_NormalVector[3][3]; ///< Normal vectors defining the cutting plane orientation
  double  m_SliceOpacity; ///< Opacity of he volume slice.

	int		 m_TextureRes; ///< Texture resolution used to render the volume slice
	int		 m_SliceDirection; ///< Store the slicing direction: SLICE_X, SLICE_Y or SLICE_)
  
  int m_EnableGPU;  ///<Non-zero, if the GPU support for slicing is used (default)

  mafGUIFloatSlider *m_SliceSlider[3]; ///< Sliders used to move the cutting plane along the normals
  vtkMAFAssembly *m_AssemblyUsed;

  mafVMEOutputVolume *m_VolumeOutput;

//BES: 10.4.2008 - vtkVolumeSlicer was replaced by vtkMAFVolumeSlicer
  vtkMAFVolumeSlicer_BES				 *m_SlicerImage[3];
	vtkMAFVolumeSlicer_BES				 *m_SlicerPolygonal[3];
	vtkImageData					 *m_Image[3];
	vtkTexture						 *m_Texture[3];
  vtkLookupTable         *m_ColorLUT;
  vtkLookupTable         *m_CustomColorLUT;
  vtkPolyDataMapper			 *m_SliceMapper[3];
	vtkPolyData						 *m_SlicePolydata[3];
  vtkActor               *m_SliceActor[3];
	
  vtkActor               *m_VolumeBoxActor;
	vtkActor							 *m_TickActor;

  vtkActor               *m_GhostActor;

	vtkOutlineSource       *m_Box;
  vtkPolyDataMapper	     *m_Mapper;
  mafLODActor            *m_Actor;

  bool                    m_SliceParametersInitialized;
  bool                    m_ShowVolumeBox;
	bool										m_ShowBounds;
	bool										m_ShowSlider;
	bool										m_ShowTICKs;
};
#endif // __mafPipeVolumeSlice_H__B
