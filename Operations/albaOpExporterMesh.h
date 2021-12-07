/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterMesh
 Authors: Simone Brazzale, Nicola Vanella, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExporterMesh_H__
#define __albaOpExporterMesh_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOpExporterFEMCommon.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVMEMesh;

//----------------------------------------------------------------------------
// albaOpExporterMesh :
//----------------------------------------------------------------------------
/** Export a Generic mesh */
class ALBA_EXPORT albaOpExporterMesh: public albaOpExporterFEMCommon
{
public:
	albaOpExporterMesh(const wxString &label = "MeshExporter");
	~albaOpExporterMesh(); 
	
  albaTypeMacro(albaOpExporterMesh, albaOpExporterFEMCommon);

  albaOp* Copy();

	/** Set the filename for the mesh to export */
  void SetFileName(const char *file_name) {this->m_File = file_name;};

  /** Set/Get nodes file name*/
  void SetNodesFileName(const char *name)   {this->m_NodesFileName = name;};
  const char *GetNodesFileName() {return this->m_NodesFileName;};

  /** Set/Get elements file name*/
  void SetElementsFileName(const char *name)   {this->m_ElementsFileName = name;};
  const char *GetElementsFileName() {return this->m_ElementsFileName;};

  /** Set/Get materials file name*/
  void SetMaterialsFileName(const char *name) {this->m_MaterialsFileName = name;};
  const char *GetMaterialsFileName() {return this->m_MaterialsFileName;};

  /** Builds operation's interface. */
	void OpRun();
		
	/** Export the mesh*/
	int Write();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	void OnOK();

	albaString GetWildcard();

	wxString m_FileDir;
  albaString m_File;

  /** Nodes file name*/
  albaString m_NodesFileName;

  /** Elements file name*/
  albaString m_ElementsFileName;

  /** Materials file name*/
  albaString m_MaterialsFileName;
};
#endif
