/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoGRFImporterWS.h,v $
  Language:  C++
  Date:      $Date: 2007-04-10 09:35:51 $
  Version:   $Revision: 1.3 $
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
#include <vnl\vnl_matrix.h>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafEvent;
class mafVMEVector;
class vtkCubeSource;
class mafVMESurface;
//class mafVMEScalar;

//----------------------------------------------------------------------------
// mmoGRFImporterWS :
//----------------------------------------------------------------------------
/** */
class mmoGRFImporterWS : public mafOp
{
public:
	mmoGRFImporterWS(wxString label);
	~mmoGRFImporterWS(); 

  void mmoGRFImporterWS::OpDo();

  void mmoGRFImporterWS::OpUndo();


	mafOp* Copy();
  virtual void OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node) {return true;};

	/** Builds operation's interface. */
	void OpRun();

  /** Read the file.
  the format of the file admits some specifics.
  1) Ignore the first line
  2) Read the second line containing the Frequency
  2) Ignore 8 lines
  3) the first element of each line is Time, then n scalar values
  4) Put the values in a mafVMEScalar item 
  */
  void Read();

  /** Set the filename for the file to import */
  void SetFileName(const char *file_name){m_File = file_name;};

protected:

  mafVMESurface       *m_PlatformLeft;
  mafVMESurface       *m_PlatformRight;
  mafVMEVector        *m_VectorLeft;
  mafVMEVector        *m_VectorRight;
  mafVMELandmarkCloud *m_AlCop;

  vnl_matrix<double>   m_Grf_matrix;
  wxString             m_FileDir;
	wxString             m_File;

};
#endif
