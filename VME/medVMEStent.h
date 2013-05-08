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

#include "mafNode.h"
#include "mafVME.h"
#include "mafEvent.h"
#include "mafGuiDialog.h"
#include "mafGUIFloatSlider.h"
#include "mmaMaterial.h"
#include "mafVMEOutputSurface.h"
#include "mafVMEOutputPolyline.h"
#include "mafRWI.h"

#include "vtkMEDStentModelSource.h"
#include "vtkMEDDeformableSimplexMeshFilter.h"
#include "medVMEStentDeploymentVisualPipe.h"

#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkTubeFilter.h"
#include "vtkTriangleFilter.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"

#include <vector>




//--------typedefs----------
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


//-----------------------------------------------------------------------------
/// medVMEStent. \n
/// This class describes a stent based on some basic parameters. \n
/// The stent is associated with an "active vessel" surface a, center line \n
/// and a catheter sheath.  Deformation consists of withdrawing the sheath \n
/// and allowing the stent to expand onto the inner surface of the vessel. \n
/// The deformation is not visualised live - the stent positions are recorded \n
/// during the deformation, and an animated visualisation is reconstructed \n
/// afterwards, controlled by a slider.
//
// The stent deformation is performed by DoDeformation3() and 
// PreComputerStentPointsBySteps(), using vtkMEDDeformableSimplexMeshFilter.
// An ITK simplex mesh acts as a control mesh.
//-----------------------------------------------------------------------------
class MED_VME_EXPORT medVMEStent : public mafVME
{
public:

  mafTypeMacro(medVMEStent, mafVME);
  /* constructor */
  medVMEStent();
  /* destructor */
  virtual ~medVMEStent();

  /// Set the time for this VME. \n
  /// It also updates the VTK representation. \n
  /// Overrides base class method, and invoked by dialog slider.
  //void SetTimeStamp(mafTimeStamp t);

  /************************************************************************/
  /* The mesh deformation is constrainted by internal forces. The internal force can be scaled via SetAlpha (typical values are 0.01 < alpha < 0.3). 
  * The external force is derived from the image one wants to delineate. Therefore an image of type GradientImageType needs to be set by calling SetGradientImage(...). The external forces are scaled via SetBeta (typical values are 0.01 < beta < 1). One still needs to play around with these values.

  * To control the smoothness of the mesh a rigidity parameter can be adjusted. Low values (1 or 0) allow areas with high curvature. Higher values (around 7 or 8) will make the mesh smoother.

  * By setting the gamma parameter the regularity of the mesh is controlled. Low values (< 0.03) produce more regular mesh. Higher values ( 0.3 < gamma < 0.2) will allow to move the vertices to regions of higher curvature.

  *This approach for segmentation follows that of Delingette et al. (1997).                                                                     */
  /************************************************************************/

  /// Deform the stent
  void DoDeformation3(int type);

  void DisplayStentExpand( int steps );

  /// Set stent points to current and update
  void ResetStentPoints( vtkPoints* currentPoints );

  /// Display stent and catheter for given step
  void DisplayStentExpandByStep(int step);

  /// Add or remove catheter display
  void ToggleDisplayCatheter();


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

  // vtkPolyData *GetPolyData(){InternalUpdate();return m_StentPolyData;};
  /** the append polydata */
  vtkPolyData *GetPolyData();//{ InternalUpdate();return  m_StentPolyData;};// m_AppendPolyData->GetOutput(); };

  /** Get the vtkPolyData generated from the model */
  //vtkPolyData  *GetVtkPolyData(){InternalUpdate();return m_StentPolyData;};

  /**
  Return the list of timestamps for this VME. Timestamps list is 
  obtained merging timestamps for matrices and VME items*/
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

  void SetAndKeepConstraintSurface( mafNode *node );

  void SetAndKeepCenterLine( mafNode *node );

  //void SetAndKeepConstraintSurface( mafVME *vme );

  //void SetAndKeepCenterLine( mafVME *vme );

