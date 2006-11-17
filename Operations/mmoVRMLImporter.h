/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoVRMLImporter.h,v $
  Language:  C++
  Date:      $Date: 2006-11-17 11:49:13 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoVRMLImporter_H__
#define __mmoVRMLImporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVMEGroup;

//----------------------------------------------------------------------------
//mmoVRMLImporter :
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
	
	-------------------------------
	
	VRML files are commonly called worlds and have the .wrl extension. 
	Although VRML worlds use a text format they may often be compressed using gzip so 
	that they transfer over the internet more quickly. 
	This modality in not supported by maf vrml importer, it can only import uncompressed wrl.

*/
class mmoVRMLImporter: public mafOp
{
public:
           mmoVRMLImporter(wxString label = "VRMLImporter");
	virtual ~mmoVRMLImporter();
	
  mafTypeMacro(mmoVRMLImporter, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node) {return true;};

	/** Builds operation's interface. */
  void OpRun();

  /** Set the filename for the .vrl to import */
  void SetFileName(const char *file_name);

  /** Import vrml data. */
  void ImportVRML();

protected:
	mafString m_File;
	mafString m_FileDir;
  
	mafVMEGroup *m_Group;
};
#endif
