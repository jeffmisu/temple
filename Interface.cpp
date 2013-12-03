
#include "Interface.h"

static void DrawCircle(unsigned int segs, float radius, Vec2i origin)
{
  glBegin(GL_TRIANGLE_FAN);
  glVertex2iv(&origin.x);
  for (unsigned int i = 0; i < segs + 1; i++)
  {
    float rad = -PI * 2 * i / segs;
    glVertex2f(origin.x + cosf(rad) * radius, origin.y + sinf(rad) * radius);
  }
  glEnd();
}

//-----rwRoot-----

rwRoot::rwRoot():
  m_child(NULL),
  m_font(NULL)
{
  for (int i = 0; i < 512; i++)
    m_keys[i] = 0;
}

rwRoot::rwRoot(FontRenderer *font):
  m_child(NULL),
  m_font(font)
{
  for (int i = 0; i < 512; i++)
    m_keys[i] = 0;
}

rwRoot::~rwRoot()
{
  if (m_child != NULL)
    delete m_child;
}

void rwRoot::SetChild(rwElement *c)
{
  if (m_child != NULL && m_child != c)
    delete m_child;

  m_child = c;
  c->m_root = this;
}

rwElement *rwRoot::GetChild()
{
  return m_child;
}

void rwRoot::Update()
{
  if (m_child == NULL)
    return;

  if (m_child->m_dead)
  {
    m_child->m_parent = NULL;
    delete m_child;
    m_child = NULL;
  }
  else
    m_child->Update();
}

void rwRoot::Render()
{
  if (m_child == NULL)
    return;

  m_child->Render();
}

bool rwRoot::ProcessMouseEvent(rwMouseEvent &e)
{
  if (e.type == rwMOUSE_BTN_DOWN)
  {
    m_mOrigin = e.pos;
    m_mBtnStates |= e.flags;
  }
  else if (e.type == rwMOUSE_BTN_UP)
    m_mBtnStates &= ~e.flags;
  else if (e.type == rwMOUSE_MOVE)
    m_mPosition = e.pos;

  if (m_child == NULL)
    return false;

  return m_child->ProcessMouseEvent(e);
}

bool rwRoot::ProcessKeyEvent(rwKeyEvent &e)
{
  if (e.type == rwKEY_DOWN)
    m_keys[e.keycode] = m_keys[e.keycode]?2:1;
  else if (e.type == rwKEY_UP)
    m_keys[e.keycode] = 0;

  if (m_child == NULL)
    return false;

  return m_child->ProcessKeyEvent(e);
}

//-----rwElement-----

rwElement::rwElement(rwElement *parent, Vec2i pos, Vec2i size):
  m_root(NULL),
  m_parent(NULL),
  m_dead(false),
  m_size(size),
  m_position(pos)
{
  if (parent != NULL)
    parent->AddChild(this);
}

rwElement::~rwElement()
{
  for (size_t i = 0; i < m_children.size(); i++)
    delete m_children[i];

  if (m_parent)
    m_parent->RemoveChild(this);
}

void rwElement::Move(Vec2i d)
{
  m_position += d;
  for (size_t i = 0; i < m_children.size(); i++)
    m_children[i]->Move(d);
}

void rwElement::AddChild(rwElement *c)
{
  for (size_t i = 0; i < m_children.size(); i++)
    if (m_children[i] == c)
      return;

  m_children.push_back(c);
  c->SetParent(this);
}

void rwElement::RemoveChild(rwElement *c)
{
  if (c->m_parent != this)
    return;

  for (size_t i = 0; i < m_children.size(); i++)
    if (m_children[i] == c)
      m_children.erase(m_children.begin() + i);
  c->SetParent(NULL);
}

void rwElement::SetParent(rwElement *p)
{
  if (m_parent == p)
    return;

  m_parent = p;
  if (p == NULL)
    return;
  SetRoot(p->m_root);
  p->AddChild(this);
}

void rwElement::SetRoot(rwRoot *r)
{
  m_root = r;
  for (size_t i = 0; i < m_children.size(); i++)
    m_children[i]->SetRoot(r);
}

void rwElement::Update()
{
  for (size_t i = 0; i < m_children.size(); i++)
  {
    if (m_children[i]->m_dead)
    {
      m_children[i]->m_parent = NULL;
      delete m_children[i];
      m_children.erase(m_children.begin() + i--);
    }
    else
      m_children[i]->Update();
  }
}

void rwElement::Render()
{
  for (size_t i = 0; i < m_children.size(); i++)
    m_children[i]->Render();
}

