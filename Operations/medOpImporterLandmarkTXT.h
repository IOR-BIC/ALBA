/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpImporterLandmarkTXT.h,v $
  Language:  C++
  Date:      $Date: 2008-04-28 08:40:14 $
  Version:   $Revision: 1.1 $
  Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpImporterLandmarkTXT_H__
#define __medOpImporterLandmarkTXT_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMELandmarkCloud;  
class mafEvent;
//----------------------------------------------------------------------------
// medOpImporterLandmarkTXT :
//----------------------------------------------------------------------------
/** */
class medOpImporterLandmarkTXT : public mafOp
{
public:
	medOpImporterLandmarkTXT(wxString label);
	~medOpImporterLandmarkTXT(); 
	mafOp* Copy();
  virtual void OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node) {return true;};

	/** Builds operation's interface. */
	void OpRun();

  /** Read the file.
  the format of the file admits some speficics.
  1) Ignore the first line
  2) Ignore the first n columns specified by user
  3) the first element of each line is Time, then x y z x1 y1 z1 ecco
  */
  void Read();

  /** Set the filename for the file to import */
  void SetFileName(const char *file_name){m_File = file_name;};

  /** Set number of column to skip */
  void SetSkipColumn(int column);

protected:
  wxString m_FileDir;
	wxString m_File;
	mafVMELandmarkCloud  *m_VmeCloud;
  int m_Start;
};
#endif
