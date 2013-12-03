
#ifndef COMPONENTGLOBALS_H
#define COMPONENTGLOBALS_H

#include <string>
#include <sstream>
#include "Utility.h"

enum ComponentType
{
  CT_NONE = 0,
  CT_DOUBLE,
  CT_BOOL
};

struct sTypeInfo
{
  std::string name;
  Vec3f color;
};

extern sTypeInfo typeInfo[];

inline void assignDefaultValue(void *value, ComponentType type)
{
  if      (type == CT_DOUBLE)
    *(double *)value = 0.0;
  else if (type == CT_BOOL)
    *(bool *)value = false;
}

inline void assignValue(void *dst, void *src, ComponentType type)
{
  if      (type == CT_DOUBLE)
    *(double *)dst = *(double *)src;
  else if (type == CT_BOOL)
    *(bool *)dst = *(bool *)src;
}

inline std::string getValueString(void *value, ComponentType type)
{
  std::stringstream s;
  if      (type == CT_DOUBLE)
    s << *(double *)value;
  else if (type == CT_BOOL)
    s << ((*(bool *)value)?"True":"False");
  else
    s << "Nothing";
  return s.str();
}

inline void *newValue(ComponentType type)
{
  if      (type == CT_DOUBLE)
    return new double;
  else if (type == CT_BOOL)
    return new bool;
  return NULL;
}

inline void deleteValue(void *value, ComponentType type)
{
  if      (type == CT_DOUBLE)
    delete (double *)value;
  else if (type == CT_BOOL)
    delete (bool *)value;
  value = NULL;
}

class Component;
Component *loadFromFile(std::istream &in);

#endif
