/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgHistogramWidget.h,v $
  Language:  C++
  Date:      $Date: 2006-01-30 08:22:49 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgHistogramWidget_H__
#define __mmgHistogramWidget_H__

#include "mafDecl.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward refs
//----------------------------------------------------------------------------
class mafRWI;
class vtkActor2D;
class vtkImageAccumulate;
class vtkGlyph3D;
class vtkImageData;
class vtkImageChangeInformation;
class vtkTextMapper;
class vtkActor2D;

//----------------------------------------------------------------------------
/** mmgHistogramWidget : widget that encapsulate render window into a gui*/
class mmgHistogramWidget: public wxPanel, public mafObserver
{
public:
  mmgHistogramWidget(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxSize(400,300), long style = wxTAB_TRAVERSAL /*| wxSUNKEN_BORDER */);
  virtual ~mmgHistogramWidget();

  virtual void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  void OnEvent( mafEventBase *event );

  void UpdateHistogram();

  /** Input data from which generate histogram.*/
  void SetData(vtkImageData *data);

  /** Set the scale factor for the histogram.*/
  void SetScaleFactor(double factor);

  /** Return scale factor from the widget.*/
  double GetScaleFactor() {return m_ScaleFactor;};

protected:
  mafObserver   *m_Listener;
  double         m_ScaleFactor;
  int            m_HisctogramValue;
  int            m_NumberOfBins;
  bool           m_AutoscaleHistogram;

  vtkTextMapper	*m_TextMapper;
  vtkActor2D    *m_TextActor;

  vtkImageData  *m_Data;
  vtkImageAccumulate *m_Accumulate;
  vtkImageChangeInformation *m_ChangeInfo;
  vtkGlyph3D         *m_Glyph;
  mafRWI        *m_HistogramRWI;
  vtkActor2D    *m_PlotActor;
};
#endif
