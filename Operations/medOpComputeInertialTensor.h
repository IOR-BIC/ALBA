/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpComputeInertialTensor.h,v $
  Language:  C++
  Date:      $Date: 2011-02-10 14:26:59 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Simone Brazzale
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpComputeInertialTensor_H__
#define __medOpComputeInertialTensor_H__

#include "mafOp.h"
#include "mafTagItem.h"
#include "vtkCell.h"
#include "vnl/vnl_vector.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMESurface;
class mafGUI;
class ctkCell;

/** 
  class name: medOpComputeInertialTensor
  Operation used to compute the inertial tensor of a surface or a group of surfaces.
  The input is a surface selected from the tree and a scalar that represents the density (assumed uniformly distributed over the volume) of the geometry.
  The outputs are the mass of the surface and the six independent components of the inertial tensor expressed in a specified reference coordinate system. The outputs (i.e. the mass and the inertial tensor) are stored as attributes associated to the input geometry.
  In the group case the resulting mass and the inertial tensor are those of the whole group of geometries and, the computed attributes (i.e. the overall mass and inertial tensor) are stored as attributes of the group.
*/
class medOpComputeInertialTensor: public mafOp
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

protected: 

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  /** Create the dialog interface for the importer. */
  virtual void CreateGui();

  /** Get surface volume. */
  double GetSurfaceVolume();

  /** Get surface area. */
  double GetSurfaceArea();

  /** Get surface mass. */
  double GetSurfaceMass();

  /** Calculate inertial tensor components and store them in the input vme. */
  int ComputeInertialTensor();

  /** Compute area of a triangular cell */
  double medOpComputeInertialTensor::TriangleArea( vtkCell* cell );

  /** Compute area of a quadratic cell */
  double medOpComputeInertialTensor::QuadArea( vtkCell* cell );

  /** Compute area of a tetrahedric cell */
  double medOpComputeInertialTensor::TetVolume( vtkCell* cell );

  /** Compute area of a hexaedric cell */
  double medOpComputeInertialTensor::HexVolume( vtkCell* cell );

  /** Fill vnl vector */
  void medOpComputeInertialTensor::FillVnlVector(vnl_vector<double> &v, double coords[3]);

  
  double  m_Density;
  double  m_Mass;

  mafTagItem m_TagTensor;
  mafTagItem m_TagMass;

};
#endif
