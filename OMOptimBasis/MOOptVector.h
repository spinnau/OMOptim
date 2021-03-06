// $Id$
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

    @file MOOptVector.h
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version

  */
#if !defined(_MOOPTVECTOR_H)
#define _MOOPTVECTOR_H

#include "MOVector.h"
#include "Variable.h"
#include "Variables.h"

class ModModelPlus;

class MOOptVector : public MOVector<VariableResult>
{
    Q_OBJECT

public:


    MOOptVector(bool ownner,bool useScan, bool usePoints,QObject* parent);
    MOOptVector(bool owner, bool useScan, bool usePoints, QString csv,QObject* parent);
    ~MOOptVector();


    QVariant data(const QModelIndex &index, int role) const;
    QString toCSV(QString separator = "\t", QList<int> listPoints = QList<int>());
    //void addItem(VariableResult*);

    bool isAvailablePoint(int iVar,int iScan,int iPoint) const;

    int nbScans();
    int nbPoints();
    void setCurPoint(int);
    void setCurScan(int);
    void setUsePoints(bool);
    void setUseScan(bool);
    bool getUsePoints();
    bool getUseScan();
    int curPoint();
    int curScan();

    MOOptVector* clone() const;
    void addItem(VariableResult* item_);
    VariableResult* findVariable(QString model,QString shortVarName);


    double value(const QString &name,bool &ok,int iScan=0,int iPoint=0);
    void append(const MOOptVector &,bool makeACopy);
    void clearAtiPoint(int iPoint);


    Variables extractPoint(int iPoint);
    void setFromCsv(QString text);
    void updateFromCsv(QString text);
    void updateFromCsv(QString text, int iPoint);



protected :
    bool _useScan;
    bool _usePoints;
    int _curScan;
    int _curPoint;

    int _nbScans;
    int _nbPoints;



signals:
    void usePointsChanged();
    void useScanChanged();
    void nbScansChanged();
    void nbPointsChanged();
    void modified();
    void curScanChanged();
};

#endif
