/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEExternalData.cpp,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:58:29 $
  Version:   $Revision: 1.6 $
  Authors:   Marco Petrone - Roberto Mucci
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMEExternalData.h"

#include <wx/filefn.h>

#include "mafTagArray.h"
#include "mafStorage.h"
#include "mafEventIO.h"
#include "mafStorageElement.h"

#include <vtkIndent.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEExternalData)
//-------------------------------------------------------------------------


mafVMEExternalData::mafVMEExternalData()
{
}

mafVMEExternalData::~mafVMEExternalData()
{
}

//-------------------------------------------------------------------------
void mafVMEExternalData::SetExtension(const char *ext)
//-------------------------------------------------------------------------
{
  mafTagItem item;

  item.SetName("EXTDATA_EXTENSION");
  item.SetValue(ext);
  this->GetTagArray()->SetTag(item);
}

//-------------------------------------------------------------------------
void mafVMEExternalData::SetFileName(const char *filename)
//-------------------------------------------------------------------------
{
  mafTagItem item;
  
  item.SetName("EXTDATA_FILENAME");
  item.SetValue(filename);
  this->GetTagArray()->SetTag(item);
}

//-------------------------------------------------------------------------
const char* mafVMEExternalData::GetExtension()
//-------------------------------------------------------------------------
{
  mafTagItem *item=this->GetTagArray()->GetTag("EXTDATA_EXTENSION");
  return item->GetValue();
}

//-------------------------------------------------------------------------
const char *mafVMEExternalData::GetFileName()
//-------------------------------------------------------------------------
{
  mafTagItem* item=this->GetTagArray()->GetTag("EXTDATA_FILENAME");
  return item->GetValue();
}

//-------------------------------------------------------------------------
mafString mafVMEExternalData::GetAbsoluteFileName()
//-------------------------------------------------------------------------
{
  mafString filename = m_MSFPath;

  if (m_MSFPath.IsEmpty())
  {
    InitializeCurrentPath();
    filename = m_MSFPath;
   }

  // if m_MSFPath is still empty, than the file is not stored yet and I must use TmpPath
  if (m_MSFPath.IsEmpty())
  {
    GetTmpPath();
    filename = m_TmpPath;
  }

  filename.Append("\\");
  filename.Append(this->GetFileName());
  filename.Append(".");
  filename.Append(this->GetExtension());

  return filename;
}

//-------------------------------------------------------------------------
mafString mafVMEExternalData::GetTmpPath()
//-------------------------------------------------------------------------
{
  mafString tmpPath = this->m_TmpPath;
  return tmpPath;
}

//-------------------------------------------------------------------------
void mafVMEExternalData::GetAbsoluteFileName(char *fname)
//-------------------------------------------------------------------------
{
  mafString::Copy(fname,this->GetAbsoluteFileName().GetCStr());
}

//-------------------------------------------------------------------------
void mafVMEExternalData::SetMimeType(const char *mimetype)
//-------------------------------------------------------------------------
{
  mafTagItem item;
  item.SetName("EXTDATA_MIMETYPE");
  item.SetValue(mimetype);
  this->GetTagArray()->SetTag(item);
}
 
//-------------------------------------------------------------------------
const char* mafVMEExternalData::GetMimeType()
//-------------------------------------------------------------------------
{
  mafTagItem *item=this->GetTagArray()->GetTag("EXTDATA_MIMETYPE");
  return item->GetValue();  
}

//-----------------------------------------------------------------------
int mafVMEExternalData::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{
  InitializeCurrentPath();

  wxString path, pathTarget, name, ext;
  
  mafTagItem *t = GetTagArray()->GetTag("EXTDATA_FILENAME");
  wxString nameNew = t->GetValue();

  mafTagItem *item=this->GetTagArray()->GetTag("EXTDATA_EXTENSION");
  wxString extNew = item->GetValue();

  wxSplitPath(nameNew, &pathTarget, &name, &ext);

  mafString fileNameOrigin =  GetTmpPath() + "/" + GetFileName() + "." + GetExtension(); 
  mafString fileNameTarget = m_MSFPath + "/" + name + "." + extNew;

  bool copySuccess = wxCopyFile(fileNameOrigin.GetCStr(), fileNameTarget.GetCStr());
  if (copySuccess)
  {
    t->SetValue(name);
  }
  return Superclass::InternalStore(parent);
}

//-----------------------------------------------------------------------
int mafVMEExternalData::InternalRestore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{
  Superclass::InternalRestore(parent);
  m_TmpPath = parent->GetStorage()->GetURL();
  m_TmpPath.ExtractPathName();
  
  m_MSFPath = "";

  return MAF_OK;
}

//-----------------------------------------------------------------------
void mafVMEExternalData::InitializeCurrentPath()
//-----------------------------------------------------------------------
{
  mafEventIO e(this,NODE_GET_STORAGE);
  ForwardUpEvent(e);
  mafStorage *storage=e.GetStorage();
  if (storage != NULL)
  {
    m_MSFPath = storage->GetURL();
    m_MSFPath.ExtractPathName();
  }
}

//-------------------------------------------------------------------------
void mafVMEExternalData::PrintSelf(std::ostream& os,const int indent)
//-------------------------------------------------------------------------
{
  if (m_MSFPath.IsEmpty())
  {
    InitializeCurrentPath();
  }

  mafVME::Print(os,indent);
  os << indent << "m_MSFPath: "  << this->m_MSFPath.GetCStr() << "\n";
  os << indent << "FileName: " << this->GetFileName() << "\n";
  os << indent << "File Extension: " << this->GetExtension() << "\n" ;
	os << indent << "MIME Type: " << this->GetMimeType() << "\n";
}
