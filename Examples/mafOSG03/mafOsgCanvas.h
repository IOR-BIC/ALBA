/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOsgCanvas.h,v $
Language:  C++
Date:      $Date: 2006-02-13 15:50:49 $
Version:   $Revision: 1.1 $
Authors:   Silvano Imboden
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
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
   void Home();

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