bool rwElement::ProcessMouseEvent(rwMouseEvent &e)
{
  bool used = false;
  for (size_t i = 0; i < m_children.size(); i++)
    used = used || m_children[i]->ProcessMouseEvent(e);
  return used;
}

bool rwElement::ProcessKeyEvent(rwKeyEvent &e)
{
  bool used = false;
  for (size_t i = 0; i < m_children.size(); i++)
    used = used || m_children[i]->ProcessKeyEvent(e);
  return used;
}

//-----rwContextMenu-----

rwContextMenu::rwContextMenu(Vec2i pos, rwElement *parent):
  rwElement(parent, pos),
  m_choiceHeight(0),
  m_activeChoice(-1),
  m_callback(NULL, NULL)
{
}

rwContextMenu::rwContextMenu(Vec2i pos, rwFunctor callback, rwElement *parent):
  rwElement(parent, pos),
  m_choiceHeight(0),
  m_activeChoice(-1),
  m_callback(callback)
{
}

rwContextMenu::~rwContextMenu()
{
}

void rwContextMenu::AddChoice(std::string name, void *data)
{
  m_choiceNames.push_back(name);
  m_choiceData.push_back(data);
  Vec2i size = m_root->m_font->GetStringSize(name);
  if (size.x > m_size.x)
    m_size.x = size.x;
  m_size.y += size.y;
  if (abs(size.y) > abs(m_choiceHeight))
    m_choiceHeight = size.y;
}

void rwContextMenu::Render()
{
  glBegin(GL_QUADS);
  glColor3f(.65, .7, .8);
  glVertex2i(m_position.x, m_position.y);
  glVertex2i(m_position.x + m_size.x + 10, m_position.y);
  glVertex2i(m_position.x + m_size.x + 10, m_position.y + m_size.y);
  glVertex2i(m_position.x, m_position.y + m_size.y);
  if (m_activeChoice != -1)
  {
    Vec2i p(m_position.x, m_position.y + m_choiceHeight * m_activeChoice);
    glColor3f(.8, .7, .65);
    glVertex2i(p.x, p.y);
    glVertex2i(p.x + m_size.x + 10, p.y);
    glVertex2i(p.x + m_size.x + 10, p.y + m_choiceHeight);
    glVertex2i(p.x, p.y + m_choiceHeight);
  }
  glEnd();

  glColor3f(0, 0, 0);

  glLineWidth(1);
  Vec2i p(m_position.x, m_position.y + m_choiceHeight);
  glBegin(GL_LINES);
  for (size_t i = 0; i < m_choiceNames.size() - 1; i++)
  {
    glVertex2i(p.x, p.y);
    glVertex2i(p.x + m_size.x + 10, p.y);
    p.y += m_choiceHeight;
  }
  glEnd();

  p = (m_position + Vec2i(5, m_choiceHeight / 2));
  for (size_t i = 0; i < m_choiceNames.size(); i++)
  {
    m_root->m_font->WriteText(p.x, p.y, 0, m_choiceNames[i]);
    p.y += m_choiceHeight;
    p.x = m_position.x + 5;
  }
}

bool rwContextMenu::ProcessMouseEvent(rwMouseEvent &e)
{
  if (e.type == rwMOUSE_MOVE)
  {
    if (e.pos.x > m_position.x && e.pos.x < m_position.x + m_size.x + 10 &&
        e.pos.y < m_position.y && e.pos.y > m_position.y + m_size.y)
    {
      m_activeChoice = -(m_position.y - e.pos.y) / m_choiceHeight;
    }
    else
      m_activeChoice = -1;
  }
  else if (e.type == rwMOUSE_BTN_UP && e.flags & rwMOUSE_LEFT)
  {
    if (m_activeChoice != -1)
    {
      if (m_callback.func != NULL)
      {
        rwContextMenuEvent v = {m_position, rwCONTEXT_SELECT, m_activeChoice,
          m_choiceData[m_activeChoice]};
        ((rwContextMenuCallback)m_callback.func)(m_callback.caller, v);
        m_dead = true;
      }
      return true;
    }
    else
    {
      if (m_callback.func != NULL)
      {
        rwContextMenuEvent e = {m_position, rwCONTEXT_CLOSE, m_activeChoice,
          NULL};
        ((rwContextMenuCallback)m_callback.func)(m_callback.caller, e);
      }
      m_dead = true;
    }
  }
  else if (!(e.type == rwMOUSE_BTN_DOWN && e.flags & rwMOUSE_LEFT))
    m_dead = true;

  rwElement::ProcessMouseEvent(e);

  return false;
}

