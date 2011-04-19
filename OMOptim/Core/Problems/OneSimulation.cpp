﻿// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR 
 * THIS OSMC PUBLIC LICENSE (OSMC-PL). 
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE
 * OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3, ACCORDING TO RECIPIENTS CHOICE. 
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or  
 * http://www.openmodelica.org, and in the OpenModelica distribution. 
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

 	@file OneSimulation.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "OneSimulation.h"
#include "Problem.h"
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include "Variable.h"
#include "LowTools.h"
#include "CSV.h"
#include "reportingHook.h"

OneSimulation::OneSimulation()
    :Problem()
{
	_project = NULL;
	_type = Problem::ONESIMULATION;
	_name="One Simulation";
	_overwritedVariables = new MOVector<Variable>;
	_scannedVariables = new MOVector<ScannedVariable>;
}

OneSimulation::OneSimulation(Project* project,ModClass* rootModClass,ModReader* modReader,ModPlusCtrl* modPlusCtrl,ModModelPlus* modModelPlus)
    :Problem()
{
	_project = project;
	_rootModClass = rootModClass;
	_modModelPlus = modModelPlus;
	_modReader = modReader;
	_modPlusCtrl = modPlusCtrl;
	_type = Problem::ONESIMULATION;
	_name="One Simulation";
	

	_overwritedVariables = new MOVector<Variable>;
	_scannedVariables = new MOVector<ScannedVariable>;

}

OneSimulation::OneSimulation(const OneSimulation &oneSim)
:Problem(oneSim)
{
	_modModelPlus = oneSim._modModelPlus;
	_neededFiles = oneSim._neededFiles;
	_filesToCopyNames = oneSim._filesToCopyNames;

        _overwritedVariables = oneSim._overwritedVariables->clone();
        _scannedVariables = oneSim._scannedVariables->clone();

	if(oneSim.result()!=NULL)
		_result = new OneSimResult(*((OneSimResult*)oneSim.result())) ;
}


OneSimulation::~OneSimulation(void)
{
	delete _overwritedVariables;
	delete _scannedVariables;

        clearResult();
}



//
//void OneSimulation::fillTempDir(QString tempDir)
//{
//	QDir modelTempDir(tempDir);
//	//if(modelTempDir.exists())
//	//	LowTools::removeDir(tempDir);
//
//	modelTempDir.mkdir(tempDir);
//	
//	modPlusReader->copyFilesToTempDir(modModelPlus,tempDir);
//}

bool OneSimulation::checkBeforeComp(QString & error)
{
	bool ok = true;
	return ok;
}

