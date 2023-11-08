/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpValidateTree
 Authors: Paolo Quadrani , Stefano Perticoni
 
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


#include "albaOpValidateTree.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"

#include "albaStorage.h"
#include "albaEventIO.h"
#include "albaDataVector.h"
#include "albaVMEItem.h"
#include "albaVMEGenericAbstract.h"
#include "albaVMERoot.h"
#include "albaVMEIterator.h"
#include "albaVMEExternalData.h"
#include "vtkDirectory.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpValidateTree);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpValidateTree::albaOpValidateTree(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_MSFPath = "";
}
//----------------------------------------------------------------------------
albaOpValidateTree::~albaOpValidateTree()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
albaOp* albaOpValidateTree::Copy()   
//----------------------------------------------------------------------------
{
	return new albaOpValidateTree(m_Label);
}
//----------------------------------------------------------------------------
void albaOpValidateTree::OpRun()
//----------------------------------------------------------------------------
{
  int result = ValidateTree();
  if(m_TestMode == false)
  {
    if (result == albaOpValidateTree::VALIDATE_SUCCESS)
    {
      wxMessageBox(_("Tree validation terminated succesfully!!."), _("Info"));
    }
    else if (result == albaOpValidateTree::VALIDATE_WARNING)
    {
      wxMessageBox(_("Tree Patched!! In log area you can find details."), _("Warning"));
    }
    else
    {
      wxMessageBox(_("Tree invalid!! In log area you can find details."), _("Warning"));
    }
  }
  albaEventMacro(albaEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
int albaOpValidateTree::ValidateTree()
//----------------------------------------------------------------------------
{
  m_MSFTreeAbsFileNamesSet.clear();
  assert(m_MSFTreeAbsFileNamesSet.size() == 0);

  int result = albaOpValidateTree::VALIDATE_SUCCESS;

  albaVME *node;
  albaVMERoot *root = albaVMERoot::SafeDownCast(m_Input->GetRoot());
  assert(root != NULL);
  int max_item_id = root->GetMaxItemId();
  int max_node_id = root->GetMaxNodeId();

  albaVMEIterator *iter = root->NewIterator();
  try
  {
    for (node = iter->GetFirstNode(); node; node = iter->GetNextNode())
    {
      // check node ID
      bool valid = node->IsValid();
      if (!valid && !node->IsALBAType(albaVMERoot))
      {
        ErrorLog(albaOpValidateTree::INVALID_NODE, node->GetName());
        node->UpdateId();
        if (result != albaOpValidateTree::VALIDATE_ERROR)
        {
          result = albaOpValidateTree::VALIDATE_WARNING;
        }
      }
      int numLinks = node->GetNumberOfLinks();
      if (numLinks > 0)
      {
        // check node links
        albaVME::albaLinksMap::iterator lnk_it;
        for (lnk_it = node->GetLinks()->begin(); lnk_it != node->GetLinks()->end(); lnk_it++)
        {
          if (lnk_it->second.m_Node == NULL)
          {
            ErrorLog(albaOpValidateTree::LINK_NULL, node->GetName(), lnk_it->first);
            result = albaOpValidateTree::VALIDATE_ERROR;
            continue;
          }
          valid = root->IsInTree(lnk_it->second.m_Node);
          if (!valid)
          {
            ErrorLog(albaOpValidateTree::LINK_NOT_PRESENT, lnk_it->second.m_Node->GetName());
            result = albaOpValidateTree::VALIDATE_ERROR;
          }
          valid = lnk_it->second.m_Node->IsValid();
          if (!valid && !lnk_it->second.m_Node->IsALBAType(albaVMERoot))
          {
            ErrorLog(albaOpValidateTree::INVALID_NODE, lnk_it->second.m_Node->GetName());
            result = albaOpValidateTree::VALIDATE_ERROR;
          }
        }
      }
      // check data (binary data files are associated to that file that 
      // have the DataVector => inherit from albaVMEGenericAbstract
      albaString urlString = "";
      albaString archiveFilename = "";
      wxString absFilename = "";
      albaVMEItem *item = NULL;
      bool singleFileMode = false;
      albaVMEGenericAbstract *vme = albaVMEGenericAbstract::SafeDownCast(node);

      if (vme != NULL && vme->IsA("albaVMEExternalData"))
      {
        albaVMEExternalData *ed = albaVMEExternalData::SafeDownCast(vme);
        albaString fileName = ed->GetFileName();
        albaString extension = ed->GetExtension();

        if (m_MSFPath.IsEmpty())
        {
          albaEventIO e(this,NODE_GET_STORAGE);
          vme->ForwardUpEvent(e);
          albaStorage *storage = e.GetStorage();
          if (storage != NULL)
          {
            m_MSFPath = storage->GetURL();
            m_MSFPath.ExtractPathName();
          }
        }

        wxString absFilename = m_MSFPath;
        absFilename << "\\";
        absFilename << fileName;
        absFilename << ".";
        absFilename << extension;
        absFilename.Replace("/","\\");

        if (!wxFileExists(absFilename.ToAscii()))
        {
          ErrorLog(albaOpValidateTree::BINARY_FILE_NOT_PRESENT, absFilename.ToAscii());
          result = albaOpValidateTree::VALIDATE_ERROR;
        }
        else
        {
          m_MSFTreeAbsFileNamesSet.insert(absFilename);
        }
      }
      else if (vme != NULL ) // any other vme type
      {         
				albaVMEGenericAbstract *absVME = albaVMEGenericAbstract::SafeDownCast(vme);
				
        albaDataVector *dv = vme->GetDataVector();
        if (dv && (!absVME || absVME->GetStoreDataVector()) )
        {
          for (int t = 0; t < dv->GetNumberOfItems(); t++)
          {
            if (m_MSFPath.IsEmpty())
            {
              albaEventIO e(this,NODE_GET_STORAGE);
              vme->ForwardUpEvent(e);
              albaStorage *storage = e.GetStorage();
              if (storage != NULL)
              {
                m_MSFPath = storage->GetURL();
                m_MSFPath.ExtractPathName();
              }
            }
            // binary files exists => VME is animated on data
            item = dv->GetItemByIndex(t);
            if (item == NULL)
            {
              ErrorLog(albaOpValidateTree::ITEM_NOT_PRESENT, vme->GetName());
              result = albaOpValidateTree::VALIDATE_ERROR;
            }
            else
            {
              int item_id = item->GetId();
              if (max_item_id < item_id)
              {
                ErrorLog(albaOpValidateTree::MAX_ITEM_ID_PATCHED, vme->GetName());
                root->SetMaxItemId(item_id);
                if (result != albaOpValidateTree::VALIDATE_ERROR)
                {
                  result = albaOpValidateTree::VALIDATE_WARNING;
                }
              }

              urlString = item->GetURL();
              if (urlString.IsEmpty())
              {
                ErrorLog(albaOpValidateTree::URL_EMPTY, vme->GetName());
                result = albaOpValidateTree::VALIDATE_ERROR;
              }
              archiveFilename = item->GetArchiveFileName();
              singleFileMode = dv->GetSingleFileMode();
              if (singleFileMode && archiveFilename.IsEmpty())
              {
                ErrorLog(albaOpValidateTree::ARCHIVE_FILE_NOT_PRESENT, vme->GetName());
                result = albaOpValidateTree::VALIDATE_ERROR;
              }
              absFilename = m_MSFPath;
              absFilename << "\\";
              absFilename.Replace("/","\\");

              if (singleFileMode)
              {
                absFilename << archiveFilename;                
              }
              else
              {
                absFilename << urlString;
              }

              if (!wxFileExists(absFilename.ToAscii()))
              {
                ErrorLog(albaOpValidateTree::BINARY_FILE_NOT_PRESENT, absFilename.ToAscii());
                result = albaOpValidateTree::VALIDATE_ERROR;
              }
              else
              {
                m_MSFTreeAbsFileNamesSet.insert(absFilename.ToAscii());
              }
            } // item != NULL
          } // for() on items
        } // if(dv)
      } // if(vme) vme is inherited from a albaVMEGenericAbstract => has binary data file
    } // for () on node into the tree
  } // try
  catch (...)
  {
    ErrorLog(albaOpValidateTree::EXCEPTION_ON_ITERATOR, iter->GetCurrentNode()->GetName());
  }
  iter->Delete();
 
  return result;
}
//----------------------------------------------------------------------------
void albaOpValidateTree::ErrorLog(int error_num, const char *node_name, const char *description)
//----------------------------------------------------------------------------
{
  switch(error_num)
  {
    case albaOpValidateTree::INVALID_NODE:
      (m_TestMode==false)?albaLogMessage(_("Patched Node '%s' with an invalid ID!!"), node_name):printf(_("Patched Node '%s' with an invalid ID!!"), node_name);
    break;
    case albaOpValidateTree::LINK_NOT_PRESENT:
      albaLogMessage(_("Link node '%s' is not present into the tree"), node_name);
    break;
    case albaOpValidateTree::LINK_NULL:
      albaLogMessage(_("Link '%s' of node '%s' is null"), description, node_name);
    break;
    case albaOpValidateTree::EXCEPTION_ON_ITERATOR:
      albaLogMessage(_("Exception occurred during iteration on node '%s'"), node_name);
    break;
    case albaOpValidateTree::ITEM_NOT_PRESENT:
      albaLogMessage(_("Item not present in node '%s'"), node_name);
    break;
    case albaOpValidateTree::MAX_ITEM_ID_PATCHED:
      albaLogMessage(_("Item of Node '%s' has caused Max item Id to be patched!!"), node_name);
    break;
    case albaOpValidateTree::URL_EMPTY:
      albaLogMessage(_("URL referring to binary data not present in node '%s'"), node_name);
    break;
    case albaOpValidateTree::BINARY_FILE_NOT_PRESENT:
      albaLogMessage(_("Binary data file '%s' not present"), node_name);
    break;
    case albaOpValidateTree::ARCHIVE_FILE_NOT_PRESENT:
      albaLogMessage(_("Archive data file not present for node '%s'"), node_name);
    break;
  }
}

int albaOpValidateTree::GetMSFTreeABSFileNamesSet( set<wxString> &fileNamesSet )
{
  int result = this->ValidateTree(); // needed to fill m_MSFTreeBinaryFilesSet ivar
  
  if (result != albaOpValidateTree::VALIDATE_SUCCESS)
  {
    fileNamesSet.clear();
    albaLogMessage("MSF Tree is invalid: GetMSFTreeABSFileNamesSet returning an empty set");
    return ALBA_ERROR;
  }
 
  fileNamesSet = m_MSFTreeAbsFileNamesSet;
  return ALBA_OK;
 
}