
#ifndef COMPONENTGROUP_H
#define COMPONENTGROUP_H

#include "Component.h"

class ComponentBoard;

// ComponentGroups are collections of components that (probably) form a
// function. They can be visually condensed into a single unit for convenience,
// exposing only those inputs that are not filled within the scope of the
// group.

class ComponentGroup
{
  protected:
  ComponentBoard *m_host;
  std::vector<Component *> m_members;
  Vec3f m_color;
  std::string m_name;

  public:
  ComponentGroup(ComponentBoard *host);
  ComponentGroup(ComponentBoard *host, Vec2i pos, Vec2i size);
  ~ComponentGroup();

  Vec2i m_position;
  Vec2i m_size;

  std::vector<Component *> &GetMembers();
  void AddComponent(Component *c);
  void RemoveComponent(Component *c);
  bool CheckInBox(Vec2i p);
  bool CheckInTag(Vec2i p);
  Vec3f GetColor() const;
  void ConstrainPoint(Vec2i &p);

  void Move(Vec2i d);
  void Render(bool selected);

  void OnRightClicked(rwElement *parent);
  static void OnTextEntered(ComponentGroup *caller, rwTextBoxEvent &e);

  void ToFile(std::ostream &out);
  static ComponentGroup *FromFile(std::istream &in, ComponentBoard *host);
};

#endif
