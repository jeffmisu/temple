
#include "Temple.h"

#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "Temp.h"

// Event tables

BEGIN_EVENT_TABLE(TempleFrame, wxFrame)
  EVT_SIZE(TempleFrame::OnSize)

  EVT_MENU(wxID_EXIT, TempleFrame::OnExit)

  EVT_IDLE(TempleFrame::OnIdle)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(TempleCanvas, wxGLCanvas)
  EVT_SIZE(TempleCanvas::OnSize)
  EVT_PAINT(TempleCanvas::OnPaint)
  EVT_ERASE_BACKGROUND(TempleCanvas::OnEraseBackground)
  EVT_ENTER_WINDOW(TempleCanvas::OnEnterWindow)

  EVT_KEY_DOWN(TempleCanvas::OnKeyDown)
  EVT_KEY_UP(TempleCanvas::OnKeyUp)

  EVT_MOUSE_EVENTS(TempleCanvas::OnMouse)
END_EVENT_TABLE()

// TempleCanvas

TempleCanvas::TempleCanvas(wxWindow *parent, wxWindowID id,
  const wxPoint &pos, const wxSize &size, long style, const wxString &name):
  wxGLCanvas(parent, (wxGLCanvas *) NULL, id, pos, size, style | wxFULL_REPAINT_ON_RESIZE, name)
{
  SetCurrent();
  InitMembers();
  InitGL();
}

TempleCanvas::TempleCanvas(wxWindow *parent, const TempleCanvas *other,
  wxWindowID id, const wxPoint &pos, const wxSize &size, long style,
  const wxString &name):
  wxGLCanvas(parent, other->GetContext(), id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name)
{
  SetCurrent();
  InitMembers();
  InitGL();
}

TempleCanvas::~TempleCanvas()
{
  m_logfile.close();
}

void TempleCanvas::Ortho2D(bool invertY)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (invertY)
    glOrtho(-m_winWidth / 2, m_winWidth / 2, m_winHeight / 2, -m_winHeight / 2,
      -5.0, 5.0);
  else
    glOrtho(-m_winWidth / 2, m_winWidth / 2, -m_winHeight / 2, m_winHeight / 2,
      -5.0, 5.0);
}

void TempleCanvas::InitMembers()
{
  m_mOld = Vec2i(0, 0);
  m_mNew = Vec2i(0, 0);

  m_lbd = m_mbd = m_rbd = false;

  memset(m_keys, 0, sizeof(char) * 512);

  m_logfile.open("log.txt", std::ios::out);
}

void TempleCanvas::InitGL()
{
  SetCurrent();

	glShadeModel(GL_SMOOTH);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClearDepth(1.0f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW);
  glPolygonOffset(2.1, 4.0);
  glLineWidth(2.0);
  //wglSwapIntervalEXT(1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

void TempleCanvas::DrawScene()
{
  ProcessKeys();
  m_board.Update();

  if (!GetContext()) return;
  SetCurrent();

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, m_winWidth, m_winHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
  Ortho2D(false);

  Mat4f shift;

  shift.load_identity();
  shift.rotate_global(totalRunTime * -45, 0, 0, 1);
  Vec3f a = Vec3f(0.5, 0.5, 0.5) + shift * Vec3f(0.5, 0, 0);
  shift.load_identity();
  shift.rotate_global(totalRunTime * 90, 0, 0, 1);
  Vec3f b = Vec3f(0.5, 0.5, 0.5) + shift * Vec3f(0, 0.5, 0);
  shift.load_identity();
  shift.rotate_global(totalRunTime * 45, 0, 0, 1);
  Vec3f c = Vec3f(0.5, 0.5, 0.5) + shift * Vec3f(0, 0, 0.5);

  a.normalize();
  b.normalize();
  c.normalize();

  glPushMatrix();
  glRotatef(totalRunTime * 40, 0, 0, 1);
  float triSize = 150 + 75 * sinf((totalRunTime / 15) * (2 * M_PI));
  glBegin(GL_TRIANGLES);
  glColor4f(a.x, a.y, a.z, 0.2);
  glVertex3f(cosf(2 * M_PI * (0 / 3.0)) * triSize, sinf(2 * M_PI * (0 / 3.0)) *
    triSize, -1);
  glColor4f(b.x, b.y, b.z, 0.2);
  glVertex3f(cosf(2 * M_PI * (2 / 3.0)) * triSize, sinf(2 * M_PI * (2 / 3.0)) *
    triSize, -1);
  glColor4f(c.x, c.y, c.z, 0.2);
  glVertex3f(cosf(2 * M_PI * (1 / 3.0)) * triSize, sinf(2 * M_PI * (1 / 3.0)) *
    triSize, -1);
  glEnd();
  glPopMatrix();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glLineWidth(4.0);

  m_board.Render();

  //glFinish();
  SwapBuffers();
}

void TempleCanvas::OnEnterWindow( wxMouseEvent& WXUNUSED(event) )
{
}

void TempleCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
  DrawScene();
}

void TempleCanvas::OnSize(wxSizeEvent& event)
{
  wxGLCanvas::OnSize(event);
  GetClientSize(&m_winWidth, &m_winHeight);

  m_board.Resize(Vec2i(m_winWidth, m_winHeight));

  if (GetContext())
  {
    SetCurrent();
    glViewport(0, 0, (GLint) m_winWidth, (GLint) m_winHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
  }
}

void TempleCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
  // Do nothing, to avoid flashing.
}

