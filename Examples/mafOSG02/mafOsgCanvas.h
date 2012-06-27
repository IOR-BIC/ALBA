/*=========================================================================

 Program: MAF2
 Module: mafOsgCanvas
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOsgCanvas_h__
#define __mafOsgCanvas_h__

#include <assert.h>

#include <osg/Group>
#include <osgProducer/Viewer>

//--------------------------------------------------------------------------------
class mafOsgCanvas: public wxWindow 
//--------------------------------------------------------------------------------
{
public:
    mafOsgCanvas(wxWindow *parent,const wxWindowID id = -1, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,long style = 0);
   ~mafOsgCanvas(void);
   bool LoadModel(const char *filename);   
   bool SaveModel(const char *filename);

protected:
    void OnSize(wxSizeEvent &event);
    void OnPaint(wxPaintEvent &event) {wxPaintDC dc(this);}; // required
    void OnEraseBackground(wxEraseEvent &event) {}; // Do nothing, to avoid flashing on MSW
    void OnTimer(wxTimerEvent &event);

    wxTimer *m_Timer;
    osg::ref_ptr<osgProducer::Viewer> m_Viewer;
    osg::ref_ptr<osg::Group> m_Root;

DECLARE_EVENT_TABLE()
};

#endif //__mafOsgCanvas_h__



