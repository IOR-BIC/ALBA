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
#include "mafEvent.h"
#include "mmaMaterial.h"
#include "mafVMEOutputPolyline.h"
#include "vtkMEDStentModelSource.h"
#include "vtkMEDDeformableSimplexMeshFilter.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"



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
//-----------------------------------------------------------------------------
class MED_VME_EXPORT medVMEStent : public mafVMEGeneric
{
public:
  mafTypeMacro(medVMEStent, mafVMEGeneric);

  medVMEStent(); ///< Constructor
  ~medVMEStent(); ///< Destructor

  int DeepCopy(mafNode *a) ; ///< Deep copy

  mafString GetVisualPipe() {return mafString("mafPipePolyline");}; ///< Get pipe name
  mafVMEOutputPolyline *GetPolylineOutput(); ///< return the right type of output 
  vtkPolyData *GetStentPolyData(); ///< Get stent polydata
  vtkPolyData *GetSimplexPolyData(); ///< Get simplex polydata

  void OnEvent(mafEventBase *maf_event); ///< Event handler

  /// Initialize. \n
  /// This method allows an external op to initialize the vme, \n
  /// the vessel and the centerline.
  void Initialize() ;

  /// Get the vessel vme
  mafVME* GetVesselVME() {return m_VesselVME ;}

  /// Get the centerline vme
  mafVME* GetCenterLineVME() {return m_CenterLineVME ;}

  /// Set the start position of the stent. \n
  /// The position is the id along the centerline.
  void SetStentStartPos(int startId) ;

  void SetStentLink(const char *link_name, mafNode *ns);

  /// Set the vessel centerline and derive the stent centerline
  void SetVesselCenterLine(vtkPolyData *line);

  /// Set the vessel surface
  void SetVesselSurface(vtkPolyData *surface);

  void SetStentCompanyName(wxString *CompanyName) {m_CompanyName = CompanyName;}
  void SetStentModelName(wxString *ModelName) {m_ModelName = ModelName;}
  void SetStentMaterial(wxString *Material) {m_Material = Material;}
  void SetStentDeliverySystem(double delsys) {m_DeliverySystem = delsys;}
  void SetStentDiameter(double d) {m_Stent_Diameter = d;}
  void SetStentDLength(double length) {m_Stent_Length = length; } 
  void SetStentCrownLength(double crownL) {m_Crown_Length = crownL;}
  void SetStrutThickness(double strutThickness) {m_Strut_Thickness = strutThickness; }
  void SetStentConfiguration(int stentCfig) {m_Id_Stent_Configuration = stentCfig;}
  void SetLinkLength(double linkLength) {m_Link_Length = linkLength;}
  void SetLinkAlignment(int linkAlgn) {m_Link_Alignment = linkAlgn;}
  void SetLinkOrientation(int linkOrit) {m_Link_orientation = linkOrit;}
  void SetLinkConnection(int linkConnection) {m_Id_Link_Connection = linkConnection;}
  void SetStentCrownNumber(int crownNumber) {m_Crown_Number = crownNumber; }
  void SetStrutsNumber(int strutsNumber){m_Struts_Number = strutsNumber;}
  void SetLinkNumber(int linkNumber){m_Link_Number = linkNumber;}
  
  /// calculate strut angle, given diameter, crown length and struts-per-crown.
  void CalcStrutAngle() ; 
  
  /// calculate strut length, given strut angle and crown length.
  void CalcStrutLength() ; 

