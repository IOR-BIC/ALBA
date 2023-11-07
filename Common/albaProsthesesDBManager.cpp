/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaProsthesisDBManager
 Authors: Gianluigi Crimi
 
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
#include "albaProsthesesDBManager.h"
#include <wx/tokenzr.h>
#include "albaLogicWithManagers.h"
#include "wx/dir.h"
#include "mmuDOMTreeErrorReporter.h"
#include "albaXMLElement.h"
#include "mmuXMLDOMElement.h"
#include "albaXMLString.h"
#include "albaTagArray.h"
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include "albaCrypt.h"
#include "vtkPolyDataReader.h"
#include "vtkDataSetWriter.h"
#include "vtkPolyData.h"
#include "vtkALBASmartPointer.h"
#include "albaCrypt.h"

//albaProsthesisDBManager defines
#define PRODB_NAME "ProstesesDB"
#define DB_VERSION "1.0"
#define NODE_PRODUCERS "Producers"
#define NODE_TYPES "Types"
#define NODE_PROSTHESES "Prostheses"
#define NODE_PRODUCER "Producer"
#define NODE_TYPE "Type"
#define NODE_PROSTHESIS "Prosthesis"
#define NODE_COMPONENTS "Components"
#define NODE_COMPONENT "Component"
#define NODE_MATRIX "Matrix"
#define ATTR_VERSION "Version"
#define ATTR_NAME "Name"
#define ATTR_IMG "Img"
#define ATTR_SITE "Site"
#define ATTR_SIDE "Side"
#define ATTR_BENDINGANGLE "BendingAngle"
#define ATTR_TYPE "Type"
#define ATTR_PRODUCER "Producer"
#define ATTR_FILE "File"


//----------------------------------------------------------------------------
albaProsthesesDBManager::albaProsthesesDBManager()
{
	m_DBDir = (albaGetAppDataDirectory());
	m_DBDir += "\\ProsthesesDB\\";

	if(!wxDir::Exists(m_DBDir.GetCStr()))
		wxMkdir(m_DBDir.GetCStr());

	m_DBFilename = m_DBDir + "ProsthesesDB.xml";

	m_PassPhrase = "fattinonfostepervivercomebruti";
}

//----------------------------------------------------------------------------
albaProsthesesDBManager::~albaProsthesesDBManager()
{

}


//----------------------------------------------------------------------------
int albaProsthesesDBManager::LoadDB()
{
	if (!wxFileExists(m_DBFilename.GetCStr()))
		return ALBA_OK;

	//Open the file xml with manufacture and model information
	try {
		XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch) {
		// Do your failure processing here
		return ALBA_ERROR;
	}

	XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser *XMLParser = new  XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser;

	XMLParser->setValidationScheme(XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser::Val_Auto);
	XMLParser->setDoNamespaces(false);
	XMLParser->setDoSchema(false);
	XMLParser->setCreateEntityReferenceNodes(false);

	mmuDOMTreeErrorReporter *errReporter = new mmuDOMTreeErrorReporter();
	XMLParser->setErrorHandler(errReporter);

	try {
		XMLParser->parse(m_DBFilename.GetCStr());
		int errorCount = XMLParser->getErrorCount();

		if (errorCount != 0)
		{
			// errors while parsing...
			albaErrorMessage("Errors while parsing XML file");
			return ALBA_ERROR;
		}

		// extract the root element and wrap inside a albaXMLElement
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = XMLParser->getDocument();
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *root = doc->getDocumentElement();
		assert(root);

		if (CheckNodeElement(root, PRODB_NAME))
		{
			albaString version = GetElementAttribute(root, ATTR_VERSION);

			//Check the DB version
			if (version != DB_VERSION)
			{
				albaLogMessage("Wrong DB Version:\n DB version:%s, Software Version:%s", version.GetCStr(), DB_VERSION);
				return ALBA_ERROR;
			}
		}
		else
		{
			albaLogMessage("Wrong DB check root node");
			return ALBA_ERROR;
		}

		Clear();

		Load(root);
	}
	catch (...) {
		return ALBA_ERROR;
	}

	cppDEL(errReporter);
	delete XMLParser;

	// terminate the XML library
	XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();

	albaLogMessage(_("DB Loaded"));

	return ALBA_OK;
}