void OneSimulation::launch(ProblemConfig config){

	emit begun(this);

	// Creating a variables instance containing updated variables
	//MOVector<Variable> inputVariables(*modModelPlus->variables());
	//inputVariables.replaceIn(overwritedVariables);
	
	MOVector<Variable> updatedVariables(*_overwritedVariables);
	
	// loop indexes on scannVariables
	QList<int> indexes,maxIndexes;
	Variable* clonedVar;
	ScannedVariable *scannedVar;
	for(int iScanV=0; iScanV < _scannedVariables->items.size(); iScanV++)
	{
		indexes.push_back(0);
		scannedVar = _scannedVariables->items.at(iScanV);
		maxIndexes.push_back(scannedVar->nbScans());
		clonedVar = new Variable(*(dynamic_cast<Variable*>(scannedVar)));
		updatedVariables.addItem(clonedVar);
	}

	if(_result)
		delete _result;
	_result = new OneSimResult(_project,_modModelPlus,this,_modReader,_modPlusCtrl);
	_result->setName(this->name());
	
	OneSimResult* thisResult = dynamic_cast<OneSimResult*>(_result);
	MOVector<Variable> curVariables;
	bool simSuccess=true;
	do
	{
		// Update values
		VariablesManip::updateScanValues(&updatedVariables,_scannedVariables,indexes);
		// Simulate
		curVariables.clear();
		simSuccess = simSuccess && _modModelPlus->ctrl()->simulate(config.tempDir, &updatedVariables, &curVariables);

		if(simSuccess)
		{
			// Add values
			double curValue;
			//if it is first scan, finalvariables is an empy vector -> fill with curVariables
			if(thisResult->finalVariables()->items.isEmpty())
			{
				for(int i=0;i<curVariables.items.size();i++)
				{
					thisResult->finalVariables()->addItem(new VariableResult(*curVariables.items.at(i)));
					curValue = curVariables.items.at(i)->getFieldValue(Variable::VALUE).toDouble();
					thisResult->finalVariables()->items.at(i)->appendScanValue(curValue,true);
				}
			}
			else
			{
				// append scan values
				int iVar;
				for(int i=0;i<thisResult->finalVariables()->items.size();i++)
				{
					iVar = curVariables.findItem(thisResult->finalVariables()->items.at(i)->name());
					if(iVar>-1)
					{
						curValue = curVariables.items.at(iVar)->getFieldValue(Variable::VALUE).toDouble();
						thisResult->finalVariables()->items.at(i)->appendScanValue(curValue,true);
					}
					else
						thisResult->finalVariables()->items.at(i)->appendScanValue(-1,false);
				}
			}
		}
		
		indexes = LowTools::nextIndex(indexes,maxIndexes);
	}
	while(!indexes.isEmpty() && simSuccess);

	_result->setSuccess(simSuccess);
	emit finished(this);
}
//
//void OneSimulation::readResults(QString dirPath)
//{
//	//Checking if successed
//	bool success=QFile::exists(dirPath+QDir::separator()+"success");
//
//	OneSimResult* thisResult = dynamic_cast<OneSimResult*>(result);
//
//	thisResult->setSuccess(success);
//
//
//	if(success)
//	{
//		// Copying input variables
//		MOVector<Variable> inputVariables_(*modModelPlus->variables());
//		inputVariables_.replaceIn(overwritedVariables);
//		thisResult->inputVariables->cloneFromOtherVector(&inputVariables_);
//
//		// Getting result variables values from dymosim output file
//		QString dsresFile = dirPath + QDir::separator() + "dsres.txt";
//		MOVector<Variable> *curVariables = new MOVector<Variable>();
//		modPlusReader->readFinalVariables(modModelPlus,curVariables,dsresFile);
//
//
//		// Adding values
//		double curValue;
//		//if it is first scan, finalvariables is an empy vector -> fill with curVariables
//		if(thisResult->finalVariables->items.isEmpty())
//		{
//			for(int i=0;i<curVariables->items.size();i++)
//			{
//				thisResult->finalVariables->addItem(new VariableResult(*curVariables->items.at(i)));
//				curValue = curVariables->items.at(i)->getFieldValue(Variable::VALUE).toDouble();
//				thisResult->finalVariables->items.at(i)->appendScanValue(
//					curValue,true);
//			}
//		}
//		else
//		{
//			// append scan values
//			int iVar;
//			for(int i=0;i<thisResult->finalVariables->items.size();i++)
//			{
//				iVar = curVariables->findItem(thisResult->finalVariables->items.at(i)->name());
//				if(iVar>-1)
//				{
//					curValue = curVariables->items.at(iVar)->getFieldValue(Variable::VALUE).toDouble();
//					thisResult->finalVariables->items.at(i)->appendScanValue(
//						curValue,true);
//				}
//				else
//					thisResult->finalVariables->items.at(i)->appendScanValue(-1,false);
//			}
//		}
//		delete curVariables;
//
//	}
//}

OneSimResult* OneSimulation::result() const
{
	return (OneSimResult*)_result;
}

void OneSimulation::store(QString destFolder, QString tempDir)
{
	Problem::store(destFolder,tempDir);
}

QDomElement OneSimulation::toXMLData(QDomDocument & doc)
{
	QDomElement cProblem = doc.createElement(getClassName());

	//***********************
	// Problem definition
	//***********************
	QDomElement cInfos = doc.createElement("Infos");
	cProblem.appendChild(cInfos);
	cInfos.setAttribute("name",name());
	cInfos.setAttribute("type",type());
	cInfos.setAttribute("model", _modModelPlus->modModelName());

	// overwrited variables
	QDomElement cOverVariables = _overwritedVariables->toXmlData(doc,"OverwritedVariables");
	cProblem.appendChild(cOverVariables);

		// Scanned Variables
	QDomElement cScanVars = _scannedVariables->toXmlData(doc,"ScannedVariables");
	cProblem.appendChild(cScanVars);

	return cProblem;
}

ModModelPlus* OneSimulation::modModelPlus()
{
	return _modModelPlus;
}

void OneSimulation::setModModelPlus(ModModelPlus* modModelPlus)
{
	_modModelPlus = modModelPlus;
}