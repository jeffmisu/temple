
#include "ComponentBoard.h"

#include <GLee.h>
#include <fstream>
#include <iostream>
#include <wx/wx.h>
#include "Component.h"
#include "components/ComponentGraph.h"
#include "ComponentGroup.h"
#include "Timer.h"

struct sConnection
{
  int nodeTo;
  int patchTo;
  int nodeFrom;
  int patchFrom;
};

// Values for the size of the "placeholder" graphics
const float BIG_RADIUS = 20.0f;
const float SMALL_RADIUS = 10.0f;

ComponentBoard::ComponentBoard():
  m_mBtnStates(0),
  m_draggedType(0),
  m_font(NULL)
{
  memset(m_keys, 0, sizeof(m_keys));
  m_interfaceRoot = new rwRoot();
  m_interfaceRoot->SetChild(new rwElement());

  Open("systems/default.tbd");
}

ComponentBoard::~ComponentBoard()
{
  for (size_t i = 0; i < m_comps.size(); i++)
    delete m_comps[i];
}

Vec2i ComponentBoard::MatchComponentIn(Vec2i pos)
{
  unsigned int bestComp = -1;
  unsigned int bestPatch = -1;
  float bestCompD = INFINITY;
  float bestPatchD = INFINITY;

  for (size_t i = 0; i < m_comps.size(); i++)
  {
    Vec2f diff(pos.x - m_comps[i]->m_position.x,
      pos.y - m_comps[i]->m_position.y);
    if (length(diff) < bestCompD && bestPatch == -1)
    {
      bestCompD = length(diff);
      bestComp = i;
    }

    std::vector<Vec2i> patches = m_comps[i]->PlaceInputPatches();

    for (size_t j = 0; j < patches.size(); j++)
    {
      diff = Vec2f(pos.x - patches[j].x, pos.y - patches[j].y);
      if (length(diff) < bestPatchD && length(diff) < SMALL_RADIUS)
      {
        bestCompD = bestPatchD = length(diff);
        bestComp = i;
        bestPatch = j;
      }
    }
  }
  if (bestPatchD > SMALL_RADIUS)
    bestPatch = -1;
  if (bestCompD > BIG_RADIUS)
    bestComp = -1;

  return Vec2i(bestComp, bestPatch);
}

Vec2i ComponentBoard::MatchComponentOut(Vec2i pos)
{
  unsigned int bestComp = -1;
  unsigned int bestPatch = -1;
  float bestCompD = INFINITY;
  float bestPatchD = INFINITY;

  for (size_t i = 0; i < m_comps.size(); i++)
  {
    Vec2f diff(pos.x - m_comps[i]->m_position.x,
      pos.y - m_comps[i]->m_position.y);
    if (length(diff) < bestCompD && bestPatch == -1)
    {
      bestCompD = length(diff);
      bestComp = i;
    }

    std::vector<Vec2i> patches = m_comps[i]->PlaceOutputPatches();

    for (size_t j = 0; j < patches.size(); j++)
    {
      diff = Vec2f(pos.x - patches[j].x, pos.y - patches[j].y);
      if (length(diff) < bestPatchD && length(diff) < SMALL_RADIUS)
      {
        bestCompD = bestPatchD = length(diff);
        bestComp = i;
        bestPatch = j;
      }
    }
  }
  if (bestPatchD > SMALL_RADIUS)
    bestPatch = -1;
  if (bestCompD > BIG_RADIUS)
    bestComp = -1;

  return Vec2i(bestComp, bestPatch);
}

int ComponentBoard::MatchGroup(Vec2i pos)
{
  for (size_t i = 0; i < m_groups.size(); i++)
    if (m_groups[i]->CheckInTag(pos))
      return i;

  return -1;
}

