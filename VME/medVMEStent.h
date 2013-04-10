/*=========================================================================
  Program:   MAF2Medical
  Module:    $RCSfile: medVMEStent.h,v $
  Language:  C++
  Date:      $Date: 2012-10-23 10:15:31 $
  Version:   $Revision: 1.1.2.7 $
  Authors:   Hui Wei
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __medVMEStent_h 
#define __medVMEStent_h

/**----------------------------------------------------------------------------*/
// Include:
/**----------------------------------------------------------------------------*/
#include "medVMEDefines.h"  //very important for MED_VME_EXPORT
#include "mafVME.h"
#include "mafEvent.h"
#include "mafGuiDialog.h"
#include "mafGuiButton.h"

#include "vtkMEDStentModelSource.h"
#include "vtkMEDDeformableSimplexMeshFilter.h"

#include <vector>
using std::vector;
/**----------------------------------------------------------------------------*/
// forward declarations :
/**----------------------------------------------------------------------------*/
class mafNode;
class mmaMaterial;
class mafVMEOutputSurface;
class mafVMEOutputPolyline;
class vtkPolyData;
class vtkTransform;
class vtkAppendPolyData;
class vtkTubeFilter;
class vtkCellArray;
class vtkPoints;
/**----------------------------------------------------------------------------*/
//--------typedef----------
/**----------------------------------------------------------------------------*/
typedef itk::DefaultDynamicMeshTraits<double, 3, 3, double, double> MeshTraits;
typedef itk::SimplexMesh<double,3,MeshTraits>           SimplexMeshType;
typedef SimplexMeshType::NeighborListType               NeighborsListType;
typedef itk::SimplexMeshGeometry::IndexArray            IndexArray;
typedef SimplexMeshType::PointType                      PointType;
typedef vtkMEDStentModelSource::Strut                         Strut;
typedef vtkMEDStentModelSource::StentConfigurationType        enumStCfgType;
typedef vtkMEDStentModelSource::LinkConnectionType            enumLinkConType;
typedef vtkMEDStentModelSource::LinkOrientationType           enumLinkOrtType;

typedef vector<Strut>::const_iterator    StrutIterator;

/**
this class create a stent based on some basic parameters. 
It also can be depolyed to a curve line as its centerline.
After it was given an constrain surface, it can expand inside this
constrain surface.
*/
class MED_VME_EXPORT medVMEStent : public mafVME
{

public:

  mafTypeMacro(medVMEStent, mafVME);
  /* constructor */
  medVMEStent();
  /* destructor */
  virtual ~medVMEStent();

   /** Set the time for this VME.
  It updates also the VTK representation .*/
  void SetTimeStamp(mafTimeStamp t);
  
  	/************************************************************************/
	/* The mesh deformation is constrained by internal forces. The interal force can be scaled via SetAlpha (typical values are 0.01 < alpha < 0.3). 
	 * The external force is derived from the image one wants to delineate. Therefore an image of type GradientImageType needs to be set by calling SetGradientImage(...). The external forces are scaled via SetBeta (typical values are 0.01 < beta < 1). One still needs to play around with these values.

	 * To control the smoothness of the mesh a rigidity parameter can be adjusted. Low values (1 or 0) allow areas with high curvature. Higher values (around 7 or 8) will make the mesh smoother.

	 * By setting the gamma parameter the regularity of the mesh is controlled. Low values (< 0.03) produce more regular mesh. Higher values ( 0.3 < gamma < 0.2) will allow to move the vertices to regions of higher curvature.

	 *This approach for segmentation follows that of Delingette et al. (1997).                                                                     */
	/************************************************************************/

	void DoDeformation3(int type);

	void DisplayStentExpend( int steps );

	void ResetStentPoints( vtkPoints* currentPoints );

	void DisplayStentExpendByStep(mafTimeStamp t);

	void DisplayCatheter();


  //---------------------------Setter-/-Getter------------------------------------  
  /** Copy the contents of another medVMEStent into this one. */
  virtual int DeepCopy(mafNode *a);
  /** Compare with another mafVMESurfaceParametric. */
  virtual bool Equals(mafVME *vme);
  
  /** Return the suggested pipe-typename for the visualization of this vme */
  //virtual mafString GetVisualPipe() {return mafString("mafPipeSurface");};
  //mafVMEOutputSurface *GetSurfaceOutput();//return the right type of output 

  /** used to visualize this vme */
  virtual mafString GetVisualPipe() {return mafString("mafPipePolyline");};
  mafVMEOutputPolyline *GetPolylineOutput(); //return the right type of output 

 // vtkPolyData *GetPolyData(){InternalUpdate();return m_PolyData;};
  /** the append polydata */
  vtkPolyData *GetPolyData();//{ InternalUpdate();return  m_PolyData;};// m_AppendPolyData->GetOutput(); };
  
