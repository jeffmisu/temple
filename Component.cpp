
#include "Component.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>

#include "ComponentBoard.h"
#include "ComponentGroup.h"

// Values for the size of the "placeholder" graphics
const float BIG_RADIUS = 20.0f;
const float SMALL_RADIUS = 10.0f;

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

//-----Component-----

Component::Component():
  m_group(NULL),
  m_timestamp(0)
{
}

Component::~Component()
{
  for (size_t i = 0; i < m_outputs.size(); i++)
    deleteValue(m_outputs[i].value, m_outputs[i].type);

  for (size_t i = 0; i < m_inputs.size(); i++)
    if (m_inputs[i].comp != NULL)
      m_inputs[i].comp->RemoveOutput(this, false);
  for (size_t i = 0; i < m_outputs.size(); i++)
    for (size_t j = 0; j < m_outputs[i].clients.size(); j++)
      m_outputs[i].clients[j]->RemoveInput(this, false);
}

void Component::InitializePatches(const sPatchIn blueprintIn[], size_t ct,
  ComponentType outType)
{
  for (size_t i = 0; i < ct; i++)
    m_inputs.push_back(blueprintIn[i]);

  sPatchOut p;
  p.type = outType;
  p.name = "Value";
  p.dynamic = false;
  p.value = newValue(p.type);
  m_outputs.push_back(p);
}

void Component::InitializePatches(const sPatchIn blueprintIn[], size_t ctIn,
  const sPatchOut blueprintOut[], size_t ctOut)
{
  for (size_t i = 0; i < ctIn; i++)
    m_inputs.push_back(blueprintIn[i]);
  for (size_t i = 0; i < ctOut; i++)
  {
    sPatchOut p = blueprintOut[i];
    p.value = newValue(p.type);
    m_outputs.push_back(p);
  }
}

void Component::EradicateSubtree()
{
  for (size_t i = 0; i < m_inputs.size(); i++)
  {
    if (m_inputs[i].comp == NULL)
      continue;
    m_inputs[i].comp->EradicateSubtree();
    m_inputs[i].comp->RemoveOutput(this);
    delete m_inputs[i].comp;
    m_inputs[i].comp = NULL;
  }
  UpdateDynamicInputs(CT_NONE);
}

void Component::UpdateDynamicInputs(ComponentType type)
{
  bool clear = true;

  for (size_t i = 0; i < m_inputs.size(); i++)
    if (m_inputs[i].dynamic && m_inputs[i].comp != NULL)
      clear = false;

  if (clear)
    for (size_t i = 0; i < m_inputs.size(); i++)
      if (m_inputs[i].dynamic)
        m_inputs[i].type = type;
}

std::vector<sPatchIn> &Component::GetInputs()
{
  return m_inputs;
}

std::vector<sPatchOut> &Component::GetOutputs()
{
  return m_outputs;
}

std::vector<Component *> Component::GetAllClients()
{
  std::vector<Component *> clients;
  for (size_t i = 0; i < m_outputs.size(); i++)
    clients.insert(clients.end(),
      m_outputs[i].clients.begin(), m_outputs[i].clients.end());
  sort(clients.begin(), clients.end());
  clients.resize(unique(clients.begin(), clients.end()) - clients.begin());
  clients.resize(remove(clients.begin(), clients.end(),
    (Component *)NULL) - clients.begin());
  return clients;
}

void Component::SetInput(size_t ind, Component *node, int patchInd)
{
  if (ind >= m_inputs.size())
    return;

  ComponentType type = CT_NONE;
  if (node != NULL)
  {
    type = node->m_outputs[patchInd].type;
    // If we're trying to connect an abyssal dynamic output to this component,
    // we just flat out reject it
    if (type == CT_NONE)
      return;
  }

  if (m_inputs[ind].comp != NULL)
  {
    m_inputs[ind].comp->RemoveOutput(this, false);
    m_inputs[ind].comp = NULL;
  }

  bool valid = true;
  bool clearDynamicOut = true;
  if (node == NULL || (!m_inputs[ind].dynamic && type != m_inputs[ind].type))
    valid = false;
  if (m_inputs[ind].dynamic)
    for (size_t i = 0; i < m_inputs.size(); i++)
      if (i != ind && m_inputs[i].dynamic)
        if (!(m_inputs[i].type == CT_NONE || m_inputs[i].type == type)
          && m_inputs[i].comp != NULL)
        {
          valid = false;
          clearDynamicOut = false;
          type = m_inputs[i].type;
          break;
        }

  if (m_inputs[ind].dynamic)
  {
    UpdateDynamicInputs(type);
    m_inputs[ind].type = type;
    if (clearDynamicOut)
    {
      for (size_t i = 0; i < m_outputs.size(); i++)
        if (m_outputs[i].dynamic && m_outputs[i].type != type)
        {
          for (size_t j = 0; j < m_outputs[i].clients.size(); j++)
            m_outputs[i].clients[j]->RemoveInput(this, false);
          m_outputs[i].clients.clear();
          deleteValue(m_outputs[i].value, m_outputs[i].type);
          m_outputs[i].type = type;
          m_outputs[i].value = newValue(m_outputs[i].type);
        }
    }
  }

  if (valid)
  {
    m_inputs[ind].comp = node;
    m_inputs[ind].index = patchInd;
    m_inputs[ind].value = node->m_outputs[patchInd].value;
    node->AddOutput(this, patchInd);
  }
  else
  {
    m_inputs[ind].comp = NULL;
    m_inputs[ind].index = -1;
    m_inputs[ind].value = NULL;
  }
}

