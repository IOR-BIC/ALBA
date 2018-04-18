/*=========================================================================

 Program: MAF2
 Module: mafPrintSupport
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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

#include "mafPrintManager.h"

#include <ctype.h>
#include "wx/metafile.h"
#include "wx/printdlg.h"

//=============================== mafPrintout ===============================
#ifndef __mafPrintout_H__
#define __mafPrintout_H__

#include "mafSceneGraph.h"
#include "mafVMEImage.h"
#include <wx/print.h>
#include <wx/html/htmprint.h>

//----------------------------------------------------------------------------
// mafPrintout :
class mafPrintout: public wxPrintout
{
public:
	mafPrintout(mafView *view, wxRect margins);
	mafPrintout(mafVMEImage *img, wxRect margins);
	virtual ~mafPrintout();
  bool OnPrintPage(int page);
  bool HasPage(int page);
  //bool OnBeginDocument(int startPage, int endPage);
  void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

protected:


	bool Print(mafView *view);

	bool Print(mafVMEImage *image);

	/** Print the bitmap filling the paper size considering margins.*/
	bool PrintBitmap(wxBitmap *bmp);

	mafView *m_View;
	mafVMEImage *m_Image;
  wxRect m_Margins;
};
#endif

//----------------------------------------------------------------------------
mafPrintout::mafPrintout(mafView *view, wxRect margins) : wxPrintout("Printout")
{
	m_View = view;
	m_Image = NULL;
	m_Margins = margins;
}
//----------------------------------------------------------------------------
mafPrintout::mafPrintout(mafVMEImage *img, wxRect margins) : wxPrintout("Printout")
{
	m_View = NULL;
	m_Image = img;
	m_Margins = margins;
}
//----------------------------------------------------------------------------
mafPrintout::~mafPrintout()
{
}
//----------------------------------------------------------------------------
bool mafPrintout::OnPrintPage(int page)
//----------------------------------------------------------------------------
{
  if (m_View == NULL && m_Image == NULL)  
	{
		wxMessageBox("nothing to print", "Warning");    
		return false;
	}
  
	if (m_View)
		Print(m_View);
	else
		Print(m_Image);

  return TRUE;
}
//----------------------------------------------------------------------------
void mafPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
  *minPage = 1;
  *maxPage = 1;
  *selPageFrom = 1;
  *selPageTo = 1;
}
//----------------------------------------------------------------------------
bool mafPrintout::HasPage(int pageNum)
{
  return (pageNum == 1);
}

//----------------------------------------------------------------------------
bool  mafPrintout::Print(mafView *view)
{
	wxBitmap image;

	wxColor color = view->GetBackgroundColor();
	view->SetBackgroundColor(wxColor(255, 255, 255));
	view->GetImage(image/*, 2*/);
	view->SetBackgroundColor(color);


	return PrintBitmap(&image);
}

//----------------------------------------------------------------------------
bool mafPrintout::Print(mafVMEImage *vmeImage)
{
	wxBitmap image = vmeImage->GetImageAsBitmap();
	return PrintBitmap(&image);
}

//----------------------------------------------------------------------------
bool mafPrintout::PrintBitmap(wxBitmap *bmp)
{
	wxDC *dc = GetDC();
	if (!dc)
	{
		wxMessageBox("failed to retrieve Printing DC", "Warning");
		return false;
	}

	assert(bmp);
	float iw = bmp->GetWidth();
	float ih = bmp->GetHeight();
	float maxX = iw;
	float maxY = ih;

	// Add the margin to the graphic size
	maxX += (m_Margins.GetLeft() + m_Margins.GetRight());
	maxY += (m_Margins.GetTop() + m_Margins.GetBottom());

	// Get the size of the DC in pixels
	int w, h;
	dc->GetSize(&w, &h);

	// Calculate a suitable scaling factor
	float scaleX = (float)(w / maxX);
	float scaleY = (float)(h / maxY);

	// Use x or y scaling factor, whichever fits on the DC
	float actualScale = wxMin(scaleX, scaleY);

	// Calculate the position on the DC for centering the graphic
	float posX = (float)((w - (iw*actualScale)) / 2.0);
	float posY = (float)((h - (ih*actualScale)) / 2.0);

	// Set the scale and origin
	dc->SetUserScale(actualScale, actualScale);
	dc->SetDeviceOrigin((long)posX, (long)posY);

	wxMemoryDC mdc;
	mdc.SelectObject(*bmp);
	dc->SetBackground(*wxWHITE_BRUSH);
	dc->Clear();
	return dc->Blit(0, 0, maxX, maxY, &mdc, 0, 0);
}

//=============================== mafPrintout ===============================

//----------------------------------------------------------------------------
mafPrintManager::mafPrintManager()
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
mafPrintManager::~mafPrintManager()
{
  cppDEL(m_PrintData);
  cppDEL(m_PageSetupData);
}
//----------------------------------------------------------------------------
void mafPrintManager::OnPrintPreview(mafView *v)
{
  if (!v)return;

  // Pass two printout objects: for preview, and possible printing.
  wxPrintDialogData printDialogData(*m_PrintData);
  wxPoint tl = m_PageSetupData->GetMarginTopLeft();
  wxPoint br = m_PageSetupData->GetMarginBottomRight();
  wxRect margins(tl,br);

  wxPrintout *printout, *printOutForPrinting;
 
  printout = new mafPrintout(v,margins);
  printOutForPrinting= new mafPrintout(v,margins);
 
  wxPrintPreview *preview = new wxPrintPreview(printout, printOutForPrinting, & printDialogData);
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
void mafPrintManager::OnPrint(mafView *v)
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
  }
  else
  {
    (*m_PrintData) = printer.GetPrintDialogData().GetPrintData();
  }
}

//----------------------------------------------------------------------------
void mafPrintManager::OnPrint(mafVMEImage *img)
{
	if (!img)return;

	wxPrintDialogData printDialogData(*m_PrintData);
	wxPrinter printer(&printDialogData);
	wxPoint tl = m_PageSetupData->GetMarginTopLeft();
	wxPoint br = m_PageSetupData->GetMarginBottomRight();
	wxRect margins(tl, br);


	mafPrintout printout(img, margins);
	if (!printer.Print(mafGetFrame(), &printout, TRUE))
	{
		if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
			wxMessageBox("There was a problem printing.\nPerhaps your current printer is not set correctly?", "Printing", wxOK);
	}
	else
	{
		(*m_PrintData) = printer.GetPrintDialogData().GetPrintData();
	}
}

//----------------------------------------------------------------------------
void mafPrintManager::OnPrintSetup()
{
  wxPrintDialogData printDialogData(*m_PrintData);
  wxPrintDialog printerDialog(mafGetFrame(), & printDialogData);

  printerDialog.GetPrintDialogData().SetSetupDialog(TRUE);
  printerDialog.ShowModal();

  (*m_PrintData) = printerDialog.GetPrintDialogData().GetPrintData();
}
//----------------------------------------------------------------------------
void mafPrintManager::OnPageSetup()
{
  (*m_PageSetupData) = *m_PrintData;

  wxPageSetupDialog pageSetupDialog(mafGetFrame(), m_PageSetupData);
  pageSetupDialog.ShowModal();

  (*m_PrintData) = pageSetupDialog.GetPageSetupData().GetPrintData();
  (*m_PageSetupData) = pageSetupDialog.GetPageSetupData();
}

