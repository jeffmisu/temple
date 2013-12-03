
#ifndef COMPONENT_H
#define COMPONENT_H

#include <vector>
#include <string>
#include "Utility.h"
#include "Interface.h"
#include "ComponentGlobals.h"

class Component;
class ComponentBoard;
class ComponentGroup;

struct sPatchOut
{
  std::vector<Component *> clients;
  ComponentType type;
  std::string name;

  // Dynamic output patches assume the type of the dynamic input patches on
  // the component.
  bool dynamic;

  // Yeah, values are retrieved by dereferencing void *. I don't feel terrible
  // about it because the system performs "type-checking" as it's being built.
  void *value;

  sPatchOut()
  {
    type = CT_NONE;
    name = "";
    dynamic = false;
    value = NULL;
  };

  sPatchOut(ComponentType type, std::string name, bool dynamic=false)
  {
    this->type = type;
    this->name = name;
    this->dynamic = dynamic;
    value = NULL;
  };
};

struct sPatchIn
{
  Component *comp;      // Whence this patch reads
  int index;            // The index of the output accessed from comp
  ComponentType type;
  std::string name;

  // Dynamically typed patches can take any input type as long as all other
  // dynamically typed patches on the component have that same (or no) type.
  bool dynamic;

  // This 'value' is actually shared with that of the corresponding sPatchOut.
  // The outputting component is responsible for the allocation and destruction
  // of the referenced value.
  void *value;

  sPatchIn()
  {
    comp = NULL;
    index = -1;
    type = CT_NONE;
    name = "";
    dynamic = false;
    value = NULL;
  };

  sPatchIn(ComponentType type, std::string name, bool dynamic=false)
  {
    comp = NULL;
    index = -1;
    this->type = type;
    this->name = name;
    this->dynamic = dynamic;
    value = NULL;
  };
};

class Component: public rwElement
{
  protected:
  std::vector<sPatchIn> m_inputs;
  std::vector<sPatchOut> m_outputs;
  ComponentGroup *m_group;
  ComponentBoard *m_host;
  int m_timestamp;

  // InitializePatches turns the input/output blueprints into actual patches
  // for the calling object. (Do it once at construction time.) The first is a
  // convenience version for single-output components so they don't have to
  // bother with an output blueprint.
  void InitializePatches(const sPatchIn blueprintIn[], size_t ct,
    ComponentType outType);
  void InitializePatches(const sPatchIn blueprintIn[], size_t ctIn,
    const sPatchOut blueprintOut[], size_t ctOut);

  void UpdateDynamicInputs(ComponentType type);

  void WriteDefaultParams(std::ostream &out);
  void ReadDefaultParams(std::istream &in);

  public:
  Component();
  virtual ~Component();

  void EradicateSubtree();

  inline int GetTimestamp() const {return m_timestamp;};
  std::vector<sPatchIn> &GetInputs();
  std::vector<sPatchOut> &GetOutputs();
  std::vector<Component *> GetAllClients();
  virtual void SetInput(size_t ind, Component *node, int patchInd);
  void AddOutput(Component *node, int index);
  void RemoveInput(Component *node, bool mirror = true);
  void RemoveOutput(Component *node, bool mirror = true);
  void ClearOutputs();
  void SetHost(ComponentBoard *host);
  ComponentGroup *GetGroup();
  void LeaveGroup();
  void Move(Vec2i d);

  std::vector<Vec2i> PlaceInputPatches();
  std::vector<Vec2i> PlaceOutputPatches();
  std::string GetInputText(int ind);
  std::string GetOutputText(int ind);
  virtual void RenderConnections();
  virtual void Render();
  void DebugPrint(int depth = 0);

  template <typename T> inline void GetInput(int ind, T *dest);
  template <typename T> inline void SetOutput(int ind, T val);
  virtual void Evaluate(int timestamp) = 0;

  // Allows components to define their own behavior when right clicked. For
  // example, the value component allows text entry to set a new value.
  virtual void OnRightClicked() {};

  virtual std::string GetNodeType() = 0;

  // These functions should always be defined by any subclass of Component.
  virtual void ToFile(std::ostream &out) = 0;
  //static Component *FromFile(std::istream &in);
  //static Component *Create();
  virtual Component *Copy() = 0;

  friend class ComponentGroup;
};

#define UPDATE_TIMESTAMP() (m_timestamp = timestamp)
#define ADVANCE_TIMESTAMP(timestamp) (timestamp ^ 1)
#define PASS_TIMESTAMP(patch) (patch.comp->GetTimestamp() != m_timestamp)

template <typename T> inline void Component::GetInput(int ind, T *dest)
{
  sPatchIn &p = m_inputs[ind];
  if (p.comp != NULL)
  {
    if (p.comp->GetTimestamp() != m_timestamp)
      p.comp->Evaluate(m_timestamp);
    *dest = *(T *)p.value;
  }
};

template <typename T> inline void Component::SetOutput(int ind, T val)
{
  *(T *)m_outputs[ind].value = val;
};

#endif

