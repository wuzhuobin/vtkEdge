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

#include "vtkKWMyWindow.h"

#include "vtkBMPReader.h"
#include "vtkCommand.h"
#include "vtkImageReader2.h"
#include "vtkImageReader2Factory.h"
#include "vtkImageViewer2.h"
#include "vtkMetaImageReader.h"
#include "vtkObjectFactory.h"
#include "vtkPNGReader.h"
#include "vtkPNMReader.h"
#include "vtkToolkits.h"
#include "vtkXMLImageDataReader.h"
#include "vtkCollection.h"
#include "vtkKWEPaintbrushWidget.h"

#include "vtkKWDialog.h"
#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWListBox.h"
#include "vtkKWMenu.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenuButtonWithSpinButtons.h"
#include "vtkKWMenuButtonWithSpinButtonsWithLabel.h"
#include "vtkKWNotebook.h"
#include "vtkKWScaleWithEntry.h"
#include "vtkKWSelectionFrame.h"
#include "vtkKWSelectionFrameLayoutManager.h"
#include "vtkKWText.h"
#include "vtkKWTextWithScrollbars.h"
#include "vtkKWTextWithScrollbarsWithLabel.h"
#include "vtkKWWindow.h"
#include "vtkKWRadioButton.h"
#include "vtkKWRadioButtonSet.h"
#include "vtksys/ios/sstream"

static const char *vtkKWMyWindowWidgetStrings[] = {
  "Axial", 
  "Coronal",
  "Sagittal",
  "Blank",
  NULL
};

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWMyWindow );
vtkCxxRevisionMacro(vtkKWMyWindow, "$Revision: 787 $");
vtkCxxSetObjectMacro( vtkKWMyWindow, Input, vtkImageData );

//----------------------------------------------------------------------------
vtkKWMyWindow::vtkKWMyWindow()
{
  this->SelectionFrameLayoutManager = vtkKWSelectionFrameLayoutManager::New();
  this->SelectionFrame = vtkKWSelectionFrame::New();
  this->SelectionFrame->SetTitle( vtkKWMyWindowWidgetStrings[0] );
  this->SelectionFrame->AllowChangeTitleOff();
  for (int i = 0; i < 3; i++)
    {
    this->ImageViewer[i] = vtkImageViewer2::New();
    this->SliceScale[i] = vtkKWScaleWithEntry::New();
    }
  this->Filename = NULL;
  this->FourPaneView = 1;
  this->Window = NULL;
  this->Input = NULL;
  this->InfoDialog = vtkKWDialog::New();
  this->InfoDialog->ModalOff();
  this->CxxSourceText = vtkKWTextWithScrollbarsWithLabel::New();
  this->SelectEditRadioButtons = NULL;
  this->Widgets = vtkCollection::New();
}

//----------------------------------------------------------------------------
vtkKWMyWindow::~vtkKWMyWindow()
{
  for (int i = 0; i < 3; i++)
    {
    this->ImageViewer[i]->Delete();
    this->SliceScale[i]->Delete();
    }

  // TODO I don't know why the following 6 lines are necessary, but their absence
  // causes a warning "A TkRenderWidget is being destroyed before it associated 
  // vtkRenderWindow is destroyed.".
  this->GetNthRenderWidget(0)->SetParent(NULL);
  if (this->FourPaneView)
    {
    this->GetNthRenderWidget(1)->SetParent(NULL);
    this->GetNthRenderWidget(2)->SetParent(NULL);
    }
  
  this->SelectionFrame->Delete();
  this->SelectionFrameLayoutManager->Delete();
  if (this->Filename)
    {
    delete [] this->Filename;
    }
  if (this->SelectEditRadioButtons)
    {
    this->SelectEditRadioButtons->Delete();
    }
  if (this->Window)
    {
    this->Window->Delete();
    }
  this->SetInput(NULL);
  this->InfoDialog->Delete();
  this->CxxSourceText->Delete();
  this->Widgets->Delete();
}

//----------------------------------------------------------------------------
void vtkKWMyWindow::SetApplication( vtkKWApplication * app ) 
{
  this->Application = app;
}