void ComponentBoard::InitRenderer()
{
  std::ifstream prefs("config.txt", std::ios::in);

  std::string fontname;
  int fontsize;
  getline(prefs, fontname);
  prefs >> fontsize;

  m_font = new FontRenderer(fontname.c_str(), fontsize);

  m_interfaceRoot->m_font = m_font;

  double r, g, b;
  prefs >> r >> g >> b;
  glClearColor(r, g, b, 1);
}

void ComponentBoard::Resize(Vec2i size)
{
  m_size = size;
}

Vec2i ComponentBoard::GetSize() const
{
  return m_size;
}

void ComponentBoard::ProcessClick(Vec2i pos, int btn)
{
  if (btn & rwMOUSE_LEFT)
    m_mDistances.x = 0;
  if (btn & rwMOUSE_RIGHT)
    m_mDistances.y = 0;
  if (btn & rwMOUSE_MIDDLE)
    m_mDistances.z = 0;

  if (btn == rwMOUSE_LEFT)
    m_mOrigin = pos;
  m_mBtnStates |= btn;

  rwMouseEvent e = {pos, rwMOUSE_BTN_DOWN, btn};
  if (m_interfaceRoot->ProcessMouseEvent(e))
    return;

  // If it's a right click and no other buttons are held, drag something
  if (btn == (int)rwMOUSE_RIGHT && m_mBtnStates == (int)rwMOUSE_RIGHT)
  {
    // First look for a component
    Vec2i match = MatchComponentIn(pos);
    if (match.x != -1 && match.y == -1)
    {
      m_draggedType = 1;
      m_draggedIndex = match.x;
    }
    else
      m_draggedType = 0;

    // Then give groups priority
    for (size_t i = 0; i < m_groups.size(); i++)
      if (m_groups[i]->CheckInTag(pos))
      {
        m_draggedIndex = i;
        m_draggedType = 2;
      }
  }
}

void ComponentBoard::ProcessUnclick(Vec2i pos, int btn)
{
  m_mBtnStates &= ~btn;

  if (btn == rwMOUSE_RIGHT)
  {
    Vec2i match = MatchComponentIn(pos);
    if (m_draggedType == 0 && m_mDistances.y < 5)
    {
      if (match.y != -1)
        CreateCM_InputVar();
      else if (match.x == -1)
        CreateCM_NewNode();
    }
    else if (m_draggedType == 1 && m_mDistances.y < 5)
      m_comps[m_draggedIndex]->OnRightClicked();
    else if (m_draggedType == 2 && m_mDistances.y < 5)
      m_groups[m_draggedIndex]->OnRightClicked(m_interfaceRoot->GetChild());
    m_draggedType = 0;
  }

  if (btn != rwMOUSE_LEFT || m_mBtnStates != 0)
    return;

  rwMouseEvent e = {pos, rwMOUSE_BTN_UP, btn};
  if (m_interfaceRoot->ProcessMouseEvent(e))
    return;

  if (!m_keys[WXK_CONTROL])
  {
    // Try to make the endpoints form a connection
    Vec2i matchOut = MatchComponentOut(m_mOrigin);

    if (matchOut.x != -1 && matchOut.y == -1)
    {
      int grpIndex = -1;
      for (size_t i = 0; i < m_groups.size(); i++)
        if (m_groups[i]->CheckInTag(pos))
        {
          grpIndex = i;
          break;
        }
      if (m_comps[matchOut.x]->GetGroup() != NULL)
        m_comps[matchOut.x]->GetGroup()->RemoveComponent(m_comps[matchOut.x]);
      if (grpIndex != -1)
      {
        m_groups[grpIndex]->AddComponent(m_comps[matchOut.x]);
        m_groups[grpIndex]->ConstrainPoint(m_comps[matchOut.x]->m_position);
      }
      return;
    }

    Vec2i matchIn = MatchComponentIn(pos);
    if (matchOut.x == -1 || matchOut.y == -1)
      return;
    if (matchIn.x == -1 || matchIn.y == -1)
      return;

    m_comps[matchIn.x]->SetInput(matchIn.y, m_comps[matchOut.x], matchOut.y);
  }
  else
  {
    // Create a new group from the selection
    Vec2i min(std::min(m_mOrigin.x, m_mPosition.x),
      std::min(m_mOrigin.y, m_mPosition.y));
    Vec2i max(std::max(m_mOrigin.x, m_mPosition.x),
      std::max(m_mOrigin.y, m_mPosition.y));
    m_groups.push_back(new ComponentGroup(this, min, max - min));
    ComponentGroup &g = *m_groups[m_groups.size() - 1];

    for (size_t i = 2; i < m_comps.size(); i++)
      if (g.CheckInBox(m_comps[i]->m_position)
        && m_comps[i]->GetGroup() == NULL)
        g.AddComponent(m_comps[i]);
  }
}