void Component::AddOutput(Component *node, int index)
{
  // Enforce unique membership
  for (size_t i = 0; i < m_outputs[index].clients.size(); i++)
    if (m_outputs[index].clients[i] == node)
      return;
  m_outputs[index].clients.push_back(node);
}

void Component::RemoveInput(Component *node, bool mirror)
{
  int ind = -1;
  for (size_t i = 0; i < m_inputs.size(); i++)
  {
    if (m_inputs[i].comp == node)
    {
      ind = i;
      m_inputs[i].comp = NULL;
      if (m_inputs[i].dynamic)
        SetInput(i, NULL, -1);
      // Don't break - components can be used for multiple input patches
    }
  }

  if (m_inputs[ind].dynamic && ind != -1)
    UpdateDynamicInputs(m_inputs[ind].type);

  if (ind != -1 && mirror && node != NULL)
    node->RemoveOutput(this, false);
}

void Component::RemoveOutput(Component *node, bool mirror)
{
  bool found = false;
  for (size_t i = 0; i < m_outputs.size(); i++)
    for (size_t j = 0; j < m_outputs[i].clients.size(); j++)
      if (m_outputs[i].clients[j] == node)
      {
        found = true;
        m_outputs[i].clients.erase(m_outputs[i].clients.begin() + j);
        break;
      }

  if (found && mirror)
    node->RemoveInput(this, false);
}

void Component::ClearOutputs()
{
  for (size_t i = 0; i < m_outputs.size(); i++)
  {
    for (size_t j = 0; j < m_outputs[i].clients.size(); j++)
      m_outputs[i].clients[j]->RemoveInput(this, false);
    m_outputs[i].clients.clear();
  }
}

void Component::SetHost(ComponentBoard *host)
{
  m_host = host;
}

ComponentGroup *Component::GetGroup()
{
  return m_group;
}

void Component::LeaveGroup()
{
  if (m_group != NULL)
  {
    m_group->RemoveComponent(this);
    m_group = NULL;
  }
}

void Component::Move(Vec2i d)
{
  d = d + m_position;
  if (m_group != NULL)
    m_group->ConstrainPoint(d);
  d = d - m_position;

  rwElement::Move(d);
}

std::vector<Vec2i> Component::PlaceInputPatches()
{
  std::vector<Vec2i> places(m_inputs.size());

  size_t ct = m_inputs.size();
  float start = ((int)ct - 1) / 2.0;
  float end = -start;
  float step = 1.0f;
  if (ct > 1)
    step = (end - start) / (ct - 1);

  const float radius = BIG_RADIUS * 1.5;

  Vec2i place;
  size_t i = 0;
  for (float theta = start; i < places.size(); theta += step, i++)
  {
    float tp = PI + PI * (theta / 4);
    place.x = m_position.x + cosf(tp) * radius;
    place.y = m_position.y + sinf(tp) * radius;
    places[i] = place;
  }

  return places;
}

std::vector<Vec2i> Component::PlaceOutputPatches()
{
  std::vector<Vec2i> places(m_outputs.size());

  size_t ct = m_outputs.size();
  float start = -((int)ct - 1) / 2.0;
  float end = -start;
  float step = 1.0f;
  if (ct > 1)
    step = (end - start) / (ct - 1);

  const float radius = BIG_RADIUS * 1.5;

  Vec2i place;
  size_t i = 0;
  for (float theta = start; i < places.size(); theta += step, i++)
  {
    float tp = PI * (theta / 4);
    place.x = m_position.x + cosf(tp) * radius;
    place.y = m_position.y + sinf(tp) * radius;
    places[i] = place;
  }

  return places;
}

std::string Component::GetInputText(int ind)
{
  std::stringstream s;
  s << "\"" << m_inputs[ind].name << "\": "
    << typeInfo[m_inputs[ind].type].name;
  return s.str();
}

std::string Component::GetOutputText(int ind)
{
  std::stringstream s;
  s << "\"" << m_outputs[ind].name << "\": "
    << typeInfo[m_outputs[ind].type].name << " ";
  s << "(" << getValueString(m_outputs[ind].value, m_outputs[ind].type) << ")";
  return s.str();
}