//-----rwTextBox-----

static char SHIFT_MAP[] =
{
  '!', // !
  '"', // "
  '#', // #
  '$', // $
  '%', // %
  '&', // &
  '"', // '
  '(', // (
  ')', // )
  '*', // *
  '+', // +
  '<', // ,
  '_', // -
  '>', // .
  '?', // /
  ')', // 0
  '!', // 1
  '@', // 2
  '#', // 3
  '$', // 4
  '%', // 5
  '^', // 6
  '&', // 7
  '*', // 8
  '(', // 9
  ':', // :
  ':', // ;
  '<', // <
  '+', // =
  '>', // >
  '?', // ?
  '@', // @
  'A', // A
  'B', // B
  'C', // C
  'D', // D
  'E', // E
  'F', // F
  'G', // G
  'H', // H
  'I', // I
  'J', // J
  'K', // K
  'L', // L
  'M', // M
  'N', // N
  'O', // O
  'P', // P
  'Q', // Q
  'R', // R
  'S', // S
  'T', // T
  'U', // U
  'V', // V
  'W', // W
  'X', // X
  'Y', // Y
  'Z', // Z
  '{', // [
  '|', // Backslash
  '}', // ]
  '^', // ^
  '_', // _
  '~', // `
  'A', // a
  'B', // b
  'C', // c
  'D', // d
  'E', // e
  'F', // f
  'G', // g
  'H', // h
  'I', // i
  'J', // j
  'K', // k
  'L', // l
  'M', // m
  'N', // n
  'O', // o
  'P', // p
  'Q', // q
  'R', // r
  'S', // s
  'T', // t
  'U', // u
  'V', // v
  'W', // w
  'X', // x
  'Y', // y
  'Z', // z
  '{', // {
  '|', // |
  '}', // }
  '~', // ~
};

rwTextBox::rwTextBox(Vec2i pos, rwElement *parent):
  rwElement(parent, pos),
  m_callback(NULL, NULL),
  m_maxChars(64),
  m_cursorIndex(0)
{
  m_size = m_root->m_font->GetMaxSizes();
}

rwTextBox::rwTextBox(Vec2i pos, rwFunctor callback, rwElement *parent):
  rwElement(parent, pos),
  m_callback(callback),
  m_maxChars(64),
  m_cursorIndex(0)
{
  m_size = m_root->m_font->GetMaxSizes();
}

rwTextBox::~rwTextBox()
{
}

void rwTextBox::SetDefaultFilter()
{
  for (int i = 0; i < 512; i++)
    m_filter[i] = false;
  m_filter[rwK_SPACE] = true;
}

void rwTextBox::SetKeyFilter(int code, bool state)
{
  m_filter[code] = state;
}

void rwTextBox::SetNumericFilter()
{
  SetDefaultFilter();
  for (int i = '0'; i <= '9'; i++)
    m_filter[i] = true;
  m_filter['.'] = true;
  m_filter[','] = true;
  m_filter['-'] = true;
}

void rwTextBox::SetAlphanumericFilter()
{
  SetDefaultFilter();
  for (int i = 32; i < 127; i++)
    m_filter[i] = true;
}

void rwTextBox::SetMaxChars(int maxChars)
{
  m_maxChars = maxChars;
}

void rwTextBox::SetText(std::string text)
{
  m_text = text;
  m_size = m_root->m_font->GetStringSize(text);
  m_size.x = std::max(m_size.x, 100);
}

void rwTextBox::Render()
{
  glBegin(GL_QUADS);
  glColor3f(.65, .7, .8);
  glVertex2i(m_position.x - 5, m_position.y + 5);
  glVertex2i(m_position.x + 5 + m_size.x + 10, m_position.y + 5);
  glVertex2i(m_position.x + 5 + m_size.x + 10, m_position.y + m_size.y - 5);
  glVertex2i(m_position.x - 5, m_position.y + m_size.y - 5);

  glColor3f(1, 1, 1);
  glVertex2i(m_position.x, m_position.y);
  glVertex2i(m_position.x + m_size.x + 10, m_position.y);
  glVertex2i(m_position.x + m_size.x + 10, m_position.y + m_size.y);
  glVertex2i(m_position.x, m_position.y + m_size.y);
  glEnd();

  glColor3f(0, 0, 0);

  Vec2i fsize(m_root->m_font->GetStringSize(m_text.substr(0, m_cursorIndex)));
  Vec2i cursorCoords(m_position + Vec2i(fsize.x + 5, -2));

  if (time(NULL) % 2)
  {
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2i(cursorCoords.x, cursorCoords.y);
    glVertex2i(cursorCoords.x, cursorCoords.y + fsize.y + 4);
    glEnd();
  }

  m_root->m_font->WriteText(m_position.x + 5, m_position.y + fsize.y / 2, 0,
    m_text);
}