  /**
  Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
  set or get the Pose for a specified time. When setting, if the time does not exist
  the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
  interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const mafMatrix &mat);

  /**
  select center line or constraint surface 
  */
  void SetStentLink(const char *link_name, mafNode *ns);
  //------set properties-----------
  void SetCenterLine(vtkPolyData *line);
  void SetConstraintSurface(vtkPolyData *surface);

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
  vtkPolyData* GetCenterLine(){return m_VesselCenterLine;};
  vtkPolyData* GetConstraintSurface(){return m_ConstraintSurface;};

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
    CHANGED_STENT_PARAM,
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
    ID_CONSTRAINT_SURFACE,
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
    ID_DEF_PARAMS_CATHETER_SPEED,
    ID_DEF_PARAMS_PAUSE_CHKBOX,
    ID_DEF_PARAMS_PAUSE_POSITION,
    ID_DEF_PARAMS_PAUSE_DURATION,
    DISPLAY_CATHETER,
    DEPLOYMENT_MODE_POSITION,
    DEPLOYMENT_MODE_DEFORM,
    ID_DEPLOY_CTRL_SLIDER,
    ID_DEPLOY_CTRL_TXT,
    ID_DEPLOY_CTRL_DEC1,
    ID_DEPLOY_CTRL_INC1,
    ID_DEPLOY_SHOW_VESSEL_CHKBOX,
    ID_DEPLOY_SHOW_CENTERLINE_CHKBOX,
    ID_DEPLOY_SHOW_CATHETER_CHKBOX,
    ID_DEPLOY_SHOW_STENT_CHKBOX,
    ID_DEPLOY_CTRL_OK,
    ID_DEPLOY_CTRL_CANCEL,
    ID_LAST
  };

  /** store attributes and matrix*/
  virtual int InternalStore(mafStorageElement *parent);
  /** restore attributes and matrix*/
  virtual int InternalRestore(mafStorageElement *node);

  /** called to prepare the update of the output */
  virtual void InternalPreUpdate();

  /// Create polydata stent from current parameters and update
  virtual void InternalUpdate();

  /** do deformation under the constraint of constraint surface */
  //void DoDeformation(int type);
  /** do deformation under the constraint of constraint surface */
  //void DoDeformation2(int type);
  /** do deformation under the constraint of constraint surface */


  /** to update data and view after each deformation */
  void UpdateViewAfterDeformation();

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();

  void CreateDefParamsDialog() ; ///< Create deformation parameters dialog
  void DeleteDefParamsDialog() ; ///< Delete deformation parameters dialog

  void CreateDeployCtrlDialog() ; ///< Create deployment control dialog
  void DeleteDeployCtrlDialog() ; ///< Delete deployment control dialog
  void UpdateDeploymentSliderRange() ; ///< Update range of slider
  void ClampSliderPos() ; ///< clamp position to range of slider

