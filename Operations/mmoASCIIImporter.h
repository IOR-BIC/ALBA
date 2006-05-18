/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoASCIIImporter.h,v $
  Language:  C++
  Date:      $Date: 2006-05-18 10:29:00 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoASCIIImporter_H__
#define __mmoASCIIImporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;

//----------------------------------------------------------------------------
// mmoASCIIImporter :
//----------------------------------------------------------------------------
/** */
class mmoASCIIImporter: public mafOp
{
public:
  mmoASCIIImporter(wxString label);
 ~mmoASCIIImporter(); 
 
  virtual void OnEvent(mafEventBase *maf_event);
  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode* node) {return true;};

	/** Builds operation's interface. */
  void OpRun();

	/** Import ASCII data. */
  void ImportASCII();

  /** Add ASCII filename to the files list to be imported. 
  This is used when the operation is executed not using user interface. */
  void AddFileName(std::string &file);

  /** Set the ASCII filename to be imported. 
  This is used when the operation is executed not using user interface. */
  void SetFileName(std::string &file);

  /** Set the order of the ASCII data inside the file/s.
  This is used when the operation is executed not using user interface. */
  void SetDataOrder(int order = DATA_ORDER_ROW) {m_DataOrder = order;};

  enum ASCII_DATA_ORDER
  {
    DATA_ORDER_ROW = 0,
    DATA_ORDER_COLUMN,
    DATA_ORDER_MATRIX
  };

protected:
  std::vector<std::string> m_Files;
  wxString m_FileDir;

  int      m_DataOrder;
};
#endif
