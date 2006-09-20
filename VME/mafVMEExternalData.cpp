/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEExternalData.cpp,v $
  Language:  C++
  Date:      $Date: 2006-09-20 12:24:03 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEExternalData_cxx
#define __mafVMEExternalData_cxx

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMEExternalData.h"
#include "mafTagArray.h"
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
{
  mafTagItem item;

  item.SetName("EXTDATA_EXTENSION");
  item.SetValue(ext);
  this->GetTagArray()->SetTag(item);
}

//-------------------------------------------------------------------------
void mafVMEExternalData::SetFileName(const char *filename)
{
  mafTagItem item;

  item.SetName("EXTDATA_FILENAME");
  item.SetValue(filename);
  this->GetTagArray()->SetTag(item);
}

//-------------------------------------------------------------------------
const char *
mafVMEExternalData::GetExtension()
{
  mafTagItem *item=this->GetTagArray()->GetTag("EXTDATA_EXTENSION");
  return item->GetValue();
}

//-------------------------------------------------------------------------
const char *
mafVMEExternalData::GetFileName()
{
  mafTagItem *item=this->GetTagArray()->GetTag("EXTDATA_FILENAME");
  return item->GetValue();
}

//-------------------------------------------------------------------------
mafString mafVMEExternalData::GetAbsoluteFileName()
{
  mafString filename=this->CurrentPath;
  filename.Append(this->GetFileName());

  return filename;
}

//-------------------------------------------------------------------------
void mafVMEExternalData::GetAbsoluteFileName(char *fname)
{
  mafString::Copy(fname,this->GetAbsoluteFileName().GetCStr());
}

//-------------------------------------------------------------------------
void mafVMEExternalData::SetMimeType(const char *mimetype)
{
  mafTagItem item;
  item.SetName("EXTDATA_MIMETYPE");
  item.SetValue(mimetype);
  this->GetTagArray()->SetTag(item);
}
 
//-------------------------------------------------------------------------
const char *
mafVMEExternalData::GetMimeType()
{
  mafTagItem *item=this->GetTagArray()->GetTag("EXTDATA_MIMETYPE");
  return item->GetValue();  
}
 
//-------------------------------------------------------------------------
void mafVMEExternalData::PrintSelf(std::ostream& os,const int indent)
{
  mafVME::Print(os,indent);
  os << indent << "CurrentPath: "  << this->CurrentPath.GetCStr() << "\n";
  os << indent << "FileName: " << this->GetFileName() << "\n";
  os << indent << "File Extension: " << this->GetExtension() << "\n" ;
	os << indent << "MIME Type: " << this->GetMimeType() << "\n";
}
  
#endif