//----------------------------------------------------------------------------
void vtkKWMyWindow::Initialize()
{
  vtkKWApplication *app = this->GetApplication();
  app->SetName("Paintbrush Example");
  app->RestoreApplicationSettingsFromRegistry();

  this->Window = vtkKWWindow::New();
  app->AddWindow(this->Window);
  this->Window->Create();
  this->Window->SecondaryPanelVisibilityOff();
  this->Window->MainPanelVisibilityOff();

  if (this->FourPaneView)
    {
    this->SelectionFrameLayoutManager->SetParent(this->Window->GetViewFrame());
    this->SelectionFrameLayoutManager->SetResolution(2, 2);
    this->SelectionFrameLayoutManager->Create();
    for (int i = 0; i < 3; i++)
      {
      vtkKWSelectionFrame * sel_frame = vtkKWSelectionFrame::New();
      sel_frame->SetTitle(vtkKWMyWindowWidgetStrings[i]);
      this->SelectionFrameLayoutManager->AddWidget(sel_frame);
      sel_frame->AllowCloseOff();
      sel_frame->AllowChangeTitleOff();
      sel_frame->Delete();
      }
    
    app->Script("pack %s -expand y -fill both -anchor c -expand y", 
                this->SelectionFrameLayoutManager->GetWidgetName());

    // Add a render widget, attach it to the view frame, and pack
    for (int i = 0; i < 3; i++)
      {
      vtkKWRenderWidget * renwidget = vtkKWRenderWidget::New();
      renwidget->SetParent(this->SelectionFrameLayoutManager->
          GetWidgetWithTitle(vtkKWMyWindowWidgetStrings[i])->GetBodyFrame());
      renwidget->Create();
      renwidget->CornerAnnotationVisibilityOn();
      app->Script("pack %s -expand y -fill both -anchor c -expand y", 
                  renwidget->GetWidgetName());
      renwidget->Delete();
      }
    }
  else
    {
    this->SelectionFrame->SetParent(this->Window->GetViewFrame());
    this->SelectionFrame->SetAllowChangeTitle(0);
    this->SelectionFrame->AllowCloseOff();
    this->SelectionFrame->Create();
    this->SelectionFrame->SetSelectionList(3, vtkKWMyWindowWidgetStrings);

    app->Script("pack %s -expand y -fill both -anchor c -expand y", 
                this->SelectionFrame->GetWidgetName());

    vtkKWRenderWidget * renwidget = vtkKWRenderWidget::New();
    renwidget->SetParent(this->SelectionFrame->GetBodyFrame());
    renwidget->Create();
    renwidget->CornerAnnotationVisibilityOn();
    app->Script("pack %s -expand y -fill both -anchor c -expand y", 
                renwidget->GetWidgetName());
    renwidget->Delete();
    }
  

  // Read the data, if an input hasn't already been specified

  if (!this->Input)
    {
    vtkImageReader2 *reader;
    this->GetValidReader( reader, this->Filename );
    if (!reader)
      {
      std::cerr << "Cannot read input image file ! " << std::endl;
      return;
      }
    reader->Update();
    this->SetInput(reader->GetOutput());
    reader->Delete();
    }

  // Create an image viewer
  // Use the render window and renderer of the renderwidget

  if (this->FourPaneView == 0)
    {
    vtkKWRenderWidget * rw = this->GetAxialRenderWidget();
    this->ImageViewer[0]->SetRenderWindow(rw->GetRenderWindow());
    this->ImageViewer[0]->SetRenderer(rw->GetRenderer());
    this->ImageViewer[0]->SetInput(this->Input);
    this->ImageViewer[0]->SetupInteractor(
      rw->GetRenderWindow()->GetInteractor());

    // Reset the window/level and the camera

    double *range = this->Input->GetScalarRange();
    this->ImageViewer[0]->SetColorWindow(range[1] - range[0]);
    this->ImageViewer[0]->SetColorLevel(0.5 * (range[1] + range[0])+ 0.0000001);

    rw->ResetCamera();

    this->SliceScale[0]->SetParent(this->Window->GetViewPanelFrame());
    this->SliceScale[0]->SetEntryPositionToLeft();
    this->SliceScale[0]->Create();
    this->SliceScale[0]->SetCommand(this, "SetSliceFromScaleCallback");

    app->Script("pack %s -side top -expand n -fill x -padx 2 -pady 2", 
                this->SliceScale[0]->GetWidgetName());

    this->SelectionFrame->SetSelectionListCommand(
      this, "RenderWidgetSelectionChangedCallback" );
    }
  else
    {
    for (int i = 0; i < 3; i++)
      {
      vtkKWSelectionFrame * sel_frame = 
        this->SelectionFrameLayoutManager->
             GetWidgetWithTitle(vtkKWMyWindowWidgetStrings[i]);
      vtkKWRenderWidget * rw = this->GetRenderWidget(sel_frame);
      this->ImageViewer[i]->SetRenderWindow(rw->GetRenderWindow());
      this->ImageViewer[i]->SetRenderer(rw->GetRenderer());
      this->ImageViewer[i]->SetInput(this->Input);
      this->ImageViewer[i]->SetupInteractor(
        rw->GetRenderWindow()->GetInteractor());

      // Reset the window/level and the camera

      double *range = this->Input->GetScalarRange();
      this->ImageViewer[i]->SetColorWindow(range[1] - range[0]);
      this->ImageViewer[i]->SetColorLevel(0.5 * (range[1] + range[0])+ 0.0000001);
      this->ImageViewer[i]->SetSliceOrientation(2-i);

      rw->ResetCamera();

      sel_frame->LeftUserFrameVisibilityOn();
      this->SliceScale[i]->SetParent(sel_frame->GetLeftUserFrame());
      this->SliceScale[i]->SetOrientationToVertical();
      this->SliceScale[i]->EntryVisibilityOff();
      this->SliceScale[i]->Create();
      vtksys_ios::ostringstream s;
      s << "SetSliceFromScaleCallback" << i << std::ends;
      this->SliceScale[i]->SetCommand(this, s.str().c_str());

      app->Script("pack %s -side top -anchor nw -expand yes -fill y -padx 0 -pady 0", 
                  this->SliceScale[i]->GetWidgetName());
      }
    }

  this->UpdateSliceRanges();

  this->Window->Display();
  
  // Repeatedly set the layout manager resolution here. When win->Display
  // is invoked the layout manager is automatically re-adjusted.
  if (this->FourPaneView)
    {
    this->SelectionFrameLayoutManager->SetResolution(2,2);
    }
}

