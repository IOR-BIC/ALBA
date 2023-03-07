/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVolumeArbSlice
 Authors: Paolo Quadrani, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeVolumeArbSlice_H__B
#define __albaPipeVolumeArbSlice_H__B

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
class vtkALBAVolumeSlicer;
class vtkCamera;
class vtkALBAAssembly;
class vtkOutlineSource;
class albaLODActor;
class albaVMEOutputVolume;
class vtkTransformFilter;
class vtkTransform;


/** 
class name: albaPipeVolumeArbSlice
  This visual pipe allows to represent a volume data as a slice according to the 
  position of a plane representing the cutter. The default position of the slice is the center 
  of the volume data. The visual pipe can also render 3 different slices centered in a origin point
  and oriented along 3 different axes.
*/
class ALBA_EXPORT albaPipeVolumeArbSlice : public albaPipe, public albaPipeSlice
{
public:
  /** RTTI Macro */
  albaTypeMacro(albaPipeVolumeArbSlice,albaPipe);
  /** Constructor */
	  albaPipeVolumeArbSlice();
  /** Destructor */
  virtual ~albaPipeVolumeArbSlice();
	
  /** process events coming from Gui */
  virtual void OnEvent(albaEventBase *alba_event);

  /** IDs for the GUI */
  enum PIPE_VOLUME_SLICE_WIDGET_ID
  {
    ID_LUT_CHOOSER = Superclass::ID_LAST,
		ID_OPACITY_SLIDER,
    ID_ENABLE_GPU,
    ID_ENABLE_TRILINEAR_INTERPOLATION,
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
  void InitializeSliceParameters(bool show_vol_bbox, bool show_bounds=false, bool interpolate=true);

  /** Initialize the slicing parameter to show the volume box, cut direction and the cut origin.*/
  void InitializeSliceParameters(double slice_origin[3], bool show_vol_bbox,bool show_bounds=false, bool interpolate=true);

  /** Initialize the slicing parameter to show the volume box, cut origin and orientation.*/
  void InitializeSliceParameters( double slice_origin[3], float slice_xVect[3], float slice_yVect[3], bool show_vol_bbox,bool show_bounds=false, bool interpolate=true);
  
  /** Create the VTK rendering pipeline*/
  virtual void Create(albaSceneNode *n);

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

  /** Show ticks in the rendering window */
	void ShowTICKsOn();
  /** Hide ticks in the rendering window */
	void ShowTICKsOff();

  /* This methods are added to allow views access m_EnableGPU member of albaPipeVolumeArbSlice:
  This is done in order to change the location of the enable GPU flag check box from the visual props panel to the view panel.
  An alternative, and less invasive, way to do this is to implement a GetGUI method. But this method generate duplicate check boxes in compound views.
  */

  /** Set the flag that enable/disable GPU */
  void SetEnableGPU(int enable);

  /** Get the flag that enable/disable GPU */
  int GetEnableGPU();

	/** Get ImageData */
	vtkImageData *GetImageData() { return m_Image; };
	vtkALBAVolumeSlicer *GetSliceImageData() { return m_SlicerImage; };
	vtkTexture *GetTexture() { return m_Texture; };

  /** Set tri-linear interpolation */  
  void SetTrilinearInterpolation(int on){m_TrilinearInterpolationOn = on; UpdateSlice();};

  /** Set tri-linear interpolation to off */
  void SetTrilinearInterpolationOff(){SetTrilinearInterpolation(0);};

  /** Set tri-linear interpolation to on */
  void SetTrilinearInterpolationOn(){SetTrilinearInterpolation(1);};

	bool isEnabledSliceViewCorrection() const { return m_EnableSliceViewCorrection; }
	void SetEnableSliceViewCorrection(bool val);
protected:
	/** Create the slicer pipeline. */
	void CreateSlice();

  /** Create all ticks */
	void CreateTICKs();

  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
  virtual albaGUI  *CreateGui();

  /** Updates VTK slicers. It also sets GPUEnabled flag.  */
  void UpdateSlice();

protected:		
	float		m_XVector[3]; ///< X Vector director of the cutting plane
	float		m_YVector[3]; ///< Y Vector director of the cutting plane
  float		m_NormalVector[3]; ///< Normal vector defining the cutting plane orientation
	float		m_EpisolonNormal[3];
  double  m_SliceOpacity; ///< Opacity of he volume slice.

	int		 m_TextureRes; ///< Texture resolution used to render the volume slice
	
  int m_EnableGPU;  ///<Non-zero, if the GPU support for slicing is used (default)

  vtkALBAAssembly *m_AssemblyUsed;
  albaVMEOutputVolume *m_VolumeOutput;

  vtkALBAVolumeSlicer				 *m_SlicerImage;
	vtkALBAVolumeSlicer				 *m_SlicerPolygonal;

	vtkTransform					*m_NormalTranform;
	vtkTransformFilter		*m_NormalTranformFilter;
	vtkImageData					 *m_Image;
	vtkTexture						 *m_Texture;
	int		 m_SliceDirection; ///< Store the slicing direction: SLICE_X, SLICE_Y or SLICE_)

  vtkLookupTable         *m_ColorLUT;
  vtkLookupTable         *m_CustomColorLUT;
  vtkPolyDataMapper			 *m_SliceMapper;
	vtkPolyData						 *m_SlicePolydata;
  vtkActor               *m_SliceActor;
	
  vtkActor               *m_VolumeBoxActor;
	vtkActor							 *m_TickActor;

  vtkActor               *m_GhostActor;

	vtkOutlineSource       *m_Box;
  vtkPolyDataMapper	     *m_Mapper;
  albaLODActor           *m_Actor;


	bool										m_VolIdentityMtr;
	albaMatrix						 *m_VolInverseMtr;
	albaMatrix						 *m_VolInvRotMtr;

  bool                    m_SliceParametersInitialized;
  bool                    m_ShowVolumeBox;
	bool										m_ShowBounds;
	bool										m_ShowTICKs;
  bool                    m_Interpolate;
	bool										m_EnableSliceViewCorrection;
  int m_TrilinearInterpolationOn; //<define if tri-linear interpolation is performed or not on slice's texture
};
#endif // __albaPipeVolumeArbSlice_H__B
