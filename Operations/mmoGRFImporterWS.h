/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoGRFImporterWS.h,v $
  Language:  C++
  Date:      $Date: 2007-10-11 11:22:08 $
  Version:   $Revision: 1.8 $
  Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoGRFImporterWS_H__
#define __mmoGRFImporterWS_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEVector;
class mafVMESurface;

//----------------------------------------------------------------------------
// mmoGRFImporterWS :
//----------------------------------------------------------------------------
/** */
class mmoGRFImporterWS : public mafOp
{
public:
	mmoGRFImporterWS(const wxString &label = "GRF Importer");
	~mmoGRFImporterWS(); 

  void mmoGRFImporterWS::OpDo();

  void mmoGRFImporterWS::OpUndo();

	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node) {return true;};

	/** Builds operation's interface. */
	void OpRun();

  /** Read the file.  */
  void Read();

  /** Set the filename for the file to import */
  void SetFileName(const char *file_name){m_File = file_name;};

protected:

  mafVMESurface       *m_PlatformLeft;
  mafVMESurface       *m_PlatformRight;
  mafVMEVector        *m_VectorLeft;
  mafVMEVector        *m_VectorRight;

  wxString             m_FileDir;
	wxString             m_File;

};
#endif