void ComponentBoard::ProcessMotion(Vec2i pos)
{
  int dist = sqrt(pow(pos.x - m_mPosition.x, 2)
    + pow(pos.y - m_mPosition.y, 2));
  if (m_mBtnStates & rwMOUSE_LEFT)
    m_mDistances.x += dist;
  if (m_mBtnStates & rwMOUSE_RIGHT)
    m_mDistances.y += dist;
  if (m_mBtnStates & rwMOUSE_MIDDLE)
    m_mDistances.z += dist;

  rwMouseEvent e = {pos, rwMOUSE_MOVE, 0};
  if (!m_interfaceRoot->ProcessMouseEvent(e))
  {
    if (m_mBtnStates & rwMOUSE_RIGHT && m_draggedType == 1)
      m_comps[m_draggedIndex]->Move(pos - m_mPosition);
    else if (m_mBtnStates & rwMOUSE_RIGHT && m_draggedType == 2)
      m_groups[m_draggedIndex]->Move(pos - m_mPosition);
    else if (m_mBtnStates & rwMOUSE_MIDDLE)
    {
      for (size_t i = 0; i < m_comps.size(); i++)
        if (m_comps[i]->GetGroup() == NULL)
          m_comps[i]->Move(pos - m_mPosition);
      for (size_t i = 0; i < m_groups.size(); i++)
        m_groups[i]->Move(pos - m_mPosition);
    }
  }

  m_mPosition = pos;
}

void ComponentBoard::ProcessKeyDown(int key)
{
  int flags = 0;
  if (m_keys[key])          flags |= rwKEY_REPEAT;
  if (m_keys[WXK_SHIFT])    flags |= rwKEY_SHIFT_HELD;
  if (m_keys[WXK_CONTROL])  flags |= rwKEY_CTRL_HELD;
  if (m_keys[WXK_ALT])      flags |= rwKEY_ALT_HELD;
  rwKeyEvent e = {rwKEY_DOWN, flags, key};
  if (m_interfaceRoot->ProcessKeyEvent(e))
  {
    m_keys[key] = m_keys[key]?1:2;
    return;
  }

  if (key == WXK_DELETE && m_keys[key] == 0)
  {
    Vec2i match = MatchComponentIn(m_mPosition);
    if (match.y != ~0)
      m_comps[match.x]->SetInput(match.y, NULL, -1);
    else if (match.x > 1)
    {
      if (m_draggedType == 1 && match.x == m_draggedIndex)
        m_draggedType = 0;
      delete m_comps[match.x];
      m_comps.erase(m_comps.begin() + match.x);
    }
    else
    {
      int group = MatchGroup(m_mPosition);
      if (group != -1)
      {
        delete m_groups[group];
        m_groups.erase(m_groups.begin() + group);
      }
    }
  }

  if (key == WXK_SPACE)
    CreateVoice(m_comps[1]);

  m_keys[key] = m_keys[key]?1:2;

  if (key == 'S' && m_keys[WXK_CONTROL])
  {
    m_keys[WXK_CONTROL] = m_keys['S'] = 0;
    Save();
  }
  if (key == 'O' && m_keys[WXK_CONTROL])
  {
    m_keys[WXK_CONTROL] = m_keys['O'] = 0;
    Open();
  }
}

