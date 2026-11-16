/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaOpImporterPolyline.h
Language:  C++
Authors:   Gianluigi Crimi
==========================================================================
Copyright (c) BIC-IOR 2026 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpImporterPolyline_H__
#define __albaOpImporterPolyline_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include "albaTextFileReaderHelper.h"
#include "albaDefines.h"
#include "albaString.h"


//----------------------------------------------------------------------------
// albaOpImporterPolyline :
//----------------------------------------------------------------------------
/**
Reads a polyline from an ASCII text file with one point per row.
Points are connected in file order; an empty line closes the current polyline
and starts a new one, so a file can contain more than one polyline.
*/
class ALBA_EXPORT albaOpImporterPolyline : public albaTextFileReaderHelper, public albaOp
{
public:

	albaOpImporterPolyline(const wxString &label = "Load Polyline");
	~albaOpImporterPolyline();

	albaTypeMacro(albaOpImporterPolyline, albaOp);

	/** Builds operation's interface. */
	void OpRun();

	albaOp* Copy();

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

	/** Get the column of the first coordinate */
	int GetFirstCoordColumn() const { return m_FirstCoordCol; }
	/** Set the column of the first coordinate */
	void SetFirstCoordColumn(int val) { m_FirstCoordCol = val; }

	/** Get the number of scalars to read */
	int GetNumberOfScalars() const { return m_ScalarsNum; }

	/** Set the number of scalars to read */
	void SetNumberOfScalars(int val);

	int GetScalarColumn(int pos) const;
	void SetScalarColumn(int pos, int val);
protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** Create the dialog interface for the importer. */
	virtual void CreateGui();

	/** Enables*Disables Gui Components */
	void EnableDisableGui();

	/** imports informations */
	int Import(void);

	int m_FirstCoordCol;
	int m_ScalarsNum;
	int m_ScalarCol[3];
	albaString m_ScalarNames[3];

	albaString m_CommentLine;
	albaString m_FileName;

	friend class albaOpImporterPolylineTest;
	friend class albaOpExporterPolylineTest;

private:
	bool IsEmptyLine(char *line);
};
#endif
