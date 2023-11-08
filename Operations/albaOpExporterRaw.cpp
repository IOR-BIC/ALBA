/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterRaw
 Authors: Stefano Perticoni - Matteo Giacomoni
 
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

#include "albaOpExporterRaw.h"
#include "wx/wxprec.h"
#include "wx/busyinfo.h"

#include "albaDecl.h"
#include "albaString.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaOp.h"
#include "albaVME.h"
#include "albaVMEVolumeGray.h"

#include "vtkSmartPointer.h"

#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "vtkDirectory.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkImageWriter.h"
#include "vtkImageData.h"
#include "vtkDoubleArray.h"
#include "vtkShortArray.h"
#include "vtkDataSet.h"

#include "wx\filename.h"

#include <fstream>

//----------------------------------------------------------------------------
albaOpExporterRAW::albaOpExporterRAW(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_FileName = "";
	m_FileNameZ = "";
  m_Input = NULL;
	m_SingleFile = 1;
	m_Offset = 0;

  m_ProposedDirectory = albaGetLastUserFolder();
}
//----------------------------------------------------------------------------
albaOpExporterRAW::~albaOpExporterRAW()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool albaOpExporterRAW::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
	return (node != NULL) && node->IsA("albaVMEVolumeGray");
}
//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
enum RAW_EXPORTER_ID
{
	ID_SINGLE_FILE = MINID,
	ID_INT,

};
//----------------------------------------------------------------------------
void albaOpExporterRAW::OpRun()   
//----------------------------------------------------------------------------
{
	m_ProposedDirectory += m_Input->GetName();
	m_ProposedDirectory += ".raw";
	if(!m_TestMode)
	{
		wxString wildc = "raw file (*.raw)|*.raw";
		wxString file = albaGetSaveFile(m_ProposedDirectory,wildc).ToAscii(); 
		m_FileName = file;

		//Crete GUI
		m_Gui = new albaGUI(this);
		m_Gui->SetListener(this);

		m_Gui->Bool(ID_SINGLE_FILE,"single file",&m_SingleFile);
		m_Gui->Divider();

		m_Gui->Integer(ID_INT,"slice offset: ", &m_Offset,MININT,MAXINT,"only if not single file");

		m_Gui->Label("");
		m_Gui->OkCancel(); 

		m_Gui->Enable(ID_INT,false);

		m_Gui->Divider();

		ShowGui(); 
	}
    
}
//----------------------------------------------------------------------------
void albaOpExporterRAW::OpDo()   
//----------------------------------------------------------------------------
{					
	assert(m_Input);
	assert(m_FileName != "");
	this->SaveVolume();
}

