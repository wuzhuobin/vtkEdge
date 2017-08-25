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
#ifndef __pqKWESurfaceLICDisplayPanelDecoratorImplementation_h 
#define __pqKWESurfaceLICDisplayPanelDecoratorImplementation_h

#include <QObject>
#include "pqDisplayPanelDecoratorInterface.h"

class pqKWESurfaceLICDisplayPanelDecoratorImplementation : 
  public QObject, public pqDisplayPanelDecoratorInterface
{
  Q_OBJECT
  Q_INTERFACES(pqDisplayPanelDecoratorInterface);
public:
  pqKWESurfaceLICDisplayPanelDecoratorImplementation(QObject* parent=0);
  virtual ~pqKWESurfaceLICDisplayPanelDecoratorImplementation();

  /// Returns true if this implementation can decorate the given panel type.
  virtual bool canDecorate(pqDisplayPanel* panel) const;

  /// Called to allow the implementation to decorate the panel. This is called
  /// only if canDecorate(panel) returns true.
  virtual void decorate(pqDisplayPanel* panel) const;

private:
  pqKWESurfaceLICDisplayPanelDecoratorImplementation(
    const pqKWESurfaceLICDisplayPanelDecoratorImplementation&); // Not implemented.
  void operator=(
    const pqKWESurfaceLICDisplayPanelDecoratorImplementation&); // Not implemented.
};

#endif


