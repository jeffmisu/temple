
#include "ComponentRenderer.h"

#include "ComponentBoard.h"
#include <fstream>

ComponentRenderer::ComponentRenderer():
  m_records(NULL)
{
  sPatchIn pi;
  pi.comp = NULL;
  pi.dynamic = false;
  pi.name = "Value";
  pi.type = CT_DOUBLE;

  sPatchOut po;
  po.dynamic = false;
  po.name = "Value";
  po.type = CT_DOUBLE;

  Component::InitializePatches(&pi, 1, &po, 1);

  SetRecordCount(256);

  rwButton *btn = new rwButton(m_position, 10, rwFUNCTOR(OnButtonClicked), "",
    this);
}

ComponentRenderer::~ComponentRenderer()
{
  free(m_records);
}

void ComponentRenderer::SetRecordCount(int ct)
{
  if (ct == m_recordCount)
    return;

  m_recordCount = ct;
  m_records = (double *)realloc(m_records, sizeof(double) * m_recordCount);
  memset(m_records, 0, sizeof(double) * m_recordCount);
}

void ComponentRenderer::Render()
{
  glColor4f(0, 0, 0, 0.5);
  glLineWidth(2);
  Vec2i size = m_host->GetSize();

  float indScale = (float)m_recordCount / size.x;

  glBegin(GL_LINE_STRIP);
  //for (int i = 0, x = size.x / 2; i < s_recordCount && x > -size.x / 2; i++, x--)
  //  glVertex2i(x, m_records[(i + m_index) % s_recordCount] * size.y / 2);
  for (float ind = 0, x = -size.x / 2; (int)ind < m_recordCount && x < size.x / 2; ind = ind + indScale, x++)
    // Render with nonzero z to go behind everything else
    glVertex3f(x, m_records[(int)ind] * size.y / 2, -0.1);
  glEnd();

  Component::Render();
}

void ComponentRenderer::Evaluate(int timestamp)
{
  if (m_inputs[0].comp != NULL)
  {
    for (int i = 0; i < m_recordCount; i++)
    {
      m_inputs[0].comp->Evaluate(
        ADVANCE_TIMESTAMP(m_inputs[0].comp->GetTimestamp()));
      m_records[i] = *(double *)m_inputs[0].value;
    }
    SetOutput(0, *(double *)m_inputs[0].value);
  }
  else
  {
    for (int i = 0; i < m_recordCount; i++)
      m_records[i] = 0;
    SetOutput(0, 0.0);
  }
}

void ComponentRenderer::OnRightClicked()
{
  rwTextBox *t = new rwTextBox(Component::m_position, rwFUNCTOR(OnTextEntered),
    this);
  t->SetNumericFilter();
  t->SetText("");
}

void ComponentRenderer::OnTextEntered(ComponentRenderer *caller,
  rwTextBoxEvent &e)
{
  if (e.type == rwTEXT_RETURN)
  {
    double ct = caller->m_recordCount;
    sscanf(e.text.c_str(), "%lf", &ct);
    caller->SetRecordCount(ct);
  }
}

void ComponentRenderer::OnButtonClicked(ComponentRenderer *caller,
  rwButtonEvent &e)
{
  if (e.type == rwBTN_CLICKED)
    caller->Evaluate(0);
}

std::string ComponentRenderer::GetNodeType()
{
  std::stringstream t;
  t << "Renderer (" << m_recordCount << " samples)";
  return t.str();
}

void ComponentRenderer::ToFile(std::ostream &out)
{
  out << "Renderer";
  WriteDefaultParams(out);
  out << m_recordCount;
}

Component *ComponentRenderer::FromFile(std::istream &in)
{
  ComponentRenderer *c = new ComponentRenderer();
  c->ReadDefaultParams(in);
  in >> c->m_recordCount;

  return c;
}

Component *ComponentRenderer::Create()
{
  return new ComponentRenderer();
}

Component *ComponentRenderer::Copy()
{
  std::stringstream t;
  ToFile(t);
  return FromFile(t);
}
