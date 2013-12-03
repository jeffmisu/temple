
#ifndef INTERFACE_H
#define INTERFACE_H

#include <vector>
#include <string>
#include "FontRenderer.h"
#include "Utility.h"

// Here's my implementation of a GUI system. It's not terribly pretty, but it's
// terribly easy for me to use because I wrote it for what I want to do. The
// 'rw' stands for 'reinvented wheel'.

// Don't mind this
struct rwFunctor
{
  void (*func)();
  void *caller;

  rwFunctor(void (*f)(), void *c):
    func(f),
    caller(c)
  {
  };
};

#define rwFUNCTOR(x) rwFunctor((void (*)())x, (void *)this)

//-----Input things-----

enum
{
  // Mouse event types
  rwMOUSE_BTN_DOWN,
  rwMOUSE_BTN_UP,
  rwMOUSE_MOVE,
  // Mouse event flags
  rwMOUSE_LEFT = 1,
  rwMOUSE_RIGHT = 2,
  rwMOUSE_MIDDLE = 4,

  // Key event types
  rwKEY_DOWN,
  rwKEY_UP,
  // Key event flags
  rwKEY_REPEAT = 1,
  rwKEY_SHIFT_HELD = 2,
  rwKEY_CTRL_HELD = 4,
  rwKEY_ALT_HELD = 8,
};

enum
{
  rwK_START = 128,
  rwK_UP = 315, rwK_DOWN = 317, rwK_LEFT = 314, rwK_RIGHT = 316,
  rwK_SPACE = 32, rwK_BACKSPACE = 8, rwK_DELETE = 127, rwK_RETURN = 13,
  rwK_ESCAPE = 27, rwK_CONTROL = 308,
};

struct rwMouseEvent
{
  Vec2i pos;
  int type;
  int flags;
};

struct rwKeyEvent
{
  int type;
  int flags;
  int keycode;
};

//-----rwRoot-----
// There should be one root per 'interface system' - an independent set of
// interface elements. The root stores the mouse position, the key states, etc.
// for convenient access by its children.

class rwElement;

class rwRoot
{
  protected:
  rwElement *m_child;

  public:
  rwRoot();
  rwRoot(FontRenderer *font);
  ~rwRoot();

  FontRenderer *m_font;
  char m_keys[512];
  Vec2i m_mOrigin;
  Vec2i m_mPosition;
  char m_mBtnStates;

  void SetChild(rwElement *e);
  rwElement *GetChild();

  // The following functions are called recursively on children
  void Update();
  void Render();
  bool ProcessMouseEvent(rwMouseEvent &e);
  bool ProcessKeyEvent(rwKeyEvent &e);
};

//-----rwElement-----
// The base class for everything in the RW interface

class rwElement
{
  protected:
  rwRoot *m_root;
  rwElement *m_parent;
  std::vector<rwElement *> m_children;
  Vec2i m_size;

  // When dead, this element will be destroyed by its parent during Update()
  bool m_dead;

  public:
  rwElement(rwElement *parent = NULL, Vec2i pos = Vec2i::zero,
    Vec2i size = Vec2i::zero);
  virtual ~rwElement();

  Vec2i m_position;

  virtual void Move(Vec2i d);

  void AddChild(rwElement *c);
  void RemoveChild(rwElement *c);
  void SetParent(rwElement *p);
  void SetRoot(rwRoot *r);

  // The following functions are called recursively on children
  virtual void Update();
  virtual void Render();
  virtual bool ProcessMouseEvent(rwMouseEvent &e);
  virtual bool ProcessKeyEvent(rwKeyEvent &e);

  friend class rwRoot;
};

//-----rwContextMenu-----

enum
{
  // Context menu event types
  rwCONTEXT_SELECT, // A successful selection
  rwCONTEXT_CLOSE,  // Closing the menu (clicking outside it, usually)
};

struct rwContextMenuEvent
{
  Vec2i pos;
  int type;
  int index;
  void *data;
};

typedef void (*rwContextMenuCallback)(void *caller, rwContextMenuEvent &);

class rwContextMenu: public rwElement
{
  protected:
  int m_choiceHeight;
  int m_activeChoice;
  std::vector<std::string> m_choiceNames;
  std::vector<void *> m_choiceData;
  rwFunctor m_callback;

  public:
  rwContextMenu(Vec2i pos, rwElement *parent = NULL);
  rwContextMenu(Vec2i pos, rwFunctor callback, rwElement *parent = NULL);
  ~rwContextMenu();

  void AddChoice(std::string name, void *data);

  void Render();
  bool ProcessMouseEvent(rwMouseEvent &e);
};

//-----rwTextBox-----

enum
{
  // Text box event types
  rwTEXT_RETURN,  // The user pressed enter
  rwTEXT_ESCAPE,  // The user pressed escape
};

struct rwTextBoxEvent
{
  int type;
  std::string text;
};

typedef void (*rwTextBoxCallback)(void *caller, rwTextBoxEvent &);

class rwTextBox: public rwElement
{
  protected:
  std::string m_text;
  rwFunctor m_callback;
  bool m_filter[512];
  int m_maxChars;
  int m_cursorIndex;

  void SetDefaultFilter();

  public:
  rwTextBox(Vec2i pos, rwElement *parent = NULL);
  rwTextBox(Vec2i pos, rwFunctor callback, rwElement *parent = NULL);
  ~rwTextBox();

  void SetKeyFilter(int code, bool state);
  void SetNumericFilter();
  void SetAlphanumericFilter();
  void SetMaxChars(int maxChars);
  void SetText(std::string text);

  void Render();
  bool ProcessKeyEvent(rwKeyEvent &e);
};

//-----rwButton-----

enum
{
  // Button event types
  rwBTN_CLICKED,   // The user clicked the button!

  // Button types
  rwBTN_ROUND,
  rwBTN_RECT,
};

struct rwButtonEvent
{
  int type;
};

typedef void (*rwButtonCallback)(void *caller, rwButtonEvent &);

class rwButton: public rwElement
{
  protected:
  rwFunctor m_callback;
  std::string m_label;
  int m_type;

  public:
  // Round button constructor
  rwButton(Vec2i pos, float radius, rwFunctor callback, std::string label = "",
    rwElement *parent = NULL);
  // Rectangular button constructor
  rwButton(Vec2i pos, Vec2i size, rwFunctor callback, std::string label = "",
    rwElement *parent = NULL);
  ~rwButton();

  void Render();
  bool ProcessMouseEvent(rwMouseEvent &e);
};

#endif
