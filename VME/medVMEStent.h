/*=========================================================================
Program:   MAF2Medical
Module:    $RCSfile: medVMEStent.h,v $
Language:  C++
Date:      $Date: 2012-10-23 10:15:31 $
Version:   $Revision: 1.1.2.7 $
Authors:   Hui Wei
==========================================================================
Copyright (c) 2013
University of Bedfordshire, UK
=========================================================================*/

#ifndef __medVMEStent_h 
#define __medVMEStent_h

#include "medVMEDefines.h"  //very important for MED_VME_EXPORT

#include "mafNode.h"
#include "mafVME.h"
#include "mafVMEGeneric.h"
#include "mafVMESurface.h"
#include "mafVMEPolyline.h"
#include "mafEvent.h"
#include "mmaMaterial.h"
#include "mafVMEOutputPolyline.h"
#include "vtkMEDStentModelSource.h"
#include "vtkMEDDeformableSimplexMeshFilter.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkIdList.h"



//--------typedefs----------
typedef itk::DefaultDynamicMeshTraits<double, 3, 3, double, double> MeshTraits;
typedef itk::SimplexMesh<double,3,MeshTraits>           SimplexMeshType;
//typedef SimplexMeshType::NeighborListType               NeighborsListType;
//typedef itk::SimplexMeshGeometry::IndexArray            IndexArray;
//typedef SimplexMeshType::PointType                      PointType;
typedef vtkMEDStentModelSource::Strut                         Strut;
typedef vtkMEDStentModelSource::StentConfigurationType        enumStCfgType;
typedef vtkMEDStentModelSource::LinkConnectionType            enumLinkConType;
typedef vtkMEDStentModelSource::LinkOrientationType           enumLinkOrtType;
typedef std::vector<Strut>::const_iterator    StrutIterator;
typedef itk::vtkMEDDeformableSimplexMeshFilter<SimplexMeshType,SimplexMeshType> DeformFilterType;




//-----------------------------------------------------------------------------
/// medVMEStent. \n
/// This class describes a stent based on some basic parameters. \n
/// The stent is associated with an "active vessel" surface and a center line. \n
/// The inputs are the stent parameters and the center line. \n
/// The output is the stent polydata.
/// This also contains a simplex mesh of the stent and a customised \n
/// deformation filter which can be used by an external op to deform the stent.
//
// Note that the stent position requires a vessel and a center line.
// Some applications set the polydata directly and some load it from
// the corresponding vme's.
//-----------------------------------------------------------------------------
class MED_VME_EXPORT medVMEStent : public mafVMEGeneric
{
public:
  /// Risk categories
  enum{
    LOW = 0,
    AVG,
    HIGH,
    VHIGH
  };

  mafTypeMacro(medVMEStent, mafVMEGeneric);

  medVMEStent(); ///< Constructor
  ~medVMEStent(); ///< Destructor

  int DeepCopy(mafNode *a) ; ///< Deep copy

  mafString GetVisualPipe() {return mafString("mafPipePolyline");}; ///< Get pipe name
  mafVMEOutputPolyline *GetPolylineOutput(); ///< return the right type of output 
  vtkPolyData *GetStentPolyData(); ///< Get stent polydata

  /// Set stent polydata. \n
  /// Normally this is generated internally, but this allows an external op \n
  /// to impose a temporary position on the output polydata.
  void SetStentPolyData(vtkPolyData* pd) ;

  vtkPolyData *GetSimplexPolyData(); ///< Get simplex polydata

  void OnEvent(mafEventBase *maf_event); ///< Event handler

  /// Initialize. \n
  /// This method allows an external op to initialize the vme, \n
  /// the vessel and the centerline.
  void Initialize() ;

  /// Set the vessel centerline and derive the stent centerline
  void SetVesselCenterLine(vtkPolyData *line);

  /// Set the vessel surface
  void SetVesselSurface(vtkPolyData *surface);

  /// Get the vessel vme
  mafVME* GetVesselVME() {return m_VesselVME ;}