//----------------------------------------------------------------------------
int vtkKWMyWindow::Run()
{
  vtkKWApplication *app = this->GetApplication();
  app->Start();
  int ret = app->GetExitStatus();
  this->Window->Close();
  return ret;
}

//----------------------------------------------------------------------------
vtkKWRenderWidget * vtkKWMyWindow::GetNthRenderWidget( int i )
{
  return this->GetRenderWidget( 
      this->FourPaneView ? 
          this->SelectionFrameLayoutManager->
             GetWidgetWithTitle(vtkKWMyWindowWidgetStrings[i])
        : this->SelectionFrame);
}

//----------------------------------------------------------------------------
vtkImageActor * vtkKWMyWindow::GetNthImageActor( int i )
{
  vtkKWRenderWidget * rw = this->GetNthRenderWidget(i);
  for (int j = 0; j < 3; j++)
    {
    if (this->ImageViewer[j]->GetRenderWindow() == rw->GetRenderWindow())
      {
      return this->ImageViewer[j]->GetImageActor();
      }
    }
  return NULL;
}

//----------------------------------------------------------------------------
vtkKWRenderWidget * vtkKWMyWindow
::GetRenderWidget( vtkKWSelectionFrame *widget )
{
  vtkKWRenderWidget *rw = NULL;
  if (widget)
    {
    vtkKWFrame *frame = widget->GetBodyFrame();
    if (frame)
      {
      int nb_children = frame->GetNumberOfChildren();
      for (int i = 0; i < nb_children; i++)
        {
        vtkKWWidget *child = frame->GetNthChild(i);
        if (child)
          {
          rw = vtkKWRenderWidget::SafeDownCast(child);
          if (rw)
            {
            return rw;
            }
          int nb_grand_children = child->GetNumberOfChildren();
          for (int j = 0; j < nb_grand_children; j++)
            {
            vtkKWWidget *grand_child = child->GetNthChild(j);
            if (grand_child)
              {
              rw = vtkKWRenderWidget::SafeDownCast(grand_child);
              if (rw)
                {
                return rw;
                }
              }
            }
          }
        }
      }
    }
  return rw;
}

//----------------------------------------------------------------------------
void vtkKWMyWindow::SetSliceFromScaleCallback( double value )
{
  if (this->FourPaneView == 0)
    {
    this->ImageViewer[0]->SetSlice((int)value);
    }
}

