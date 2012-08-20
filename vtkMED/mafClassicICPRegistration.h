/*=========================================================================

 Program: MAF2Medical
 Module: mafClassicICPRegistration
 Authors: Stefania Paperini paperini@tecno.ior.it
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// .NAME mflClassicICPRegistration - Implementation of the ICP algorithm.
// .SECTION Description
// Match two surfaces using the iterative closest point (ICP) algorithm.
// The core of the algorithm is to match each vertex in one surface with 
// the closest surface point on the other, then apply the transformation
// that modify one surface to best match the other (in a least square sense).
// This has to be iterated to get proper convergence of the surfaces.
// .SECTION Note
// Use vtkTransformPolyDataFilter to apply the resulting ICP transform to 
// your data. You might also set it to your actor's user transform.
// .SECTION Note
// This class makes use of vtkLandmarkTransform internally to compute the
// best fit. Use the GetLandmarkTransform member to get a pointer to that
// transform and set its parameters. You might, for example, constrain the
// number of degrees of freedom of the solution (i.e. rigid body, similarity,
// etc.) by checking the vtkLandmarkTransform documentation for its SetMode
// member.
// .SECTION see also
// vtkLandmarkTransform


#ifndef __mafClassicICPRegistration_h
#define __mafClassicICPRegistration_h

#include "vtkMEDConfigure.h"

//vtk include
#include <vtkLinearTransform.h>
#include "vtkIterativeClosestPointTransform.h"
#include <vtkPolyData.h>
#include <vtkPointLocator.h>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkCellLocator;
class vtkDataSet;
class mafICPUtility;

/** 
  class name: mafClassicICPRegistration
  Match two surfaces using the iterative closest point (ICP) algorithm.
  The core of the algorithm is to match each vertex in one surface with 
  the closest surface point on the other, then apply the transformation
  that modify one surface to best match the other (in a least square sense).
  This has to be iterated to get proper convergence of the surfaces.
*/
class VTK_vtkMED_EXPORT mafClassicICPRegistration : public vtkIterativeClosestPointTransform
{
public:
  static mafClassicICPRegistration *New();
	vtkTypeRevisionMacro(mafClassicICPRegistration, vtkIterativeClosestPointTransform);
  //vtkTypeMacro(mflClassicICPRegistration,vtkIterativeClosestPointTransform);
  //void PrintSelf(ostream& os, vtkIndent indent);

  /**
  Specify the source and target data sets.*/
  void SetSource(vtkDataSet *source);
  void SetTarget(vtkDataSet *target);
  vtkGetObjectMacro(Source, vtkDataSet);
  vtkGetObjectMacro(Target, vtkDataSet);

  /**
  Set/Get a spatial locator for speeding up the search process. 
  An instance of vtkCellLocator is used by default.*/
  void SetLocator(vtkCellLocator *locator);
  vtkGetObjectMacro(Locator,vtkCellLocator);

  
  void SetResultsFileName(const char *name);
  
  const char *GetResultsFileName() {return this->ResultsFile.c_str(); }

  vtkSetMacro(Convergence, float);
  vtkGetMacro(Convergence, float);
  
  /** 
  On/Off the debug information saved in ResultsFile*/
  vtkSetMacro(SaveResults, int);
  vtkGetMacro(SaveResults, int);
  vtkBooleanMacro(SaveResults, int);

  /**
  Make another transform of the same type.*/
  vtkAbstractTransform *MakeTransform();

  //modified by Stefano 7-11-2004
  /**
  Get the registration error*/
  vtkGetMacro(RegistrationError, double);

protected:

  /**
  Release source and target*/
  void ReleaseSource(void);
  void ReleaseTarget(void);

  /**
  Release locator*/
  void ReleaseLocator(void);

  /**
  Create default locator. Used to create one when none is specified.*/
  void CreateDefaultLocator(void);

  /**
  Get the MTime of this object also considering the locator.*/
  unsigned long int GetMTime();

  mafClassicICPRegistration();
  ~mafClassicICPRegistration();

  void InternalUpdate();

  /**
  This method does no type checking, use DeepCopy instead.*/
  void InternalDeepCopy(vtkAbstractTransform *transform);

  vtkDataSet* Source;
  vtkDataSet* Target;
  vtkCellLocator *Locator;

  float Convergence;
  
  int SaveResults;
  std::string ResultsFile;

  mafICPUtility *ICPUtil;

  //modified by Stefano 7-11-2004
  double RegistrationError;

private:
  mafClassicICPRegistration(const mafClassicICPRegistration&);  // Not implemented.
  void operator=(const mafClassicICPRegistration&);  // Not implemented.
};

#endif