  /// Get the centerline vme
  mafVME* GetCenterLineVME() {return m_CenterLineVME ;}

  /// Set the deployed polydata vme.
  void SetDeployedPolydataVME(mafNode* inputNode) ;

  /// Set the start position of the stent. \n
  /// The position is the id along the centerline.
  void SetStentStartPos(int startId) ;

  int GetStentStartPos() { return m_StentStartPosId; };

  void SetStentLink(const char *link_name, mafNode *ns);

  void SetStentCompanyName(wxString CompanyName) {m_CompanyName = CompanyName;}
  void SetStentModelName(wxString ModelName) {m_ModelName = ModelName;}
  void SetStentMaterial(wxString Material) {m_Material = Material;}
  void SetStentDeliverySystem(double delsys) {m_DeliverySystem = delsys;}
  void SetStentDiameter(double d) {m_Stent_Diameter = d;}
  void SetStentDBDiameter(double d) {m_Stent_DBDiameter = d;}
  void SetStentDLength(double length) {m_Stent_DBLength = length; } 
  void SetStentCrownLength(double crownL) {m_Crown_Length = crownL;}
  void SetStrutThickness(double strutThickness) {m_Strut_Thickness = strutThickness; }
  void SetStentConfiguration(int stentCfig) {m_Id_Stent_Configuration = stentCfig;}
  void SetLinkLength(double linkLength) {m_Link_Length = linkLength;}
  void SetLinkAlignment(int linkAlgn) {m_Link_Alignment = linkAlgn;}
  void SetLinkOrientation(int linkOrit) {m_Link_Orientation = linkOrit;}
  void SetLinkConnection(int linkConnection) {m_Id_Link_Connection = linkConnection;}
  void SetNumberOfCrowns(int crownNumber) {m_NumberOfCrowns = crownNumber; }
  void SetStrutsNumber(int strutsNumber){m_StrutPairsPerCrown = strutsNumber;}
  void SetLinkNumber(int linkNumber){m_Link_Number = linkNumber;}
  void SetStentType(int type){m_Stent_Type = type;}

  /// calculate strut angle, given diameter, crown length and struts-per-crown.
  void CalcStrutAngle() ; 

  /// calculate strut length, given strut angle and crown length.
  void CalcStrutLength() ; 

  wxString GetStentCompanyName() const {return m_CompanyName;};
  wxString GetStentModelName() const {return m_ModelName;};
  wxString GetStentMaterial() const {return m_Material;};
  double GetStentDeliverySystem() const {return m_DeliverySystem;};
  double GetStentRadius() const {return m_Stent_Diameter/2.0;}
  double GetStentDiameter() const {return m_Stent_Diameter;}
  double GetStentDBDiameter() const {return m_Stent_DBDiameter;}
  double GetStentDLength() const {return m_Stent_DBLength;}  
  double GetStentCrownLength() const {return m_Crown_Length;}
  double GetStrutThickness() const {return m_Strut_Thickness;}
  int GetStentConfiguration() const {return m_Id_Stent_Configuration;}
  double GetLinkLength() const {return m_Link_Length;}
  int GetLinkAlignment() const {return m_Link_Alignment;}
  int GetLinkOrientation() const {return m_Link_Orientation;}
  int GetLinkConnection() const {return m_Id_Link_Connection;}
  int GetNumberOfCrowns() const {return m_NumberOfCrowns ;}
  double GetStrutLength() const {return m_Strut_Length ;}
  double GetStrutAngle() const {return m_Strut_Angle ;}
  int GetStrutNumber() const {return m_StrutPairsPerCrown;}
  int GetLinkNumber() const {return m_Link_Number;}
  int GetStentType() const {return m_Stent_Type;}

  vtkPolyData* GetVesselCenterLine() {return m_CenterLine;} ///< get vessel centerline
  vtkPolyData* GetVesselCenterLineLong() {return m_CenterLineLong;} ///< get long extrapolated vessel centerline
  vtkPolyData* GetStentCenterLine() {return m_StentCenterLine;} ///< get stent centerline
  vtkPolyData* GetVesselSurface() {return m_VesselSurface;}; ///< get vessel surface

