
#include "ComponentAverager.h"

#include <fstream>

const sPatchIn blueprint[] = {
  sPatchIn(CT_DOUBLE, "Value"),
  sPatchIn(CT_BOOL, "Ignore sign")
};

ComponentAverager::ComponentAverager():
  m_window(NULL),
  m_index(0),
  m_operations(0)
{
  sPatchOut po;
  po.dynamic = false;
  po.name = "Averaged value";
  po.type = CT_DOUBLE;

  InitializePatches(blueprint, sizeof(blueprint) / sizeof(sPatchIn), &po, 1);

  SetWindow(16);
  memset(m_window, 0, sizeof(double) * m_windowSize);
  SetOutput(0, 0.0);
}

ComponentAverager::~ComponentAverager()
{
  free(m_window);
}

void ComponentAverager::SetWindow(int ct)
{
  if (ct == m_windowSize)
    return;

  m_windowSize = ct;
  m_window = (double *)realloc(m_window, sizeof(double) * m_windowSize);
  memset(m_window, 0, sizeof(double) * m_windowSize);
  *(double *)m_outputs[0].value = 0.0;
}

void ComponentAverager::Evaluate(int timestamp)
{
  UPDATE_TIMESTAMP();

  bool ignoreSign = true;
  GetInput(1, &ignoreSign);

  // Remove the stale value
  *(double *)m_outputs[0].value -=
    m_window[m_index = (m_index + 1) % m_windowSize] / m_windowSize;

  // Evaluate and add a new value
  GetInput(0, &m_window[m_index]);
  if (ignoreSign)
    m_window[m_index] = fabs(m_window[m_index]);
  SetOutput(0, m_window[m_index] / m_windowSize);

  // Here's where the FP correction comes into play. After a fixed number of
  // operations, we start accumulating a new average independent of the old
  // one. When the new average is ready for usage (it represents the average of
  // an entire window length), we make the substitution.
  const int OP_LIMIT = 50000;
  m_operations++;
  if (m_operations == OP_LIMIT)
    m_remediator = 0.0;
  else if (m_operations > OP_LIMIT)
  {
    m_remediator += m_window[m_index] / m_windowSize;
    if (m_operations == OP_LIMIT + m_windowSize)
    {
      SetOutput(0, m_remediator);
      m_operations = 0;
    }
  }
}

void ComponentAverager::OnRightClicked()
{
  rwTextBox *t = new rwTextBox(Component::m_position, rwFUNCTOR(OnTextEntered),
    this);
  t->SetNumericFilter();
  t->SetText("");
}

void ComponentAverager::OnTextEntered(ComponentAverager *caller,
  rwTextBoxEvent &e)
{
  if (e.type == rwTEXT_RETURN)
  {
    double ct = caller->m_windowSize;
    sscanf(e.text.c_str(), "%lf", &ct);
    caller->SetWindow(ct);
  }
}

std::string ComponentAverager::GetNodeType()
{
  std::stringstream t;
  t << "Averager (" << m_windowSize << " samples)";
  return t.str();
}

void ComponentAverager::ToFile(std::ostream &out)
{
  out << "Averager";
  WriteDefaultParams(out);
  out << m_windowSize;
}

Component *ComponentAverager::FromFile(std::istream &in)
{
  ComponentAverager *c = new ComponentAverager();
  c->ReadDefaultParams(in);
  int size;
  in >> size;
  c->SetWindow(size);

  return c;
}

Component *ComponentAverager::Create()
{
  return new ComponentAverager();
}

Component *ComponentAverager::Copy()
{
  std::stringstream t;
  ToFile(t);
  return FromFile(t);
}
