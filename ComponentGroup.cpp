
#include "ComponentGroup.h"

#include <iostream>
#include "ComponentBoard.h"

ComponentGroup::ComponentGroup(ComponentBoard *host):
  m_host(host),
  m_name("Group")
{
  m_color = Vec3f((float)rand() / RAND_MAX, (float)rand() / RAND_MAX,
    (float)rand() / RAND_MAX);
}

ComponentGroup::ComponentGroup(ComponentBoard *host, Vec2i pos, Vec2i size):
  m_host(host),
  m_name("Group"),
  m_position(pos),
  m_size(size)
{
  m_color = Vec3f(0.5, 0.5, 0.5) + Vec3f((float)rand() / RAND_MAX,
    (float)rand() / RAND_MAX, (float)rand() / RAND_MAX) * 0.5;
}

ComponentGroup::~ComponentGroup()
{
  for (size_t i = 0; i < m_members.size(); i++)
    m_members[i]->m_group = NULL;
}

std::vector<Component *> &ComponentGroup::GetMembers()
{
  return m_members;
}

void ComponentGroup::AddComponent(Component *c)
{
  for (size_t i = 0; i < m_members.size(); i++)
    if (m_members[i] == c)
      return;

  c->m_group = this;
  m_members.push_back(c);
}

void ComponentGroup::RemoveComponent(Component *c)
{
  for (size_t i = 0; i < m_members.size(); i++)
    if (m_members[i] == c)
    {
      m_members.erase(m_members.begin() + i);
      c->m_group = NULL;
      return;
    }
}

bool ComponentGroup::CheckInBox(Vec2i p)
{
  return pointInRectangle(m_position, m_position + m_size, p);
}

bool ComponentGroup::CheckInTag(Vec2i p)
{
  Vec2i size = m_host->GetFont()->GetStringSize(m_name);
  return pointInRectangle(
    Vec2i(m_position.x, m_position.y + m_size.y),
    Vec2i(m_position.x + size.x + 10, m_position.y + m_size.y - size.y),
    p);
}

Vec3f ComponentGroup::GetColor() const
{
  return m_color;
}

void ComponentGroup::ConstrainPoint(Vec2i &p)
{
  if (p.x < m_position.x)
    p.x = m_position.x;
  if (p.y < m_position.y)
    p.y = m_position.y;
  if (p.x > m_position.x + m_size.x)
    p.x = m_position.x + m_size.x;
  if (p.y > m_position.y + m_size.y)
    p.y = m_position.y + m_size.y;
}

void ComponentGroup::Render(bool selected)
{
  glLineWidth(2);
  if (!selected)
    glColor4f(m_color.x, m_color.y, m_color.z, 1);
  else
    glColor4f(1, 1, 0, 1);

  FontRenderer *font = m_host->GetFont();
  Vec2i size = font->GetStringSize(m_name);

  glBegin(GL_QUADS);
  glVertex2i(m_position.x, m_position.y + m_size.y - size.y);
  glVertex2i(m_position.x + size.x + 10, m_position.y + m_size.y - size.y);
  glVertex2i(m_position.x + size.x + 10, m_position.y + m_size.y);
  glVertex2i(m_position.x, m_position.y + m_size.y);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex2i(m_position.x, m_position.y);
  glVertex2i(m_position.x + m_size.x, m_position.y);
  glVertex2i(m_position.x + m_size.x, m_position.y + m_size.y);
  glVertex2i(m_position.x, m_position.y + m_size.y);
  glVertex2i(m_position.x, m_position.y);
  glEnd();

  glColor4f(0, 0, 0, 1);
  font->WriteText(m_position.x + 5, m_position.y + m_size.y - size.y / 2, 0,
    m_name);
}

void ComponentGroup::Move(Vec2i d)
{
  m_position += d;
  for (size_t i = 0; i < m_members.size(); i++)
    m_members[i]->Move(d);
}

void ComponentGroup::OnRightClicked(rwElement *parent)
{
  rwTextBox *t = new rwTextBox(m_position + Vec2i(0, m_size.y),
    rwFUNCTOR(OnTextEntered), parent);
  t->SetAlphanumericFilter();
  // ':' is used to delimit the name when saving, so I'm just not allowing it
  // in names.
  t->SetKeyFilter(':', false);
  t->SetText("");
}

void ComponentGroup::OnTextEntered(ComponentGroup *caller, rwTextBoxEvent &e)
{
  caller->m_name = e.text;
}

void ComponentGroup::ToFile(std::ostream &out)
{
  std::vector<Component *> &comps = m_host->GetComponents();

  out << m_name << ": " << m_position.x << " " << m_position.y << " "
    << m_size.x << " " << m_size.y;
  for (size_t i = 0; i < m_members.size(); i++)
  {
    int ind = -1;
    for (size_t j = 0; j < comps.size(); j++)
      if (m_members[i] == comps[j])
      {
        ind = j;
        break;
      }
    if (ind != -1)
      out << " " << ind;
  }
}

ComponentGroup *ComponentGroup::FromFile(std::istream &in,
  ComponentBoard *host)
{
  std::string name;
  Vec2i pos, size;

  in.ignore(16, '\n');
  getline(in, name, ':');
  if (!(in >> pos.x >> pos.y >> size.x >> size.y))
    return NULL;

  ComponentGroup *g = new ComponentGroup(host, pos, size);
  g->m_name = name;

  int ind;
  while (in.peek() != '\n')
  {
    if (in >> ind)
      g->AddComponent(host->GetComponents()[ind]);
    std::cout << "ind " << ind << std::endl;
  }

  return g;
}