  /// Get the approximate length of the stent. \n
  /// This the no. of centerline vertices from end to end.
  int CalcStentLengthVerts() ;

  /// Get the current length of the stent in mm
  double CalcStentLengthMM() ;

  /// Measure the current diameter of the stent in mm
  double CalcStentDiameterMM() ;

  /// Get the simplex mesh
  SimplexMeshType::Pointer GetSimplexMesh() {return m_SimplexMesh ;}

  /// Get the deformation filter
  DeformFilterType::Pointer GetDeformationFilter() {return m_DeformFilter ;}

  /// Utility method to create extrapolated copy of a polyline. \n
  /// The output line is the input plus an extrapolated section.
  static void CreateExtrapolatedLine(vtkPolyData* lineIn, vtkPolyData* lineOut, double extrapFactor) ;

  /// Utility method to create truncated copy of a polyline. \n
  /// The output line is a subset of the input from id0 to id1 inclusive. \n
  /// If id1 > n-1, the line is copied to the end.
  static void CreateTruncatedLine(vtkPolyData* lineIn, vtkPolyData* lineOut, int id0, int id1) ;

  /// Do one step of the deformation filter
  void DoDeformationStep() ;

  /// Crimp the stent to a smaller diameter. \n
  /// The input params are for the expanded stent, \n
  /// so this should be the last step when the stent is created. \n
  /// This changes the diameter, strut angle and crown length, \n
  /// keeping the strut length const.
  void CrimpStent(double crimpedDiameter) ;


  /// Calculate arc lengths of points along a center line.
  static void CalcArcLengthsOfPoints(vtkPolyData *pd, double* arcLengths) ;

  /// Calc arc length along center line given id position (id, lambda) \n
  /// where the position is a fraction lambda along the next segment id to id+1. \n
  /// This assumes that the pd is a monotonic curve of points.
  static double CalcArclengthFromIdPosition(vtkPolyData *pd, int id, double lambda) ;

  /// Calc id position (id, lambda) along center line given arc length \n
  /// where the position is a fraction lambda along the next segment id to id+1. \n
  /// This assumes that the pd is a monotonic curve of points.
  static void CalcIdPositionFromArcLength(vtkPolyData *pd, double arclen, int& id, double& lambda) ;

  /// Calc coord position along center line given id position (id, lambda) \n
  /// where the position is a fraction lambda along the next segment id to id+1. \n
  /// This assumes that the pd is a monotonic curve of points.
  static void CalcCoordsFromIdPosition(vtkPolyData *pd, int id, double lambda, double* x) ;

  /// Find nearest point id on center line.
  static void FindNearestPointOnCenterLine(double* p0, vtkPolyData *pd, int& id, double& distSq) ;

  /// Find nearest point (id, lambda) on center line. \n
  static void FindNearestPointOnCenterLine(double* p0, vtkPolyData *pd, int& id, double& lambda, double& distSq) ;

  /// Set the max risk (LOW, AVG, HIGH, VHIGH)
  void SetMaxRisk(int risk) {m_MaxRisk = risk ;  this->Modified() ;}

  /// Get the max risk (LOW, AVG, HIGH, VHIGH)
  int GetMaxRisk() const {return m_MaxRisk ;}

  /** Set the Fatigue Bending */
  void SetFatigueBending(const double fatiguebending) { m_FatigueBending = fatiguebending; };

  /** Get the Fatigue Bending */
  double GetFatigueBending() const {return m_FatigueBending; };

  /** Set the tube initial stretching */
  void SetTubeInitialStretching(const std::vector<double>& tubeinitialstretching) { m_TubeInitialStretching = tubeinitialstretching; };

  /** Get the tube initial stretching */
  const std::vector<double>& GetTubeInitialStretching() const { return m_TubeInitialStretching; };

  /** Get the tube initial stretching in the id-th stent unit */
  const double GetTubeInitialStretchingPerUnit(unsigned stentunitid) const { return m_TubeInitialStretching.at(stentunitid); };

