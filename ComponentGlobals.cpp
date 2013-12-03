
#include "ComponentGlobals.h"

#include <fstream>
#include <iostream>
#include "components/ComponentGraph.h"

sTypeInfo typeInfo[] =
{                                                                              
  {"Abyss",   Vec3f(0.3, 0.3, 0.3)},                                           
  {"Real",    Vec3f(0.3, 0.85, 0.4)},                                          
  {"Boolean", Vec3f(0.918, 0.369, 0.380)},                                     
};

Component *loadFromFile(std::istream &in)
{
  std::string type;
  in >> type;

  for (int i = 0; i < cmpTotal; i++)
    if (type == cmpGraph[i].cname)
      return cmpGraph[i].fromFile(in);

  std::cerr << "Malformed save data! I don't know what to do with this..."
    << std::endl;
  return NULL;
}