  /** Get the vtkPolyData generated from the model */
  //vtkPolyData  *GetVtkPolyData(){InternalUpdate();return m_PolyData;};

  /**
  Return the list of timestamps for this VME. Timestamps list is 
  obtained merging timestamps for matrixes and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
  is the same for all timestamps). */
  virtual bool IsAnimated();



  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();
	
  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  void SetAndKeepConstrainSurface( mafNode *node );

  void SetAndKeepCenterLine( mafNode *node );

  //void SetAndKeepConstrainSurface( mafVME *vme );

 //void SetAndKeepCenterLine( mafVME *vme );

  /**
  Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
  set or get the Pose for a specified time. When setting, if the time does not exist
  the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
  interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const mafMatrix &mat);
  
  /**
  select center line or constrain surface 
  */
    void SetStentLink(const char *link_name, mafNode *ns);
  //------set properties-----------
  void SetCenterLine(vtkPolyData *line);
  void SetConstrainSurface(vtkPolyData *surface);
   
  void SetStentDiameter(double diameter){m_Stent_Diameter = diameter;}
  void SetStentCrownLength(double crownL){m_Crown_Length = crownL;}
  void SetStrutThickness(double strutThickness){m_Strut_Thickness = strutThickness; }
  void SetStentConfiguration(int stentCfig){m_Id_Stent_Configuration = stentCfig;}
  void SetLinkLength(double linkLength){m_Link_Length = linkLength;}
  void SetLinkAlignment(int linkAlgn){m_Link_Alignment = linkAlgn;}
  void SetLinkOrientation(int linkOrit){m_Link_orientation = linkOrit;}
  void SetLinkConnection(int linkConnection){m_Id_Link_Connection = linkConnection;}
  void SetStentCrownNumber(int crownNumber){m_Crown_Number = crownNumber; }
  
  //------get properties-----------
  vtkPolyData* GetCenterLine(){return m_Centerline;};
  vtkPolyData* GetConstrainSurface(){return m_ConstrainSurface;};

  double GetStentDiameter(){ return m_Stent_Diameter;}
  double GetStentCrownLength(){ return m_Crown_Length;}
  double GetStrutThickness(){ return m_Strut_Thickness;}
  int GetStentConfiguration(){ return m_Id_Stent_Configuration;}
  double GetLinkLength(){ return m_Link_Length;}
  int GetLinkAlignment(){ return m_Link_Alignment;}
  int GetLinkOrientation(){ return m_Link_orientation;}
  int GetLinkConnection(){ return m_Id_Link_Connection;}
  int GetStentCrownNumber(){ return m_Crown_Number ; }
  //--------convert tube strips into triangle polydata 
  vtkPolyData *TubeToPolydata( vtkTubeFilter * sheath );


protected:
  /** for gui control */
  enum STENT_WIDGET_ID
  {
    ID = MINID,
    CHANGE_VALUE,
    CHANGE_VALUE_CROWN,
    STENT_DIAMETER,
    CROWN_LENGTH,
    STRUT_ANGLE,
    STRUT_THICKNESS,
    ID_STENT_CONFIGURATION,
    ID_LINK_CONNECTION,
    LINK_LENGTH,
    NUMBER_OF_LINKS,//(along circumference)
    LINK_ALIGNMENT,
    LINK_ORIENTATION,
    ID_CENTERLINE,
    ID_CONSTRAIN_SURFACE,
    ID_DEFORMATION,
    ID_DEFORMATION_PARAMETERS,
    ID_DEF_PARAMS_RESTORE,
    ID_DEF_PARAMS_UNDO,
    ID_DEF_PARAMS_OK,
    ID_DEF_PARAMS_CANCEL,
    ID_DEF_PARAMS_ALPHA,
    ID_DEF_PARAMS_BETA,
    ID_DEF_PARAMS_GAMMA,
    ID_DEF_PARAMS_RIGIDITY,
    ID_DEF_PARAMS_DAMPING,
    ID_DEF_PARAMS_EPSILON,
    ID_DEF_PARAMS_ITS_PER_STEP,
    ID_DEF_PARAMS_STEPS,
    CHANGE_VIEW,
    ID_LAST
  };

  /** store attributes and matrix*/
  virtual int InternalStore(mafStorageElement *parent);
  /** restore attributes and matrix*/
  virtual int InternalRestore(mafStorageElement *node);

  /** called to prepare the update of the output */
  virtual void InternalPreUpdate();
  /** update the output data structure */
  virtual void InternalUpdate();
  /** do deformation under the constrain of constrain surface */
  //void DoDeformation(int type);
  /** do deformation under the constrain of constrain surface */
  //void DoDeformation2(int type);
  /** do deformation under the constrain of constrain surface */


  /** to update data and view after each deformation */
  void UpdateViewAfterDeformation();
  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();


  
  void CreateDefParamsDialog() ;  ///< Create deformation parameters dialog
  void DeleteDefParamsDialog() ;  ///< Delete deformation parameters dialog

