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

#ifndef __albaProsthesisDBManager_H__
#define __albaProsthesisDBManager_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>
#include <set>
#include "albaServiceClient.h"
#include "albaMatrix.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class albaWizardWaitOp;
class albaGUISettingsDialog;
class vtkPolyData;

class ProStorable
{
public:
	virtual int Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) = 0;
	virtual void Store(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *node) = 0;
	virtual void Clear() = 0;

protected:
	bool CheckNodeElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *elementName);
	albaString GetElementAttribute(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *attributeName);
};

/**
Class Name: ProProducer.
*/
class albaProDBProducer : public ProStorable
{

public:
	albaString GetName() const { return m_Name; }
	void SetName(albaString val) { m_Name = val; }
	albaString GetImgFileName() const { return m_ImgFileName; }
	void SetImgFileName(albaString val) { m_ImgFileName = val; }
	albaString GetWebSite() const { return m_WebSite; }
	void SetWebSite(albaString val) { m_WebSite = val; }
	
	// Inherited via ProStorable
	virtual int Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) override;
	virtual void Store(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *node) override;
	virtual void Clear() override;

protected:
	albaString m_Name;
	albaString m_ImgFileName;
	albaString m_WebSite;
};

/**
Class Name: ProType.
*/
class albaProDBType : public ProStorable
{
public:
	albaString GetName() const { return m_Name; }
	void SetName(albaString val) { m_Name = val; }

	// Inherited via ProStorable
	virtual int Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) override;
	virtual void Store(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *node) override;
	virtual void Clear() override;

protected:
	albaString m_Name;
};

class albaProDBComponent : public ProStorable, public albaServiceClient
{
public:
	albaMatrix GetMatrix() const { return m_Matrix; }
	void SetMatrix(albaMatrix val) { m_Matrix = val; }
	albaString GetName() const { return m_Name; }
	void SetName(albaString val) { m_Name = val; }
	vtkPolyData *GetVTKData();
	void SetVTKData(vtkPolyData *vtkData);
	albaString GetDataFileName() const { return m_Filename; }

	// Inherited via ProStorable
	virtual int Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) override;
	virtual void Store(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *node) override;
	virtual void Clear() override;

protected:
	albaMatrix m_Matrix;
	albaString m_Name;
	albaString m_Filename;
};

/**
Class Name: albaProsthesisDBManager.
*/
class albaProDBCompGroup : public ProStorable
{
public:
	albaString GetName() const { return m_Name; }
	void SetName(albaString val) { m_Name = val; }

	// Inherited via ProStorable
	virtual int Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) override;
	virtual void Store(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *node) override;
	virtual void Clear() override;

	/** Returns the components list*/
	std::vector<albaProDBComponent *> *GetComponents() { return &m_Components; }
	void AddComponent(albaProDBComponent *component);
protected:
	albaString m_Name;
	std::vector<albaProDBComponent *> m_Components;
};


class albaProDBProsthesis: public ProStorable
{
public: 

	enum PRO_SIDES
	{
		PRO_UKNOWN = -1,
		PRO_LEFT,
		PRO_RIGHT,
		PRO_BOTH,
	};

	albaString GetName() const { return m_Name; }
	void SetName(albaString val) { m_Name = val; }
	albaString GetImgFileName() const { return m_ImgFileName; }
	void SetImgFileName(albaString val) { m_ImgFileName = val; }
	albaString GetType() const { return m_Type; }
	void SetType(albaString val) { m_Type = val; }
	albaString GetProducer() const { return m_Producer; }
	void SetProducer(albaString val) { m_Producer = val; }
	albaProDBProsthesis::PRO_SIDES GetSide() const { return m_Side; }
	void SetSide(albaProDBProsthesis::PRO_SIDES val) { m_Side = val; }
	double GetBendingAngle() const { return m_BendingAngle; }
	void SetBendingAngle(double val) { m_BendingAngle = val; }


	// Inherited via ProStorable
	virtual int Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) override;
	virtual void Store(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *node) override;
	virtual void Clear() override;

	static PRO_SIDES GetSideByString(albaString sideName);
	static char *GetSideAsStr(PRO_SIDES side);

	/** returns the vector of components */
	std::vector<albaProDBCompGroup *> *GetCompGroups() { return &m_CompGroups; }
	void AddCompGroup(albaProDBCompGroup *group);
protected:
	albaString m_Name;
	albaString m_ImgFileName;
	albaString m_Producer;
	albaString m_Type;
	double m_BendingAngle;
	PRO_SIDES m_Side;

	std::vector<albaProDBCompGroup *> m_CompGroups;

};

/**
  Class Name: albaProsthesesDBManager.
*/
class ALBA_EXPORT albaProsthesesDBManager : public ProStorable
{
public:

  /** Default constructor */
  albaProsthesesDBManager();

  /** Default destructor */
  ~albaProsthesesDBManager();

	// Inherited via ProStorable

	int LoadDB();
	int SaveDB();

	virtual int Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) override;
	virtual void Store(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *node) override;

	virtual void Clear() override;

	void AddComponentFile(albaString fileName);

	//This function decrease the count for stored files and delete the file if there will be no components that uses it
	void RemoveComponentFile(albaString fileName);

	/** returns the number of components that uses the file, zero if there are no components */
	int GetComponentFileCount(albaString fileName);

	std::vector<albaProDBProsthesis *> SearchProstheses(albaString producer, albaString type, albaString side);
	std::vector<albaProDBProsthesis *>& GetProstheses() { return m_Prostheses; };
	bool HasProsthesis(albaString prosthesis, albaProDBProsthesis::PRO_SIDES side);
	albaProDBProsthesis * GetProsthesis(albaString prosthesis, albaProDBProsthesis::PRO_SIDES side);
	void DeleteProsthesis(albaString prosthesis, albaProDBProsthesis::PRO_SIDES side);
	void AddProsthesis(albaProDBProsthesis *prosthesis);

	std::vector<albaProDBProducer *>& GetProducers() { return m_Producers; };
	bool HasProducer(albaString producer);
	void AddProducer(albaProDBProducer *producer);

	std::vector<albaProDBType *>& GetTypes() { return m_Types; };
	bool HasType(albaString type);
	void AddType(albaProDBType *type);




// 	void LoadDBFromFile(albaString DBFile) { Clear(); m_DBFilename = DBFile; LoadDB(); };
 	void LoadDBFromFile(albaString DBFile, bool append = true) { if(!append) Clear(); m_DBFilename = DBFile; LoadDB(); };
 	void SaveDBToFile(albaString DBFile) { m_DBFilename = DBFile; SaveDB(); };
// 
	albaString GetDBDir() const { return m_DBDir; }
	void SetDBDir(albaString val);
	albaString GetPassPhrase() const { return m_PassPhrase; }
	void SetPassPhrase(albaString val) { m_PassPhrase = val; }
private:
	albaString m_DBDir;
	albaString m_DBFilename;
	albaString m_PassPhrase;

	std::vector<albaProDBProducer *> m_Producers;
	std::vector<albaProDBType *> m_Types;
	std::vector<albaProDBProsthesis *>  m_Prostheses;

	std::vector<albaString> m_ComponentsFiles;
	std::vector<int> m_CompFilesNum;


};
#endif
