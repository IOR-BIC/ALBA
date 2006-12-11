/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: mafClassicICPRegistration.h,v $
  Language:  C++
  Date:      $Date: 2006-12-11 09:13:36 $
  Version:   $Revision: 1.1 $
  Authors:   Stefania Paperini paperini@tecno.ior.it 
  Project:   MultiMod Project (www.ior.it/multimod)

  Copyright (c) 2002/2003
  B3C -  BioComputing Competence Centre (www.cineca.it/B3C)
  v. Magnanelli 6/3
  40033 Casalecchio di Reno (BO)
  Italy
  ph. +39-051-6171411 (90 lines) - Fax +39-051-6132198

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
   form, must retain the above copyright notice, this license,
   the following disclaimer, and any notices that refer to this
   license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
   notice, a copy of this license and the following disclaimer
   in the documentation or with other materials provided with the
   distribution.

3) Modified copies of the source code must be clearly marked as such,
   and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.
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

#include "vtkMAFConfigure.h"

//vtk include
#include <vtkLinearTransform.h>
#include "vtkIterativeClosestPointTransform.h"
#include <vtkPolyData.h>
#include <vtkPointLocator.h>

#include "mafString.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkCellLocator;
class vtkDataSet;
class mafICPUtility;
//----------------------------------------------------------------------------
// mafClassicICPRegistration :
//----------------------------------------------------------------------------
class VTK_vtkMAF_EXPORT mafClassicICPRegistration : public vtkIterativeClosestPointTransform
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
  
  const char *GetResultsFileName() {return this->ResultsFile.GetCStr(); }

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
  mafString ResultsFile;

  mafICPUtility *ICPUtil;

  //modified by Stefano 7-11-2004
  double RegistrationError;

private:
  mafClassicICPRegistration(const mafClassicICPRegistration&);  // Not implemented.
  void operator=(const mafClassicICPRegistration&);  // Not implemented.
};

#endif
