#include "answer/Adapter.hh"

#include <dlfcn.h>
#include <stdexcept>

using namespace std;

namespace answer
{

void dlOpen(const std::string& path)
{
  if (!dlopen(path.c_str(), RTLD_GLOBAL | RTLD_NOW))
  {
    throw runtime_error(dlerror());
  }
}

}

