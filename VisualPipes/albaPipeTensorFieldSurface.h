/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: albaPipeTensorFieldSurface.h,v $ 
  Language: C++ 
  Date: $Date: 2012-04-06 10:06:18 $ 
  Version: $Revision: 1.1.2.6 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#ifndef albaPipeTensorFieldSurface_h__
#define albaPipeTensorFieldSurface_h__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipeTensorField.h"

//----------------------------------------------------------------------------
// Forward declarations:
//----------------------------------------------------------------------------
class albaGUI;

class vtkPolyDataMapper;
class vtkDataSet;
class vtkLookupTable;
class vtkActor;
class vtkScalarBarActor;
class vtkImageData;
class vtkGeometryFilter;

/** Displays the surface of input VME (even, if it is volume),
using color mapping according to X,Y,Z or magnitude of associated
tensor field (selected in the gui). */
class ALBA_EXPORT albaPipeTensorFieldSurface : public albaPipeTensorField
{
public:
  albaTypeMacro(albaPipeTensorFieldSurface, albaPipeTensorField);

protected:
  /** IDs for the GUI */
  enum PIPE_VIEWFLOW_WIDGET_ID
  {
    ID_VECTORFIELD = Superclass::ID_LAST,     
    ID_COLOR_MAPPING_MODE,
    ID_COLOR_MAPPING_LUT,
    ID_SHOW_COLOR_MAPPING,
    ID_LAST,
  };    

  enum COLOR_MAPPING_MODES
  {
    CMM_MAGNITUDE,
	CMM_EIGENVALUE0,
	CMM_EIGENVALUE1,
	CMM_EIGENVALUE2,
    CMM_COMPONENT1,
    CMM_COMPONENT2,
    CMM_COMPONENT3,
    CMM_COMPONENT4,
    CMM_COMPONENT5,
    CMM_COMPONENT6,
    CMM_COMPONENT7,
    CMM_COMPONENT8,
    CMM_COMPONENT9,
  };
  
  int m_ColorMappingMode;             ///<color mapping mode    
  vtkLookupTable* m_ColorMappingLUT;  ///<lookup table used for the mapping    
  int m_ShowMapping;                  ///<non-zero, if the mapping should be displayed in the main view
  
  vtkScalarBarActor* m_MappingActor;  ///<actor that displays the mapping bar

  vtkPolyDataMapper* m_SurfaceMapper;  ///<mapper for glyphs
  vtkGeometryFilter* m_GeometricFilter;
  vtkActor* m_SurfaceActor;            ///<actor for glyphs  

  wxComboBox* m_ComboColorBy;           ///<combo box with list of components
  double m_Covariance[3][3] ;
  double m_Lambda[3] ;
  double m_V[3][3] ;
public:	
  albaPipeTensorFieldSurface();
  virtual ~albaPipeTensorFieldSurface();

public:  
  /** Processes events coming from GUI */
  /*virtual*/ void OnEvent(albaEventBase *alba_event);
  void EigenVectors3x3(double A[3][3], double lambda[3], double V[3][3]);
  void MultiplyColumnsByScalars(const double *s, const double *A, double *B) const;
  bool ComputeEigenvalues(vtkImageData* tensorVolume,double sr[2],int mode);

protected:
  /*virtual*/ albaGUI  *CreateGui();

  /** Constructs VTK pipeline. */
  virtual void CreateVTKPipe();

  /** Updates VTK pipeline (setting radius, etc.). */
  virtual void UpdateVTKPipe();  

  /** Updates the content of m_comboColorBy combobox.
  "magnitude" and 0..NumberOfComponents-1 will be listed.*/
  virtual void UpdateColorByCombo();
};
#endif // albaPipeTensorFieldSurface_h__