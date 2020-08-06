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
#include "albaString.h"


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

	/** Get File Name */
	albaString GetFileName() const { return m_FileName; }
	/**Set File Name */
	void SetFileName(albaString val) { m_FileName = val; }

	/**Get Comment Line */
	albaString GetCommentLine() const { return m_CommentLine; }
	/**Set Comment Line */
	void SetCommentLine(albaString val) { m_CommentLine = val; }
	
	/** Get Scalar Name */
	albaString GetScalarName(int pos) const;
	/** Set Scalar Name */
	void SetScalarName(int pos, albaString val);
	
	/** Get First the coordinate of the first Column */
	int GetFirstCoordColumn() const { return m_FirstCoordCol; }
	/** Get First the coordinate of the first Column */
	void SetFirstCoordColumn(int val) { m_FirstCoordCol = val; }

	/** Get the number of scalars to read */
	int GetNumberOfScalars() const { return m_ScalarsNum; }
	
	/** Set the number of scalars to read */
	void SetNumberOfScalars(int val);

	int GetScalarColumn(int pos) const;
	void SetScalarColumn(int pos, int val);
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

	friend class albaOpImporterPointCloudTest;
  	
};
#endif
