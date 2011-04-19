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

 	@file MOVector.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#if !defined(_MOVECTOR_H)
#define _MOVECTOR_H

#include <vector>
#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QAbstractTableModel>
#include <QtCore/QTextStream>
#include <QtGui/QSortFilterProxyModel>
#include <QtXml/QDomDocument>
#include <QtCore/QStringList>

#include "XMLTools.h"
#include "InfoSender.h"

/** MOVector :Container for MOElements (e.g. Variable, OptObjective...)
It uses template C++ functionality
It is therefore needed to keep in the same file functions declarations and definitions
*/

template<class ItemClass>
class MOVector : public QAbstractTableModel 
{

public:
	QList<ItemClass*> items;


	MOVector();
	MOVector(const MOVector &);
	MOVector(QString savedData);
	MOVector(QDomElement & domList);

	~MOVector();

	QStringList getItemNames();

	void append(std::vector<ItemClass*>* toAppend);
	void setItems(QDomElement & domList);
	void append(const MOVector &,bool makeACopy);
	void update(const QDomElement & domList);

	int rowCount(const QModelIndex &parent ) const;
	int columnCount(const QModelIndex &parent ) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	virtual void addItem(ItemClass*);
	void removeRow(int index,const QModelIndex &parent = QModelIndex());
	void removeRow(QString);
	bool removeRows(int index, int count,const QModelIndex &parent = QModelIndex());
	bool removeRows(QList<int> indexes);
	int findItem(QString,Qt::CaseSensitivity=Qt::CaseInsensitive);
        int findItem(QVariant itemFieldValue,int iField);
	bool alreadyIn(QString);
	void replaceIn(MOVector<ItemClass> *);

	void cloneFromOtherVector(const MOVector*);
	MOVector<ItemClass>* clone();
	void clear();

	QModelIndex index(int row, int column, const QModelIndex &parent)const;

	// save and load functions
	virtual QString toSavingString();
	virtual QDomElement toXmlData(QDomDocument & doc,QString listTitle);

};





template<class ItemClass>
MOVector<ItemClass>::MOVector() : QAbstractTableModel()
{
	//items = new std::vector<ItemClass*>;
}



template<class ItemClass>
MOVector<ItemClass>::MOVector(const MOVector & test_)
{
	int iv;
	for(iv=0;iv<test_.items.size();iv++)
	{
		addItem(new ItemClass(*test_.items.at(iv)));
	}
}

template<class ItemClass>
MOVector<ItemClass>::MOVector(QString savedData)
{

	QStringList lineList = savedData.split("\n");
	QStringList fields;


	for(int nl=0;nl<lineList.size();nl++)
	{
		///QString test("#");

		if(!lineList[nl].isEmpty() && (QString(lineList[nl].at(0)).compare(QString("#")))!=0)
		{
			items.push_back(new ItemClass(lineList[nl]));
		}
	}
}


template<class ItemClass>
MOVector<ItemClass>::MOVector(QDomElement & domList)
{
	setItems(domList);
}



template<class ItemClass>
MOVector<ItemClass>::~MOVector() 
{
	//delete contents
	clear();
}

template<class ItemClass>
void MOVector<ItemClass>::update(const QDomElement & domList)
{
	QDomNode n = domList.firstChild();
	while( !n.isNull() )
	{
		QDomElement domItem = n.toElement();
		QDomNamedNodeMap attributes = domItem.attributes();
		QDomNode nameNode = attributes.namedItem("Name");
		QString name = nameNode.toAttr().value().replace(XMLTools::space()," ");

		bool in=false;
		int index=-1;
		if(!nameNode.isNull())
		{
			index = findItem(name);
		}
		if(index>-1)
			items.at(index)->update(domItem);
		else
			addItem(new ItemClass(domItem));

		n = n.nextSibling();
	}
}

template<class ItemClass>
QStringList MOVector<ItemClass>::getItemNames()
{
	QStringList _names;
	for(int i=0;i<items.size();i++)
		_names.push_back(items.at(i)->name());
	return _names;
}

template<class ItemClass>
void MOVector<ItemClass>::append(std::vector<ItemClass*>* toAppend )
{
	int iCurItem;
	for(int i=0;i<toAppend->size();i++)
	{
		iCurItem = findItem(toAppend->at(i)->name());
		if(iCurItem==-1)
			addItem(toAppend->at(i));
		else
		{
			delete items.at(iCurItem);
			items.replace(iCurItem,toAppend->at(i));
		}
	}
}