  wxString* GetStentCompanyName() const {return m_CompanyName;};
  wxString* GetStentModelName() const {return m_ModelName;};
  wxString* GetStentMaterial() const {return m_Material;};
  double GetStentDeliverySystem() const {return m_DeliverySystem;};
  double GetStentRadius() const {return m_Stent_Diameter/2.0;}
  double GetStentDiameter() const {return m_Stent_Diameter;}
  double GetStentDLength() const {return m_Stent_Length;}  
  double GetStentCrownLength() const {return m_Crown_Length;}
  double GetStrutThickness() const {return m_Strut_Thickness;}
  int GetStentConfiguration() const {return m_Id_Stent_Configuration;}
  double GetLinkLength() const {return m_Link_Length;}
  int GetLinkAlignment() const {return m_Link_Alignment;}
  int GetLinkOrientation() const {return m_Link_orientation;}
  int GetLinkConnection() const {return m_Id_Link_Connection;}
  int GetStentCrownNumber() const {return m_Crown_Number ;}
  double GetStrutLength() const {return m_Strut_Length ;}
  double GetStrutAngle() const {return m_Strut_Angle ;}

  vtkPolyData* GetVesselCenterLine() {return m_CenterLine;} ///< get vessel centerline
  vtkPolyData* GetVesselCenterLineLong() {return m_CenterLineLong;} ///< get long extrapolated vessel centerline
  vtkPolyData* GetStentCenterLine() {return m_StentCenterLine;} ///< get stent centerline
  vtkPolyData* GetVesselSurface() {return m_VesselSurface;}; ///< get vessel surface

  /// Get the approximate length of the stent. \n
  /// This the no. of centerline vertices from end to end.
  int GetStentLength() ;

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

  /// Find or select the associated center line vme. \n
  /// Adds tag if not already present. \n
  /// Does nothing and returns true if vme already found. \n
  /// Returns false if failed.
  mafNode* FindOrSelectCenterLineVME(mafNode* inputNode) ;

  /// Find or select the associated vessel surface vme. \n
  /// Adds tag if not already present. \n
  /// Does nothing and returns true if vme already found. \n
  /// Returns false if failed.
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

  /// Update stent polydata from simplex
  void UpdateStentPolydataFromSimplex() ;

  /// Copy simplex directly to polydata. \n
  /// Replacing UpdateStentPolydataFromSimplex() everywhere with this version \n
  /// will show the simplex instead of the derived stent polydata.
  void UpdateStentPolydataFromSimplex_ViewAsSimplex() ;

  /// Calculate extra mid-points on struts of Abbott stent polydata. \n
  /// Input is a pair of adjacent struts - output is four midpoints.
  void CalculateMidPointsFromPairOfStruts(const double strutEndPts[4][3], double midPts[4][3]) const ;

  /// Create long vessel centerline
  void CreateLongVesselCenterLine() ;

  /// Create stent centerline from long vessel centerline and start pos
  void CreateStentCenterLine() ;

  /// Get the highest index of the valid points, \n
  /// ie points which are members of cells. \n
  /// Needed because the stent contains unused and undefined points.
  int GetHighestValidPointIndex(vtkPolyData *pd) const ;

  /// Partially initialize the deformation filter. \n
  /// This copies information from the stent model to the filter.
  void PartialInitDefFilterFromStentModel() ;

  /// Has the simplex mesh been modified since the last call
  bool IsSimplexMeshModified() ;

  // basic stent parameters */
  wxString *m_CompanyName;
  wxString *m_ModelName;
  wxString *m_Material;
  double m_DeliverySystem;
  
  double m_Stent_Diameter;
  double m_Crown_Length;
  int m_Crown_Number;
  int m_Struts_Number;
  double m_Strut_Angle; ///< angle in radians
  double m_Strut_Thickness;
  double m_Strut_Length; 
  double m_Stent_Length; 
  double m_Link_Length;
  int m_Link_Alignment;
  int m_Link_orientation;
  int m_Link_Number;
  int m_Id_Link_Connection;
  int m_Id_Stent_Configuration;
  int m_Id_Stent_Type;
  int m_ComputedCrownNumber;
  bool m_StentParamsModified ;

  // vessel  
  mafVME* m_VesselVME ;
  vtkPolyData *m_VesselSurface;
  int m_VesselSurfaceSetFlag; 
  
  // vessel centerline
  mafVME* m_CenterLineVME ;
  vtkPolyData *m_CenterLine ;
  vtkPolyData *m_CenterLineLong ;
  int m_CenterLineSetFlag ;
 
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
};

#endif
