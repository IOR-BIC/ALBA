/*=========================================================================
Program:   AlbaMaster
Module:    albaOpExtractIsosurface.h
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2018 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpExtractIsosurface_H__
#define __albaOpExtractIsosurface_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUIDialog;
class albaGUIFloatSlider;
class albaVMESurface;
class albaSceneNode;
class albaVMEGroup;
class albaVME;
class albaRWI;

class vtkActor;
class albaPipeIsosurface; 
class vtkPolyDataMapper;
class vtkOutlineCornerFilter;
class vtkRenderer;

class albaInteractorExtractIsosurface;
class vtkImageData;
class vtkTexture;
class vtkPolyData;
class vtkALBAVolumeSlicer;
class vtkALBAFixedCutter;
class vtkPlane;
class vtkVolume;


#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,double);
#endif

//----------------------------------------------------------------------------
// albaOpExtractIsosurface :
//----------------------------------------------------------------------------
/** This operation generate a surface representing a collection of a selected scalar value of the input volume.
The operation offers a preview of the Iso-Density-Surface and offers also a section of the volume with the 
contour of the surface itself. The user can pick also into the slice with the left mouse button to generate the 
corresponding iso-density-surface.*/
class ALBA_EXPORT albaOpExtractIsosurface: public albaOp
{
public:

  albaOpExtractIsosurface(const wxString &label = "Extract Isosurface");
 ~albaOpExtractIsosurface(); 
	
  albaTypeMacro(albaOpExtractIsosurface, albaOp);

  albaOp* Copy();
  void OnEvent(albaEventBase *alba_event);

public:
  void UpdateCutterInput();

	/** Builds operation's interface by calling CreateOpDialog() method. */
  void OpRun();
    
  /** Set iso value used by operation. */
  void SetIsoValue(double isoValue);

  /** Extract the isosurface and build the related vme. */
  void ExtractSurface();

  /** Create the pipeline to generate the isosurface of the volume. */
  void CreateVolumePipeline();

  /** Re-generate the surface according to the new threshold value. */
  void UpdateSurface();

  /** Set the triangulate flag.*/
  void SetTriangulate(bool triangulate);

  /** Set the clean flag.*/
  void SetClean(bool clean);

	void SetConnectivity(bool connectivity);

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:

  albaVMEGroup *m_OutputGroup; 
  albaGUIDialog		*m_Dialog;
	albaRWI      *m_Rwi;
  vtkRenderer *m_PIPRen;
	double       m_IsoValue;
	double       m_MaxDensity;
	double       m_MinDensity;
	double       m_StepDensity;
  double       m_Slice;
  double       m_SliceMin;
  double       m_SliceMax;
	double       m_SliceStep;
  int          m_ShowSlice;
  int          m_Optimize;
	int					 m_Clean;
	int					 m_Triangulate;
	int					 m_Connectivity;
  int          m_Autolod;
  double       m_BoundingBox[6];
  double       m_SliceOrigin[3];
  float        m_SliceXVect[3];
  float        m_SliceYVect[3];
  int m_TrilinearInterpolationOn;


  int m_MultiContoursFlag;
  int m_NumberOfContours;
  double m_MinRange;
  double m_MaxRange;

  albaGUIFloatSlider *m_IsoSlider;
  albaGUIFloatSlider *m_SliceSlider;
  
  vtkActor                *m_Box;
  albaSceneNode         *m_VolNode;
  albaPipeIsosurface    *m_PipeIso;
  vtkOutlineCornerFilter  *m_OutlineFilter;
  vtkPolyDataMapper       *m_OutlineMapper;

  vtkALBAVolumeSlicer   *m_VolumeSlicer;
  vtkALBAVolumeSlicer   *m_PolydataSlicer;
  vtkImageData      *m_SliceImage;
  vtkTexture        *m_SliceTexture;
  vtkPolyData       *m_Polydata;          // copy of polydata used by slicer
  vtkPolyDataMapper *m_SlicerMapper;
  vtkActor          *m_SlicerActor;
  vtkPolyDataMapper *m_PolydataMapper;
  vtkActor          *m_PolydataActor;
  vtkPlane          *m_CutterPlane;
  vtkALBAFixedCutter    *m_IsosurfaceCutter;

  albaInteractorExtractIsosurface *m_DensityPicker;

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** Builds operation's interface and visualization pipeline. */
  void CreateOpDialog();

	/** Remove operation's interface. */
  void DeleteOpDialog();
	
	/** Create the pipeline to generate the slice of the volume. */
  void CreateSlicePipeline();
	
	/** Re-slice the volume according to the new coordinate value. */
  void UpdateSlice();
};
#endif
