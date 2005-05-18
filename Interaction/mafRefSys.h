/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafRefSys.h,v $
  Language:  C++
  Date:      $Date: 2005-05-18 17:29:07 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone, Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafRefSys_h
#define __mafRefSys_h

#include "mafConfigure.h"
#include "mafSmartPointer.h"
#include <iostream>

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafTransform;
class mafVME;
class mafTransformBase;
class mafMatrix;
class vtkMatrix4x4;
class vtkRenderer;


/** class representing reference system

  @todo
  - costruttori che prende le costanti o matrice o transform o VME e si setta il  modo. (FATTO!)
  - si deve tenere un puntatore al VME (FATTO!)
  - Set Renderer/VME/Transform etc che non cambia il modo (FATTO!)
  - write documentation
  - complete the PrintSelf
  - use a mafRenderer in place of vtkRenderer
*/
class MAF_EXPORT mafRefSys
{
public:
  mafRefSys();
  mafRefSys(int type);
  mafRefSys(mafVME *vme);
  mafRefSys(vtkMatrix4x4 *matrix);
  mafRefSys(vtkRenderer *ren);
  virtual ~mafRefSys();
  
  /** copy constructor */
  operator =(const mafRefSys &source);
  

  //----------------------------------------------------------------------------
  // Ref Sys Type:
  //----------------------------------------------------------------------------
  enum 
  {
    CUSTOM = 0, ///< auxiliar ref sys 
    GLOBAL, 
    PARENT,
    LOCAL,   ///< the local ref sys of the VME
    VIEW     ///< the view ref sys
  };

 /*
    //----------------------------------------------------------------------------
    // VIEW ref sys
    //----------------------------------------------------------------------------

    Y 
    ^
    |
    |
    |
    . ----> X 
    Z

    X: View Right
    Y: View Up 
    Z: View Look

 */

  /** 
    Set the reference system to CUSTOM. A linear transform of the refsys must be provided,
    if not the GetTransform will provide the identity by default.
    Notice, the transform is referenced and not copied, thus its changes reflect
    into mafRefSys changes */
  void SetTypeToCustom(mafTransformBase *transform=NULL);

  /** Set the reference system to CUSTOM. A linear transform of the refsys must be provided,
    if not the GetTransform will provide the identity by default.
    Notice, the matrix is referenced and not copied, thus its changes reflect
    into mafRefSys changes */
  void SetTypeToCustom(vtkMatrix4x4 *matrix);
  void SetTypeToCustom(mafMatrix *matrix);

  /** Set the reference system to the VME's abs matrix.*/
  void SetTypeToLocal(mafVME *vme=NULL);

  /** Set the reference system to VIEW. Optionally a vtkRenderer can be passed. */
  void SetTypeToView(vtkRenderer *renderer=NULL);

  /** 
    Set the reference system to the parent Abs matrix of a VME. The argument is the VME to which
    the parent refers. If no parent exists GetTransform() return the identity. The VME is stored 
    and changes to its parent reflect into ref sys changes.*/
  void SetTypeToParent(mafVME *vme);

  /** Set the reference system to identity */ 
  void SetTypeToGlobal();

  /**
    Set the type of reference system, which can be CUSTOM, GLOBAL, LOCAL, PARENT or VIEW. Beware if you
    set directly the Type, you also have to provide a Transform or a Renderer  or the VME */
  void SetType(int type) {m_Type=type;}

  /** Get the type of reference system, which can be CUSTOM, GLOBAL, LOCAL,PARENT or VIEW */
  int GetType() {return m_Type;}

  /**
    Used to set the transform used for for the CUSTOM ref sys type. Notice the provided object
    is referenced and not copied! The Transform member variable is used only for CUSTOM ref sys type!!!!
    */
  void SetTransform(mafTransformBase *transform);
  
  /** 
    return a transform representing this RefSys, this could be either the
    internally stored transform (CUSTOM), the renderer's ViewTransform (VIEW),
    the VME AbsMatrixMatrixPipe (LOCAL) or an identity transform when in GLOBAL */
  mafTransformBase *GetTransform();

  /** return the matrix of this ref_sys ( @sa GetTransform() )*/
  mafMatrix *GetMatrix();

  /** 
    Set the matrix relative to the ref_sys: mafTransform is
    created on the fly and stored in the Transform field to link
    the matrix. This function follows same rules as the SetTransform() function.
  */
  void SetMatrix(vtkMatrix4x4 *matrix);

  /** 
    Set the matrix relative to the ref_sys: mafTransform is
    created on the fly and stored in the Transform field to link
    the matrix. This function follows same rules as the SetTransform() function.
  */
  void SetMatrix(mafMatrix *matrix);

  /** 
    Set the internal Renderer variable. This has effect only if in VIEW mode.
    Notice the internal transform is set to point the active camera's ViewTransform */
  void SetRenderer(vtkRenderer *renderer);

  /** return Renderer stored in this ref_sys */
  vtkRenderer *GetRenderer() {return m_Renderer;}

  /** 
    Set the reference to the VME. This is used for types LOCAL and GLOBAL to
    retrieve the transform */    
  void SetVME(mafVME *vme);

  /** return the reference to the VME stored inside the RefSys */
  mafVME *GetVME() {return m_VME;}


  /** Debug printing of internal data */
  virtual void Print(std::ostream& os, const int tabs);
  
  /**
  DeepCopy the source matrix into the target. The target is the object
  invoking the DeepCopy method */
  void DeepCopy(const mafRefSys *source);
  
protected:

  /** Reset the class to default value.*/
  void Initialize();

  mafAutoPointer<mafTransformBase>    m_Identity;  
  mafAutoPointer<mafTransformBase>    m_Transform;///< the ref sys matrix
  vtkRenderer*                        m_Renderer; ///< ref sys renderer
  mafAutoPointer<mafVME>              m_VME;     ///< reference to VME
  int                                 m_Type;     ///< type of ref sys (CUSTOM, GLOBAL, LOCAL, PARENT, VIEW)
};

#endif 