bool rwTextBox::ProcessKeyEvent(rwKeyEvent &e)
{
  if (e.type == rwKEY_DOWN)
  {
    if (e.keycode == rwK_LEFT && m_cursorIndex > 0)
      m_cursorIndex--;
    else if (e.keycode == rwK_RIGHT && m_cursorIndex < m_text.size())
      m_cursorIndex++;
    else if (e.keycode == rwK_BACKSPACE && m_cursorIndex > 0)
    {
      m_text = m_text.substr(0, m_cursorIndex - 1)
        + m_text.substr(m_cursorIndex);
      m_cursorIndex--;
    }
    else if (e.keycode == rwK_DELETE && m_cursorIndex < m_text.size())
      m_text = m_text.substr(0, m_cursorIndex)
        + m_text.substr(m_cursorIndex + 1);
    else if (m_filter[e.keycode] && m_text.size() < m_maxChars)
    {
      char c = (char)e.keycode;
      if (c >= 'A' && c <= 'Z')
        c += 'a' - 'A';
      if (e.flags & rwKEY_SHIFT_HELD && c >= SHIFT_MAP[0]
        && c <= SHIFT_MAP[sizeof(SHIFT_MAP) / sizeof(char) - 1])
        c = SHIFT_MAP[c - SHIFT_MAP[0]];
      m_text = m_text.insert(m_cursorIndex++, 1, c);
    }
    SetText(m_text);

    if (e.keycode == rwK_RETURN)
    {
      if (m_callback.func != NULL)
      {
        rwTextBoxEvent v = {rwTEXT_RETURN, m_text};
        ((rwTextBoxCallback)m_callback.func)(m_callback.caller, v);
      }
      m_dead = true;
    }
    if (e.keycode == rwK_ESCAPE)
      m_dead = true;
  }

  return true;

  //rwElement::ProcessKeyEvent(e);
}

//-----rwButton-----

rwButton::rwButton(Vec2i pos, float radius, rwFunctor callback,
  std::string label, rwElement *parent):
  rwElement(parent, pos, Vec2i(radius, 0)),
  m_callback(callback),
  m_label(label),
  m_type(rwBTN_ROUND)
{
}

rwButton::rwButton(Vec2i pos, Vec2i size, rwFunctor callback,
  std::string label, rwElement *parent):
  rwElement(parent, pos, size),
  m_callback(callback),
  m_label(label),
  m_type(rwBTN_RECT)
{
}

rwButton::~rwButton()
{
}

void rwButton::Render()
{
  if (m_type == rwBTN_ROUND)
  {
    glColor3f(0, 0, 0);
    DrawCircle(32, m_size.x + 1, m_position);
    glColor3f(.65, .7, .8);
    DrawCircle(32, m_size.x, m_position);
  }
  else if (m_type == rwBTN_RECT)
  {
    glBegin(GL_QUADS);
    glColor3f(.65, .7, .8);
    glVertex2i(m_position.x - 5, m_position.y + 5);
    glVertex2i(m_position.x + 5 + m_size.x + 10, m_position.y + 5);
    glVertex2i(m_position.x + 5 + m_size.x + 10, m_position.y + m_size.y - 5);
    glVertex2i(m_position.x - 5, m_position.y + m_size.y - 5);
    glEnd();
  }
  glColor3f(1, 1, 1);
  m_root->m_font->WriteText(m_position.x, m_position.y, 0, m_label);
}

bool rwButton::ProcessMouseEvent(rwMouseEvent &e)
{
  if (e.type == rwMOUSE_BTN_DOWN && e.flags & rwMOUSE_LEFT)
  {
    if ((m_type == rwBTN_ROUND && length(m_position - e.pos) < m_size.x) ||
      (m_type == rwBTN_RECT && pointInRectangle(m_position,
        m_position + m_size, e.pos)))
    {
      //m_dead = true;
      rwButtonEvent v = {rwBTN_CLICKED};
      ((rwButtonCallback)m_callback.func)(m_callback.caller, v);
      return true;
    }
  }

  return false;
}