void ComponentBoard::ProcessKeyUp(int key)
{
  m_keys[key] = 0;

  rwKeyEvent e = {rwKEY_UP, 0, key};
  if (m_interfaceRoot->ProcessKeyEvent(e))
    return;
}

void ComponentBoard::AddComponent(Component *c)
{
  m_comps.push_back(c);
}

void ComponentBoard::PushApps(sApplication app)
{
  m_applications.push_front(app);
}

sApplication ComponentBoard::PopApps()
{
  sApplication exp = m_applications[0];
  m_applications.pop_front();
  return exp;
}

FontRenderer *ComponentBoard::GetFont() const
{
  return m_font;
}

std::vector<Component *> &ComponentBoard::GetComponents()
{
  return m_comps;
}

void ComponentBoard::CreateCM_InputVar()
{
  rwContextMenu *cm = new rwContextMenu(m_mPosition,
    rwFUNCTOR(OnContextMenu_InputVar), m_interfaceRoot->GetChild());
  cm->AddChoice("Keyboard input", m_comps[1]);
  cm->AddChoice("Who knows!", m_comps[2]);
  cm->AddChoice("Nothing!", NULL);
}

void ComponentBoard::CreateCM_NewNode()
{
  rwContextMenu *cm = new rwContextMenu(m_mPosition,
    rwFUNCTOR(OnContextMenu_NewNode), m_interfaceRoot->GetChild());
  for (int i = 0; i < cmpTotal; i++)
    cm->AddChoice(cmpGraph[i].name, NULL);
}

void ComponentBoard::OnContextMenu_InputVar(ComponentBoard *caller,
  rwContextMenuEvent &e)
{
  Vec2i match = caller->MatchComponentIn(e.pos);
  if (match.x == -1)
    return;

  if (e.type == rwCONTEXT_SELECT)
    caller->m_comps[match.x]->SetInput(match.y, (Component *)e.data, 0);
}

void ComponentBoard::OnContextMenu_NewNode(ComponentBoard *caller,
  rwContextMenuEvent &e)
{
  if (e.type == rwCONTEXT_SELECT)
    caller->m_mPosition = e.pos;

  Component *c = NULL;
  if (e.index >= 0 && e.index < cmpTotal)
    c = cmpGraph[e.index].factory();
  if (c != NULL)
  {
    c->Move(caller->m_mPosition);
    c->SetHost(caller);
    caller->m_interfaceRoot->GetChild()->AddChild(c);
    caller->m_comps.push_back(c);
    c->OnRightClicked();
  }
}

void ComponentBoard::Update()
{
  m_interfaceRoot->Update();
}

void ComponentBoard::Render()
{
  if (m_font == NULL)
    InitRenderer();

  // If the left button is held, render the guides
  if (m_mBtnStates & rwMOUSE_LEFT)
  {
    glColor4f(1, 1, 0, 1);
    glLineWidth(2);
    if (!m_keys[WXK_CONTROL])
    {
      glBegin(GL_LINES);
      glVertex2iv(&m_mOrigin.x);
      glVertex2iv(&m_mPosition.x);
      glEnd();
    }
    else
    {
      glBegin(GL_LINE_STRIP);
      glVertex2i(m_mOrigin.x,   m_mOrigin.y);
      glVertex2i(m_mOrigin.x,   m_mPosition.y);
      glVertex2i(m_mPosition.x, m_mPosition.y);
      glVertex2i(m_mPosition.x, m_mOrigin.y);
      glVertex2i(m_mOrigin.x,   m_mOrigin.y);
      glEnd();
    }
  }

  int matchG = MatchGroup(m_mPosition);
  for (size_t i = 0; i < m_groups.size(); i++)
    m_groups[i]->Render(i == matchG);

  Vec2i match = MatchComponentIn(m_mPosition);
  bool input = true;
  if (match.y == -1)
  {
    match = MatchComponentOut(m_mPosition);
    input = false;
  }

  if (matchG != -1)
    match.x = match.y = -1;

  // Output informative text!
  glColor4f(0, 0, 0.2, 1);
  if (match.x != -1 && match.y != -1)
  {
    std::string text;
    if (input)
      text = m_comps[match.x]->GetInputText(match.y);
    else
      text = m_comps[match.x]->GetOutputText(match.y);
    m_font->WriteText(-m_size.x / 2 + 5,
      -m_size.y / 2 + 5 - m_font->GetMaxSizes().y, 0, text);
  }

  m_interfaceRoot->Render();
}

