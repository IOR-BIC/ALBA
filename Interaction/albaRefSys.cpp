/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaRefSys
 Authors: Marco Petrone, Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"


#include "albaRefSys.h"

#include "albaVME.h"
#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaAbsMatrixPipe.h"
#include "albaIndent.h"

#include "vtkTransform.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"

#include <assert.h>

//------------------------------------------------------------------------------
albaRefSys::albaRefSys()
//------------------------------------------------------------------------------
{
  Initialize();
}

//------------------------------------------------------------------------------
albaRefSys::albaRefSys(int type)
//------------------------------------------------------------------------------
{
  Initialize();
  SetType(type);
}
//------------------------------------------------------------------------------
albaRefSys::albaRefSys(albaVME *vme)
//------------------------------------------------------------------------------
{
  Initialize();
  SetTypeToLocal(vme);
}

//------------------------------------------------------------------------------
albaRefSys::albaRefSys(vtkMatrix4x4 *matrix)
//------------------------------------------------------------------------------
{
  Initialize();
  SetTypeToCustom(matrix);
}
//------------------------------------------------------------------------------
albaRefSys::albaRefSys(vtkRenderer *ren)
//------------------------------------------------------------------------------
{
  Initialize();
  SetTypeToView(ren);
}

//------------------------------------------------------------------------------
albaRefSys::~albaRefSys()
//------------------------------------------------------------------------------
{
  albaDEL(m_Identity);
  vtkDEL(m_Renderer);
}

//----------------------------------------------------------------------------
void albaRefSys::operator =(const albaRefSys &source)
//----------------------------------------------------------------------------
{
  m_Type = source.m_Type;
  m_Transform=source.m_Transform;
  m_Renderer=source.m_Renderer; 
  m_VME=source.m_VME;
}

//----------------------------------------------------------------------------
void albaRefSys::DeepCopy(const albaRefSys *source)
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
void albaRefSys::Initialize()
//----------------------------------------------------------------------------
{
  albaNEW(m_Identity);
  SetTypeToGlobal();
  m_Renderer  = NULL; 
  m_Transform = NULL;
  m_VME       = NULL;
}

//----------------------------------------------------------------------------
void albaRefSys::Reset()
//----------------------------------------------------------------------------
{
  albaDEL(m_Identity);
  Initialize();
}

//----------------------------------------------------------------------------
void albaRefSys::SetTransform(albaTransformBase *transform)
//----------------------------------------------------------------------------
{
  m_Transform = transform;
}

//----------------------------------------------------------------------------
albaTransformBase *albaRefSys::GetTransform()
//----------------------------------------------------------------------------
{
  switch (m_Type)
  {
  case CUSTOM:
    return m_Transform?m_Transform:(albaTransformBase*)m_Identity;
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
    return m_Transform?m_Transform:(albaTransformBase*)m_Identity;
  }
  return m_Identity;
}

//----------------------------------------------------------------------------
void albaRefSys::SetMatrix(vtkMatrix4x4 *matrix)
//---------------------------------------------------------------------------- 
{
  if (matrix)
  {
    albaSmartPointer<albaMatrix> mat;
    mat->SetVTKMatrix(matrix);
    SetMatrix(mat);
  }
  else
  {
    SetTransform(NULL);
  }
}

//----------------------------------------------------------------------------
void albaRefSys::SetMatrix(albaMatrix *matrix)
//---------------------------------------------------------------------------- 
{
  if (matrix)
  {
    albaSmartPointer<albaTransform> trans; 
    trans->SetMatrixPointer(matrix);
    SetTransform(trans);
  }
  else
  {
    SetTransform(NULL);
  }
}

//----------------------------------------------------------------------------
albaMatrix *albaRefSys::GetMatrix()
//----------------------------------------------------------------------------
{
  return GetTransform() ? GetTransform()->GetMatrixPointer() : NULL;
}

//----------------------------------------------------------------------------
void albaRefSys::SetVME(albaVME *vme)
//----------------------------------------------------------------------------
{
  m_VME = vme;
}

//----------------------------------------------------------------------------
void albaRefSys::SetRenderer(vtkRenderer *renderer)
//----------------------------------------------------------------------------
{
  if (m_Renderer!=renderer)
  {
    if(m_Renderer)
    {
      m_Renderer->UnRegister(NULL);
    }
    m_Renderer = renderer;
		if(m_Renderer)
			m_Renderer->Register(NULL);
  }
}
//----------------------------------------------------------------------------
void albaRefSys::SetTypeToCustom(vtkMatrix4x4 *matrix)
//----------------------------------------------------------------------------
{
  SetMatrix(matrix);
  SetType(CUSTOM);
}

//----------------------------------------------------------------------------
void albaRefSys::SetTypeToCustom(albaMatrix *matrix)
//----------------------------------------------------------------------------
{
  SetMatrix(matrix);
  SetType(CUSTOM);
}

//----------------------------------------------------------------------------
void albaRefSys::SetTypeToCustom(albaTransformBase *transform)
//----------------------------------------------------------------------------
{
  SetTransform(transform);
  SetType(CUSTOM);
}
//----------------------------------------------------------------------------
void albaRefSys::SetTypeToView(vtkRenderer *renderer)
//----------------------------------------------------------------------------
{
	SetRenderer(renderer);
  SetType(VIEW);
}
 
//----------------------------------------------------------------------------
void albaRefSys::SetTypeToParent(albaVME *vme)
//----------------------------------------------------------------------------
{
  assert(vme);
  SetVME(vme);
  SetType(PARENT);
}

//----------------------------------------------------------------------------
void albaRefSys::SetTypeToLocal(albaVME *vme)
//----------------------------------------------------------------------------
{
  //assert(vme);
  SetVME(vme);
  SetType(LOCAL);
}

//----------------------------------------------------------------------------
void albaRefSys::SetTypeToGlobal()
//----------------------------------------------------------------------------
{
  SetType(GLOBAL);
}

//----------------------------------------------------------------------------
void albaRefSys::Print(std::ostream& os, const int tabs)
//----------------------------------------------------------------------------
{
  albaIndent indent(tabs);

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
