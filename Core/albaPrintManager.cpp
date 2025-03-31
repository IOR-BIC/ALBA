/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPrintSupport
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaDecl.h"
//#include "albaPrintout.h"
#include "albaView.h"

#if !wxUSE_PRINTING_ARCHITECTURE
#error You must set wxUSE_PRINTING_ARCHITECTURE to 1 in setup.h to compile this demo.
#endif

// Set this to 1 if you want to test PostScript printing under MSW.
// However, you'll also need to edit src/msw/makefile.nt.
#define wxTEST_POSTSCRIPT_IN_MSW 0

#include "albaPrintManager.h"

#include <ctype.h>
#include "wx/metafile.h"
#include "wx/printdlg.h"

//=============================== albaPrintout ===============================
#ifndef __albaPrintout_H__
#define __albaPrintout_H__

#include "albaSceneGraph.h"
#include "albaVMEImage.h"
#include <wx/print.h>
#include <wx/html/htmprint.h>

//----------------------------------------------------------------------------
// albaPrintout :
class albaPrintout: public wxPrintout
{
public:
	albaPrintout(albaView *view, wxRect margins);
	albaPrintout(albaVMEImage *img, wxRect margins);
	virtual ~albaPrintout();
  bool OnPrintPage(int page);
  bool HasPage(int page);
  //bool OnBeginDocument(int startPage, int endPage);
  void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

protected:


	bool Print(albaView *view);

	bool Print(albaVMEImage *image);

	/** Print the bitmap filling the paper size considering margins.*/
	bool PrintBitmap(wxBitmap *bmp);

	albaView *m_View;
	albaVMEImage *m_Image;
  wxRect m_Margins;
};
#endif

//----------------------------------------------------------------------------
albaPrintout::albaPrintout(albaView *view, wxRect margins) : wxPrintout("Printout")
{
	m_View = view;
	m_Image = NULL;
	m_Margins = margins;
}
//----------------------------------------------------------------------------
albaPrintout::albaPrintout(albaVMEImage *img, wxRect margins) : wxPrintout("Printout")
{
	m_View = NULL;
	m_Image = img;
	m_Margins = margins;
}
//----------------------------------------------------------------------------
albaPrintout::~albaPrintout()
{
}
//----------------------------------------------------------------------------
bool albaPrintout::OnPrintPage(int page)
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

  return true;
}
//----------------------------------------------------------------------------
void albaPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
  *minPage = 1;
  *maxPage = 1;
  *selPageFrom = 1;
  *selPageTo = 1;
}
//----------------------------------------------------------------------------
bool albaPrintout::HasPage(int pageNum)
{
  return (pageNum == 1);
}

//----------------------------------------------------------------------------
bool  albaPrintout::Print(albaView *view)
{
	wxBitmap image;

	wxColor color = view->GetBackgroundColor();
	view->SetBackgroundColor(wxColor(255, 255, 255));
	view->GetImage(image/*, 2*/);
	view->SetBackgroundColor(color);


	return PrintBitmap(&image);
}

//----------------------------------------------------------------------------
bool albaPrintout::Print(albaVMEImage *vmeImage)
{
	wxBitmap image = vmeImage->GetImageAsBitmap();
	return PrintBitmap(&image);
}

//----------------------------------------------------------------------------
bool albaPrintout::PrintBitmap(wxBitmap *bmp)
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

//=============================== albaPrintout ===============================

//----------------------------------------------------------------------------
albaPrintManager::albaPrintManager()
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
albaPrintManager::~albaPrintManager()
{
  cppDEL(m_PrintData);
  cppDEL(m_PageSetupData);
}
//----------------------------------------------------------------------------
void albaPrintManager::OnPrintPreview(albaView *v)
{
  if (!v)return;

  // Pass two printout objects: for preview, and possible printing.
  wxPrintDialogData printDialogData(*m_PrintData);
  wxPoint tl = m_PageSetupData->GetMarginTopLeft();
  wxPoint br = m_PageSetupData->GetMarginBottomRight();
  wxRect margins(tl,br);

  wxPrintout *printout, *printOutForPrinting;
 
  printout = new albaPrintout(v,margins);
  printOutForPrinting= new albaPrintout(v,margins);
 
  wxPrintPreview *preview = new wxPrintPreview(printout, printOutForPrinting, & printDialogData);
  preview->SetZoom(20);
  if (!preview->Ok())
  {
    delete preview;
    wxMessageBox("There was a problem previewing.\nPerhaps your current printer is not set correctly?", "Previewing", wxOK);
    return;
  }

  wxPreviewFrame *frame = new wxPreviewFrame(preview, (wxFrame *)albaGetFrame(), "Print Preview", wxPoint(100, 100), wxSize(600, 650));
  frame->Centre(wxBOTH);
  frame->Initialize();
  frame->Show(true);
}
//----------------------------------------------------------------------------
void albaPrintManager::OnPrint(albaView *v)
{
  if (!v)return;
  
  wxPrintDialogData printDialogData(*m_PrintData);
  wxPrinter printer(& printDialogData);
  wxPoint tl = m_PageSetupData->GetMarginTopLeft();
  wxPoint br = m_PageSetupData->GetMarginBottomRight();
  wxRect margins(tl,br);

  
  albaPrintout printout(v,margins);
  if (!printer.Print( albaGetFrame(), &printout, true))
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
void albaPrintManager::OnPrint(albaVMEImage *img)
{
	if (!img)return;

	wxPrintDialogData printDialogData(*m_PrintData);
	wxPrinter printer(&printDialogData);
	wxPoint tl = m_PageSetupData->GetMarginTopLeft();
	wxPoint br = m_PageSetupData->GetMarginBottomRight();
	wxRect margins(tl, br);


	albaPrintout printout(img, margins);
	if (!printer.Print(albaGetFrame(), &printout, true))
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
void albaPrintManager::OnPrintSetup()
{
  wxPrintDialogData printDialogData(*m_PrintData);
  wxPrintDialog printerDialog(albaGetFrame(), & printDialogData);

  printerDialog.GetPrintDialogData().EnablePrintToFile(true);
	printerDialog.ShowModal();

  (*m_PrintData) = printerDialog.GetPrintDialogData().GetPrintData();
}
//----------------------------------------------------------------------------
void albaPrintManager::OnPageSetup()
{
  (*m_PageSetupData) = *m_PrintData;

  wxPageSetupDialog pageSetupDialog(albaGetFrame(), m_PageSetupData);
  pageSetupDialog.ShowModal();

  (*m_PrintData) = pageSetupDialog.GetPageSetupData().GetPrintData();
  (*m_PageSetupData) = pageSetupDialog.GetPageSetupData();
}

