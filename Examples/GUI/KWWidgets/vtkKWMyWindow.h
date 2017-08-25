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

#ifndef __vtkKWMyWindow_h
#define __vtkKWMyWindow_h

#include "vtkKWObject.h"
#include "vtkKWApplication.h"
#include "vtkKWRenderWidget.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include <vtksys/SystemTools.hxx>
#include <vtksys/CommandLineArguments.hxx>
#include <vtksys/stl/string>
#include <vtksys/stl/vector>
#include <iostream>
#include "VTKEdgeConfigure.h" // export

//BTX
// Convenience macro for the examples "KWLIBS_CREATE_EXAMPLE"
#define KWLIBS_CREATE_EXAMPLE_MACRO                                          \
                                                                             \
int my_main(int argc, char *argv[])                                          \
{                                                                            \
  Tcl_Interp *interp = vtkKWApplication::InitializeTcl(argc, argv, &cerr);   \
  if (!interp)                                                               \
    {                                                                        \
    cerr << "Error: InitializeTcl failed" << endl ;                          \
    return 1;                                                                \
    }                                                                        \
                                                                             \
  Kwmywindowlib_Init(interp);                                                \
                                                                             \
  vtkKWApplication *app = vtkKWApplication::New();                           \
  vtkKWMyWindow *example = vtkKWMyWindow::New();                             \
  example->SetApplication(app);                                              \
                                                                             \
  vtksys::CommandLineArguments arg;                                          \
  arg.Initialize(argc, argv);                                                \
  typedef vtksys::CommandLineArguments argT;                                 \
  char * data_filename = NULL;                                               \
  arg.AddArgument("--data", argT::SPACE_ARGUMENT, &data_filename,            \
                  "Input data to paint on");                                 \
  arg.Parse();                                                               \
                                                                             \
  if (data_filename && vtksys::SystemTools::FileExists(                      \
                                data_filename))                              \
    {                                                                        \
    example->SetFilename(data_filename);                                     \
    }                                                                        \
  else                                                                       \
    {                                                                        \
    vtkstd::vector< vtksys_stl::string > stringArray;                        \
    stringArray.push_back(vtkKWMyWindow::GetExampleDataPath());              \
    stringArray.push_back("HeadMR.mha");                                     \
    vtksys_stl::string fn = vtksys::SystemTools::JoinPath(stringArray);      \
    vtksys::SystemTools::ConvertToUnixSlashes(fn);                           \
    example->SetFilename(fn.c_str());                                        \
    }                                                                        \
  example->CreateInfo( __FILE__, app );                                      \
                                                                             \
  int res = my_example( argc, argv, app, example );                          \
  example->Delete();                                                         \
  app->Delete();                                                             \
  return res;                                                                \
}

#define KWLIBS_CREATE_WINDOWS_EXAMPLE                                        \
extern "C" int Kwmywindowlib_Init(Tcl_Interp *interp);                       \
int my_main(int argc, char *argv[]);                                         \
int my_example( int argc, char *argv[],                                      \
                vtkKWApplication * app,                                      \
                vtkKWMyWindow    * example_window );                         \
int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR lpCmdLine, int)            \
{                                                                            \
  int argc;                                                                  \
  char **argv;                                                               \
  vtksys::SystemTools::ConvertWindowsCommandLineToUnixArguments(lpCmdLine, &argc, &argv); \
  int ret = my_main(argc, argv);                                             \
  for (int i = 0; i < argc; i++) { delete [] argv[i]; }                      \
  delete [] argv;                                                            \
  return ret;                                                                \
}                                                                            \
KWLIBS_CREATE_EXAMPLE_MACRO

#define KWLIBS_CREATE_UNIX_EXAMPLE                                           \
extern "C" int Kwmywindowlib_Init(Tcl_Interp *interp);                       \
int my_main(int argc, char *argv[]);                                         \
int my_example( int argc, char *argv[],                                      \
                vtkKWApplication * app,                                      \
                vtkKWMyWindow    * example_window );                         \
int main(int argc, char *argv[]) { return my_main(argc, argv); }             \
KWLIBS_CREATE_EXAMPLE_MACRO

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#define KWLIBS_CREATE_EXAMPLE KWLIBS_CREATE_WINDOWS_EXAMPLE
#else
#define KWLIBS_CREATE_EXAMPLE KWLIBS_CREATE_UNIX_EXAMPLE
#endif
//ETX

class vtkImageViewer2;
class vtkImageReader2;
class vtkKWEPaintbrushWidget;
class vtkKWSelectionFrame;
class vtkKWSelectionFrameLayoutManager;
class vtkCollection;
class vtkKWScaleWithEntry;
class vtkKWWindow;
class vtkKWDialog;
class vtkKWTextWithScrollbarsWithLabel;
class vtkKWRadioButtonSet;

// ------- The window class used by all examples ------------
class vtkKWMyWindow : public vtkObject
{
public:
  static vtkKWMyWindow* New();
  vtkTypeRevisionMacro(vtkKWMyWindow,vtkObject);

  // Description:
  // Initialize() must be called before Run()
  void Initialize();
  int Run();