  mafTransform *m_Transform; 

  //vtkPolyData *m_CenterlineModify;//sheathVTK
  /**----------- parameters -----------*/
  double m_StentRadius;
  /**----------- stent parameters-----------*/
  /** basic stent  */
  double m_Stent_Diameter;
  double m_Crown_Length;
  int m_Crown_Number;
  double m_Strut_Angle;
  double m_Strut_Thickness;
  int m_Id_Stent_Configuration;
  /**  stent link  */
  int m_Id_Link_Connection;
  double m_Link_Length;
  int m_Link_Alignment;
  int m_Link_orientation;  
  /**----------- center line and constrain surface-----------*/
  vtkPolyData  *m_Centerline; 
  vtkPolyData  *m_ConstrainSurface;
  mafString m_CenterLineName;
  mafString m_ConstrainSurfaceName;
  vtkPolyData *m_TestVesselPolyData;

private:
  double m_StrutLength;
  vtkPolyData *m_StentPolyLine;  
  vector<double> m_StentCenterLineSerial;
  vector<vector<double>> m_StentCenterLine;
  vector<vector<double>>::const_iterator m_CenterLineStart; //could be remove
  vector<vector<double>>::const_iterator m_CenterLineEnd;  //could be remove
  vtkPolyData* CreateAConstrainSurface();
  void moveCatheter(mafTimeStamp currentIter);
  void createCatheter(vtkPolyData  *centerLine);
  void createTestVesselPolydata(vtkPolyData  *centerLine);
  void expandStent(int numberOfCycle);

  void SetDefParamsToDefaults() ; ///< Restore deformation parameters to defaults
  void SetDefParamsToSaved() ; ///< Restore deformation parameters to saved values
  void SaveDefParams() ; ///< Save current deformation parameters (in case of undo or cancel)

  /** three output in append polydata*/
  vtkPolyData  *m_PolyData;
  vtkPolyData  *m_CatheterPolyData;

  vtkAppendPolyData *m_AppendPolyData;
  /** the output of this vme */
  vtkPolyData *m_AppendPolys;

  vtkPolyData *m_SheathVTK;
  /** used to create stent */
  //vtkMEDStentModelSource m_StentSource;

  SimplexMeshType::Pointer m_SimplexMesh;
  /** to check if deformation in progress */
  int m_DeformFlag ;

  /*from get Strut Length from stentModelSource*/
  double GetStrutLength(){return m_StrutLength;};
  /*if show catheter*/
  int m_ShowCatheter;

  vtkCellArray *m_StrutArray;
  vtkCellArray *m_LinkArray;
  vector<int>::const_iterator m_centerLocationIdx;
  vector<int> m_centerLocation;
  /*-------for store vme------*/
  vector<int> m_VmeLinkedList; //a list of VME ID , centerline first then surface

  void SetCenterLocationIdx(vector<int>::const_iterator centerLocationIndex);//{m_centerLocationIdx = centerLocationIndex;}
  void SetCenterLocationIdxRef(vector<int> const&ve);
  int m_CenterLineSetFlag,m_ConstrainSurfaceSetFlag; 
  int m_ComputedCrownNumber;
  /*------------pre compute a container to keep all the points of deformation iterator steps  ----------*/
  void PreComputeStentPointsBySteps(int step);
  vector<vtkPoints*> m_ItPointsContainer;//keep points for every deform iterator
  //-------for test
  int m_numberOfCycle;

  typedef itk::vtkMEDDeformableSimplexMeshFilter<SimplexMeshType,SimplexMeshType> DeformFilterType;
  DeformFilterType::Pointer m_DeformFilter;
  
  // Setting deformation parameters
  double m_Alpha ; // internal force weight
  double m_Beta ;  // external for weight
  double m_Gamma ; // regularity
  double m_Rigidity ; // smoothness
  double m_Damping ; // damping
  double m_Epsilon ; // distance scale for calculating force attracting stent to vessel 
  int m_IterationsPerStep ; // no. of iterations per step (ie per execution of filter)
  int m_NumberOfSteps ; // total no. of iterations
  double m_Alpha_Saved ;
  double m_Beta_Saved ;
  double m_Gamma_Saved ;
  double m_Rigidity_Saved ;
  double m_Damping_Saved ;
  double m_Epsilon_Saved ;
  int m_IterationsPerStep_Saved ;
  int m_NumberOfSteps_Saved ; 
  const double m_Alpha_Default ;
  const double m_Beta_Default ;
  const double m_Gamma_Default ;
  const double m_Rigidity_Default ;
  const double m_Damping_Default ;
  const double m_Epsilon_Default ;
  const int m_IterationsPerStep_Default ;
  const int m_NumberOfSteps_Default ; 

  mafGUIDialog *m_DefParamsDlg ; // dialog for deformation params
};
#endif
