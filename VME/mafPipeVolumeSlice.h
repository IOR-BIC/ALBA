/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeVolumeSlice.h,v $
  Language:  C++
  Date:      $Date: 2008-02-01 13:59:03 $
  Version:   $Revision: 1.22 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipeVolumeSlice_H__
#define __mafPipeVolumeSlice_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class mmgFloatSlider;
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
class vtkOutlineSource;
class mafLODActor;
class mafVMEOutputVolume;

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
/** This visual pipe allows to represent a volume data as a slice according to the 
position of a plane representing the cutter. The default position of the slice is the center 
of the volume data. The visual pipe can also render 3 different slices centered in a origin point
and oriented along 3 different axes.*/
class mafPipeVolumeSlice : public mafPipe
{
public:
  mafTypeMacro(mafPipeVolumeSlice,mafPipe);

	mafPipeVolumeSlice();
  virtual ~mafPipeVolumeSlice();
	
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
    ID_LAST
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
	
  /** Set slicer parameter to generate the slice. */
	void SetSlice(double origin[3]);
	
	/** Get slice parameters: origin and normal */
	void GetSliceOrigin(double origin[3]);
	
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
  virtual mmgGui  *CreateGui();

	double	m_Origin[3];    ///< Origin position of the cutting plane
	float		m_Normal[3][3]; ///< Normal vectors defining the cutting plane orientation
	float		m_XVector[3][3]; ///< X Vector director of the cutting plane
	float		m_YVector[3][3]; ///< Y Vector director of the cutting plane
  double  m_SliceOpacity; ///< Opacity of he volume slice.

	int		 m_TextureRes; ///< Texture resolution used to render the volume slice
	int		 m_SliceDirection; ///< Store the slicing direction: SLICE_X, SLICE_Y or SLICE_)

  mmgFloatSlider *m_SliceSlider[3]; ///< Sliders used to move the cutting plane along the normals
  vtkMAFAssembly *m_AssemblyUsed;

  mafVMEOutputVolume *m_VolumeOutput;

  vtkVolumeSlicer				 *m_SlicerImage[3];
	vtkVolumeSlicer				 *m_SlicerPolygonal[3];
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
#endif // __mafPipeVolumeSlice_H__
