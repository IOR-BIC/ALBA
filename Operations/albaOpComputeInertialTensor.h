/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpComputeInertialTensor
 Authors: Simone Brazzale , Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpComputeInertialTensor_H__
#define __albaOpComputeInertialTensor_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"
#include "albaVME.h"
#include "albaTagItem.h"
#include "vtkCell.h"
#include "vtkPolyData.h"
#include "vnl/vnl_vector.h"

using namespace std;

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMESurface;
class albaGUI;
class vtkCell;
class albaVMELandmarkCloud;

/** 
  class name: albaOpComputeInertialTensor

  Operation used to compute the inertial tensor of a surface or a group of surfaces.

  The input is a surface selected from the tree and a scalar that represents the density (assumed uniformly distributed over the volume) of the geometry.
  The outputs are the mass of the surface and the six independent components of the inertial tensor expressed in a specified reference coordinate system. 
  The outputs (i.e. the mass and the inertial tensor) are stored as attributes associated to the input geometry.
  In the group case the resulting mass and the inertial tensor are those of the whole group of geometries and, the computed attributes 
  (i.e. the overall mass and inertial tensor) are stored as attributes of the group.

  https://picasaweb.google.com/104563709532324716730/MedOpComputeInertialTensor?authuser=0&feat=directlink

  Density for each VME will be read from the "DENSITY" tag: if this tag is not found the Default density value will be used. 
	
*/
class ALBA_EXPORT albaOpComputeInertialTensor: public albaOp
{
public:
  /** constructor */
  albaOpComputeInertialTensor(const wxString &label = "ComputeInertialTensor");
  /** destructor */
  ~albaOpComputeInertialTensor();

  /** RTTI macro*/
  albaTypeMacro(albaOpComputeInertialTensor, albaOp);

  /** clone the object and retrieve a copy*/
  albaOp* Copy();

  /** Builds operation's interface. */
  void OpRun();

  /** Execute the operation. */
  void OpDo();

  /** Makes the undo for the operation. */
  void OpUndo();

  /** Precess events coming from other objects */
  virtual void OnEvent(albaEventBase *alba_event);

  /** Set the default density value used for computation */
  void SetDefaultDensity(double val);

  /** Return the default density value used for computation */
  double GetDefaultDensity();

	/** Calculate inertial tensor components from a surface */
	int ComputeInertialTensor(albaVME* node);

  /** Calculate inertial tensor components from a group of surfaces and store them in the input vme. */
  int ComputeInertialTensorFromGroup();

   /** Add results to the vme in the attributes section. */
  void AddAttributes();

  /** Get the VME mass from the "SURFACE_MASS" tag if existent otherwise returns: */  enum {SURFACE_MASS_NOT_FOUND = -1};
  static double GetMass( albaVME* node);

protected:

	typedef struct locInTensor{
		double _xx; double _yy; double _zz;
		double _yx; double _zx; double _zy;
		double _Cx; double _Cy; double _Cz;
		double _m; 
		double _density;
		albaVME *_node;
	} LocalInertiaTensor;

	/** Computes Inertial tensor referred to a single node, and fill m_LocalInertiaTensors vector */
	int ComputeLocalInertialTensor(albaVME* node, int current_node = 1, int n_of_nodes = 1);

	/** Uses m_LocalInertiaTensors partial data to calculate the inertia tensor of the total system */
	void ComputeGlobalInertiaTensor();
	
	/** Get the VME density from the "DENSITY" tag if existent otherwise returns: */  enum {DENSITY_NOT_FOUND = -1};
	double GetDensity( albaVME* node);
	
	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  /** Create the dialog interface for the importer. */
  virtual void CreateGui();

  vector <LocalInertiaTensor> m_LocalInertiaTensors;
  
  enum GUI_METHOD_ID
  {
	  ID_ACCURACY = MINID,
		ID_GEN_COM_LM,
	  ID_VTKCOMP,
    MINID,
  };	

   
  double  m_DefaultDensity;                            // Material density
  double  m_Mass;                               // Material mass
  double  m_Principal_I1,m_Principal_I2,m_Principal_I3;  // Principal Inertial Tensor components.
  
  /* Inertial tensor
  | Ixx  Iyx  Izx |
  | Iyx  Iyy  Izy | 
  | Izx  Izy  Izz | */
  double m_InertialTensor[9]; // [Ixx  Iyx  Izx ,  Iyx  Iyy  Izy , Izx  Izy  Izz]
  albaTagItem m_InertialTensorTag;

  albaTagItem m_PrincipalInertialTensorTag;
  albaTagItem m_TagMass;
  albaTagItem m_LocalCenterOfMassTag;

  double m_CenterOfMass[3];

	albaVMELandmarkCloud *m_Lmc;

  int m_Accuracy;
	int m_GenerateCenterOfMassLM;

  vector<pair<albaVME * , double>> m_NodeMassPairVector;

private:
	void CreateLMC();
};
#endif
