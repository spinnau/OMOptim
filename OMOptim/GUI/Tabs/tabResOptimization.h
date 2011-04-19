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

 	@file tabResOptimization.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#ifndef TABRESOPTIMIZATIONCLASS_H
#define TABRESOPTIMIZATIONCLASS_H

#include <QtGui/QWidget>
#include "OptimResult.h"
#include "ui_tabResOptimization.h"
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QHeaderView>
#include "Project.h"
#include "MOOptPlot.h"
#include "OptimResult.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>
#include <qwt_picker.h>

#include "tabOptimization.h"
#include "MO2ColTab.h"
#include "WidgetBlocks.h"
#include "WidgetMooPlot.h"
#include "widgetmoopointslist.h"
#include "WidgetOptTable.h"
#include "WidgetTableRecVar.h"
#include "WidgetToolBar.h"
#include "WidgetCalculateMooPoints.h"


namespace Ui {
    class TabResOptimizationClass;
}

class TabResOptimization : public MO2ColTab {
    Q_OBJECT

public:
	TabResOptimization(Project *project, Optimization *problem_, QWidget *parent = 0);
    ~TabResOptimization();

	Project *project;
	Optimization *problem;
	OptimResult* result;

        WidgetMooPointsList* widgetMooPointsList;
	WidgetMooPlot* widgetMooPlot;
	WidgetTableRecVar* widgetTableRecVar;
        WidgetCalculateMooPoints* widgetCalculateMooPoints;


};




#endif // TABRESOPTIMIZATION_H