private:
  void CreateCatheter(vtkPolyData  *centerLine); ///< Create the catheter
  void MoveCatheter(int step); ///< Move the catheter

  void createTestVesselPolydata(vtkPolyData  *centerLine);
  void expandStent(int numberOfCycle);

  void SetDefParamsToDefaults() ; ///< Restore deformation parameters to defaults
  void SetDefParamsToSaved() ; ///< Restore deformation parameters to saved values
  void SaveDefParams() ; ///< Save current deformation parameters (in case of undo or cancel)

  /*from get Strut Length from stentModelSource*/
  double GetStrutLength(){return m_StrutLength;};

  void SetCenterLocationIdx(vector<int>::const_iterator centerLocationIndex);//{m_centerLocationIdx = centerLocationIndex;}
  void SetCenterLocationIdxRef(vector<int> const&ve);

  /// Run filter for no. of steps. \n
  /// Store stent position at each step for later display.
  void PreComputeStentPointsBySteps(int steps);

  /// Find the tagged center line vme. \n
  /// Returns NULL if failed.
  mafNode* FindTaggedCenterLineVME(mafNode* inputNode) ;

  /// Find the tagged constraintt surface vme. \n
  /// Returns NULL if failed.
  mafNode* FindTaggedVesselVME(mafNode* inputNode) ;

  mafTransform *m_Transform; // Nigel: do we need this?


  // basic stent parameters */
  double m_Stent_Radius;
  double m_Stent_Diameter;
  double m_Crown_Length;
  int m_Crown_Number;
  double m_Strut_Angle;
  double m_Strut_Thickness;
  int m_Id_Stent_Configuration;
  double m_StrutLength; // one length for all struts - may not work for all designs
  int m_Id_Link_Connection;
  double m_Link_Length;
  int m_Link_Alignment;
  int m_Link_orientation;  

  // vessel
  vtkPolyData *m_VesselCenterLine ;
  vtkPolyData *m_ConstraintSurface;
  vtkPolyData *m_TestVesselPolyData;
  mafString m_VesselCenterLineName;
  mafString m_ConstraintSurfaceName;
 
  // stent
  vtkPolyData *m_StentPolyData;
  vtkPolyData *m_StentCenterLine ;
  vector<double> m_StentCenterLineSerial;
  int m_StentStartPosId ; // init position - id along vessel center line

  // collected visual output
  vtkAppendPolyData *m_AppendPolyData; // collects components into one polydata for view
  vtkPolyData *m_AppendPolys; // pointer to append output

  /** used to create stent */
  //vtkMEDStentModelSource m_StentSource;

  SimplexMeshType::Pointer m_SimplexMesh;
  /** to check if deformation in progress */
  int m_DeformFlag ;

  /*if show catheter*/
  int m_ShowCatheter;

  vtkCellArray *m_StrutArray;
  vtkCellArray *m_LinkArray;
  vector<int>::const_iterator m_centerLocationIdx;
  vector<int> m_centerLocation;

  /*-------for store vme------*/
  vector<int> m_VmeIdList; //a list of VME ID , centerline first then surface

  int m_VesselCenterLineSetFlag ;
  int m_ConstraintSurfaceSetFlag; 
  int m_ComputedCrownNumber;

  vector<vtkPoints*> m_ItPointsContainer; // store points at each step 


  //-------for test
  int m_numberOfCycle;



  //-------------------------------------------------
  // Catheter
  //-------------------------------------------------
  vtkPolyData* m_CatheterCenterLine ; // center line about which catheter is generated
  vtkTubeFilter* m_CatheterTubeFilter ;
  vtkPolyData* m_CatheterPolyData ;  // just ptr to m_CatheterTubeFilter->GetOutput()


  //-------------------------------------------------
  // Deformation filter
  //-------------------------------------------------
  typedef itk::vtkMEDDeformableSimplexMeshFilter<SimplexMeshType,SimplexMeshType> DeformFilterType;
  DeformFilterType::Pointer m_DeformFilter;


  //-------------------------------------------------
  // Dialog and variables for deformation parameters
  //-------------------------------------------------
  mafGUIDialog *m_DefParamsDlg ; // dialog for deformation params
  wxCheckBox *m_PauseChkBox ;    // check box for catheter pause

  double m_Alpha ; // internal force weight
  double m_Beta ;  // external for weight
  double m_Gamma ; // regularity
  double m_Rigidity ; // smoothness
  double m_Damping ; // damping
  double m_Epsilon ; // distance scale for calculating force attracting stent to vessel 
  int m_IterationsPerStep ; // no. of iterations per step (ie per execution of filter)
  int m_NumberOfSteps ; // total no. of iterations
  double m_CatheterSpeed ;
  int m_CatheterPauseOn ;
  double m_CatheterPausePosition ;
  double m_CatheterPauseDuration ;
  double m_Alpha_Saved ;
  double m_Beta_Saved ;
  double m_Gamma_Saved ;
  double m_Rigidity_Saved ;
  double m_Damping_Saved ;
  double m_Epsilon_Saved ;
  int m_IterationsPerStep_Saved ;
  int m_NumberOfSteps_Saved ; 
  double m_CatheterSpeed_Saved ;
  int m_CatheterPauseOn_Saved ;
  double m_CatheterPausePosition_Saved ;
  double m_CatheterPauseDuration_Saved ;
  const double m_Alpha_Default ;
  const double m_Beta_Default ;
  const double m_Gamma_Default ;
  const double m_Rigidity_Default ;
  const double m_Damping_Default ;
  const double m_Epsilon_Default ;
  const int m_IterationsPerStep_Default ;
  const int m_NumberOfSteps_Default ; 
  const double m_CatheterSpeed_Default ;
  const int m_CatheterPauseOn_Default ;
  const double m_CatheterPausePosition_Default ;
  const double m_CatheterPauseDuration_Default ;


  //-------------------------------------------------
  // Dialog and variables for deployment control
  //-------------------------------------------------
  mafGUIDialog *m_DeployCtrlDlg ; // dialog for deployment control
  mafRWI* m_DeployRwi ; // render window
  mafGUIFloatSlider *m_DeploymentSlider ; // slider to control deployment (position and deformation)
  double m_SliderPos ;
  int m_DeploymentMode ;  // slider used for positioning or deforming
  wxCheckBox *m_ShowVesselChkBox ;      
  wxCheckBox *m_ShowCenterLineChkBox ;  
  wxCheckBox *m_ShowCatheterChkBox ;    
  wxCheckBox *m_ShowStentChkBox ;
  int m_DeployDlg_ShowVessel ;
  int m_DeployDlg_ShowCenterLine ;
  int m_DeployDlg_ShowCatheter ;
  int m_DeployDlg_ShowStent ;

  medVMEStentDeploymentVisualPipe* m_DeploymentVisualPipe ;

};
#endif
