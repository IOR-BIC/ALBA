/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: mafPipeSlice.h,v $ 
  Language: C++ 
  Date: $Date: 2009-05-12 16:21:24 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
  This is a base class for all pipes that slices some VMEs
  (such as mafPipeMeshSlice, mafPipeVolumeSlice, etc.)
  Here should go everything common to more pipes!
  =========================================================================
*/

#ifndef mafPipeSlice_h__
#define mafPipeSlice_h__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkPolyDataMapper;
class vtkActor;

//----------------------------------------------------------------------------
// mafPipeSlice :
//----------------------------------------------------------------------------
class mafPipeSlice : public mafPipe
{
public:
  mafTypeMacro(mafPipeSlice, mafPipe);

  mafPipeSlice();
  virtual ~mafPipeSlice();

protected: 
  double m_Origin[3];     ///< origin of the cutting plane
  double m_Normal[3];     ///< normal of the cutting plane    

public:	  
  /** gets the slice normal */
  inline virtual const double* GetOrigin() {
    return m_Origin;
  }

  /** gets the slice origin */
  inline void GetOrigin(double* Origin) {
    GetSlice(Origin, NULL);
  }

  /** Set the origin of the slice*/
  inline void SetOrigin(double *Origin) {
    SetSlice(Origin, NULL);
  }

  /** gets the slice normal */
  inline virtual const double* GetNormal() {
    return m_Normal;
  }

  /** gets the slice normal */
  inline void GetNormal(double* Normal) {
    GetSlice(NULL, Normal);
  }

  /** Set the normal of the slice*/
  inline void SetNormal(double *Normal) {
    SetSlice(NULL, Normal);
  }

  /** Get the slice origin coordinates and normal.
  Both, Origin and Normal may be NULL, if the value is not to be retrieved.*/
  virtual void GetSlice(double* Origin, double* Normal);  
  
  /** Set the origin and normal of the slice.
  Both, Origin and Normal may be NULL, if the current value is to be preserved. */
  virtual void SetSlice(double* Origin, double* Normal);   
};  

#endif // mafPipeSlice_h__