  /** Set the tube final stretching */
  void SetTubeFinalStretching(const std::vector<double>& tubefinalstretching) { m_TubeFinalStretching = tubefinalstretching; };
  
  /** Get the tube final stretching in the id-th stent unit */
  const double GetTubeFinalStretchingPerUnit(unsigned stentunitid) const { return m_TubeFinalStretching.at(stentunitid); };
  
  /** Get the tube final stretching */
  const std::vector<double>& GetTubeFinalStretching() const { return m_TubeFinalStretching; };

  /** Set the tube young modulus */
  void SetTubeYoungModulus(const std::vector<double>& tubeyoungmodulus) { m_TubeYoungModulus = tubeyoungmodulus; };

  /** Get the tube young modulus in the id-th stent unit */
  const double GetTubeYoungModulusPerUnit(unsigned stentunitid) const { return m_TubeYoungModulus.at(stentunitid); };

  /** Get the tube young modulus */
  const std::vector<double>& GetTubeYoungModulus() const { return m_TubeYoungModulus; };

  /** Set the tube diameter */
  void SetTubeDiameter(const std::vector<double>& tubediameter) { m_TubeDiameter = tubediameter; };

  /** Get the tube diameter in the id-th stent unit */
  const double GetTubeDiameterPerUnit(unsigned stentunitid) const { return m_TubeDiameter.at(stentunitid); };

  /** Get the tube diameter */
  const std::vector<double>& GetTubeDiameter() const { return m_TubeDiameter; };

  /// Get number of units in stent. \n
  /// This includes partial units at the end, if the no. of crowns is odd.
  int GetNumberOfUnits() ;

