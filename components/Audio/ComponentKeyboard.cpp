
#include "ComponentKeyboard.h"

#include <iostream>
#include <string>

// Frequencies from C4 to B4
double noteFreqs[] =
{
  261.63, // C
  277.18, // C#
  293.66, // D
  311.13, // D#
  329.63, // E
  349.23, // F
  369.99, // F#
  392.00, // G
  415.30, // G#
  440.00, // A
  466.16, // A#
  493.88  // B
};

struct sNoteKey
{
  char key;
  double freq;
};

sNoteKey noteKeys[] =
{
  {'Z', noteFreqs[ 0] * 1},
  {'S', noteFreqs[ 1] * 1},
  {'X', noteFreqs[ 2] * 1},
  {'D', noteFreqs[ 3] * 1},
  {'C', noteFreqs[ 4] * 1},
  {'V', noteFreqs[ 5] * 1},
  {'G', noteFreqs[ 6] * 1},
  {'B', noteFreqs[ 7] * 1},
  {'H', noteFreqs[ 8] * 1},
  {'N', noteFreqs[ 9] * 1},
  {'J', noteFreqs[10] * 1},
  {'M', noteFreqs[11] * 1},
  {',', noteFreqs[ 0] * 2},
  {'L', noteFreqs[ 1] * 2},
  {'.', noteFreqs[ 2] * 2},
  {';', noteFreqs[ 3] * 2},
  {'/', noteFreqs[ 4] * 2},

  {'Q', noteFreqs[ 0] * 2},
  {'2', noteFreqs[ 1] * 2},
  {'W', noteFreqs[ 2] * 2},
  {'3', noteFreqs[ 3] * 2},
  {'E', noteFreqs[ 4] * 2},
  {'R', noteFreqs[ 5] * 2},
  {'5', noteFreqs[ 6] * 2},
  {'T', noteFreqs[ 7] * 2},
  {'6', noteFreqs[ 8] * 2},
  {'Y', noteFreqs[ 9] * 2},
  {'7', noteFreqs[10] * 2},
  {'U', noteFreqs[11] * 2},
  {'I', noteFreqs[ 0] * 4},
  {'9', noteFreqs[ 1] * 4},
  {'O', noteFreqs[ 2] * 4},
  {'0', noteFreqs[ 3] * 4},
  {'P', noteFreqs[ 4] * 4},
};

const int noteKeyCount = sizeof(noteKeys) / sizeof(sNoteKey);

const sPatchOut blueprintOut[] = {
  sPatchOut(CT_DOUBLE, "Frequency")
};

ComponentKeyboard::ComponentKeyboard()
{
  InitializePatches(
    NULL, 0,
    blueprintOut, sizeof(blueprintOut) / sizeof(sPatchOut)
  );

  m_octaveScale = 1.0;
}

ComponentKeyboard::~ComponentKeyboard()
{
}

bool ComponentKeyboard::ProcessKeyEvent(rwKeyEvent &e)
{
  // On key down, set the frequency based on the pressed key
  if (e.type == rwKEY_DOWN)
  {
    if (e.keycode < 512 && !(e.flags & rwKEY_REPEAT))
    {
      for (int i = 0; i < noteKeyCount; i++)
      {
        if (noteKeys[i].key == (char)e.keycode)
        {
          SetOutput(0, noteKeys[i].freq * m_octaveScale);
          break;
        }
      }
    }

    if (e.keycode == rwK_RIGHT)
      m_octaveScale *= 2.0;
    else if (e.keycode == rwK_LEFT)
      m_octaveScale /= 2.0;
  }
  // On key up, set it to 0
  else if (e.type == rwKEY_UP)
  {
    SetOutput(0, 0.0);

    // and then set it back to any key that is still pressed
    for (int i = noteKeyCount - 1; i >= 0; i--)
    {
      if (m_root->m_keys[noteKeys[i].key])
      {
        SetOutput(0, noteKeys[i].freq * m_octaveScale);
        break;
      }
    }
  }

  return false;
}

void ComponentKeyboard::Evaluate(int timestamp)
{
  UPDATE_TIMESTAMP();
}

std::string ComponentKeyboard::GetNodeType()
{
  std::stringstream t;
  t << "Keyboard";

  return t.str();
}

void ComponentKeyboard::ToFile(std::ostream &out)
{
  out << "Keyboard";
  WriteDefaultParams(out);
}

Component *ComponentKeyboard::FromFile(std::istream &in)
{
  ComponentKeyboard *c = new ComponentKeyboard();
  c->ReadDefaultParams(in);

  return c;
}

Component *ComponentKeyboard::Create()
{
  return new ComponentKeyboard();
}

Component *ComponentKeyboard::Copy()
{
  std::stringstream t;
  ToFile(t);
  return FromFile(t);
}

