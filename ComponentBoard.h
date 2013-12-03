
#ifndef COMPONENTBOARD_H
#define COMPONENTBOARD_H

#include "FontRenderer.h"
#include "Interface.h"
#include <deque>

class Component;
class ComponentGroup;
class ComponentApp;

// The ComponentBoard is in charge of displaying the component network visually
// and handling input to allow the user to manipulate it.

struct sApplication
{
  void *value;
  ComponentApp *app;
};

class ComponentBoard
{
  protected:
  char m_keys[512];
  Vec2i m_size;
  Vec2i m_mOrigin;
  Vec2i m_mPosition;
  Vec3i m_mDistances;
  char m_mBtnStates;

  // These tell us what we're dragging around with the mouse. For the dragged
  // type, 0 means nothing, 1 means a component, and 2 means a group.
  int m_draggedIndex;
  int m_draggedType;

  FontRenderer *m_font;

  std::vector<ComponentGroup *> m_groups;
  std::vector<Component *> m_comps;
  rwRoot *m_interfaceRoot;

  std::deque<sApplication> m_applications;

  // Looks for a component at the specified position, returning {component
  // index, patch index} or -1 for no valid match (determined per index)
  Vec2i MatchComponentIn(Vec2i pos);
  Vec2i MatchComponentOut(Vec2i pos);

  // As above, but for groups.
  int MatchGroup(Vec2i pos);

  public:
  ComponentBoard();
  ~ComponentBoard();

  void InitRenderer();

  void Resize(Vec2i size);
  Vec2i GetSize() const;
  void ProcessClick(Vec2i pos, int btn);
  void ProcessUnclick(Vec2i pos, int btn);
  void ProcessMotion(Vec2i pos);
  void ProcessKeyDown(int key);
  void ProcessKeyUp(int key);
  void AddComponent(Component *c);

  void PushApps(sApplication app);
  sApplication PopApps();

  FontRenderer *GetFont() const;
  std::vector<Component *> &GetComponents();

  // Creation and callback functions for various context menus
  void CreateCM_InputVar();
  void CreateCM_NewNode();
  static void OnContextMenu_InputVar(ComponentBoard *caller,
    rwContextMenuEvent &e);
  static void OnContextMenu_NewNode(ComponentBoard *caller,
    rwContextMenuEvent &e);

  void Update();
  void Render();

  void Evaluate();

  void CreateVoice(Component *source);
  void Save(std::string filename = "");
  void Open(std::string filename = "");
};

#endif
