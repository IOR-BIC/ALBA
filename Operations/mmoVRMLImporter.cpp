/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoVRMLImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2008-01-24 16:25:29 $
  Version:   $Revision: 1.11 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmoVRMLImporter.h"

#include "mafDecl.h"
#include "mmgDialog.h"

#include "mmaMaterial.h"
#include "mafTagArray.h"
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
mafCxxTypeMacro(mmoVRMLImporter);
//----------------------------------------------------------------------------

/** 
  VRML is a text file format where, e.g., vertices and edges for a 3D polygon can be specified along 
	with the surface color, image-mapped textures, shininess, transparency, and so on. 
	
	example:
	-------------------------------
	#VRML V2.0 utf8
	
	#example
	
	Transform {
	  children	Shape {
	    appearance	Appearance {
	      material	DEF _0 Material {
	      }
	
	    }
	
	    geometry	Sphere {
	    }
	
	  }
	
	  translation	-1.31076 1 -1
	}
	Transform {
	  children	Shape {
	    appearance	Appearance {
	      material	USE _0
	
	    }
	
	    geometry	Cylinder {
	    }
	
	  }
	
	  translation	1.30622 1 -1
	}
	Transform {
	  children	Shape {
	    appearance	Appearance {
	      material	USE _0
	
	    }
	
	    geometry	Cone {
	    }
	
	  }
	
	  translation	-2.27822 1 2.49587
	}
	Transform {
	  children	Shape {
	    appearance	Appearance {
	      material	USE _0
	
	    }
	
	    geometry	Box {
	    }
	
	  }
	
	  translation	2.87718 1 3
	}
	Background {
	  groundAngle	1.57079
	  groundColor	[ 1 0.8 0.6,
		    0.6 0.4 0.2 ]
	  skyAngle	[ 0.2, 1.57079 ]
	  skyColor	[ 1 1 0,
		    1 1 1,
		    0.2 0.2 1 ]
	}
	Viewpoint {
	  position	-8.91322e-009 2.69392 9.61694
	  orientation	-1 -5.48858e-009 -3.84749e-010  0.23922
	  fieldOfView	0.785398
	  description	"start"
	}
*/
//----------------------------------------------------------------------------
 mmoVRMLImporter:: mmoVRMLImporter(const wxString &label) : mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_File		= "";
	m_Canundo	= true;
	m_Group		= NULL;

 	m_FileDir = "";//mafGetApplicationDirectory().c_str();
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
	mafString vrml_wildc	= "VRML Data (*.wrl)|*.wrl";
  mafString f;

  if (m_File.IsEmpty())
  {
    f = mafGetOpenFile(m_FileDir.GetCStr(),vrml_wildc.GetCStr()).c_str(); 	
    m_File = f;
  }

  int result = OP_RUN_CANCEL;

  if(!m_File.IsEmpty()) 
  {
    result = OP_RUN_OK;
    ImportVRML();
  }

  mafEventMacro(mafEvent(this,result));
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
  wxSplitPath(m_File.GetCStr(),&path,&name,&ext);

  mafNEW(m_Group);
  m_Group->SetName(name.c_str());

  vtkMAFSmartPointer<vtkRenderWindow> rw;

  vtkMAFSmartPointer<vtkVRMLImporter> importer;
  importer->SetRenderWindow(rw.GetPointer());
  importer->SetFileName(m_File.GetCStr());
  importer->Update();

  mafMatrix matrix;

  vtkRendererCollection *rc = rw->GetRenderers();
  rc->InitTraversal();
  vtkRenderer *ren = rc->GetNextItem(); 

  vtkActorCollection *ac = ren->GetActors();
  int num_actors = ac->GetNumberOfItems();
  ac->InitTraversal();

  if(!m_TestMode)
  {
    wxMessageBox("MAF VRML does  not support all entities these standard allows to encode, because some were\nconsidered not relevant and did not may to MAF data model.");
  }
  
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
      data->Update();
      if(data->GetNumberOfPolys() != 0)
      {
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
        mafTagItem tag_Nature;
        tag_Nature.SetName("VME_NATURE");
        tag_Nature.SetValue("NATURAL");
        surface->GetTagArray()->SetTag(tag_Nature);
      }
      
    }
  }
  m_Output = m_Group;
}
