/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpComputeInertialTensor.h,v $
  Language:  C++
  Date:      $Date: 2012-04-06 09:09:53 $
  Version:   $Revision: 1.1.2.7 $
  Authors:   Simone Brazzale , Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpComputeInertialTensor_H__
#define __medOpComputeInertialTensor_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medOperationsDefines.h"
#include "mafOp.h"
#include "mafVME.h"
#include "mafTagItem.h"
#include "vtkCell.h"
#include "vtkPolyData.h"
#include "vnl/vnl_vector.h"

using namespace std;

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMESurface;
class mafGUI;
class vtkCell;

/** 
  class name: medOpComputeInertialTensor

  Operation used to compute the inertial tensor of a surface or a group of surfaces.

  The input is a surface selected from the tree and a scalar that represents the density (assumed uniformly distributed over the volume) of the geometry.
  The outputs are the mass of the surface and the six independent components of the inertial tensor expressed in a specified reference coordinate system. 
  The outputs (i.e. the mass and the inertial tensor) are stored as attributes associated to the input geometry.
  In the group case the resulting mass and the inertial tensor are those of the whole group of geometries and, the computed attributes 
  (i.e. the overall mass and inertial tensor) are stored as attributes of the group.

  https://picasaweb.google.com/104563709532324716730/MedOpComputeInertialTensor?authuser=0&feat=directlink

  Density for each VME will be read from the "DENSITY" tag: if this tag is not found the Default density value will be used. 
	
*/
class MED_OPERATION_EXPORT medOpComputeInertialTensor: public mafOp
{
public:
  /** constructor */
  medOpComputeInertialTensor(const wxString &label = "ComputeInertialTensor");
  /** destructor */
  ~medOpComputeInertialTensor();

  /** RTTI macro*/
  mafTypeMacro(medOpComputeInertialTensor, mafOp);

  /** clone the object and retrieve a copy*/
  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

  /** Builds operation's interface. */
  void OpRun();

  /** Execute the operation. */
  void OpDo();

  /** Makes the undo for the operation. */
  void OpUndo();

  /** Precess events coming from other objects */
  virtual void OnEvent(mafEventBase *maf_event);

  /** Set the default density value used for computation */
  void SetDefaultDensity(double val);

  /** Return the default density value used for computation */
  double GetDefaultDensity();

  /** Calculate inertial tensor components from a surface and store them in the input vme. */
  int ComputeInertialTensor(mafNode* node, int current_node = 1, int n_of_nodes = 1);

  /** Calculate inertial tensor components from a group of surfaces and store them in the input vme. */
  int ComputeInertialTensorFromGroup();

   /** Add results to the vme in the attributes section. */
  void AddAttributes();

  /** Get the VME mass from the "SURFACE_MASS" tag if existent otherwise returns: */  enum {SURFACE_MASS_NOT_FOUND = -1};
  static double GetMass( mafNode* node);

protected: 

	/** Get the VME density from the "DENSITY" tag if existent otherwise returns: */  enum {DENSITY_NOT_FOUND = -1};
	double GetDensity( mafNode* node);
	
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  /** Create the dialog interface for the importer. */
  virtual void CreateGui();

  /** Get surface volume. */
  double GetSurfaceVolume(mafNode* node);

  /** Get surface area. */
  double GetSurfaceArea(mafNode* node);

  /** Get surface mass using volume. */
  double GetSurfaceMassFromVolume(mafNode* node);

  /** Get surface mass using area. */
  double GetSurfaceMassFromArea(mafNode* node);

  /** Compute mass using both surface and volume. (VTK method, most accurate)*/
  double GetSurfaceMassFromVTK(mafNode* node);

  /** Get if a point is inside or outside a surface */
  int IsInsideSurface(vtkPolyData* surface, double x[3]);

  /** Calculate inertial tensor using Monte Carlo approach.
      This algorithm requires time and resources but is efficient for complex surfaces.
  */
  int ComputeInertialTensorUsingMonteCarlo(mafNode* node, int current_node = 1, int n_of_nodes = 1);

  /** Calculate inertial tensor using geometry.
      This algorithm is fast but 
  */
  int ComputeInertialTensorUsingGeometry(mafNode* node, int current_node = 1, int n_of_nodes = 1);
 
  
  enum GUI_METHOD_ID
  {
    ID_COMBO = MINID,
	  ID_ACCURACY,
	  ID_VTKCOMP,
    MINID,
  };	

  enum COMPUTATION_METHOD
  {
    MONTE_CARLO = 0,
    GEOMETRY,
  };	
  
  double  m_DefaultDensity;                            // Material density
  double  m_Mass;                               // Material mass
  double  m__Principal_I1,m_Principal_I2,m_Principal_I3;  // Principal Inertial Tensor components.
  
  /* Inertial tensor
  | Ixx  Iyx  Izx |
  | Iyx  Iyy  Izy | 
  | Izx  Izy  Izz | */
  double m_InertialTensor[9]; // [Ixx  Iyx  Izx ,  Iyx  Iyy  Izy , Izx  Izy  Izz]
  mafTagItem m_InertialTensorTag;

  mafTagItem m_PrincipalInertialTensorTag;
  mafTagItem m_TagMass;

  int m_MethodToUse;
  int m_Accuracy;
  int m_Vtkcomp;

  vector<pair<mafNode * , double>> m_NodeMassPairVector;

};
#endif
