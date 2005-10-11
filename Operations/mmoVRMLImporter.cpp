/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoVRMLImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2005-10-11 13:01:05 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mmoVRMLImporter.h"

#include "mafDecl.h"
#include "mmgDialog.h"

#include "mmaMaterial.h"
#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMEGroup.h"
#include "mafMatrixVector.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkVRMLImporter.h"
#include "vtkRendererCollection.h"

//----------------------------------------------------------------------------
 mmoVRMLImporter:: mmoVRMLImporter(wxString label) : mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_File		= "";
	m_Canundo	= true;
	m_Group		= NULL;

 	m_FileDir = mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
 mmoVRMLImporter::~ mmoVRMLImporter()
//----------------------------------------------------------------------------
{
  mafDEL(m_Group);
}	
//----------------------------------------------------------------------------
mafOp * mmoVRMLImporter::Copy()
//----------------------------------------------------------------------------
{
  mmoVRMLImporter *cp = new  mmoVRMLImporter(m_Label);
	cp->m_File		= m_File;
  cp->m_FileDir = m_FileDir;
  return cp; 
}
//----------------------------------------------------------------------------
void  mmoVRMLImporter::OpRun()   
//----------------------------------------------------------------------------
{
	wxString vrml_wildc	= "VRML Data (*.wrl)|*.wrl";

	wxString f = mafGetOpenFile(m_FileDir,vrml_wildc).c_str(); 
	if(f == "")
	{
		mafEventMacro(mafEvent(this,OP_RUN_CANCEL));
    return;
	}
	m_File = f;
  ImportVRML();
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void  mmoVRMLImporter::SetFileName(const char *file_name)
//----------------------------------------------------------------------------
{
  m_File = file_name;
}
//----------------------------------------------------------------------------
void  mmoVRMLImporter::ImportVRML()
//----------------------------------------------------------------------------
{
  wxString path, name, ext;
  wxSplitPath(m_File.c_str(),&path,&name,&ext);

  mafNEW(m_Group);
  m_Group->SetName(name.c_str());

  vtkMAFSmartPointer<vtkRenderWindow> rw;

  vtkMAFSmartPointer<vtkVRMLImporter> importer;
  importer->SetRenderWindow(rw.GetPointer());
  importer->SetFileName(m_File.c_str());
  importer->Update();

  mafMatrix matrix;

  vtkRendererCollection *rc = rw->GetRenderers();
  rc->InitTraversal();
  vtkRenderer *ren = rc->GetNextItem(); 

  vtkActorCollection *ac = ren->GetActors();
  int num_actors = ac->GetNumberOfItems();
  ac->InitTraversal();

  for (int i = 0; i < num_actors; i++)
  {
    vtkActor *actor = ac->GetNextActor();
    matrix.DeepCopy(actor->GetMatrix());

    if (actor->GetMapper() != NULL && actor->GetMapper()->GetInput() != NULL)
    {
      wxString name;
      name.Printf("surface_%d", i);

      mafTimeStamp t;
      t = ((mafVME *)m_Input)->GetTimeStamp();
      mafSmartPointer<mafVMESurface> surface;
      surface->SetName(name.c_str());
      vtkPolyData *data = (vtkPolyData *)actor->GetMapper()->GetInput();
      surface->SetData(data,t);

      double rgb[3];
      actor->GetProperty()->GetColor(rgb);
      mmaMaterial *m = surface->GetMaterial();
      m->m_Diffuse[0] = rgb[0];
      m->m_Diffuse[1] = rgb[1];
      m->m_Diffuse[2] = rgb[2];
      m->UpdateProp();

      surface->GetMatrixVector()->AppendKeyMatrix(matrix);
      surface->ReparentTo(m_Group);
    }
  }
  m_Output = m_Group;
}
