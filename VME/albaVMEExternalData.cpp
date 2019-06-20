/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEExternalData
 Authors: Marco Petrone - Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaVMEExternalData.h"

#include <wx/filefn.h>

#include "albaTagArray.h"
#include "albaStorage.h"
#include "albaEventIO.h"
#include "albaStorageElement.h"

#include <vtkIndent.h>

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEExternalData)
//-------------------------------------------------------------------------


albaVMEExternalData::albaVMEExternalData()
{
}

albaVMEExternalData::~albaVMEExternalData()
{
}

//-------------------------------------------------------------------------
int albaVMEExternalData::DeepCopy(albaVME *a)
//-------------------------------------------------------------------------
{
  if (Superclass::DeepCopy(a)==ALBA_OK)
  {
    albaVMEExternalData *external = albaVMEExternalData::SafeDownCast(a);
    this->SetCurrentPath(external->GetCurrentPath());
    return ALBA_OK;
  }  
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
bool albaVMEExternalData::Equals(albaVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    albaVMEExternalData *external = albaVMEExternalData::SafeDownCast(vme);
    ret = (m_MSFPath == external->GetCurrentPath());
  }
  return ret;
}

//-------------------------------------------------------------------------
void albaVMEExternalData::SetExtension(const char *ext)
//-------------------------------------------------------------------------
{
  albaTagItem item;

  item.SetName("EXTDATA_EXTENSION");
  item.SetValue(ext);
  this->GetTagArray()->SetTag(item);
}

//-------------------------------------------------------------------------
void albaVMEExternalData::SetFileName(const char *filename)
//-------------------------------------------------------------------------
{
  albaTagItem item;
  
  item.SetName("EXTDATA_FILENAME");
  item.SetValue(filename);
  this->GetTagArray()->SetTag(item);
}

//-------------------------------------------------------------------------
const char* albaVMEExternalData::GetExtension()
//-------------------------------------------------------------------------
{
  albaTagItem *item=this->GetTagArray()->GetTag("EXTDATA_EXTENSION");
  return item->GetValue();
}

//-------------------------------------------------------------------------
const char *albaVMEExternalData::GetFileName()
//-------------------------------------------------------------------------
{
  albaTagItem* item=this->GetTagArray()->GetTag("EXTDATA_FILENAME");
  return item->GetValue();
}

//-------------------------------------------------------------------------
albaString albaVMEExternalData::GetAbsoluteFileName()
//-------------------------------------------------------------------------
{
  albaString filename = m_MSFPath;

  if (m_MSFPath.IsEmpty())
  {
    InitializeCurrentPath();
    filename = m_MSFPath;
  }
  
  filename.Append("\\");
  filename.Append(this->GetFileName());
  filename.Append(".");
  filename.Append(this->GetExtension());

  if (wxFileExists(filename.GetCStr()))
  {
    return filename;
  }
  else
  {
    // if file not exists, than the file is not stored yet and I must use TmpPath
    GetTmpPath();
    filename = m_TmpPath;
    filename.Append("\\");
    filename.Append(this->GetFileName());
    filename.Append(".");
    filename.Append(this->GetExtension());
    return filename; 
  }
}

//-------------------------------------------------------------------------
albaString albaVMEExternalData::GetTmpPath()
//-------------------------------------------------------------------------
{
  albaString tmpPath = this->m_TmpPath;
  return tmpPath;
}

//-------------------------------------------------------------------------
albaString albaVMEExternalData::GetCurrentPath()
//-------------------------------------------------------------------------
{
  InitializeCurrentPath();
  albaString currentPath= this->m_MSFPath;
  return currentPath;
}

//-------------------------------------------------------------------------
void albaVMEExternalData::GetAbsoluteFileName(char *fname)
//-------------------------------------------------------------------------
{
  albaString::Copy(fname,this->GetAbsoluteFileName().GetCStr());
}

//-------------------------------------------------------------------------
void albaVMEExternalData::SetMimeType(const char *mimetype)
//-------------------------------------------------------------------------
{
  albaTagItem item;
  item.SetName("EXTDATA_MIMETYPE");
  item.SetValue(mimetype);
  this->GetTagArray()->SetTag(item);
}
 
//-------------------------------------------------------------------------
const char* albaVMEExternalData::GetMimeType()
//-------------------------------------------------------------------------
{
  albaTagItem *item=this->GetTagArray()->GetTag("EXTDATA_MIMETYPE");
  return item->GetValue();  
}

//-----------------------------------------------------------------------
int albaVMEExternalData::InternalStore(albaStorageElement *parent)
//-----------------------------------------------------------------------
{
  InitializeCurrentPath();
  
  albaString fileNameOrigin =  GetAbsoluteFileName(); 
  albaString fileNameTarget = m_MSFPath + "\\" + GetFileName() + "." + GetExtension();

  if (!fileNameOrigin.Equals(fileNameTarget))
  {
    bool copySuccess = wxCopyFile(fileNameOrigin.GetCStr(), fileNameTarget.GetCStr());
    if (!copySuccess)
    {
      albaErrorMessage( "Error copying external file!" );
      return ALBA_ERROR;
    }
  }
  this->SetCurrentPath(m_MSFPath);
  return Superclass::InternalStore(parent);
}

//-----------------------------------------------------------------------
int albaVMEExternalData::InternalRestore(albaStorageElement *parent)
//-----------------------------------------------------------------------
{
  Superclass::InternalRestore(parent);
  m_TmpPath = parent->GetStorage()->GetURL();
  m_TmpPath.ExtractPathName();
  
  m_MSFPath = "";

  return ALBA_OK;
}

//-----------------------------------------------------------------------
void albaVMEExternalData::InitializeCurrentPath()
//-----------------------------------------------------------------------
{
  albaEventIO e(this,NODE_GET_STORAGE);
  ForwardUpEvent(e);
  albaStorage *storage=e.GetStorage();
  if (storage != NULL)
  {
    m_MSFPath = storage->GetURL();
    m_MSFPath.ExtractPathName();
  }
}

//-------------------------------------------------------------------------
void albaVMEExternalData::PrintSelf(std::ostream& os,const int indent)
//-------------------------------------------------------------------------
{
  if (m_MSFPath.IsEmpty())
  {
    InitializeCurrentPath();
  }

  albaVME::Print(os,indent);
  os << indent << "m_MSFPath: "  << this->m_MSFPath.GetCStr() << "\n";
  os << indent << "FileName: " << this->GetFileName() << "\n";
  os << indent << "File Extension: " << this->GetExtension() << "\n" ;
	os << indent << "MIME Type: " << this->GetMimeType() << "\n";
}