//----------------------------------------------------------------------------
int albaProsthesesDBManager::SaveDB()
{
	//Open the file xml with manufacture and model information
	try {
		XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch) {
		// Do your failure processing here
		return ALBA_ERROR;
	}

	//CREATE BACKUP OF THE DB
	wxString backFile = m_DBFilename + ".bak";
	wxCopyFile(m_DBFilename.GetCStr(), backFile);

	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc;
	XMLCh tempStr[100];
	XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("LS", tempStr, 99);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation *impl = XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationRegistry::getDOMImplementation(tempStr);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMWriter* theSerializer = ((XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationLS*)impl)->createDOMWriter();
	theSerializer->setNewLine(albaXMLString("\r"));
	doc = impl->createDocument(NULL, albaXMLString(PRODB_NAME), NULL);

	doc->setEncoding(albaXMLString("UTF-8"));
	doc->setStandalone(true);
	doc->setVersion(albaXMLString("1.0"));

	// optionally you can set some features on this serializer
	if (theSerializer->canSetFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTDiscardDefaultContent, true))
		theSerializer->setFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTDiscardDefaultContent, true);

	if (theSerializer->canSetFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true))
		theSerializer->setFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true);

	XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatTarget *XMLTarget;
	XMLTarget = new XERCES_CPP_NAMESPACE_QUALIFIER LocalFileFormatTarget(m_DBFilename.GetCStr());
	
	// extract root element and wrap it with an albaXMLElement object
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *root = doc->getDocumentElement();
	assert(root);

	// attach version attribute to the root node
	root->setAttribute(albaXMLString(ATTR_VERSION), albaXMLString(DB_VERSION));

	
	//STORE Elements to DB
	Store(doc, root);

	try {
		// do the serialization through DOMWriter::writeNode();
		theSerializer->writeNode(XMLTarget, *doc);
	}
	catch (...) {
			return ALBA_ERROR;
	}

	theSerializer->release();
	cppDEL(XMLTarget);
	doc->release();

	XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();

	albaLogMessage(albaString::Format("New DB has been written %s", m_DBFilename.GetCStr()));

	return ALBA_OK;
}

//----------------------------------------------------------------------------
int albaProsthesesDBManager::Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *dbChildren = node->getChildNodes();

	for (unsigned int i = 0; i < dbChildren->getLength(); i++)
	{
		//Reading Type nodes 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *currentNode = dbChildren->item(i);


		//PRODUCERS
		if (CheckNodeElement(currentNode, NODE_PRODUCERS))
		{
			XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *producersChildren = currentNode->getChildNodes();

			for (unsigned int i = 0; i < producersChildren->getLength(); i++)
			{
				//Reading manufacturer nodes
				XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *producerNode = producersChildren->item(i);

				if (!CheckNodeElement(producerNode, NODE_PRODUCER))
					continue;

				albaProDBProducer *newProducer = new albaProDBProducer();
				m_Producers.push_back(newProducer);
				if (newProducer->Load(producerNode) == ALBA_ERROR)
					return ALBA_ERROR;
			}
		}
		//TYPES
		else if (CheckNodeElement(currentNode, NODE_TYPES))
		{
			XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *typesChildren = currentNode->getChildNodes();

			for (unsigned int i = 0; i < typesChildren->getLength(); i++)
			{
				//Reading manufacturer nodes
				XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *typeNode = typesChildren->item(i);
	
				if (!CheckNodeElement(typeNode, NODE_TYPE))
					continue;
	
				albaProDBType *newType = new albaProDBType();
				m_Types.push_back(newType);
				if (newType->Load(typeNode) == ALBA_ERROR)
					return ALBA_ERROR;
			}
		}
		//PROSTHESES
		else if (CheckNodeElement(currentNode, NODE_PROSTHESES))
		{
			XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *prosthesesChildren = currentNode->getChildNodes();

			for (unsigned int i = 0; i < prosthesesChildren->getLength(); i++)
			{
				//Reading manufacturer nodes
				XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *prosthesesNode = prosthesesChildren->item(i);
				
				if (!CheckNodeElement(prosthesesNode, NODE_PROSTHESIS))
					continue;

				albaProDBProsthesis *newProsthesis = new albaProDBProsthesis();
				m_Prostheses.push_back(newProsthesis);
				if (newProsthesis->Load(prosthesesNode) == ALBA_ERROR)
					return ALBA_ERROR;
			}
		}
		else 
		{
			continue;
		}
	}
}

