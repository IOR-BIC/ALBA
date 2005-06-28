/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeVolumeSlice.h,v $
  Language:  C++
  Date:      $Date: 2005-06-28 09:51:54 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef _mafPipeVolumeSlice_H_
#define _mafPipeVolumeSlice_H_

#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyData;
class vtkImageData;
class vtkTexture;
class vtkLookupTable;
class vtkPolyDataMapper;
class vtkActor;
class vtkVolumeSlicer;

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
	
  virtual void Create(mafSceneNode *n, int mode, double slice_origin[3], double slice_xVect[3], double slice_yVect[3], bool show_vol_bbox);

  /** Show-Hide the slice actor. */
	//void Show       (bool show); 
	
	/** Show the slice bounding box actor. */
	void Select     (bool select); 
	
	/** Set the range to the lookup table for the slice. */
	void SetLutRange(double low, double hi);
	
	/** Get the range of the slice's lookup table. */
	void GetLutRange(double range[2]);
	
	/** Set slicer parameter to generate the slice. */
	void SetSlice(double origin[3], double xVect[3], double yVect[3]);
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
					
protected:
	/** Create the slicer pipeline. */
	void CreateSlice(int mode);

	double m_Origin[3];
	double m_Normal[3][3];
	double m_XVector[3][3];
	double m_YVector[3][3];
  double m_SliceOpacity;

	int		m_TextureRes;
	int		m_SliceMode;

	vtkVolumeSlicer						*m_SlicerImage[3];
	vtkVolumeSlicer						*m_SlicerPolygonal[3];
	vtkImageData							*m_Image[3];
	vtkTexture								*m_Texture[3];
  vtkLookupTable            *m_ColourLUT[3];
  vtkPolyDataMapper					*m_SliceMapper[3];
	vtkPolyData								*m_SlicePolydata[3];
  vtkActor                  *m_SliceActor[3];
	
	vtkOutlineCornerFilter		*m_VolumeBox;
  vtkPolyDataMapper					*m_VolumeBoxMapper;
  vtkActor                  *m_VolumeBoxActor;

  vtkActor                  *m_GhostActor;

  bool                       m_ColorLUTEnabled;
};  

#endif // _mafPipeVolumeSlice_H_
