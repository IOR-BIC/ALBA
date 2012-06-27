/*=========================================================================

 Program: MAF2
 Module: DragAndDrop
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