void TempleCanvas::OnKeyDown(wxKeyEvent& event)
{
  long keycode = event.GetKeyCode();

  m_board.ProcessKeyDown(keycode);

  if (keycode < 512)
    m_keys[keycode] = (m_keys[keycode] == 0)?1:2;
}

void TempleCanvas::OnKeyUp(wxKeyEvent& event)
{
  long keycode = event.GetKeyCode();

  m_board.ProcessKeyUp(keycode);

  if (keycode < 512)
    m_keys[keycode] = 0;
}

void TempleCanvas::OnMouse(wxMouseEvent &event)
{
  m_lbd = event.LeftIsDown();
  m_mbd = event.MiddleIsDown();
  m_rbd = event.RightIsDown();

  static bool moving = false;
  static Vec3f movement(0, 0, 0);

  if (event.Moving() || event.Dragging())
  {
    long nX, nY;
    event.GetPosition(&nX, &nY);
    m_mNew = Vec2i(nX, nY);

    // Transform the mouse position to agree with OpenGL's perspective
    m_mNew.y = -(m_mNew.y - m_winHeight / 2);
    m_mNew.x -= m_winWidth / 2;

    float deltaX = m_mNew.x - m_mOld.x;
    float deltaY = m_mNew.y - m_mOld.y;

    int viewport[4];
    double modelview[16];
    double projection[16];
    float depth;
    double x, y, z;

    m_mWorld = Vec3f((float)x, (float)y, (float)z);

    if (moving)
    {
      m_mNew = m_mOld + (m_mNew - m_mIni);
      WarpPointer(m_mIni.x, m_mIni.y);
    }

    Vec2i diff = m_mNew - m_mOld;
    float cMovement = sqrtf(diff.x * diff.x + diff.y * diff.y);

    if (m_lbd)
      movement.x += cMovement;
    if (m_mbd)
      movement.y += cMovement;
    if (m_rbd)
      movement.z += cMovement;

    m_board.ProcessMotion(m_mNew);

    m_mOld = m_mNew;
    m_mWorldOld = m_mWorld;
  }

  if (event.ButtonDown(wxMOUSE_BTN_LEFT))
  {
    movement.x = 0.0f;

    m_board.ProcessClick(m_mNew, rwMOUSE_LEFT);

    m_mIni = m_mNew;
    m_mWorldIni = m_mWorld;
  }
  else if (event.ButtonUp(wxMOUSE_BTN_LEFT))
  {
    m_board.ProcessUnclick(m_mNew, rwMOUSE_LEFT);
  }

  if (event.ButtonDown(wxMOUSE_BTN_MIDDLE))
  {
    movement.y = 0.0f;
    m_board.ProcessClick(m_mNew, rwMOUSE_MIDDLE);
  }
  else if (event.ButtonUp(wxMOUSE_BTN_MIDDLE))
    m_board.ProcessUnclick(m_mNew, rwMOUSE_MIDDLE);

  if (event.ButtonDown(wxMOUSE_BTN_RIGHT))
  {
    movement.z = 0.0f;
    m_board.ProcessClick(m_mNew, rwMOUSE_RIGHT);
  }
  else if (event.ButtonUp(wxMOUSE_BTN_RIGHT))
    m_board.ProcessUnclick(m_mNew, rwMOUSE_RIGHT);
}

void TempleCanvas::ProcessKeys()
{
}

// TempleFrame

TempleFrame::TempleFrame(wxWindow *parent, const wxString& title,
  const wxPoint& pos, const wxSize& size, long style):
  wxFrame(parent, wxID_ANY, title, pos, size, style)
{
  m_canvas = NULL;
  FrameTimer::init();
}

void TempleFrame::OnExit( wxCommandEvent& WXUNUSED(event) )
{
  FrameTimer::cleanup();
  Close(true);
}

TempleFrame *TempleFrame::Create()
{
  wxString str = wxT("Come on and slam");
  wxSize size(800, 600);

  TempleFrame *frame = new TempleFrame(NULL, str, wxDefaultPosition, size);

  frame->m_canvas = new TempleCanvas(frame, wxID_ANY, wxDefaultPosition,
    wxDefaultSize, wxWANTS_CHARS);

  frame->Show(true);

  return frame;
}

void TempleFrame::LayoutChildren()
{
  wxSize size = GetClientSize();

  if (m_canvas)
    m_canvas->SetSize(0, 0, size.x, size.y);
}

void TempleFrame::OnSize(wxSizeEvent& event)
{
  if (m_canvas)
  {
    wxSize size(GetClientSize());
    m_canvas->SetSize(0, 0, size.x, size.y);
    m_canvas->OnSize(event);
  }

  LayoutChildren();
}

void TempleFrame::OnIdle(wxIdleEvent &event)
{
  FrameTimer::update(1 / 60.0);
  if (m_canvas)
    m_canvas->DrawScene();
  event.RequestMore();

  if (m_canvas)
    m_canvas->SetFocus();
}

void TempleFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  Close(true);
}

// TempleApp

IMPLEMENT_APP(TempleApp)

bool TempleApp::OnInit()
{
  time_t t;
  time(&t);
  srand(t);

  //wxSetWorkingDirectory("../resources");
  TempleFrame::Create();

  return true;
}
