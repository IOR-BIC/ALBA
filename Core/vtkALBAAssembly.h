/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAAssembly
 Authors: Sivano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAAssembly_h
#define __vtkALBAAssembly_h

#include "vtkProp3D.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class vtkAssemblyPaths;
class vtkProp3DCollection;
class vtkMapper;
class vtkActor;
class albaVME;

/** create hierarchies of vtkProp3Ds (transformable props)
  vtkALBAAssembly same as vtkAssembly, but has an extra link 
  for client data.
  @sa vtkActor vtkAssembly vtkTransform vtkMapper vtkPolyDataMapper vtkPropAssembly
  */
class ALBA_EXPORT vtkALBAAssembly : public vtkProp3D
{
public:
  static vtkALBAAssembly *New();

  vtkTypeMacro(vtkALBAAssembly,vtkProp3D);
  void PrintSelf(ostream& os, vtkIndent indent);

  /**
  Get the VME associated to this Assembly */
  albaVME *  GetVme(){return m_Vme;};

  /**
  Set the VME associated to this Assembly */
  void      SetVme(albaVME *vme){m_Vme=vme;};

  /**
  Add a part to the list of parts.*/
  void AddPart(vtkProp3D *);

  /**
  Remove a part from the list of parts,*/
  void RemovePart(vtkProp3D *);

  /**
  Return the parts (direct descendants) of this assembly.*/
  vtkProp3DCollection *GetParts();

  /** 
  For some exporters and other other operations we must be
  able to collect all the actors or volumes. These methods
  are used in that process.*/
  void GetActors(vtkPropCollection *);
  void GetVolumes(vtkPropCollection *);

  /**
  Render this assembly and all its parts. 
  The rendering process is recursive.
  Note that a mapper need not be defined. If not defined, then no geometry 
  will be drawn for this assembly. This allows you to create "logical"
  assemblies; that is, assemblies that only serve to group and transform
  its parts.*/
  int RenderOpaqueGeometry(vtkViewport *ren);
  int RenderTranslucentPolygonalGeometry(vtkViewport *ren);

  /**
  Release any graphics resources that are being consumed by this actor.
  The parameter window could be used to determine which graphic
  resources to release.*/
  void ReleaseGraphicsResources(vtkWindow *);

  /**
  Methods to traverse the parts of an assembly. Each part (starting from
  the root) will appear properly transformed and with the correct
  properties (depending upon the ApplyProperty and ApplyTransform ivars).
  Note that the part appears as an instance of vtkProp. These methods
  should be contrasted to those that traverse the list of parts using
  GetParts().  GetParts() returns a list of children of this assembly, not
  necessarily with the correct transformation or properties. To use the
  methods below - first invoke InitPathTraversal() followed by repeated
  calls to GetNextPath().  GetNextPath() returns a NULL pointer when the
  list is exhausted.*/
  void InitPathTraversal();
  vtkAssemblyPath *GetNextPath();
  int GetNumberOfPaths();

  /**
  Get the bounds for the assembly as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax).*/
  void GetBounds(double bounds[6]) {this->vtkProp3D::GetBounds( bounds );};
  virtual double  *GetBounds();

  /**
  Override default GetMTime method to also consider all of the
  assembly's parts.*/
	vtkMTimeType GetMTime();

  /**
  Shallow copy of an assembly. Overloads the virtual vtkProp method.*/
  void ShallowCopy(vtkProp *prop);

//BTX
  /**
  WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE DO NOT USE THIS
  METHOD OUTSIDE OF THE RENDERING PROCESS Overload the superclass' vtkProp
  BuildPaths() method. Paths consist of an ordered sequence of actors,
  with transformations properly concatenated.*/
  void BuildPaths(vtkAssemblyPaths *paths, vtkAssemblyPath *path);
//ETX  

	/** Does this prop have some translucent polygonal geometry?
	 This method is called during the rendering process to know if there is
	 some translucent polygonal geometry. */
	virtual int HasTranslucentPolygonalGeometry();

protected:
  vtkALBAAssembly();
  ~vtkALBAAssembly();

  // pointer to the VME that is represented by this Assembly
  albaVME      *m_Vme;

  // Keep a list of direct descendants of the assembly hierarchy
  vtkProp3DCollection *m_Parts;

  // Support the BuildPaths() method. Caches last paths built for
  // performance.
  vtkTimeStamp PathTime;
  virtual void UpdatePaths(); //apply transformations and properties recursively
  
private:
  vtkALBAAssembly(const vtkALBAAssembly&);  // Not implemented.
  void operator=(const vtkALBAAssembly&);  // Not implemented.
};

/**
Get the list of parts for this assembly.*/
inline vtkProp3DCollection *vtkALBAAssembly::GetParts() {return this->m_Parts;}

#endif




