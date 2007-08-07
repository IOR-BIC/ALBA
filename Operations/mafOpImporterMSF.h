/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpImporterMSF.h,v $
  Language:  C++
  Date:      $Date: 2007-08-07 08:15:58 $
  Version:   $Revision: 1.1 $
  Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpImporterMSF_H__
#define __mafOpImporterMSF_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// class forwarding:
//----------------------------------------------------------------------------
class mafVMEGroup;

//----------------------------------------------------------------------------
// mafOpImporterMSF :
//----------------------------------------------------------------------------
/** Import an existing MSF or ZMSF file while working to another one, to merge or compare data. */
class mafOpImporterMSF: public mafOp 
{
public:
  mafOpImporterMSF(const wxString &label = "MSFImporter");
  ~mafOpImporterMSF(); 

  mafTypeMacro(mafOpImporterMSF, mafOp);

  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* node) {return true;};

  /** Builds operation's interface. */
  void OpRun();

  /** Import MSF data. */
  int ImportMSF();

  /** Set the MSF filename to be imported. 
  This is used when the operation is executed not using user interface. */
  void SetFileName(const char *name) {m_File = name;};

  /** Open the compressed zmsf filename. */
  const char *ZIPOpen(mafString filename);

  /** Remove temporary directory used for compressed msf files.*/
  void RemoveTempDirectory();

protected:
  mafString         m_File;
  mafString         m_TmpDir;

  mafVMEGroup      *m_Group;
  mafString         m_MSFFile;
};
#endif
