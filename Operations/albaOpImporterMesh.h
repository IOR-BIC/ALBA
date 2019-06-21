/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterMesh
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterMesh_H__
#define __albaOpImporterMesh_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVMEMesh;
class albaEvent;

//----------------------------------------------------------------------------
// albaOpImporterMesh :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpImporterMesh: public albaOp
{
public:
	albaOpImporterMesh(const wxString &label = "MeshImporter");
	~albaOpImporterMesh(); 
	
  albaTypeMacro(albaOpImporterMesh, albaOp);

  virtual void OnEvent(albaEventBase *alba_event);

  albaOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(albaVME *node);

	/** Set the filename for the mesh to import */
  void SetFileName(const char *file_name);

  /** Set/Get nodes file name*/
  void SetNodesFileName(const char *name)   {this->m_NodesFileName = name;};
  const char *GetNodesFileName() {return this->m_NodesFileName;};

  /** Set/Get elements file name*/
  void SetElementsFileName(const char *name)   {this->m_ElementsFileName = name;};
  const char *GetElementsFileName() {return this->m_ElementsFileName;};

  void SetMode(int mode){}

  /** Builds operation's interface. */
	void OpRun();

	/** Import the mesh*/
	int Read();

protected:
  /** Create the dialog interface for the importer. */
  virtual void CreateGui();  

  int m_ImporterType;
	albaVMEMesh *m_ImportedVmeMesh;
  albaString	m_FileDir;

  /** Nodes file name*/
  albaString m_NodesFileName;

  /** Elements file name*/
  albaString m_ElementsFileName;
};
#endif
