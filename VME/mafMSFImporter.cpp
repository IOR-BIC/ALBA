/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMSFImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-07 17:26:44 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafIncludeWX.h" // to be removed

#include "mafMSFImporter.h"
#include "mafVMERoot.h"
#include "mmuUtility.h"
#include "mafStorable.h"
#include "mafStorageElement.h"

//------------------------------------------------------------------------------
// mmuMSFDocument
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int mmuMSFDocument::InternalStore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  mafErrorMessage("Writing MSF 1.x files is not supported!");
  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mmuMSFDocument::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------
{
  // here should restore elements specific for the document
  int ret = node->RestoreObject("Root",m_Root);

  //TO BE ADDED HERE the code to import the file from old MSF file


  if (ret==MAF_OK)
  {
    return m_Root->Initialize();
  }

  return MAF_ERROR; 
}

//------------------------------------------------------------------------------
// mafMSFImporter
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafMSFImporter)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafMSFImporter::mafMSFImporter()
//------------------------------------------------------------------------------
{
  SetVersion("1.0");
  SetFileType("MSF");
  SetDocument(new mmuMSFDocument); // create a MSF doc
  
}

//------------------------------------------------------------------------------
mafMSFImporter::~mafMSFImporter()
//------------------------------------------------------------------------------
{
  cppDEL(m_Document); // delete the document object
}

//------------------------------------------------------------------------------
void mafMSFImporter::SetRoot(mafVMERoot *root)
//------------------------------------------------------------------------------
{
  ((mmuMSFDocument *)m_Document)->SetRoot(root);
}

//------------------------------------------------------------------------------
mafVMERoot *mafMSFImporter::GetRoot()
//------------------------------------------------------------------------------
{
  return ((mmuMSFDocument *)m_Document)->GetRoot();
}