//----------------------------------------------------------------------------
void vtkKWMyWindow::SetSliceFromScaleCallback0( double value )
{
  this->ImageViewer[0]->SetSlice((int)value);
}

//----------------------------------------------------------------------------
void vtkKWMyWindow::SetSliceFromScaleCallback1( double value )
{
  this->ImageViewer[1]->SetSlice((int)value);
}

//----------------------------------------------------------------------------
void vtkKWMyWindow::SetSliceFromScaleCallback2( double value )
{
  this->ImageViewer[2]->SetSlice((int)value);
}

//----------------------------------------------------------------------------
void vtkKWMyWindow::UpdateSliceRanges()
{
  if (this->FourPaneView)
    {
    for (int i = 0; i < 3; i++)
      {
      this->SliceScale[i]->SetRange( this->ImageViewer[i]->GetSliceMin(), 
                                     this->ImageViewer[i]->GetSliceMax());
      this->SliceScale[i]->SetValue( this->ImageViewer[i]->GetSlice() );
      }
    }
  else
    {
    this->SliceScale[0]->SetRange( this->ImageViewer[0]->GetSliceMin(),
                                   this->ImageViewer[0]->GetSliceMax());
    this->SliceScale[0]->SetValue( this->ImageViewer[0]->GetSlice());
    }
}

//----------------------------------------------------------------------------
void vtkKWMyWindow::RenderWidgetSelectionChangedCallback( 
  const char * title, vtkKWSelectionFrame * )
{
  for (int i = 0; i < 3; i++)
    {
    if (strcmp(title, vtkKWMyWindowWidgetStrings[i]) == 0)
      {
      if (this->ImageViewer[0]->GetSliceOrientation() != (2-i))
        {
        this->SelectionFrame->SetTitle(title);
        this->ImageViewer[0]->SetSliceOrientation(2-i);
        this->UpdateSliceRanges();
        }
      }
    }
}

//----------------------------------------------------------------------------
void vtkKWMyWindow::GetValidReader( 
    vtkImageReader2* & reader, const char *filename)
{
  if (filename == NULL)
    {
    return;
    }

  reader = vtkMetaImageReader::New();
  reader->SetFileName(filename);
  if (reader->CanReadFile(filename) == 3)
    {
    return;
    }

/*
 * Damn ! I can't read .vtk files. they don't subclass from vtkImageReader2
 * TODO : put a work around for VTK files later
 * 
 * reader = vtkXMLImageDataReader::New();
  int valid = reader->CanReadFile(filename);
  if (reader->CanReadFile(filename) == 3)
    {
    return;
    }
*/

  reader = vtkPNGReader::New();
  if (reader->CanReadFile(filename) == 3)
    {
    return;
    }
  
  reader = vtkBMPReader::New();
  if (reader->CanReadFile(filename) == 3)
    {
    return;
    }
  
  reader = vtkPNMReader::New();
  if (reader->CanReadFile(filename) == 3)
    {
    return;
    }

  reader->Delete();
  reader = NULL;
}

