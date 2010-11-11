/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpExporterMesh.h,v $
  Language:  C++
  Date:      $Date: 2010-11-11 15:55:57 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Simone Brazzale   
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpExporterMesh_H__
#define __mafOpExporterMesh_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMEMesh;

//----------------------------------------------------------------------------
// mafOpExporterMesh :
//----------------------------------------------------------------------------
/** Export a Generic mesh */
class mafOpExporterMesh: public mafOp
{
public:
	mafOpExporterMesh(const wxString &label = "MeshExporter");
	~mafOpExporterMesh(); 
	
  mafTypeMacro(mafOpExporterMesh, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

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

  mafString	m_FileDir;
  mafString m_File;

  /** Nodes file name*/
  mafString m_NodesFileName;

  /** Elements file name*/
  mafString m_ElementsFileName;

  /** Materials file name*/
  mafString m_MaterialsFileName;
};
#endif
