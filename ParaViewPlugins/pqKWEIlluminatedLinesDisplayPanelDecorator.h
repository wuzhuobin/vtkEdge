//=============================================================================
//   This file is part of VTKEdge. See vtkedge.org for more information.
//
//   Copyright (c) 2010 Kitware, Inc.
//
//   VTKEdge may be used under the terms of the BSD License
//   Please see the file Copyright.txt in the root directory of
//   VTKEdge for further information.
//
//   Alternatively, you may see: 
//
//   http://www.vtkedge.org/vtkedge/project/license.html
//
//
//   For custom extensions, consulting services, or training for
//   this or any other Kitware supported open source project, please
//   contact Kitware at sales@kitware.com.
//
//
//=============================================================================
#ifndef __pqKWEIlluminatedLinesDisplayPanelDecorator_h
#define __pqKWEIlluminatedLinesDisplayPanelDecorator_h

#include <QObject>

class pqDisplayProxyEditor;
class pqKWEIlluminatedLinesDisplayPanelDecorator : public QObject
{
  Q_OBJECT
  typedef QObject Superclass;
public:
  pqKWEIlluminatedLinesDisplayPanelDecorator(pqDisplayProxyEditor* panel);
  ~pqKWEIlluminatedLinesDisplayPanelDecorator();

private:
  pqKWEIlluminatedLinesDisplayPanelDecorator(const pqKWEIlluminatedLinesDisplayPanelDecorator&); // Not implemented.
  void operator=(const pqKWEIlluminatedLinesDisplayPanelDecorator&); // Not implemented.
};

#endif