void ComponentBoard::Evaluate()
{
  m_comps[0]->Evaluate(ADVANCE_TIMESTAMP(m_comps[0]->GetTimestamp()));
}

void ComponentBoard::CreateVoice(Component *source)
{
/*
  Timer timer;
  timer.start();

  std::stringstream image;

  // Build up an image of the graph and duplicate it.

  for (size_t i = 1; i < m_comps.size(); i++)
  {
    m_comps[i]->ToFile(image);
    image << std::endl;
  }

  std::deque<int> topo;
  std::vector<bool> flags(m_comps.size(), false);
  std::vector<sConnection> conns;
  for (size_t i = 0; i < m_comps.size(); i++)
  {
    if (m_comps[i] == source)
    {
      flags[i] = true;
      topo.push_back(i);
      break;
    }
  }

  while (topo.size() != 0)
  {
    int srcComp = topo[0];
    topo.pop_front();

    std::vector<Component *> clients = m_comps[srcComp]->GetAllClients();
    for (size_t i = 0; i < clients.size(); i++)
    {
      int dstComp = -1;
      for (size_t j = 0; j < m_comps.size(); j++)
        if (m_comps[j] == clients[i])
          dstComp = j;

      std::vector<sPatchIn> &ips = clients[i]->GetInputs();
      for (size_t j = 0; j < ips.size(); j++)
        if (ips[j].comp == m_comps[srcComp])
        {
          sConnection conn = {dstComp, j, srcComp, ips[j].index};
          conns.push_back(conn);
        }

      if (!flags[dstComp])
        topo.push_back(dstComp);
      flags[dstComp] = true;
    }
  }

  std::vector<Component *> newComps(m_comps.size());
  newComps[0] = new ComponentWave();
  newComps[0]->SetHost(this);
  for (int i = 1; i < newComps.size(); i++)
  {
    Component *c = loadFromFile(image);
    if (c != NULL)
    {
      c->SetHost(this);
      newComps[i] = c;
    }
  }

  for (size_t i = 0; i < conns.size(); i++)
  {
    sConnection &conn = conns[i];
    newComps[conn.nodeTo]->SetInput(conn.patchTo, newComps[conn.nodeFrom],
      conn.patchFrom);
  }

  std::cout << timer.getElapsedTimeInMilliSec() << " milliseconds!";
  std::cout << std::endl;

  std::cout << image.str() << std::endl;

  for (size_t i = 0; i < newComps.size(); i++)
    delete newComps[i];
*/
}

