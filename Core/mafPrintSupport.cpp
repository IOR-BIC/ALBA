/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPrintSupport.cpp,v $
Language:  C++
Date:      $Date: 2006-01-12 10:29:36 $
Version:   $Revision: 1.2 $
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

#include "mafDecl.h"
//#include "mafPrintout.h"
#include "mafView.h"

#if !wxUSE_PRINTING_ARCHITECTURE
#error You must set wxUSE_PRINTING_ARCHITECTURE to 1 in setup.h to compile this demo.
#endif

// Set this to 1 if you want to test PostScript printing under MSW.
// However, you'll also need to edit src/msw/makefile.nt.
#define wxTEST_POSTSCRIPT_IN_MSW 0

#include "mafPrintSupport.h"

#include <ctype.h>
#include "wx/metafile.h"
#include "wx/printdlg.h"

//=============================== mafPrintout ===============================
#ifndef __mafPrintout_H__
#define __mafPrintout_H__

#include <wx/print.h>

//----------------------------------------------------------------------------
// mafPrintout :
//----------------------------------------------------------------------------
class mafPrintout: public wxPrintout
{
public:
  mafPrintout( mafView *v, wxRect margins);
  virtual ~mafPrintout();
  bool OnPrintPage(int page);
  bool HasPage(int page);
  //bool OnBeginDocument(int startPage, int endPage);
  void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

protected:
  mafView *m_View;
  wxRect m_Margins;
};
#endif

//----------------------------------------------------------------------------
mafPrintout::mafPrintout(mafView *v, wxRect margins)
: wxPrintout("Printout")
//----------------------------------------------------------------------------
{
  m_View = v;
  m_Margins = margins;
}
//----------------------------------------------------------------------------
mafPrintout::~mafPrintout()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mafPrintout::OnPrintPage(int page)
//----------------------------------------------------------------------------
{
  if (m_View == NULL)  {wxMessageBox("nothing to print", "Warning");    return false;}
  wxDC *dc = GetDC();
  if (!dc)    {wxMessageBox("failed to retrieve Printing DC", "Warning"); return false;}
  m_View->Print(dc, m_Margins);  

  return TRUE;
}
//----------------------------------------------------------------------------
void mafPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
//----------------------------------------------------------------------------
{
  *minPage = 1;
  *maxPage = 1;
  *selPageFrom = 1;
  *selPageTo = 1;
}
//----------------------------------------------------------------------------
bool mafPrintout::HasPage(int pageNum)
//----------------------------------------------------------------------------
{
  return (pageNum == 1);
}
//=============================== mafPrintout ===============================


//----------------------------------------------------------------------------
mafPrintSupport::mafPrintSupport()
//----------------------------------------------------------------------------
{
  m_PrintData = new wxPrintData();
  m_PageSetupData = new wxPageSetupData();

  m_PageSetupData->SetDefaultMinMargins(true);
  
  m_PageSetupData->SetMarginTopLeft(wxPoint(25,25));
  m_PageSetupData->SetMarginBottomRight(wxPoint(25,25));
  
  m_PageSetupData->SetMinMarginTopLeft(wxPoint(5,5));
  m_PageSetupData->SetMinMarginBottomRight(wxPoint(5,5));
}
//----------------------------------------------------------------------------
mafPrintSupport::~mafPrintSupport()
//----------------------------------------------------------------------------
{
  cppDEL(m_PrintData);
  cppDEL(m_PageSetupData);
}
//----------------------------------------------------------------------------
void mafPrintSupport::OnPrintPreview(mafView *v)
//----------------------------------------------------------------------------
{
  if (!v)return;

  // Pass two printout objects: for preview, and possible printing.
  wxPrintDialogData printDialogData(*m_PrintData);
  wxPoint tl = m_PageSetupData->GetMarginTopLeft();
  wxPoint br = m_PageSetupData->GetMarginBottomRight();
  wxRect margins(tl,br);

  wxPrintPreview *preview = new wxPrintPreview(new mafPrintout(v,margins), new mafPrintout(v,margins), & printDialogData);
  preview->SetZoom(20);
  if (!preview->Ok())
  {
    delete preview;
    wxMessageBox("There was a problem previewing.\nPerhaps your current printer is not set correctly?", "Previewing", wxOK);
    return;
  }

  wxPreviewFrame *frame = new wxPreviewFrame(preview, (wxFrame *)mafGetFrame(), "Print Preview", wxPoint(100, 100), wxSize(600, 650));
  frame->Centre(wxBOTH);
  frame->Initialize();
  frame->Show(TRUE);
}
//----------------------------------------------------------------------------
void mafPrintSupport::OnPrint(mafView *v)
//----------------------------------------------------------------------------
{
  if (!v)return;
  wxPrintDialogData printDialogData(*m_PrintData);
  wxPrinter printer(& printDialogData);
  wxPoint tl = m_PageSetupData->GetMarginTopLeft();
  wxPoint br = m_PageSetupData->GetMarginBottomRight();
  wxRect margins(tl,br);
  mafPrintout printout(v,margins);
  if (!printer.Print( mafGetFrame(), &printout, TRUE))
  {
    if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
      wxMessageBox("There was a problem printing.\nPerhaps your current printer is not set correctly?", "Printing", wxOK);
    else
      wxMessageBox("You canceled printing", "Printing", wxOK);
  }
  else
  {
    (*m_PrintData) = printer.GetPrintDialogData().GetPrintData();
  }
}
//----------------------------------------------------------------------------
void mafPrintSupport::OnPrintSetup()
//----------------------------------------------------------------------------
{
  wxPrintDialogData printDialogData(*m_PrintData);
  wxPrintDialog printerDialog(mafGetFrame(), & printDialogData);

  printerDialog.GetPrintDialogData().SetSetupDialog(TRUE);
  printerDialog.ShowModal();

  (*m_PrintData) = printerDialog.GetPrintDialogData().GetPrintData();
}
//----------------------------------------------------------------------------
void mafPrintSupport::OnPageSetup()
//----------------------------------------------------------------------------
{
  (*m_PageSetupData) = *m_PrintData;

  wxPageSetupDialog pageSetupDialog(mafGetFrame(), m_PageSetupData);
  pageSetupDialog.ShowModal();

  (*m_PrintData) = pageSetupDialog.GetPageSetupData().GetPrintData();
  (*m_PageSetupData) = pageSetupDialog.GetPageSetupData();
}