//----------------------------------------------------------------------------
void albaOpExporterRAW::SaveVolume()
//----------------------------------------------------------------------------
{
	wxString path,name,ext;
	wxFileName::SplitPath(m_FileName.GetCStr(),&path,&name,&ext);
	path+= _("\\");
	if(!m_TestMode)
		wxBusyInfo wait("Please wait, working...");
	
	albaVMEVolumeGray *volume=albaVMEVolumeGray::SafeDownCast(m_Input);
	volume->Modified();
	
	//if it is a vtkImageData
	if (vtkImageData::SafeDownCast(volume->GetOutput()->GetVTKData()) || vtkImageData::SafeDownCast(volume->GetOutput()->GetVTKData()))
	{
		int dim[3];
		vtkImageData *ImageData = vtkImageData::SafeDownCast(volume->GetOutput()->GetVTKData());
		ImageData->GetDimensions(dim);
		int xdim = dim[0];
		int ydim = dim[1];
		int zdim = dim[2];
			
		vtkImageWriter *exporter = vtkImageWriter::New();
		exporter->SetInput(ImageData);

		wxString prefix;

		//if the data RAW are saved in a single file
		if (m_SingleFile)
		{
			prefix = albaString::Format("%s%s_%dx%dx%d",path.ToAscii(),name.ToAscii(),xdim,ydim,zdim);
			
			exporter->SetFileDimensionality(3); 
			exporter->SetFilePattern("%s.raw");					

		}	

		//if the data RAW are saved in a different file for each slice
		else 
		{
			prefix = albaString::Format("%s%s_%dx%d",path.ToAscii(),name.ToAscii(),xdim,ydim);

			exporter->SetFileDimensionality(2); // the writer will create a number of 2D images
			exporter->SetFilePattern("%s_%04d.raw");
		}
		
		char *c_prefix = (char*)( prefix.ToAscii()); 
		exporter->SetFilePrefix(c_prefix);
		exporter->Write();

		//if the user choose a numeration of the slices files not starting with zero: 
			
		if (m_Offset != 0)
		{
			vtkDirectory* directory;
			vtkNEW(directory);

			wxString name_file, old_name, new_name;

			directory->Open(path);
			int number_files = directory->GetNumberOfFiles();

			for (int i=0;i<number_files; i++)
			{
				name_file = directory->GetFile(i);
				old_name = path + name_file;

				if ((name_file.Find(name)) >= 0)
				{
					wxString numeration = name_file.AfterLast('_');
		
					numeration = numeration.BeforeFirst('.');
					double val;

					if (numeration.ToDouble(&val))
					{
						val = val + m_Offset;
						int value = (int)val;
						wxString new_numeration;
						new_numeration = albaString::Format("%04d",value);
						name_file.Replace(numeration, new_numeration);	
						new_name = path + name_file;

						rename(old_name,new_name);										
					}
				}
			}		
			vtkDEL(directory);
		}
		
		vtkDEL(exporter);
		return;	
	}

	//if it is a vtkRectilinearGrid
	if (vtkRectilinearGrid::SafeDownCast(volume->GetOutput()->GetVTKData()))
	{

		double bounds[6];
		volume->GetOutput()->GetBounds(bounds);

		double xmin = bounds[0];
		double xmax = bounds[1];
		double ymin = bounds[2];
		double ymax = bounds[3];
		double zmin = bounds[4];
		double zmax = bounds[5];	

		int dim[3];
		((vtkRectilinearGrid *)(volume->GetOutput()->GetVTKData()))->GetDimensions(dim);
		int xdim = dim[0];
		int ydim = dim[1];
		int zdim = dim[2];
		

		//if the data RAW are saved in a single file
		if (m_SingleFile)
		{
					
			vtkImageData *StructuredPoints;
			vtkNEW(StructuredPoints);

			StructuredPoints->SetOrigin(xmin, ymin, zmin);
			StructuredPoints->SetDimensions(xdim, ydim, zdim);

			double spacing_x = (xmax-xmin)/xdim;
			double spacing_y = (ymax-ymin)/ydim;

			//float spacing_z = (zmax-zmin)/zdim;					
			double spacing_z = 1;

			StructuredPoints->SetSpacing(spacing_x, spacing_y, spacing_z);
			StructuredPoints->SetScalarTypeToShort();
			StructuredPoints->Update();

			vtkImageWriter *exporter = vtkImageWriter::New();
				
			exporter->SetInput(StructuredPoints);

			vtkRectilinearGrid *RectilinearGrid = (vtkRectilinearGrid *)(volume->GetOutput()->GetVTKData());
			vtkDoubleArray *z_coords = ((vtkDoubleArray *)RectilinearGrid->GetZCoordinates());

			int num_of_slices = z_coords->GetNumberOfTuples(); 
		
			const char* nome;
			std::ofstream f_out;
			if(!m_TestMode)
			{
				//saving the z coordinates in a file
        wxString proposed = albaGetLastUserFolder();
				proposed += _("Z_coordinates_");
				proposed += m_Input->GetName();
				proposed += _(".txt");
				wxString wildc = _("txt file (*.txt)|*.txt");
				wxString file = albaGetSaveFile(proposed,wildc).ToAscii();   
				nome = (file);
				f_out.open(nome);
			}
			else
			{
				nome = m_FileNameZ;
				f_out.open(nome);
			} 
					
			f_out<< "Z coordinates:"<<"\n";

			for (int i = 0; i < num_of_slices; i++)
			{
				f_out<< z_coords->GetValue(i)<<"\n";	
			}					
			f_out.close();

			//int vol_size = xdim*ydim*zdim;
			int vol_size = xdim * ydim * num_of_slices;

			vtkDoubleArray *double_scalars = vtkDoubleArray::New();
			double_scalars->SetNumberOfTuples(vol_size);
			double_scalars->SetNumberOfComponents(1);

			vtkShortArray *short_scalars = vtkShortArray::New();
			short_scalars->SetNumberOfValues(vol_size);
			short_scalars->SetNumberOfComponents(1);
						
			RectilinearGrid->GetPointData()->GetScalars()->GetData(0, vol_size -1, 0, 0, double_scalars);
										
			for (int k = 0; k < vol_size; k++) 
				short_scalars->SetValue(k, double_scalars->GetValue(k));
					
			StructuredPoints->GetPointData()->SetScalars(short_scalars);		
			StructuredPoints->Update();
			exporter->Modified();								
						
			int a[3];
			StructuredPoints->GetDimensions(a);

			wxString filename = albaString::Format("%s%s_%dx%dx%d.raw",path.ToAscii(),name.ToAscii(),xdim,ydim,zdim);							
			exporter->SetFileName(filename);
			exporter->SetFileDimensionality(3);

			exporter->Write();

			vtkDEL(StructuredPoints);
			short_scalars->Delete();
			double_scalars->Delete();
			vtkDEL(exporter);
			return;					
		}					
		//if the data RAW are saved in a different file for each slice
		else
		{
			int slice_size = xdim*ydim;

			vtkImageData *StructuredPoints;
			vtkNEW(StructuredPoints);
			StructuredPoints->SetOrigin(xmin, ymin, 0);
			StructuredPoints->SetDimensions(xdim, ydim, 1);
				
			//setting the spacing
			double spacing_x = (xmax-xmin)/xdim;
			double spacing_y = (ymax-ymin)/ydim;
								
			StructuredPoints->SetSpacing(spacing_x, spacing_y, 0);
			StructuredPoints->SetScalarTypeToShort();
			StructuredPoints->Update();

			vtkImageWriter *exporter = vtkImageWriter::New();
			exporter->SetInput(StructuredPoints);

			vtkRectilinearGrid *RectilinearGrid = (vtkRectilinearGrid *)(volume->GetOutput()->GetVTKData());
			vtkDoubleArray *z_coords = ((vtkDoubleArray *)RectilinearGrid->GetZCoordinates());

			int num_of_slices = z_coords->GetNumberOfTuples(); 
				
			const char* nome;
			if(!m_TestMode)
			{
				//saving the z coordinates in a file
        wxString proposed = albaGetLastUserFolder();
				proposed += _("Z_coordinates_");
				proposed += m_Input->GetName();
				proposed += _(".txt");
				wxString wildc = _("txt file (*.txt)|*.txt");
				wxString file = albaGetSaveFile(proposed,wildc).ToAscii();   
				nome = (file);
				std::ofstream f_out;
				f_out.open(nome);
					
				f_out<< "Z coordinates:"<<"\n";

				for (int i = 0; i < num_of_slices; i++)
				{
					f_out<< z_coords->GetValue(i)<<"\n";	
				}					
				f_out.close();
			}
			else
			{
				nome = m_FileNameZ;
				std::ofstream f_out;
				f_out.open(nome);
					
				f_out<< "Z coordinates:"<<"\n";

				for (int i = 0; i < num_of_slices; i++)
				{
					f_out<< z_coords->GetValue(i)<<"\n";	
				}					
				f_out.close();
			}

			for (int i = 0; i < num_of_slices; i++)
			{	
							
				vtkDoubleArray *double_scalars = vtkDoubleArray::New();
				double_scalars->SetNumberOfTuples(slice_size);
				double_scalars->SetNumberOfComponents(1);
	
				vtkShortArray *short_scalars = vtkShortArray::New();
				short_scalars->SetNumberOfValues(slice_size);
				short_scalars->SetNumberOfComponents(1);
		
				RectilinearGrid->GetPointData()->GetScalars()->GetData(i*slice_size, ((i+1)*slice_size)-1, 0, 0, double_scalars);
                   
        //To note that I set the first value of short_scalars with the last value of float_scalars
        //In this way the exported image won't appear upside down
				for (int k = 0; k < slice_size; k++)                 
					//short_scalars->SetValue(k, double_scalars->GetValue( slice_size - k - 1 ));
					short_scalars->SetValue(k, double_scalars->GetValue( k ));
						
				StructuredPoints->GetPointData()->SetScalars(short_scalars);
								
				StructuredPoints->Update();
				exporter->Modified();								
								
				wxString filename = albaString::Format("%s%s_%dx%d_%04d.raw",path.ToAscii(),name.ToAscii(),xdim,ydim,i);							
				exporter->SetFileName(filename);

				exporter->Write();

				short_scalars->Delete();
				double_scalars->Delete();
			}

			//if the user choose a numeration of the slices files not starting with zero: 

			if (m_Offset != 0)
			{
				vtkSmartPointer<vtkDirectory> directory;
				wxString name_file, old_name, new_name;

				directory->Open(path);
				int number_files = directory->GetNumberOfFiles();

				for (int i=0;i<number_files; i++)
				{
					name_file = directory->GetFile(i);
					old_name = path + name_file;

					if ((name_file.Find(name)) >= 0)
					{
						wxString numeration = name_file.AfterLast('_');

						numeration = numeration.BeforeFirst('.');
						double val;

						if (numeration.ToDouble(&val))
						{
							val = val + m_Offset;
							int value = (int)val;
							wxString new_numeration;
							new_numeration = albaString::Format("%04d",value);
							name_file.Replace(numeration, new_numeration);	
							new_name = path + name_file;

							rename(old_name,new_name);										
						}
					}
				}
			}
			vtkDEL(exporter);
			vtkDEL(StructuredPoints);
			return;			
		}
	}
}
//----------------------------------------------------------------------------
void albaOpExporterRAW::OpUndo()   
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
albaOp* albaOpExporterRAW::Copy()   
//----------------------------------------------------------------------------
{
    albaOpExporterRAW *cp = new albaOpExporterRAW(m_Label);
    return cp;
}

//----------------------------------------------------------------------------
void albaOpExporterRAW::OnEvent(albaEventBase *alba_event) 
//----------------------------------------------------------------------------
{ 
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId())
		{

			case ID_SINGLE_FILE:
			{
				SetSingleFile(m_SingleFile);
			}
			break;
			case wxOK:          
			{ 
				OpStop(OP_RUN_OK);
			}
			break;
			case wxCANCEL:
			{    
				OpStop(OP_RUN_CANCEL);
			}
			break;
			default:
			{
				albaEventMacro(*e); 
			}
			break;
		}
	}
}
//----------------------------------------------------------------------------
void albaOpExporterRAW::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
  albaEventMacro(albaEvent(this,result));	
}
//----------------------------------------------------------------------------
void albaOpExporterRAW::SetSingleFile(int enable)
//----------------------------------------------------------------------------
{
	if(!m_TestMode)
	{
		m_Gui->Enable(ID_INT,true);
		if (enable)
			m_Gui->Enable(ID_INT,false);
	}
	if(m_SingleFile!=enable)
		m_SingleFile=enable;
}