template<class ItemClass>
void MOVector<ItemClass>::setItems(QDomElement & domList)
{
	clear();

	QDomNode n = domList.firstChild();
	while( !n.isNull() )
	{
		QDomElement domItem = n.toElement();
		QDomAttr domAttr;
		ItemClass* newItem;
		if( !domItem.isNull() )
		{
			newItem = new ItemClass(domItem);
			addItem(newItem);
		}
		n = n.nextSibling();
	}
}

template<class ItemClass>
void MOVector<ItemClass>::append(const MOVector &_vector,bool makeACopy)
{
	int iCurItem;
	for(int i=0;i<_vector.items.size();i++)
	{
		iCurItem = findItem(_vector.items.at(i)->name());
		if(iCurItem==-1)
		{
			if(makeACopy)
				addItem(new ItemClass(*_vector.items.at(i)));
			else
				addItem(_vector.items.at(i));
		}
		else
		{
			infoSender.debug("replace item in vector (name : "+_vector.items.at(i)->name()+")");
			delete items.at(iCurItem);
			if(makeACopy)
				items.replace(iCurItem,new ItemClass(*_vector.items.at(i)));
			else
				items.replace(iCurItem,_vector.items.at(i));
		}
	}
}

template<class ItemClass>
int MOVector<ItemClass>::rowCount(const QModelIndex &parent ) const
{
	Q_UNUSED(parent);
	return items.size();
}

template<class ItemClass>
int MOVector<ItemClass>::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return ItemClass::nbFields;
}

template<class ItemClass>
QVariant MOVector<ItemClass>::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation==Qt::Horizontal)
	{
		if (role == Qt::DisplayRole)
		{
			return ItemClass::sFieldName(section,role);
		}
	}
	return QAbstractItemModel::headerData(section, orientation, role);
};

template<class ItemClass>
QVariant MOVector<ItemClass>::data(const QModelIndex &index, int role) const
{
	QVariant result;
	if (!index.isValid()) return QVariant();

	ItemClass* curItem;
	if(index.row()<items.size() && index.column()<ItemClass::nbFields)
	{		
		curItem = items.at(index.row());
	}
	else
		return QVariant();
	switch(role)
	{
	case Qt::DisplayRole :
	case Qt::EditRole :
		result = items.at(index.row())->getFieldValue(index.column(),role);
		if(result.toDouble()==1E100)
			return "1e100";
		else
			return result;
		break;
	case Qt::ToolTipRole :
		return curItem->getStrToolTip();
		break;
	}
	return QVariant();
}

