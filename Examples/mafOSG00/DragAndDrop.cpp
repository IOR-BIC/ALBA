/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: DragAndDrop.cpp,v $
  Language:  C++
  Date:      $Date: 2006-02-13 15:49:13 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "DragAndDrop.h"
#include "mVueGlCanvas.h"

#if !wxUSE_DRAG_AND_DROP
    #error This application requires drag and drop support in the library
#endif

/** description: handle drop file operation.  Attempt to load a model.
 - param: x - mouse x
 - param: y - mouse y
 - param: filenames - filename list
 - return: true if dropped file was accepted, else false
*/
bool DragAndDrop::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
    bool ret = false;

    if (filenames.GetCount() == 1) {
        if (m_Canvas != NULL) {
            ret = m_Canvas->LoadModel(filenames[0].c_str());
        }
    }

    return ret;
}