void albaProsthesesDBManager::Store(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *node)
{
	//Producers
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *producersNode = doc->createElement(albaXMLString(NODE_PRODUCERS));
	node->appendChild(producersNode);
	for (int i = 0; i < m_Producers.size(); i++)
		m_Producers[i]->Store(doc, producersNode);

	//Types
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *typesNode = doc->createElement(albaXMLString(NODE_TYPES));
	node->appendChild(typesNode);
	for (int i = 0; i < m_Types.size(); i++)
		m_Types[i]->Store(doc, typesNode);

	//Prostheses
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *prosthesesNode = doc->createElement(albaXMLString(NODE_PROSTHESES));
	node->appendChild(prosthesesNode);
	for (int i = 0; i < m_Prostheses.size(); i++)
		m_Prostheses[i]->Store(doc, prosthesesNode);
}

//----------------------------------------------------------------------------
void albaProsthesesDBManager::Clear()
{
	for (int i = 0; i < m_Producers.size(); i++)
	{
		m_Producers[i]->Clear();
		delete m_Producers[i];
	}
	m_Producers.clear();

	for (int i = 0; i < m_Types.size(); i++)
	{
		m_Types[i]->Clear();
		delete m_Types[i];
	}
	m_Types.clear();

	for (int i = 0; i < m_Prostheses.size(); i++)
	{
		m_Prostheses[i]->Clear();
		delete m_Prostheses[i];
	}
	m_Prostheses.clear();
}

//----------------------------------------------------------------------------
void albaProsthesesDBManager::AddComponentFile(albaString fileName)
{
	for (int i = 0; i < m_ComponentsFiles.size(); i++)
		if (fileName == m_ComponentsFiles[i])
		{
			m_CompFilesNum[i]++;
			return;
		}

	m_ComponentsFiles.push_back(fileName);
	m_CompFilesNum.push_back(1);
}

//----------------------------------------------------------------------------
void albaProsthesesDBManager::RemoveComponentFile(albaString fileName)
{
	for (int i = 0; i < m_ComponentsFiles.size(); i++)
		if (fileName == m_ComponentsFiles[i])
		{
			m_CompFilesNum[i]--;

			if (m_CompFilesNum[i] == 0)
			{
				albaString fullFileName = GetDBDir();
				fullFileName+=fileName;

				wxRemoveFile(fullFileName.GetCStr());

				m_ComponentsFiles.erase(m_ComponentsFiles.begin() + i);
				m_CompFilesNum.erase(m_CompFilesNum.begin() + i);
			}

			return;
		}

}

//----------------------------------------------------------------------------
int albaProsthesesDBManager::GetComponentFileCount(albaString fileName)
{
	for (int i = 0; i < m_ComponentsFiles.size(); i++)
		if (fileName == m_ComponentsFiles[i])
		{
			return m_CompFilesNum[i];
		}

	return 0;
}