template<class ItemClass>
Qt::ItemFlags MOVector<ItemClass>::flags(const QModelIndex &index) const
{
	if(!index.isValid())
		return 0;

	if(items.at(index.row())->isEditableField(index.column()))
		return Qt::ItemIsEnabled | Qt::ItemIsEditable;
	else
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

template<class ItemClass>
bool MOVector<ItemClass>::setData(const QModelIndex &index, const QVariant &value, int role)
{
	return items[index.row()]->setFieldValue(index.column(),value);
}

template<class ItemClass>
void MOVector<ItemClass>::addItem(ItemClass* item_)
{
	// Add an item pointer in Vector
	int index=items.size();
	insertRow(index);//,createIndex(0,0));
	beginInsertRows(QModelIndex(),index,index);
	items.push_back(item_);
	endInsertRows();	
}

template<class ItemClass>
void MOVector<ItemClass>::removeRow(int index,const QModelIndex &parent)
{
	if(index>-1)
	{
		beginRemoveRows(parent,index,index);
		delete items.at(index);
		items.erase(items.begin()+index);
		endRemoveRows();
	}
}
template<class ItemClass>
bool MOVector<ItemClass>::removeRows(int index, int count, const QModelIndex &parent)
{
	beginRemoveRows(QModelIndex(),index,index+count-1);
	if(items.size()>=index+count)
	{
		for(int i=0;i<count;i++)
		{
			delete items.at(index);
			items.erase(items.begin()+index);
		}
		endRemoveRows();
		return true;
	}
	else
	{
		return false;
	}
}

template<class ItemClass>
bool MOVector<ItemClass>::removeRows(QList<int> indexes)
{
	qSort(indexes.begin(),indexes.end());

	// start by the end in order not to be affected
	for(int i=indexes.size()-1;i>=0;i--)
	{
		removeRow(indexes.at(i));
	}
	return true;
}


template<class ItemClass>
void MOVector<ItemClass>::removeRow(QString itemName)
{
	int index = findItem(itemName);
	if (index!=-1)
	{
		removeRow(index);
	}
}



template<class ItemClass>
int MOVector<ItemClass>::findItem(QString itemName, Qt::CaseSensitivity caseSens)
{
	bool found = false;
	int i=0;
	int nbItems=items.size();
	QString itemName2;

	while((!found)&&(i<nbItems))
	{
		itemName2=items.at(i)->getFieldValue(ItemClass::NAME).toString();
		found=(itemName.compare(itemName2,caseSens)==0);
		i++;
	}
	if(!found)
	{
		return -1;
	}
	else
	{
		return i-1;
	}
}

template<class ItemClass>
int MOVector<ItemClass>::findItem(QVariant itemFieldValue, int iField)
{
	bool found = false;
	int i=0;
	int nbItems=items.size();
	QVariant curFieldValue;

	while((!found)&&(i<nbItems))
	{
		curFieldValue=items.at(i)->getFieldValue(iField);
		found=(itemFieldValue == curFieldValue);
		i++;
	}
	if(!found)
	{
		return -1;
	}
	else
	{
		return i-1;
	}
}

template<class ItemClass>
bool MOVector<ItemClass>::alreadyIn(QString itemName)
{
	int i = findItem(itemName);
	if (i==-1)
		return false;
	else
		return true;
}


/** replace in this vector values which are in overVector
*     Illustration example : this = [A=2,B=3,C=4,D=5] and overVector = [C=99]
*    then this will be [A=2,B=3,C=99,D=5];
*     !! values are copied, not referenced in (*this) vector
*/

template<class ItemClass>
void MOVector<ItemClass>::replaceIn(MOVector<ItemClass> *overVector)
{
    int iv,iov;
    QString curName;

    for(iov=0;iov<overVector->items.size();iov++)
    {
        curName = overVector->items.at(iov)->name();
        iv=findItem(curName);

        if(iv!=-1)
        {
            delete items.at(iv);
            items.erase(items.begin()+iv,items.begin()+iv+1);
            items.insert(items.begin()+iv,new ItemClass(*overVector->items.at(iov)));
        }
        else
        {
            QString msg;
            msg.sprintf("Variable %s not found in vector. Could not replace.",curName.utf16());
            infoSender.debug(msg);
        }
    }
}


template<class ItemClass>
void MOVector<ItemClass>::cloneFromOtherVector(const MOVector *vector_)
{
	clear();
	int i;
	ItemClass* newItem;
	for(i=0;i<vector_->items.size();i++)
	{
		newItem = new ItemClass(*vector_->items.at(i));
		addItem(newItem);
	}
}

template<class ItemClass>
MOVector<ItemClass>* MOVector<ItemClass>::clone()
{
	MOVector<ItemClass>* newVector = new MOVector<ItemClass>();

	int i;
	ItemClass* newItem;
	for(i=0;i<items.size();i++)
	{
		newItem = new ItemClass(*items.at(i));
		newVector->addItem(newItem);
	}


	return newVector;
}

template<class ItemClass>
void MOVector<ItemClass>::clear()
{
	if(items.size()>0)
	{
		beginRemoveRows(QModelIndex(),0,items.size()-1);
		removeRows(0,items.size());
		endRemoveRows();
	}
}




template<class ItemClass>
QString MOVector<ItemClass>::toSavingString()
{
	QString saveString;

	// print field names
	saveString += "\n#";
	for(int iF=0; iF<ItemClass::nbFields; iF++)
		saveString += ItemClass::sFieldName(iF,Qt::UserRole) + "\t ";

	saveString += "\n";
	// print item values
	for(int i=0; i<items.size();i++)
	{
		saveString += items.at(i)->toSavingString();
		saveString += "\n";
	}
	return saveString;
}

template<class ItemClass>
QDomElement MOVector<ItemClass>::toXmlData(QDomDocument & doc,QString listTitle)
{
	// Root element
	QDomElement cList = doc.createElement(listTitle);

	for(int i=0;i<items.size();i++)
	{
		QDomElement cItem = items.at(i)->toXmlData(doc);
		cList.appendChild(cItem);
	}
	return cList;
}



template<class ItemClass>
QModelIndex MOVector<ItemClass>::index(int row, int column, const QModelIndex &parent)const
{
	if((row>-1)&&(row < items.size()))
		return createIndex(row,column,items.at(row));
	else
		return createIndex(row,column);
}

#endif