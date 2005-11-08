/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeVolumeStructuredSlice.h,v $
  Language:  C++
  Date:      $Date: 2005-11-08 16:06:53 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef _mafPipeVolumeStructuredSlice_H_
#define _mafPipeVolumeStructuredSlice_H_

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineFilter;
class vtkPlaneSource;
class vtkImageData;
class vtkTexture;
class vtkWindowLevelLookupTable;
class vtkLookupTable;
class vtkPolyDataMapper;
class vtkActor;
class vtkExtractVOI;
class vtkVolumeResample;

//----------------------------------------------------------------------------
// mafPipeVolumeStructuredSlice :
//----------------------------------------------------------------------------
class mafPipeVolumeStructuredSlice : public mafPipe
{
public:
  mafTypeMacro(mafPipeVolumeStructuredSlice,mafPipe);

	mafPipeVolumeStructuredSlice();
  virtual ~mafPipeVolumeStructuredSlice();
	
  //----------------------------------------------------------------------------
  // constant:
  //----------------------------------------------------------------------------
  enum STRUCTURED_SLICE_DIRECTION_ID
  {
    STRUCTURED_SLICE_X = 0,
    STRUCTURED_SLICE_Y, 
    STRUCTURED_SLICE_Z, 
    STRUCTURED_SLICE_ORTHO,
  };

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  /** IDs for the GUI */
  enum PIPE_VOLUME_STRUCTURED_SLICE_WIDGET_ID
  {
    ID_RGB_LUT = Superclass::ID_LAST,
    ID_SLICE_SLIDER_X,
    ID_SLICE_SLIDER_Y,
    ID_SLICE_SLIDER_Z,
    ID_LAST
  };

  void InitializeSliceParameters(int mode, bool show_vol_bbox);
  void InitializeSliceParameters(int mode, int slice_num[3], bool show_vol_bbox);
  virtual void Create(mafSceneNode *n);

	/** Show the slice bounding box actor. */
	void Select     (bool select); 
	
	/** Set the range to the lookup table for the slice. */
	void SetLutRange(double low, double hi);
	
	/** Get the range of the slice's lookup table. */
	void GetLutRange(double range[2]);
	
	/** Set slicer parameter to generate the slice. */
	void SetSlice(int slice_num[3]);
	
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
					
protected:
	/** Create the slicer pipeline. */
	void CreateSlice(int mode);

  virtual mmgGui  *CreateGui();

	double m_Origin[3];
  double m_SliceOpacity;
  double m_VolumeSpacing[3];
  double m_VolumeBounds[6];

  int    m_SliceNumber[3];
  int    m_VolumeExtent[6];
  int		 m_SliceMode;
  wxSlider *m_SliceSlider[3];

	vtkVolumeResample      *m_VolumeResampler;
  vtkImageData           *m_VolumeData;
  vtkExtractVOI			     *m_SlicerImage[3];
	vtkTexture						 *m_Texture[3];
  vtkWindowLevelLookupTable *m_GrayLUT[3];
  vtkLookupTable         *m_ColorLUT[3];
  vtkPolyDataMapper			 *m_SliceMapper[3];
  vtkPlaneSource			   *m_SlicePolygon[3];
  vtkActor               *m_SliceActor[3];
	
	vtkOutlineFilter       *m_VolumeBox;
  vtkPolyDataMapper			 *m_VolumeBoxMapper;
  vtkActor               *m_VolumeBoxActor;

  vtkActor               *m_GhostActor;

  int                     m_ColorLUTEnabled;
  bool                    m_SliceParametersInitialized;
  int                     m_ShowVolumeBox;
};  
#endif // _mafPipeVolumeStructuredSlice_H_
