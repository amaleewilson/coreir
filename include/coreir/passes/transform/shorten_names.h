#pragma once

#include "coreir.h"

namespace CoreIR {
namespace Passes {

//Should run 'rungenerators' before this pass
class ShortenNames : public InstanceGraphPass {
  private:
    int mcnt=0;
    std::map<Module*, Module*> mmap;
    Namespace* ns = nullptr;
  public :
    ShortenNames() : InstanceGraphPass("shorten_names","shortens the names of modules and instances") {}
    void initialize(int argc, char** argv) override;
    bool runOnInstanceGraphNode(InstanceGraphNode& node) override;
    bool finalize() override;
};

}
}
