#pragma once

#include "coreir.h"
// This will recusrively run all the generators and replace module definitions
// For every instance, if it is a generator, it 

namespace CoreIR {
namespace Passes {


class CullGraph : public ContextPass {
  bool nocoreir;
  public :
    static std::string ID;
    CullGraph(bool nocoreir) : ContextPass(ID + (nocoreir ? "" : "-withcoreir"),"removes all extraneous modules/generators/typegens from context"), nocoreir(nocoreir) {}
    bool runOnContext(Context* c);
};

}
}