//----------------------------------------------------------------------------
std::vector<albaProDBProsthesis *> albaProsthesesDBManager::SearchProstheses(albaString producer, albaString type, albaString side)
{
	std::vector<albaProDBProsthesis *> proList;
	for (int i = 0; i < m_Prostheses.size(); i++)
	{
		albaProDBProsthesis *prosthesis = m_Prostheses[i];
		if (!producer.IsEmpty() && producer != prosthesis->GetProducer())
			continue;
		if (!type.IsEmpty() && type != prosthesis->GetType())
			continue;
		if (!side.IsEmpty() && albaProDBProsthesis::GetSideByString(side) != prosthesis->GetSide())
			continue;
		
		proList.push_back(prosthesis);
	}

	return proList;
}

//----------------------------------------------------------------------------
bool albaProsthesesDBManager::HasProsthesis(albaString prosthesis, albaProDBProsthesis::PRO_SIDES side)
{
	return GetProsthesis(prosthesis, side);
}

//----------------------------------------------------------------------------
albaProDBProsthesis * albaProsthesesDBManager::GetProsthesis(albaString prosthesis, albaProDBProsthesis::PRO_SIDES side)
{
	for (int i = 0; i < m_Prostheses.size(); i++)
	{
		if (m_Prostheses[i]->GetName() == prosthesis && m_Prostheses[i]->GetSide() == side)
			return m_Prostheses[i];
	}
	return NULL;
}

//----------------------------------------------------------------------------
void albaProsthesesDBManager::DeleteProsthesis(albaString prosthesis, albaProDBProsthesis::PRO_SIDES side)
{
	for (int i = 0; i < m_Prostheses.size(); i++)
	{
		if (m_Prostheses[i]->GetName() == prosthesis && m_Prostheses[i]->GetSide() == side)
			m_Prostheses.erase(m_Prostheses.begin() + i);
	}
}

//----------------------------------------------------------------------------
void albaProsthesesDBManager::AddProsthesis(albaProDBProsthesis *prosthesis)
{
	m_Prostheses.push_back(prosthesis);
}

//----------------------------------------------------------------------------
bool albaProsthesesDBManager::HasProducer(albaString producer)
{

	//TODO Remove component files!!!
	for (int i = 0; i < m_Producers.size(); i++)
	{
		if (m_Producers[i]->GetName() == producer)
			return true;
	}
	return false;
}

//----------------------------------------------------------------------------
void albaProsthesesDBManager::AddProducer(albaProDBProducer *producer)
{
	m_Producers.push_back(producer);
}

//----------------------------------------------------------------------------
bool albaProsthesesDBManager::HasType(albaString type)
{
	for (int i = 0; i < m_Types.size(); i++)
	{
		if (m_Types[i]->GetName() == type)
			return true;
	}
	return false;
}

//----------------------------------------------------------------------------
void albaProsthesesDBManager::AddType(albaProDBType *type)
{
	m_Types.push_back(type);
}

//----------------------------------------------------------------------------
void albaProsthesesDBManager::SetDBDir(albaString val)
{
	m_DBDir = val;
	m_DBFilename = m_DBDir + "ProsthesesDB.xml";

}

//----------------------------------------------------------------------------
int albaProDBCompGroup::Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
	//<Components Name="Stem">
	m_Name = GetElementAttribute(node, ATTR_NAME);

	if (m_Name == "")
	{
		albaLogMessage("ERROR: Prosthesis name is empty");
		return ALBA_ERROR;
	}
	
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *components = node->getChildNodes();

	for (unsigned int i = 0; i < components->getLength(); i++)
	{
		//Reading Type nodes 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *componentNode = components->item(i);


		if (!CheckNodeElement(componentNode, NODE_COMPONENT))
			continue;

		albaProDBComponent *component = new albaProDBComponent();

		m_Components.push_back(component);
		if (component->Load(componentNode) == ALBA_ERROR)
			return ALBA_ERROR;
	}
	return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaProDBCompGroup::Store(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *node)
{
	//Types
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * groupNode = doc->createElement(albaXMLString(NODE_COMPONENTS));
	groupNode->setAttribute(albaXMLString(ATTR_NAME), albaXMLString(m_Name));
	node->appendChild(groupNode);
	for (int i = 0; i < m_Components.size(); i++)
		m_Components[i]->Store(doc, groupNode);
}

