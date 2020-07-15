/*=========================================================================
Program:   Bonemat
Module:    albaOpImporterPointCloud.h
Language:  C++
Date:      $Date: 2010-11-23 16:50:26 $
Version:   $Revision: 1.1.1.1.2.3 $
Authors:   Gianluigi Crimi
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpImporterPointCloud_H__
#define __albaOpImporterPointCloud_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include "albaTextFileReaderHelper.h"
#include "albaDefines.h"


//----------------------------------------------------------------------------
// albaOpImporterPointCloud :
//----------------------------------------------------------------------------
/** 
Reads scalars from a .DIC file
*/
class albaOpImporterPointCloud : public albaTextFileReaderHelper, public albaOp
{
public:

	albaOpImporterPointCloud(const wxString &label = "Load Point Cloud");
	~albaOpImporterPointCloud(); 

	albaTypeMacro(albaOpImporterPointCloud, albaOp);

	/** Return true for the acceptable vme type. */
	bool Accept(albaVME *node);

	/** Builds operation's interface. */
	void OpRun();

	albaOp* Copy();

	/** Create the dialog interface for the exporter. */
	virtual void CreateGui();


	/** Event management */
	virtual void OnEvent(albaEventBase *alba_event);



protected:

	/** imports informations */
	int Import(void);
	

	/** Enables*Disables Gui Components */
	void EnableDisableGui();

	int m_FirstCoordCol;
	int m_ScalarsNum;
	int m_ScalarCol[3];
	albaString m_ScalarNames[3];

	albaString m_CommentLine;
	albaString m_FileName;
  	
};
#endif