  // Description:
  // Callbacks
  virtual void SetSliceFromScaleCallback (double value);
  virtual void SetSliceFromScaleCallback0(double value);
  virtual void SetSliceFromScaleCallback1(double value);
  virtual void SetSliceFromScaleCallback2(double value);
  void RenderWidgetSelectionChangedCallback(
      const char * title, vtkKWSelectionFrame * );

  // Description:
  // Filename of the data to be loaded.
  vtkSetStringMacro( Filename );
  vtkGetStringMacro( Filename );

  // Description:
  // Get the Window class
  vtkGetObjectMacro( Window, vtkKWWindow );

  // Description:
  // Default is a single render window. However you can display
  // Axial, Sagittal, Coronal, Blank render windows simultaneously
  vtkSetMacro( FourPaneView, int );
  vtkGetMacro( FourPaneView, int );
  vtkBooleanMacro( FourPaneView, int );

  // Description:
  // Get the render widgets.
  vtkKWRenderWidget * GetAxialRenderWidget()    { return this->GetNthRenderWidget(0); }
  vtkKWRenderWidget * GetCoronalRenderWidget()  { return this->GetNthRenderWidget(1); }
  vtkKWRenderWidget * GetSagittalRenderWidget() { return this->GetNthRenderWidget(2); }
  vtkKWRenderWidget * GetNthRenderWidget( int i );

  // Description:
  // Get the image actors
  vtkImageActor * GetAxialImageActor()    { return this->GetNthImageActor(0); }
  vtkImageActor * GetCoronalImageActor()  { return this->GetNthImageActor(1); }
  vtkImageActor * GetSagittalImageActor() { return this->GetNthImageActor(2); }
  vtkImageActor * GetNthImageActor( int i );

  // Description:
  // Set/Get the input image data that the actors are showing. This may be
  // set explicitly as below, or may be supplied via a user-argument to the
  // example application as "--data somefile.mha" . If no input is specified,
  // a default head MRI image is loaded up.
  vtkGetObjectMacro( Input, vtkImageData );
  virtual void SetInput( vtkImageData * );

  // Description:
  // Internally used to display a top level dialog with code
  //BTX
  void CreateInfo(
    vtksys_stl::string name, vtkKWApplication * app );

  // Description:
  // Get the path where testing data is stored.
  static vtksys_stl::string GetExampleDataPath()
    {
    vtksys_stl::string fname_base =
      vtksys::SystemTools::GetFilenamePath(__FILE__);
    vtkstd::vector< vtksys_stl::string > stringArray;
    stringArray.push_back(fname_base);
    stringArray.push_back("/../../../Data/");
    stringArray.push_back("Data");
    vtksys_stl::string fn = vtksys::SystemTools::JoinPath(stringArray);
    vtksys::SystemTools::ConvertToUnixSlashes(fn);
    fn += "/";
    return fn;
    }

  // Description:
  // Given a filename, looks in the example path and returns the fullfilename
  // with path.
  static vtksys_stl::string ExpandFilename( const char * s );
  //ETX

  void SetApplication( vtkKWApplication * a );
  vtkKWApplication *GetApplication() { return this->Application; }

  // Description:
  // Easy way to tie the Select and edit mode callbacks to the paintbrush
  // widget whenever the radio button is depressed. Basically this will simply
  // cause the method
  //   w->SetPaintbrushMode( vtkKWEPaintbrushWidget::Edit )     or
  //   w->SetPaintbrushMode( vtkKWEPaintbrushWidget::Select )
  // to be invoked based on which radio button is depressed.
  void AddSelectEditCallback( vtkKWEPaintbrushWidget * w );

  //BTX
  typedef void (*SelectEditCallbackMethodType)( vtkKWEPaintbrushWidget *, int editMode );
  void SetSelectEditCallbackMethod( SelectEditCallbackMethodType );
  //ETX

  // Description
  // INTERNAL - do not use.
  void SelectEditCallback();

  // Description:
  // Callbacks for the examples to plug things into
  void AddSketchCallback() 
    { this->AddSketchCallbackMethod(this->CallData); };
  
  //BTX
  // User specified callbacks
  void (*AddSketchCallbackMethod)(void*);
  void *CallData;
  //ETX

protected:
  vtkKWMyWindow();
  ~vtkKWMyWindow();

  vtkKWApplication                 *Application;
  vtkImageViewer2                  *ImageViewer[3];
  vtkKWScaleWithEntry              *SliceScale[3];
  vtkKWSelectionFrameLayoutManager *SelectionFrameLayoutManager;
  vtkKWSelectionFrame              *SelectionFrame;
  vtkKWWindow                      *Window;
  vtkImageData                     *Input;
  vtkKWDialog                      *InfoDialog;
  vtkKWTextWithScrollbarsWithLabel *CxxSourceText;
  char *                           Filename;
  int                              FourPaneView;
  vtkKWRadioButtonSet              *SelectEditRadioButtons;
  vtkCollection                    *Widgets;
  SelectEditCallbackMethodType      SelectEditCallbackMethod;

  // Get the renderwidget packed into a selection frame
  static vtkKWRenderWidget * GetRenderWidget( vtkKWSelectionFrame * );

  virtual void UpdateSliceRanges();
  void GetValidReader( vtkImageReader2 * & reader, const char * filename);

private:
  vtkKWMyWindow(const vtkKWMyWindow&);   // Not implemented.
  void operator=(const vtkKWMyWindow&);  // Not implemented.
};

#endif