//----------------------------------------------------------------------------
vtkPolyData * albaProDBComponent::GetVTKData()
{
	albaProsthesesDBManager *dbManager=GetLogicManager()->GetProsthesesDBManager();

	albaString fileData = dbManager->GetDBDir();
	fileData+=m_Filename;

	std::string fileMemory;
	albaDecryptFileInMemory(fileData, fileMemory, dbManager->GetPassPhrase());
	if (fileMemory.empty())
	{
		//albaLogMessage("Decryption Error! On file:%s", fileData.GetCStr());
		//wxMessageBox(_("Decryption Error!"));
		return NULL;
	}

	vtkALBASmartPointer<vtkPolyDataReader> reader;
	reader->SetFileName(fileData.GetCStr());
	reader->ReadFromInputStringOn();
	reader->SetInputString(fileMemory.c_str(), fileMemory.size());
	reader->Update();


	vtkPolyData * output = reader->GetOutput();

	output->Register(NULL);

	return output;
}

//----------------------------------------------------------------------------
void albaProDBComponent::SetVTKData(vtkPolyData *vtkData)
{
	albaProsthesesDBManager * prosthesesDBManager = GetLogicManager()->GetProsthesesDBManager();
	
	
	if (!m_Filename.IsEmpty())
		prosthesesDBManager->RemoveComponentFile(m_Filename);
	
	vtkALBASmartPointer<vtkDataSetWriter> writer;
	writer->SetInput(vtkData);
	writer->SetFileTypeToBinary();
	writer->SetHeader("# ALBA Prosthesis component data file \n");

	writer->WriteToOutputStringOn();
	writer->Write();
	char* outStr = writer->GetOutputString();
	int outStrLen = writer->GetOutputStringLength();

	std::string sha256Str;

	albaCalculateteSHA256(outStr, outStrLen, sha256Str);

	sha256Str += ".cry";

	m_Filename = sha256Str.c_str(); 
	prosthesesDBManager->AddComponentFile(m_Filename);

	//File already exists nothing to do
	if (prosthesesDBManager->GetComponentFileCount(m_Filename) > 1)
		return;

	albaString fullFilename = prosthesesDBManager->GetDBDir() + sha256Str.c_str();

	albaEncryptFileFromMemory(outStr, outStrLen, fullFilename, prosthesesDBManager->GetPassPhrase());
}
	

