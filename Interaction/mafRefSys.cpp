/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafRefSys.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-24 16:43:05 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone, Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"


#include "mafRefSys.h"

#include "mafVME.h"
#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafAbsMatrixPipe.h"
#include "mafIndent.h"

#include "vtkTransform.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"

#include <assert.h>

//------------------------------------------------------------------------------
mafRefSys::mafRefSys()
//------------------------------------------------------------------------------
{
  Initialize();
}

//------------------------------------------------------------------------------
mafRefSys::mafRefSys(int type)
//------------------------------------------------------------------------------
{
  Initialize();
  SetType(type);
}
//------------------------------------------------------------------------------
mafRefSys::mafRefSys(mafVME *vme)
//------------------------------------------------------------------------------
{
  Initialize();
  SetTypeToLocal(vme);
}

//------------------------------------------------------------------------------
mafRefSys::mafRefSys(vtkMatrix4x4 *matrix)
//------------------------------------------------------------------------------
{
  Initialize();
  SetTypeToCustom(matrix);
}
//------------------------------------------------------------------------------
mafRefSys::mafRefSys(vtkRenderer *ren)
//------------------------------------------------------------------------------
{
  Initialize();
  SetTypeToView(ren);
}

//------------------------------------------------------------------------------
mafRefSys::~mafRefSys()
//------------------------------------------------------------------------------
{
  mafDEL(m_Identity);
  vtkDEL(m_Renderer);
}

//----------------------------------------------------------------------------
void mafRefSys::operator =(const mafRefSys &source)
//----------------------------------------------------------------------------
{
  m_Type = source.m_Type;
  m_Transform=source.m_Transform;
  m_Renderer=source.m_Renderer; 
  m_VME=source.m_VME;
}

//----------------------------------------------------------------------------
void mafRefSys::DeepCopy(const mafRefSys *source)
//----------------------------------------------------------------------------
{
  /*Initialize();
  m_Type = source->m_Type;
  m_Transform=source->m_Transform;
  m_Renderer=source->m_Renderer;
  m_VME=source->m_VME;
  */
} 

//----------------------------------------------------------------------------
void mafRefSys::Initialize()
//----------------------------------------------------------------------------
{
  mafNEW(m_Identity);
  SetTypeToGlobal();
  m_Renderer  = NULL; 
  m_Transform = NULL;
  m_VME       = NULL;
}

//----------------------------------------------------------------------------
void mafRefSys::Reset()
//----------------------------------------------------------------------------
{
  mafDEL(m_Identity);
  Initialize();
}

//----------------------------------------------------------------------------
void mafRefSys::SetTransform(mafTransformBase *transform)
//----------------------------------------------------------------------------
{
  m_Transform = transform;
}

//----------------------------------------------------------------------------
mafTransformBase *mafRefSys::GetTransform()
//----------------------------------------------------------------------------
{
  switch (m_Type)
  {
  case CUSTOM:
    return m_Transform.GetPointer()?m_Transform.GetPointer():m_Identity.GetPointer();
  case GLOBAL: 
    return m_Identity;
  case PARENT: 
    if (m_VME && m_VME->GetParent())
    {
      return m_VME->GetParent()->GetAbsMatrixPipe();
    }
    return m_Identity;    
  case LOCAL:
    if (m_VME.GetPointer())
      return m_VME->GetAbsMatrixPipe();
    
    return m_Identity;
  case VIEW:
    /*
    if (m_Renderer)
          return m_Renderer->GetActiveCamera()->GetViewTransformObject();
        else
          return m_Identity;
    //return m_Renderer?m_Renderer->GetActiveCamera()->GetViewTransformObject():m_Identity;
    // 

    */
    return m_Transform.GetPointer()?m_Transform.GetPointer():m_Identity;
  }
  return m_Identity;
}

//----------------------------------------------------------------------------
void mafRefSys::SetMatrix(vtkMatrix4x4 *matrix)
//---------------------------------------------------------------------------- 
{
  if (matrix)
  {
    mafSmartPointer<mafMatrix> mat;
    mat->SetVTKMatrix(matrix);
    SetMatrix(mat);
  }
  else
  {
    SetTransform(NULL);
  }
}

//----------------------------------------------------------------------------
void mafRefSys::SetMatrix(mafMatrix *matrix)
//---------------------------------------------------------------------------- 
{
  if (matrix)
  {
    mafSmartPointer<mafTransform> trans; 
    trans->SetMatrixPointer(matrix);
    SetTransform(trans);
  }
  else
  {
    SetTransform(NULL);
  }
}

//----------------------------------------------------------------------------
mafMatrix *mafRefSys::GetMatrix()
//----------------------------------------------------------------------------
{
  return GetTransform() ? GetTransform()->GetMatrixPointer() : NULL;
}

//----------------------------------------------------------------------------
void mafRefSys::SetVME(mafVME *vme)
//----------------------------------------------------------------------------
{
  m_VME = vme;
}

//----------------------------------------------------------------------------
void mafRefSys::SetRenderer(vtkRenderer *renderer)
//----------------------------------------------------------------------------
{
  if (m_Renderer!=renderer)
  {
    m_Renderer->UnRegister(NULL);
    m_Renderer = renderer;
    m_Renderer->Register(NULL);
  }
}
//----------------------------------------------------------------------------
void mafRefSys::SetTypeToCustom(vtkMatrix4x4 *matrix)
//----------------------------------------------------------------------------
{
  SetMatrix(matrix);
  SetType(CUSTOM);
}

//----------------------------------------------------------------------------
void mafRefSys::SetTypeToCustom(mafMatrix *matrix)
//----------------------------------------------------------------------------
{
  SetMatrix(matrix);
  SetType(CUSTOM);
}

//----------------------------------------------------------------------------
void mafRefSys::SetTypeToCustom(mafTransformBase *transform)
//----------------------------------------------------------------------------
{
  SetTransform(transform);
  SetType(CUSTOM);
}
//----------------------------------------------------------------------------
void mafRefSys::SetTypeToView(vtkRenderer *renderer)
//----------------------------------------------------------------------------
{
  SetRenderer(renderer);
  SetType(VIEW);
}
 
//----------------------------------------------------------------------------
void mafRefSys::SetTypeToParent(mafVME *vme)
//----------------------------------------------------------------------------
{
  assert(vme);
  SetVME(vme);
  SetType(PARENT);
}

//----------------------------------------------------------------------------
void mafRefSys::SetTypeToLocal(mafVME *vme)
//----------------------------------------------------------------------------
{
  assert(vme);
  SetVME(vme);
  SetType(LOCAL);
}

//----------------------------------------------------------------------------
void mafRefSys::SetTypeToGlobal()
//----------------------------------------------------------------------------
{
  SetType(GLOBAL);
}

//----------------------------------------------------------------------------
void mafRefSys::Print(std::ostream& os, const int tabs)
//----------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  os << indent << "m_Type: ";
  switch (m_Type)
  {
    case PARENT:
      os << "PARENT\n";
      break; 
    case GLOBAL:
      os << "GLOBAL\n";
      break;
    case CUSTOM:
      os << "CUSTOM\n";
      break;
    case VIEW:
      os << "VIEW\n";
      break;
    default:
      os << "UNKONOWN\n";
  }
  
  os << indent << "Reference System Matrix:\n";
  GetTransform()->GetMatrixPointer()->Print(os,indent.GetNextIndent());

  os << indent << "m_Renderer used as Reference System:"<<m_Renderer;

  os << indent << "m_VME: \""<<(m_VME?m_VME->GetName():"NULL")<<"\"\n";

}
