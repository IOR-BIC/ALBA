/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpValidateTree.cpp,v $
  Language:  C++
  Date:      $Date: 2008-03-14 13:21:29 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafOpValidateTree.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mmgGui.h"

#include "mafStorage.h"
#include "mafEventIO.h"
#include "mafDataVector.h"
#include "mafVMEItem.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMERoot.h"
#include "mafNodeIterator.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpValidateTree);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpValidateTree::mafOpValidateTree(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_MSFPath = "";
}
//----------------------------------------------------------------------------
mafOpValidateTree::~mafOpValidateTree()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpValidateTree::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpValidateTree(m_Label);
}
//----------------------------------------------------------------------------
void mafOpValidateTree::OpRun()
//----------------------------------------------------------------------------
{
  if (ValidateTree())
  {
    wxMessageBox(_("Tree validation terminated succesfully!!."), _("Info"));
  }
  else
  {
    wxMessageBox(_("Tree invalid!! In log area you can find details."), _("Warning"));
  }
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
bool mafOpValidateTree::ValidateTree()
//----------------------------------------------------------------------------
{
  bool result = true;
  mafNode *node, *root = m_Input->GetRoot();
  mafNodeIterator *iter = root->NewIterator();
  try
  {
    for (node = iter->GetFirstNode(); node; node = iter->GetNextNode())
    {
      // check node ID
      bool valid = node->IsValid();
      if (!valid && !node->IsMAFType(mafVMERoot))
      {
        ErrorLog(mafOpValidateTree::INVALID_NODE, node->GetName());
        result = false;
      }
      int numLinks = node->GetNumberOfLinks();
      if (numLinks > 0)
      {
        // check node links
        mafNode::mafLinksMap::iterator lnk_it;
        for (lnk_it = node->GetLinks()->begin(); lnk_it != node->GetLinks()->end(); lnk_it++)
        {
          if (lnk_it->second.m_Node == NULL)
          {
            ErrorLog(mafOpValidateTree::LINK_NULL, node->GetName(), lnk_it->first);
            result = false;
            continue;
          }
          valid = root->IsInTree(lnk_it->second.m_Node);
          if (!valid)
          {
            ErrorLog(mafOpValidateTree::LINK_NOT_PRESENT, lnk_it->second.m_Node->GetName());
            result = false;
          }
          valid = lnk_it->second.m_Node->IsValid();
          if (!valid && !lnk_it->second.m_Node->IsMAFType(mafVMERoot))
          {
            ErrorLog(mafOpValidateTree::INVALID_NODE, lnk_it->second.m_Node->GetName());
            result = false;
          }
        }
      }
      // check data (binary data files are associated to that file that 
      // have the DataVector => inherit from mafVMEGenericAbstract
      mafString urlString = "";
      mafString archiveFilename = "";
      mafString absFilename = "";
      mafVMEItem *item = NULL;
      bool singleFileMode = false;
      mafVMEGenericAbstract *vme = mafVMEGenericAbstract::SafeDownCast(node);
      if (vme)
      {
        mafDataVector *dv = vme->GetDataVector();
        if (dv)
        {
          for (int t = 0; t < dv->GetNumberOfItems(); t++)
          {
            if (m_MSFPath.IsEmpty())
            {
              mafEventIO e(this,NODE_GET_STORAGE);
              vme->ForwardUpEvent(e);
              mafStorage *storage = e.GetStorage();
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
              ErrorLog(mafOpValidateTree::ITEM_NOT_PRESENT, vme->GetName());
              result = false;
            }
            else
            {
              urlString = item->GetURL();
              if (urlString.IsEmpty())
              {
                ErrorLog(mafOpValidateTree::URL_EMPTY, vme->GetName());
                result = false;
              }
              archiveFilename = item->GetArchiveFileName();
              singleFileMode = dv->GetSingleFileMode();
              if (singleFileMode && archiveFilename.IsEmpty())
              {
                ErrorLog(mafOpValidateTree::ARCHIVE_FILE_NOT_PRESENT, vme->GetName());
                result = false;
              }
              absFilename = m_MSFPath;
              absFilename << "/";
              if (singleFileMode)
              {
                absFilename << archiveFilename;
              }
              else
              {
                absFilename << urlString;
              }
              if (!wxFileExists(absFilename.GetCStr()))
              {
                ErrorLog(mafOpValidateTree::BINARY_FILE_NOT_PRESENT, absFilename.GetCStr());
                result = false;
              }
            } // item != NULL
          } // for() on items
        } // if(dv)
      } // if(vme) vme is inherited from a mafVMEGenericAbstract => has binary data file
    } // for () on node into the tree
  } // try
  catch (...)
  {
    ErrorLog(mafOpValidateTree::EXCEPTION_ON_ITERATOR, iter->GetCurrentNode()->GetName());
  }
  iter->Delete();
  return result;
}
//----------------------------------------------------------------------------
void mafOpValidateTree::ErrorLog(int error_num, const char *node_name, const char *description)
//----------------------------------------------------------------------------
{
  switch(error_num)
  {
    case mafOpValidateTree::INVALID_NODE:
      mafLogMessage(_("node %s has an invalid ID"), node_name);
    break;
    case mafOpValidateTree::LINK_NOT_PRESENT:
      mafLogMessage(_("link node %s is not present into the tree"), node_name);
    break;
    case mafOpValidateTree::LINK_NULL:
      mafLogMessage(_("link %s of node %s is null"), description, node_name);
    break;
    case mafOpValidateTree::EXCEPTION_ON_ITERATOR:
      mafLogMessage(_("exception occurred during iteration on node %s"), node_name);
    break;
    case mafOpValidateTree::ITEM_NOT_PRESENT:
      mafLogMessage(_("Item not present in node %s"), node_name);
    break;
    case mafOpValidateTree::URL_EMPTY:
      mafLogMessage(_("URL referring to binary data not present in node %s"), node_name);
    break;
    case mafOpValidateTree::BINARY_FILE_NOT_PRESENT:
      mafLogMessage(_("Binary data file '%s' not present"), node_name);
    break;
    case mafOpValidateTree::ARCHIVE_FILE_NOT_PRESENT:
      mafLogMessage(_("Archive data file not present for node %s"), node_name);
    break;
  }
}
