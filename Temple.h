
#ifndef TEMPLE_H
#define TEMPLE_H

#include <GLee.h>

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/image.h>

#include <fstream>

#include "Utility.h"

// Define a new application type
class TempleApp: public wxApp
{
  public:
  bool OnInit();
};

// Define a new frame type
class TempleCanvas;

class TempleFrame: public wxFrame
{
  protected:
  TempleFrame(wxWindow *parent, const wxString& title, const wxPoint& pos,
    const wxSize& size, long style = wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN |
    wxNO_FULL_REPAINT_ON_RESIZE);

  TempleCanvas *m_canvas;

  void OnIdle(wxIdleEvent &event);
  void OnQuit(wxCommandEvent& event);
  void OnSize(wxSizeEvent& event);

  void LayoutChildren();

  public:
  static TempleFrame *Create();

  void OnExit(wxCommandEvent& event);

  DECLARE_EVENT_TABLE()
};

#include "ComponentBoard.h"

class TempleCanvas: public wxGLCanvas
{
  protected:
  int m_winWidth, m_winHeight;

  Vec2i m_mOld;
  Vec2i m_mNew;
  Vec2i m_mIni;

  Vec3f m_mWorld;     // Holds the unprojection of the mouse's screen coords
  Vec3f m_mWorldOld;  // Ditto, but for the previous frame
  Vec3f m_mWorldIni;  // Holds the location of the last click, for dragging

  bool m_lbd, m_mbd, m_rbd;
  char m_keys[512];

  std::ofstream m_logfile;

  ComponentBoard m_board;

  void Ortho2D(bool invertY);

  void InitMembers();
  void InitGL();

  void OnPaint(wxPaintEvent &event);
  void OnSize(wxSizeEvent &event);
  void OnEraseBackground(wxEraseEvent &event);
  void OnEnterWindow(wxMouseEvent &event);

  void OnKeyDown(wxKeyEvent &event);
  void OnKeyUp(wxKeyEvent &event);
  void OnMouse(wxMouseEvent &event);
  void ProcessKeys();

  public:
  TempleCanvas(wxWindow *parent, wxWindowID id = wxID_ANY,
      const wxPoint &pos = wxDefaultPosition,
      const wxSize &size = wxDefaultSize,
      long style = 0, const wxString &name = _T("TempleCanvas"));

  TempleCanvas(wxWindow *parent, const TempleCanvas *other,
      wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition,
      const wxSize &size = wxDefaultSize, long style = 0,
      const wxString &name = _T("TempleCanvas"));

  ~TempleCanvas();

  void DrawScene();

  DECLARE_EVENT_TABLE()

  friend class TempleFrame;
};

#endif