//----------------------------------------------------------------------------
void vtkKWMyWindow::CreateInfo( vtksys_stl::string name, vtkKWApplication * app ) 
{
  this->InfoDialog->SetApplication(app);
  this->InfoDialog->Create();
  this->InfoDialog->SetDisplayPositionToMasterWindowCenter();
  this->InfoDialog->SetDeleteWindowProtocolCommand(this->InfoDialog, "Withdraw");
  this->InfoDialog->Display();
  this->InfoDialog->SetSize(700, 400);

  vtksys_stl::string source, line;
  char buffer[1024];

  // Add text widget to display the C++ example source
  this->CxxSourceText->SetParent(this->InfoDialog);
  this->CxxSourceText->Create();
  this->CxxSourceText->SetLabelPositionToTop();
  this->CxxSourceText->SetLabelText("C++ Source");

  vtkKWTextWithScrollbars *text_widget = this->CxxSourceText->GetWidget();
  text_widget->VerticalScrollbarVisibilityOn();

  vtkKWText *text = text_widget->GetWidget();
  text->ReadOnlyOn();
  text->SetWrapToNone();
  text->SetHeight(3000);
  text->AddTagMatcher("^//[^\n]*", "_bold_tag_");
  text->AddTagMatcher("\n//[^\n]*", "_bold_tag_");
  text->AddTagMatcher("#[a-z]+", "_fg_red_tag_");
  text->AddTagMatcher(" //[^\n]*", "_fg_navy_tag_");
  text->AddTagMatcher("\"[^\"]*\"", "_fg_blue_tag_");
  text->AddTagMatcher("<[^>]*>", "_fg_blue_tag_");
  text->AddTagMatcher("vtk[A-Z][a-zA-Z0-9_]+", "_fg_dark_green_tag_");

  app->Script("pack %s -side top -expand y -fill both -padx 2 -pady 2", 
              this->CxxSourceText->GetWidgetName());

  vtksys_stl::string fname_path =
    vtksys::SystemTools::GetFilenamePath(__FILE__);
  sprintf(buffer, "%s/%s", 
    fname_path.c_str(), 
    vtksys::SystemTools::GetFilenameName(name).c_str());
  if (!vtksys::SystemTools::FileExists(buffer))
    {
    sprintf(buffer, "%s/%s.cxx",
              app->GetInstallationDirectory(), 
              name.c_str());
    }
  source = "";
  if (vtksys::SystemTools::FileExists(buffer))
    {
    ifstream ifs(buffer);
    while (vtksys::SystemTools::GetLineFromStream(ifs, line))
      {
      source += line;
      source += "\n";
      }
    ifs.close();
    }
  this->CxxSourceText->GetWidget()->GetWidget()->SetText(source.c_str());
}

//----------------------------------------------------------------------------
vtksys_stl::string vtkKWMyWindow::ExpandFilename( const char * s )
{
  vtkstd::vector< vtksys_stl::string > stringArray;
  stringArray.push_back(vtkKWMyWindow::GetExampleDataPath());
  stringArray.push_back(s);
  std::string fn = vtksys::SystemTools::JoinPath(stringArray);
  vtksys::SystemTools::ConvertToUnixSlashes(fn);
  return fn;
}

//----------------------------------------------------------------------------
void vtkKWMyWindow::AddSelectEditCallback( vtkKWEPaintbrushWidget * w )
{
  if (!this->Widgets->IsItemPresent(w)) 
    { 
    this->Widgets->AddItem(w);
    }

  if (this->SelectEditRadioButtons == NULL)
    {
    this->Window->MainPanelVisibilityOn();
    this->SelectEditRadioButtons = vtkKWRadioButtonSet::New();
    this->SelectEditRadioButtons->SetParent( this->Window->GetMainPanelFrame() );
    this->SelectEditRadioButtons->Create();
    this->SelectEditRadioButtons->SetBorderWidth(2);
    this->SelectEditRadioButtons->SetReliefToGroove();

    vtkKWRadioButton *editButton = this->SelectEditRadioButtons->AddWidget(0);
    editButton->SetText("Edit");
    editButton->SetBalloonHelpString(
      "The Paintbrush widget can operate either in an \"edit\" mode or in a "
      "\"select\" mode. In \"edit\" mode, you can draw, erase, create and edit "
      "segmentations.");
    editButton->SetCommand( this, "SelectEditCallback" );

    vtkKWRadioButton *selectButton = this->SelectEditRadioButtons->AddWidget(1);
    selectButton->SetText("Select");
    selectButton->SetBalloonHelpString(
      "The Paintbrush widget can operate either in an \"edit\" mode or in a "
      "\"select\" mode. The \"select\" mode facilitates one/multi-click selection "
      "of sketches and allows you merge and remove segmentations.");
    selectButton->SetCommand( this, "SelectEditCallback" );

    // Default to edit mode.
    this->SelectEditRadioButtons->GetWidget(0)->SetSelectedState(1);

    this->Application->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
       this->SelectEditRadioButtons->GetWidgetName() ); 
    }
}

//----------------------------------------------------------------------------
void vtkKWMyWindow::SetSelectEditCallbackMethod( SelectEditCallbackMethodType f )
{
  this->SelectEditCallbackMethod = f;
}

//----------------------------------------------------------------------------
void vtkKWMyWindow::SelectEditCallback()
{
  vtkKWEPaintbrushWidget *w;
  for ( this->Widgets->InitTraversal(); (w=static_cast<
        vtkKWEPaintbrushWidget*>(this->Widgets->GetNextItemAsObject())); )
    this->SelectEditCallbackMethod(w, 
      this->SelectEditRadioButtons->GetWidget(0)->GetSelectedState());
}

