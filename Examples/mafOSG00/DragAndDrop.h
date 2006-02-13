/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: DragAndDrop.h,v $
Language:  C++
Date:      $Date: 2006-02-13 15:49:13 $
Version:   $Revision: 1.1 $
Authors:   Silvano Imboden
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __DragAndDrop_h__
#define __DragAndDrop_h__

#ifdef __GNUG__
#pragma interface "DragAndDrop.h"
#endif

#include <wx/dnd.h>

// forward declaration
class mVueGlCanvas;

//-----------------------------------------------------------------------
// class DragAndDrop
//-----------------------------------------------------------------------
/** description: handle drag and drop operations.  Requires knowledge of 
    mVueGlCanvas class.  On drop, will call mVueGlCanvas::LoadModel */

class DragAndDrop : public wxFileDropTarget
{
public:
    DragAndDrop(mVueGlCanvas *canvas) { m_Canvas = canvas; }
        // constructor
    ~DragAndDrop() {}
        // destructor

    virtual bool OnDropFiles(wxCoord x, wxCoord y,
        const wxArrayString& filenames);
        // handle drop file operation

private:
    mVueGlCanvas *m_Canvas;
        // pointer to canvas
};

#endif //__DragAndDrop_h__
