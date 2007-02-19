/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoLandmarkImporterTXT.h,v $
  Language:  C++
  Date:      $Date: 2007-02-19 15:12:38 $
  Version:   $Revision: 1.2 $
  Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoLandmarkImporterTXT_H__
#define __mmoLandmarkImporterTXT_H__

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
// mmoLandmarkImporterTXT :
//----------------------------------------------------------------------------
/** */
class mmoLandmarkImporterTXT : public mafOp
{
public:
	mmoLandmarkImporterTXT(wxString label);
	~mmoLandmarkImporterTXT(); 
	mafOp* Copy();
  virtual void OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node) {return true;};

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop	(int result);

  /** Read the file.
  the format of the file admits some speficics.
  1) Ignore the first line
  2) Ignore the first n columns specified by user
  3) the first element of each line is Time, then x y z x1 y1 z1 ecc
  */
  void Read();


  /** Set the filename for the file to import */
  void SetFileName(const char *file_name){m_File = file_name;};

  /** Set number of column to skip */
  void SetSkipColumn(int column);


	/** Makes the undo for the operation. */
	//void OpUndo();                       // gia' implementata in mafOp

protected:
  wxString m_FileDir;
	wxString m_File;
	mafVMELandmarkCloud  *m_VmeCloud;
  int m_Start;

};
#endif