void ComponentBoard::Save(std::string filename)
{
  if (filename == std::string(""))
  {
    wxFileDialog *f = new wxFileDialog(NULL, wxT("Where do we save it?"),
      wxT(""), wxT(""), wxT("All files (*.*)|*.*"),
      wxFD_OVERWRITE_PROMPT | wxFD_SAVE);

    if (f->ShowModal() == wxID_OK)
      filename = f->GetPath().mb_str(wxConvUTF8);
    else
    {
      delete f;
      return;
    }
    delete f;
  }

  std::ofstream out(filename.c_str(), std::ios::out);

  // Output the type, position and parameters of each component
  out << m_comps.size() << std::endl;
  for (size_t i = 0; i < m_comps.size(); i++)
  {
    m_comps[i]->ToFile(out);
    out << std::endl;
  }

  // Output the name, position, size, and members of each group
  out << m_groups.size() << std::endl;
  for (size_t i = 0; i < m_groups.size(); i++)
  {
    m_groups[i]->ToFile(out);
    out << std::endl;
  }

  // Now, we do a topological sort on the graph to output connections in a way
  // that (hopefully) preserves typing

  std::deque<int> topo;
  std::vector<bool> flags(m_comps.size(), false);

  // Find the starting points of the topological sort - nodes with no inputs
  for (size_t i = 0; i < m_comps.size(); i++)
  {
    std::vector<sPatchIn> &ips = m_comps[i]->GetInputs();

    bool initial = true;
    for (size_t j = 0; j < ips.size(); j++)
      if (ips[j].comp != NULL)
        initial = false;
    if (initial)
    {
      flags[i] = true;
      topo.push_back(i);
    }
  }

  while (topo.size() != 0)
  {
    int srcComp = topo[0];
    topo.pop_front();

    std::vector<Component *> clients = m_comps[srcComp]->GetAllClients();
    for (size_t i = 0; i < clients.size(); i++)
    {
      int dstComp = -1;
      for (size_t j = 0; j < m_comps.size(); j++)
        if (m_comps[j] == clients[i])
          dstComp = j;

      std::vector<sPatchIn> &ips = clients[i]->GetInputs();
      for (size_t j = 0; j < ips.size(); j++)
        if (ips[j].comp == m_comps[srcComp])
        {
          // m_comps[dstComp]->SetInput(j, m_comps[srcComp], ips[j].index)
          out << dstComp << " " << j << " " << srcComp << " " << ips[j].index;
          out << std::endl;
        }

      if (!flags[dstComp])
        topo.push_back(dstComp);
      flags[dstComp] = true;
    }
  }

  out.close();
}

void ComponentBoard::Open(std::string filename)
{
  if (filename == std::string(""))
  {
    wxFileDialog *f = new wxFileDialog(NULL, wxT("Select a system"), wxT(""),
      wxT(""), wxT("All files (*.*)|*.*"), wxFD_FILE_MUST_EXIST | wxFD_OPEN);
    if (f->ShowModal() == wxID_OK)
      filename = f->GetPath().mb_str(wxConvUTF8);
    else
    {
      delete f;
      return;
    }
    delete f;
  }

  for (size_t i = 0; i < m_comps.size(); i++)
    delete m_comps[i];
  m_comps.clear();

  std::ifstream in(filename.c_str(), std::ios::in);
  int compct, groupct;

  // Read in the components
  in >> compct;
  for (int i = 0; i < compct; i++)
  {
    Component *c = loadFromFile(in);
    if (c != NULL)
    {
      c->SetHost(this);
      m_comps.push_back(c);
      m_interfaceRoot->GetChild()->AddChild(c);
    }
  }

  // Read in the groups
  in >> groupct;
  for (int i = 0; i < groupct; i++)
  {
    ComponentGroup *g = ComponentGroup::FromFile(in, this);
    if (g != NULL)
      m_groups.push_back(g);
  }

  std::vector<sConnection> conns;
  while (in.good())
  {
    int dstComp, inInd, srcComp, outInd;
    if (in >> dstComp >> inInd >> srcComp >> outInd)
    {
      //  m_comps[dstComp]->SetInput(inInd, m_comps[srcComp], outInd);
      sConnection conn = {dstComp, inInd, srcComp, outInd};
      conns.push_back(conn);
    }
  }

  for (int n = 0; n < 2; n++)
    for (size_t i = 0; i < conns.size(); i++)
    {
      sConnection &conn = conns[i];
      m_comps[conn.nodeTo]->SetInput(conn.patchTo, m_comps[conn.nodeFrom],
        conn.patchFrom);
    }

  in.close();
}
