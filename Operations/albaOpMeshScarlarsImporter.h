/*=========================================================================
Program:   Bonemat
Module:    albaOpMeshScarlarsImporter.h
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

#ifndef __albaOpMeshScarlarsImporter_H__
#define __albaOpMeshScarlarsImporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include "albaTextFileReaderHelper.h"
#include "vtkSystemIncludes.h"

class vtkFloatArray;


//----------------------------------------------------------------------------
// albaOpMeshScarlarsImporter :
//----------------------------------------------------------------------------
/** 
Reads Scalars from Lis files
*/
class ALBA_EXPORT albaOpMeshScarlarsImporter : public albaTextFileReaderHelper, public albaOp
{
public:

	albaOpMeshScarlarsImporter(const wxString &label = "Load Scalars from Lis files");
	~albaOpMeshScarlarsImporter(); 

	albaTypeMacro(albaOpMeshScarlarsImporter, albaOp);

	enum ScalarScopes {
		NODE_SCALARS,
		ELEMENT_SCALARS,
	};

	/** Builds operation's interface. */
	void OpRun();

	albaOp* Copy();

	/** imports Scalars from files */
	int Import(void);
	
	albaString GetLisScalarFileName() const { return m_LisScalarFile; }
	
	void SetLisScalarFileName(albaString val) { m_LisScalarFile = val; }
protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME *node);

	albaString m_LisScalarFile;

	ScalarScopes m_ScalarScope;

	std::vector<albaString> m_ScalarNames;
	std::vector<vtkFloatArray*> m_Arrays;
	std::vector<vtkIdType> m_Ids;

	void DeleteArrays();
	int ReadHeader();
	int ReadLine();
};
#endif
