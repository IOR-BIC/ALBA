/*=========================================================================

Program: ALBA (Agile Library for Biomedical Applications)
Module: albaPipeVolumeArbSlice
Authors: Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or


This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeVolumeArbOrthoSlice_H__B
#define __albaPipeVolumeArbOrthoSlice_H__B

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipe.h"
#include "albaPipeSlice.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class albaPipeVolumeArbSlice;
class vtkALBAAssembly;
class albaVMEOutputVolume;
class albaGUIFloatSlider;
class vtkLookupTable;
class vtkOutlineSource;
class albaLODActor;


/** 
class name: albaPipeVolumeArbOrthoSlice
  This visual pipe allows to represent a volume data as a slice according to the 
  position of a plane representing the cutter. The default position of the slice is the center 
  of the volume data. The visual pipe can also render 3 different slices centered in a origin point
  and oriented along 3 different axes.
*/
class albaPipeVolumeArbOrthoSlice : public albaPipe
{
public:
  /** RTTI Macro */
  albaTypeMacro(albaPipeVolumeArbOrthoSlice, albaPipe);
  /** Constructor */
	  albaPipeVolumeArbOrthoSlice();
  /** Destructor */
  virtual ~albaPipeVolumeArbOrthoSlice();

	enum PIPE_VOLUME_SLICE_WIDGET_ID
	{
		ID_LUT_CHOOSER = Superclass::ID_LAST,
		ID_SLICE_SLIDER_X,
		ID_SLICE_SLIDER_Y,
		ID_SLICE_SLIDER_Z,
		ID_OPACITY_SLIDER,
		ID_ENABLE_GPU,
		ID_ENABLE_TRILINEAR_INTERPOLATION,
		ID_LAST
	};
	
  /** process events coming from Gui */
  virtual void OnEvent(albaEventBase *alba_event);

    
  /** Create the VTK rendering pipeline*/
  virtual void Create(albaSceneNode *n);

	/** Set the range to the lookup table for the slice. */
	void SetLutRange(double low, double high);
	
	/** Get the range of the slice's lookup table. */
	void GetLutRange(double range[2]);
	
	/** Set slicer parameter to generate the slice. */
	void SetSlice(int direction, double origin[3], float xVect[3], float yVect[3]);
	
  /** Set the origin and normal of the slice.
  Both, Origin and Normal may be NULL, if the current value is to be preserved. */
	/*virtual*/ void SetSlice(int direction,  double* Origin, double* Normal);
	
  /** Get the slice origin coordinates and normal.
  Both, Origin and Normal may be NULL, if the value is not to be retrieved.*/
  /*virtual*/ void GetSlice(int direction, double* Origin, double* Normal);

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

  /* Added by Losi 12.02.2009
  This methods are added to allow views access m_EnableGPU member of albaPipeVolumeArbOrthoSlice:
  This is done in order to change the location of the enable GPU flag check box from the visual props panel to the view panel.
  An alternative, and less invasive, way to do this is to implement a GetGUI method. But this method generate duplicate check boxes in compound views.
  */

  /** Set the flag that enable/disable GPU */
  void SetEnableGPU(int enable);

  /** Get the flag that enable/disable GPU */
  int GetEnableGPU();

  /** Set tri-linear interpolation */  
  void SetTrilinearInterpolation(int on);;

  /** Set tri-linear interpolation to off */
  void SetTrilinearInterpolationOff(){SetTrilinearInterpolation(0);};

  /** Set tri-linear interpolation to on */
  void SetTrilinearInterpolationOn(){SetTrilinearInterpolation(1);};

protected:
	/** Create the slicer pipeline. */
	void CreateSlice(int direction, albaSceneNode *n);

  /** Create all ticks */
	void CreateTICKs();

  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
  virtual albaGUI  *CreateGui();

protected:		
	float		m_XVector[3][3]; ///< X Vector director of the cutting plane
	float		m_YVector[3][3]; ///< Y Vector director of the cutting plane
  float		m_NormalVector[3][3]; ///< Normal vectors defining the cutting plane orientation
	double  m_Origin[3];

	vtkALBAAssembly *m_AssemblyUsed;

  albaGUIFloatSlider *m_SliceSlider[3]; ///< Sliders used to move the cutting plane along the normals
  vtkActor               *m_VolumeBoxActor;
	vtkActor							 *m_TickActor;
	vtkOutlineSource       *m_Box;
  vtkPolyDataMapper	     *m_Mapper;
  albaLODActor            *m_Actor;

	albaPipeVolumeArbSlice *m_SlicingPipes[3];

	albaVMEOutputVolume			*m_VolumeOutput;
	vtkLookupTable         *m_ColorLUT;

	int m_EnableGPU;  ///<Non-zero, if the GPU support for slicing is used (default)
	bool                    m_Interpolate;
	int m_TrilinearInterpolationOn; //<define if tri-linear interpolation is performed or not on slice's texture
	double  m_SliceOpacity; ///< Opacity of he volume slice.

  bool                    m_ShowVolumeBox;
	bool										m_ShowBounds;
	bool										m_ShowSlider;
	bool										m_ShowTICKs;
};
#endif // __albaPipeVolumeArbOrthoSlice_H__B