  /// Get length of unit
  double GetLengthOfUnit() ;

private:
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
    ID_LAST
  };

  /// Create the VME GUI.
  mafGUI *CreateGui();

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  /// Find or select the associated center line vme. \n
  /// Adds tag if not already present. \n
  /// Does nothing and returns true if vme already found. \n
  /// Returns NULL if failed.
  mafNode* FindOrSelectCenterLineVME(mafNode* inputNode) ;

  /// Find or select the associated vessel surface vme. \n
  /// Adds tag if not already present. \n
  /// Does nothing and returns true if vme already found. \n
  /// Returns NULL if failed.
  mafNode* FindOrSelectVesselVME(mafNode* inputNode) ;

  /// Find the tagged center line vme. \n
  /// Returns NULL if failed.
  mafNode* FindTaggedCenterLineVME(mafNode* inputNode) ;

  /// Find the tagged vessel surface vme. \n
  /// Returns NULL if failed.
  mafNode* FindTaggedVesselVME(mafNode* inputNode) ;

  void SetVesselCenterLine(mafNode *node);
  void SetVesselSurface(mafNode *node);

  /// update stent after a change in the parameters.
  void InternalUpdate();

  /// Update stent polydata from simplex.
  //void UpdateStentPolydataFromSimplex_old() ;

  /// Update stent polydata from simplex.
  void UpdateStentPolydataFromSimplex() ;

  /// Update stent polydata from simplex - Abbott stent
  void UpdateStentPolydataFromSimplex_Abbott() ;

  /// Update stent polydata from simplex - simple stent
  void UpdateStentPolydataFromSimplex_Simple() ;

  /// Copy simplex directly to polydata. \n
  /// Replacing UpdateStentPolydataFromSimplex() everywhere with this version \n
  /// will show the simplex instead of the derived stent polydata.
  void UpdateStentPolydataFromSimplex_ViewAsSimplex() ;

  /// Calculate extra mid-points on struts of Abbott stent polydata. \n
  /// Input is a pair of adjacent struts: strutEndPts[0,1] and strutEndPts[2,3]. \n
  /// Output is four corresponding midPts, two per strut. \n
  /// Additional output is two strutLinkPts, which form the squared end of some of the Abbott struts.
  void CalculateMidPointsFromPairOfStruts(const double strutEndPts[4][3], double midPts[4][3], bool isLinkPoint, double strutLinkPts[2][3]) const ;

  /// Create long vessel centerline
  void CreateLongVesselCenterLine() ;

  /// Create stent centerline from long vessel centerline and start pos
  void CreateStentCenterLine() ;

  /// Get list of valid point id's, \n
  /// ie points which are members of cells. \n
  /// Needed because the stent contains unused and undefined points.
  void GetValidPointIds(vtkPolyData* pd, vtkIdList* ids) const ;

  /// Get the highest index of the valid points, \n
  /// ie points which are members of cells. \n
  /// Needed because the stent contains unused and undefined points.
  int GetHighestValidPointIndex(vtkPolyData *pd) const ;


  /// Partially initialize the deformation filter. \n
  /// This copies information from the stent model to the filter.
  void PartialInitDefFilterFromStentModel() ;

  /// Has the simplex mesh been modified since the last call
  bool IsSimplexMeshModified() ;

  /// Search parent and siblings for node with given id. \n
  /// Returns NULL if failed.
  mafNode* FindNodeWithId(mafID id) ;


  // basic stent parameters */
  wxString m_CompanyName;
  wxString m_ModelName;
  wxString m_Material;
  double m_DeliverySystem;

  int m_Stent_Type; // 1 Abbott 2 Bard, else Medtronic

  double m_Stent_Diameter;
  double m_Stent_DBDiameter;
  double m_Stent_DBLength;
  double m_Crown_Length;
  double m_Strut_Length; 	

  double m_Strut_Angle; ///< angle in radians
  double m_Link_Length;
  int m_StrutPairsPerCrown;
  int m_NumberOfCrowns;
  int m_Link_Number;
  int m_Id_Link_Connection;
  int m_Id_Stent_Configuration;
  int m_Link_Orientation;
  int m_Link_Alignment;
  double m_Strut_Thickness;
  int m_ComputedCrownNumber;
  bool m_StentParamsModified ;



  // vessel  
  mafVME* m_VesselVME ;
  int m_VesselNodeID ;        // for store and restore
  vtkPolyData *m_VesselSurface;
  bool m_VesselSurfaceDefined; 
  bool m_VesselVMEDefined ;

  // vessel centerline
  mafVME* m_CenterLineVME ;
  int m_CenterLineNodeID ;
  vtkPolyData *m_CenterLine ;
  vtkPolyData *m_CenterLineLong ;
  bool m_CenterLineDefined ;
  bool m_CenterLineVMEDefined ;

  // stent
  vtkMEDStentModelSource *m_StentSource;
  vtkPolyData *m_StentPolyData;
  vtkPolyData *m_StentCenterLine ;
  int m_StentStartPosId ; // init position - id along vessel center line
  bool m_StentCenterLineModified ;
  int m_StentLength ;
  bool m_StentLengthModified ; // need to recalculate length of stent

  // simplex
  vtkPolyData *m_SimplexPolyData; // polydata visualisation of simplex mesh

  // Deformation filter
  SimplexMeshType::Pointer m_SimplexMesh;
  bool m_SimplexMeshModified ;
  DeformFilterType::Pointer m_DeformFilter;


  // Deployed stent polydata vme.
  // This is needed for storing and restoring the deployed position.
  mafVMEPolyline *m_DeployedPolydataVME ; 
  int m_DeployedPolydataNodeID ;

  // Possible states regarding deployed polydata
  enum{
    DEPLOYED_PD_NONE,       // stent has no deployed polydata
    DEPLOYED_PD_NOT_LOADED, // stent has associated polydata but is not loaded
    DEPLOYED_PD_OK          // deployed polydata has been loaded.
  };
  int m_DeployedPolydataStatus ;

  // Max risk: LOW, AVG etc.  
  // Stored here where risk op can save it and reporter op can access it.
  int m_MaxRisk ;

  //Fatigue Input Parameters
  double m_FatigueBending;
  std::vector<double> m_TubeInitialStretching;
  std::vector<double> m_TubeFinalStretching;
  std::vector<double> m_TubeYoungModulus;
  std::vector<double> m_TubeDiameter;

};

#endif
