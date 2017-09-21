/*=========================================================================

 Program: MAF2
 Module: mafOpExporterMesh
 Authors: Simone Brazzale, Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpExporterMesh_H__
#define __mafOpExporterMesh_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOpExporterFEMCommon.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMEMesh;

//----------------------------------------------------------------------------
// mafOpExporterMesh :
//----------------------------------------------------------------------------
/** Export a Generic mesh */
class MAF_EXPORT mafOpExporterMesh: public mafOpExporterFEMCommon
{
public:
	mafOpExporterMesh(const wxString &label = "MeshExporter");
	~mafOpExporterMesh(); 
	
  mafTypeMacro(mafOpExporterMesh, mafOpExporterFEMCommon);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafVME*node);

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
	
	virtual void OnEvent(mafEventBase *maf_event);

	/** Export the mesh*/
	int Write();

protected:
	/** Create the dialog interface for the importer. */
	virtual void CreateGui();

	void OpStop(int result);

	void OnOK();

	mafString GetWildcard();
  
	wxString m_FileDir;
  mafString m_File;

  /** Nodes file name*/
  mafString m_NodesFileName;

  /** Elements file name*/
  mafString m_ElementsFileName;

  /** Materials file name*/
  mafString m_MaterialsFileName;
};
#endif
