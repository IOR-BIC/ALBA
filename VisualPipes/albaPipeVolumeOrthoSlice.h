/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVolumeOrthoSlice
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeVolumeOrthoSlice_H__B
#define __albaPipeVolumeOrthoSlice_H__B

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipeSlice.h"
#include "albaPipe.h"


//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class albaGUIFloatSlider;
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
class albaLODActor;
class albaVMEOutputVolume;
class vtkPlaneSource;


/** 
class name: albaPipeVolumeOrthoSlice
  This visual pipe allows to represent a volume data as a slice according to the 
  position of a plane representing the cutter. The default position of the slice is the center 
  of the volume data. The visual pipe can also render 3 different slices centered in a origin point
  and oriented along 3 different axes.
*/
class ALBA_EXPORT albaPipeVolumeOrthoSlice : public albaPipe, public albaPipeSlice
{
public:
  /** RTTI Macro */
  albaTypeMacro(albaPipeVolumeOrthoSlice,albaPipe);
  /** Constructor */
	  albaPipeVolumeOrthoSlice();
  /** Destructor */
  virtual ~albaPipeVolumeOrthoSlice();
	
  /** process events coming from Gui */
  virtual void OnEvent(albaEventBase *alba_event);

  /** IDs for the GUI */
  enum PIPE_VOLUME_SLICE_WIDGET_ID
  {
    ID_LUT_CHOOSER = Superclass::ID_LAST,
    ID_SLICE_SLIDER_X,
    ID_SLICE_SLIDER_Y,
    ID_SLICE_SLIDER_Z,
		ID_OPACITY_SLIDER,
    ID_ENABLE_TRILINEAR_INTERPOLATION,
    ID_LAST
  };

  enum SLICE_DIRECTION_ID
  {
    SLICE_X = 0,
    SLICE_Y, 
    SLICE_Z, 
    SLICE_ORTHO,
  };
  /** Initialize the slicing parameter to show the volume box and cut direction.*/
  void InitializeSliceParameters(int direction, bool show_vol_bbox, bool show_bounds=false, bool interpolate=true);

  /** Initialize the slicing parameter to show the volume box, cut direction and the cut origin.*/
  void InitializeSliceParameters(int direction, double slice_origin[3], bool show_vol_bbox,bool show_bounds=false, bool interpolate=true);

  /** Create the VTK rendering pipeline*/
  virtual void Create(albaSceneNode *n);

	/** Set the range to the lookup table for the slice. */
	void SetLutRange(double low, double high);
	
	/** Get the range of the slice's lookup table. */
	void GetLutRange(double range[2]);
	
	/** Set the origin and normal of the slice.
  Both, Origin and Normal may be NULL, if the current value is to be preserved. */
  /*virtual*/ void SetSlice(double* origin, double* normal);  
	
  /** Get the slice origin coordinates and normal.
  Both, Origin and Normal may be NULL, if the value is not to be retrieved.*/
  /*virtual*/ void GetSlice(double* origin, double* normal);  

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

  /** Show ticks in the rendering window */
	void ShowTICKsOn();
  /** Hide ticks in the rendering window */
	void ShowTICKsOff();

  /** Hide slider  */
	void HideSlider();
  /** Show slider */
	void ShowSlider();

  /** Set tri-linear interpolation */  
  void SetInterpolation(int on);;

  /** Set tri-linear interpolation to off */
  void SetInterpolationOff(){SetInterpolation(0);};

  /** Set tri-linear interpolation to on */
  void SetInterpolationOn(){SetInterpolation(1);};


	/** Returns SliceDirection */
	int GetSliceDirection() const { return m_SliceDirection; }

	/** Sets SliceDirection */
	void SetSliceDirection(int sliceDirection);

	/** returns a slicer*/
	vtkALBAVolumeOrthoSlicer *GetSlicer(int slicerDirection);

	/** Set the actor picking*/
	void SetActorPicking(int enable);

protected:
	/** Create the slicer pipeline. */
	void CreateSlice();

	/** Delete the slicer pipeline. */
	void DeleteSlice();

	void UpdatePlaneOrigin(int direction);

  /** Create all ticks */
	void CreateTICKs();

  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
  virtual albaGUI  *CreateGui();

  /** Updates VTK slicers. It also sets GPUEnabled flag.  */
  void UpdateSlice();
	
protected:		
  double  m_SliceOpacity; ///< Opacity of he volume slice.

	int		 m_TextureRes; ///< Texture resolution used to render the volume slice
	int		 m_SliceDirection; ///< Store the slicing direction: SLICE_X, SLICE_Y or SLICE_)

  albaGUIFloatSlider *m_SliceSlider[3]; ///< Sliders used to move the cutting plane along the normals
  vtkALBAAssembly *m_AssemblyUsed;
  albaVMEOutputVolume *m_VolumeOutput;
  vtkALBAVolumeOrthoSlicer *m_Slicer[3];
	vtkTexture							*m_Texture[3];
  vtkLookupTable         *m_ColorLUT;
  vtkLookupTable         *m_CustomColorLUT;
  vtkPolyDataMapper			 *m_SliceMapper[3];
  vtkActor               *m_SliceActor[3];
	vtkPlaneSource				 *m_SlicePlane[3];

	
  vtkActor               *m_VolumeBoxActor;
	vtkActor							 *m_TickActor;


	vtkOutlineSource       *m_Box;
  vtkPolyDataMapper	     *m_Mapper;
  albaLODActor            *m_Actor;

  bool                    m_SliceParametersInitialized;
  bool                    m_ShowVolumeBox;
	bool										m_ShowBounds;
	bool										m_ShowSlider;
	bool										m_ShowTICKs;
	bool										m_Pickable;
  int                     m_Interpolate;
};
#endif // __albaPipeVolumeOrthoSlice_H__B
