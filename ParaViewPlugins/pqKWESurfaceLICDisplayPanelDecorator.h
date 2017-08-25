//=============================================================================
//   This file is part of VTKEdge. See vtkedge.org for more information.
//
//   Copyright (c) 2008 Kitware, Inc.
//
//   VTKEdge may be used under the terms of the GNU General Public License 
//   version 3 as published by the Free Software Foundation and appearing in 
//   the file LICENSE.txt included in the top level directory of this source
//   code distribution. Alternatively you may (at your option) use any later 
//   version of the GNU General Public License if such license has been 
//   publicly approved by Kitware, Inc. (or its successors, if any).
//
//   VTKEdge is distributed "AS IS" with NO WARRANTY OF ANY KIND, INCLUDING
//   THE WARRANTIES OF DESIGN, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR
//   PURPOSE. See LICENSE.txt for additional details.
//
//   VTKEdge is available under alternative license terms. Please visit
//   vtkedge.org or contact us at kitware@kitware.com for further information.
//
//=============================================================================
#ifndef __pqKWESurfaceLICDisplayPanelDecorator_h 
#define __pqKWESurfaceLICDisplayPanelDecorator_h

#include <QObject>
class pqDisplayProxyEditor;

class pqKWESurfaceLICDisplayPanelDecorator : public QObject
{
  Q_OBJECT
  typedef QObject Superclass;
public:
  pqKWESurfaceLICDisplayPanelDecorator(pqDisplayProxyEditor* panel);
  ~pqKWESurfaceLICDisplayPanelDecorator();

protected slots:
  void representationTypeChanged();

private:
  pqKWESurfaceLICDisplayPanelDecorator(const pqKWESurfaceLICDisplayPanelDecorator&); // Not implemented.
  void operator=(const pqKWESurfaceLICDisplayPanelDecorator&); // Not implemented.

  class pqInternals;
  pqInternals* Internals;
};

#endif