//----------------------------------------------------------------------------
int albaProDBComponent::Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
	//<Component Name="S1" File="S1.cry">

	m_Name = GetElementAttribute(node, ATTR_NAME);
	m_Filename = GetElementAttribute(node, ATTR_FILE);


	if (m_Name == "")
	{
		albaLogMessage("ERROR: Component name is empty");
		return ALBA_ERROR;
	}
	if (m_Filename == "")
	{
		albaLogMessage("ERROR: Component filename is empty");
		return ALBA_ERROR;
	}

	GetLogicManager()->GetProsthesesDBManager()->AddComponentFile(m_Filename);

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *components = node->getChildNodes();

	for (unsigned int i = 0; i < components->getLength(); i++)
	{
		//Reading Type nodes 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *matrixNode = components->item(i);


		if (!CheckNodeElement(matrixNode, NODE_MATRIX))
			continue;

		double *el = *m_Matrix.GetElements();


		albaString mtrStr(matrixNode->getTextContent());
		
		int nRead=sscanf(mtrStr.GetCStr(), "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", el+0, el+1, el+2, el+3, el+4, el+5, el+6, el+7, el+8, el+9, el+10, el+11, el+12, el+13, el+14, el+15);

		if (nRead != 16)
		{
			albaLogMessage("ERROR wrong number of element inside matrix");
			return ALBA_ERROR;
		}


	}
	return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaProDBComponent::Store(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *node)
{
	//Types
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * componentNode = doc->createElement(albaXMLString(NODE_COMPONENT));

	componentNode->setAttribute(albaXMLString(ATTR_NAME), albaXMLString(m_Name));
	componentNode->setAttribute(albaXMLString(ATTR_FILE), albaXMLString(m_Filename));

	albaMatrix identity;

	if (!m_Matrix.Equals(&identity))
	{
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * matrixtNode = doc->createElement(albaXMLString(NODE_MATRIX));

		char tmpStr[1024];
		double *el = *m_Matrix.GetElements();
		sprintf(tmpStr, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", el[0], el[1], el[2], el[3], el[4], el[5], el[6], el[7], el[8], el[9], el[10], el[11], el[12], el[13], el[14], el[15]);
		matrixtNode->setTextContent(albaXMLString(tmpStr));

		componentNode->appendChild(matrixtNode);
	}

	node->appendChild(componentNode);
}

//----------------------------------------------------------------------------
int albaProDBType::Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
	//<Type Name="Acetabular"/>
	
	m_Name = GetElementAttribute(node, ATTR_NAME);

	if (m_Name == "")
	{
		albaLogMessage("ERROR: Type name is empty");
		return ALBA_ERROR;
	}
	return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaProDBType::Store(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *node)
{
	//Types
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * typeNode = doc->createElement(albaXMLString(NODE_TYPE));
	typeNode->setAttribute(albaXMLString(ATTR_NAME), albaXMLString(m_Name));
	node->appendChild(typeNode);
}

//----------------------------------------------------------------------------
int albaProDBProducer::Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
	//<Producer Name="Producer" Img="Producer.png" Site="http://www.prosthesisProducer.com/"/>

	m_Name = GetElementAttribute(node, ATTR_NAME);
	m_ImgFileName = GetElementAttribute(node, ATTR_IMG);
	m_WebSite = GetElementAttribute(node, ATTR_SITE);

	if (m_Name == "")
	{
		albaLogMessage("ERROR: Producer name is empty");
		return ALBA_ERROR;
	}
	return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaProDBProducer::Store(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *node)
{
	//Producers
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *producerNode = doc->createElement(albaXMLString(NODE_PRODUCER));
	producerNode->setAttribute(albaXMLString(ATTR_NAME), albaXMLString(m_Name));
	producerNode->setAttribute(albaXMLString(ATTR_IMG), albaXMLString(m_ImgFileName));
	producerNode->setAttribute(albaXMLString(ATTR_SITE), albaXMLString(m_WebSite));
	node->appendChild(producerNode);
}

int albaProDBProsthesis::Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
	//<Prosthesis Name="Example" Producer="Producer" Type="Acetabular" Side="Both" Img="Example.png">
	m_Name = GetElementAttribute(node, ATTR_NAME);
	m_ImgFileName = GetElementAttribute(node, ATTR_IMG);
	m_Side = GetSideByString(GetElementAttribute(node, ATTR_SIDE));
	m_Type = GetElementAttribute(node, ATTR_TYPE);
	m_Producer = GetElementAttribute(node, ATTR_PRODUCER);
	albaString bendingAngle = GetElementAttribute(node, ATTR_BENDINGANGLE);
	m_BendingAngle = atof(bendingAngle.GetCStr());

	if (m_Name == "")
	{
		albaLogMessage("ERROR: Prosthesis name is empty");
		return ALBA_ERROR;
	}
	if (m_Side == PRO_UKNOWN)
	{
		albaLogMessage("ERROR: Prosthesis Side is unknown");
		return ALBA_ERROR;
	}

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *componentGroups = node->getChildNodes();

	for (unsigned int i = 0; i < componentGroups->getLength(); i++)
	{
		//Reading Type nodes 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *componentGroupNode = componentGroups->item(i);


		if (!CheckNodeElement(componentGroupNode, NODE_COMPONENTS))
			continue;

		albaProDBCompGroup *componentGroup = new albaProDBCompGroup();

		m_CompGroups.push_back(componentGroup);
		if (componentGroup->Load(componentGroupNode) == ALBA_ERROR)
			return ALBA_ERROR;

	}
	return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaProDBProsthesis::Store(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *node)
{
	//Producers
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *prosthesisNode = doc->createElement(albaXMLString(NODE_PROSTHESIS));
	prosthesisNode->setAttribute(albaXMLString(ATTR_NAME), albaXMLString(m_Name));
	prosthesisNode->setAttribute(albaXMLString(ATTR_IMG), albaXMLString(m_ImgFileName));
	prosthesisNode->setAttribute(albaXMLString(ATTR_TYPE), albaXMLString(m_Type));
	prosthesisNode->setAttribute(albaXMLString(ATTR_PRODUCER), albaXMLString(m_Producer));
	prosthesisNode->setAttribute(albaXMLString(ATTR_SIDE), albaXMLString(GetSideAsStr(m_Side)));
	prosthesisNode->setAttribute(albaXMLString(ATTR_BENDINGANGLE), albaXMLString(albaString(m_BendingAngle)));
	node->appendChild(prosthesisNode);
	for (int i = 0; i < m_CompGroups.size(); i++)
		m_CompGroups[i]->Store(doc, prosthesisNode);

}


//----------------------------------------------------------------------------
bool ProStorable::CheckNodeElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *elementName)
{
	//Reading manufacturer nodes
	if (node->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
		return false;

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *nodeElement = (XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*)node;
	albaString nameElement = "";
	nameElement = albaXMLString(nodeElement->getTagName());
	return (nameElement == elementName);
}

//----------------------------------------------------------------------------
albaString ProStorable::GetElementAttribute(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *attributeName)
{
	if (node->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
		return "";
	return albaXMLString(((XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *)node)->getAttribute(albaXMLString(attributeName)));
}

//----------------------------------------------------------------------------
void albaProDBProducer::Clear()
{
	//nothing to do
}

//----------------------------------------------------------------------------
void albaProDBType::Clear()
{
	//nothing to do
}

//----------------------------------------------------------------------------
void albaProDBComponent::Clear()
{
	//nothing to do
}

//----------------------------------------------------------------------------
void albaProDBCompGroup::Clear()
{
	for (int i = 0; i < m_Components.size(); i++)
	{
		m_Components[i]->Clear();
		delete m_Components[i];
	}

	m_Components.clear();
}

//----------------------------------------------------------------------------
void albaProDBCompGroup::AddComponent(albaProDBComponent *component)
{
	m_Components.push_back(component);
}

//----------------------------------------------------------------------------
void albaProDBProsthesis::Clear()
{

	for (int i = 0; i < m_CompGroups.size(); i++)
	{
		m_CompGroups[i]->Clear();
		delete m_CompGroups[i];
	}

	m_CompGroups.clear();
}

//----------------------------------------------------------------------------
albaProDBProsthesis::PRO_SIDES albaProDBProsthesis::GetSideByString(albaString sideName)
{
	if (sideName == "Left")
		return PRO_LEFT;
	else if (sideName == "Right")
		return PRO_RIGHT;
	else if (sideName == "Both")
		return PRO_BOTH;
	else
		return PRO_UKNOWN;
}

//----------------------------------------------------------------------------
char * albaProDBProsthesis::GetSideAsStr(PRO_SIDES side)
{
	switch (side)
	{
		case albaProDBProsthesis::PRO_LEFT:
			return "Left";
			break;
		case albaProDBProsthesis::PRO_RIGHT:
			return "Right";
			break;
		case albaProDBProsthesis::PRO_BOTH:
			return "Both";
			break;
		default:
			return "Error";
			break;
	}
}

//----------------------------------------------------------------------------
void albaProDBProsthesis::AddCompGroup(albaProDBCompGroup *group)
{
	m_CompGroups.push_back(group);
}
