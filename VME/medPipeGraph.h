/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeGraph.h,v $
  Language:  C++
  Date:      $Date: 2007-12-27 13:03:45 $
  Version:   $Revision: 1.9 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medPipeGraph_H__
#define __medPipeGraph_H__

#include "mafPipe.h"
#include "mafEvent.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkRectilinearGrid;

//----------------------------------------------------------------------------
// medPipeGraph :
//----------------------------------------------------------------------------
class medPipeGraph : public mafPipe
{
public:
  mafTypeMacro(medPipeGraph,mafPipe);

  medPipeGraph();
  virtual     ~medPipeGraph ();

  //Create plots of scalar data
  vtkRectilinearGrid* GetData(mafNode *vme);

};  
#endif // __medPipeGraph_H__