void Component::RenderConnections()
{
  std::vector<Vec2i> placesIn = PlaceInputPatches();

  for (size_t i = 0; i < m_inputs.size(); i++)
  {
    if (m_inputs[i].comp == NULL)
      continue;
    std::vector<Vec2i> placesOut = m_inputs[i].comp->PlaceOutputPatches();
    glLineWidth(4);
  glBegin(GL_LINES);
    glColor4f(0, 0, 0, 1);
    glVertex3f(placesIn[i].x, placesIn[i].y, -0.5);
    glVertex3f(placesOut[m_inputs[i].index].x, placesOut[m_inputs[i].index].y,
      -1);
  glEnd();
    glLineWidth(2);
  glBegin(GL_LINES);
    glColor4f(1, 1, 0.5, 1);
    glVertex3f(placesIn[i].x, placesIn[i].y, -0.5);
    glVertex3f(placesOut[m_inputs[i].index].x, placesOut[m_inputs[i].index].y,
      -1);
  glEnd();
  }
}

void Component::Render()
{
  RenderConnections();

  std::vector<Vec2i> placesIn = PlaceInputPatches();
  std::vector<Vec2i> placesOut = PlaceOutputPatches();
  Vec3f color;

  FontRenderer *font = m_root->m_font;
  bool selected = false;
  int activeIn = -1, activeOut = -1;

  // Draw input patches
  for (size_t i = 0; i < placesIn.size(); i++)
  {
    if (length(placesIn[i] - m_root->m_mPosition) < SMALL_RADIUS)
      activeIn = i;

    glColor4f(0, 0, 0, 1);
    DrawCircle(64, SMALL_RADIUS + 1, placesIn[i]);
    if (i == activeIn)
      color = Vec3f(1, 1, 0.2);
    else
      color = typeInfo[m_inputs[i].type].color;
    glColor4f(color.x, color.y, color.z, 0.5);
    DrawCircle(64, SMALL_RADIUS, placesIn[i]);
    glColor4f(color.x, color.y, color.z, 1.0);
    DrawCircle(64, SMALL_RADIUS - 1, placesIn[i]);
  }

  // Draw output patches
  for (size_t i = 0; i < placesOut.size(); i++)
  {
    if (length(placesOut[i] - m_root->m_mPosition) < SMALL_RADIUS)
      activeOut = i;

    glColor4f(0, 0, 0, 1);
    DrawCircle(64, SMALL_RADIUS + 1, placesOut[i]);
    if (i == activeOut)
      color = Vec3f(1, 1, 0.2);
    else
      color = typeInfo[m_outputs[i].type].color;
    glColor4f(color.x, color.y, color.z, 0.5);
    DrawCircle(64, SMALL_RADIUS, placesOut[i]);
    glColor4f(color.x, color.y, color.z, 1.0);
    DrawCircle(64, SMALL_RADIUS - 1, placesOut[i]);
  }

  // Then draw the node itself
  selected = (length(m_position - m_root->m_mPosition) < BIG_RADIUS)
    || (m_root->m_mBtnStates & rwMOUSE_LEFT && m_root->m_keys[rwK_CONTROL]
    && pointInRectangle(m_root->m_mOrigin, m_root->m_mPosition, m_position));

  glColor4f(0, 0, 0, 1);
  DrawCircle(64, BIG_RADIUS + 1, m_position);
  if (selected)
    color = Vec3f(1, 1, 0.2);
  else
    color = Vec3f(0.8, 1, 0.7);
  glColor4f(0, 0, 0, 1);
  font->WriteText(m_position.x - BIG_RADIUS / 2,
    m_position.y - BIG_RADIUS * 1.5, 0, GetNodeType());
  if (GetGroup() != NULL)
    color = color * 0.5f + GetGroup()->GetColor() * 0.5f;
  glColor4f(color.x, color.y, color.z, 0.5);
  DrawCircle(64, BIG_RADIUS, m_position);
  glColor4f(color.x, color.y, color.z, 1.0);
  DrawCircle(64, BIG_RADIUS - 1, m_position);

  glPushMatrix();
  glTranslatef(0, 0, .01);
  rwElement::Render();
  glPopMatrix();
}

void Component::DebugPrint(int depth)
{
  for (int i = 0; i < depth; i++)
    std::cout << "  ";
  std::cout << GetNodeType() << std::endl;

  for (size_t i = 0; i < m_outputs.size(); i++)
    for (size_t j = 0; j < m_outputs[i].clients.size(); j++)
      m_outputs[i].clients[j]->DebugPrint(depth + 1);
}

void Component::WriteDefaultParams(std::ostream &out)
{
  out << " " << m_position.x << " " << m_position.y << " ";
}

void Component::ReadDefaultParams(std::istream &in)
{
  Vec2i pos;
  in >> pos.x >> pos.y;
  Move(pos);
}
