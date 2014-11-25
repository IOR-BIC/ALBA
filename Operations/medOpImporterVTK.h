/*=========================================================================

 Program: MAF2Medical
 Module: medOpImporterVTK
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpImporterVTK_H__
#define __medOpImporterVTK_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOpImporterVTK.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class medVMEPolylineGraph;

/**
class name: medOpImporterVTK
The same importer of the MAF , but polylines are stored inside 
the medVMEPolylineGraph instead mafVMEPolyline*/
class MAF_EXPORT medOpImporterVTK: public mafOpImporterVTK 
{
public:
  /** constructor */
  medOpImporterVTK(const wxString &label = "medVTKImporter");
  /** destructor */
  ~medOpImporterVTK(); 

  /** RTTI macro*/
  mafTypeMacro(medOpImporterVTK, mafOpImporterVTK);

  /** clone the current object */
  mafOp* Copy();


  /** Import vtk data. */
  virtual int ImportVTK();

  /** retrieve the file name*/
  wxString GetFileName(){return m_File;};

protected:

  medVMEPolylineGraph   *m_VmePolyLine;
};
#endif
