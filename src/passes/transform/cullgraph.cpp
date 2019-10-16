#include "coreir.h"
#include "coreir/passes/transform/cullgraph.h"

using namespace std;
using namespace CoreIR;

namespace {
void recurse(Module* m, set<Module*>& mused, set<Generator*>& gused) {
  if (m->isGenerated()) {
    gused.insert(m->getGenerator());
  }
  else {
    mused.insert(m);
  }
  if (!m->hasDef()) return;
  for (auto ipair : m->getDef()->getInstances()) {
    recurse(ipair.second->getModuleRef(),mused,gused);
  }
}
}

string Passes::CullGraph::ID = "cullgraph";
bool Passes::CullGraph::runOnContext(Context* c) {
  if (!c->hasTop()) return false;
  //Find a list of all used Modules and Generators
  set<Module*> mused;
  set<Generator*> gused;
  recurse(c->getTop(),mused,gused);
  //if nocoreir, I need to keep all instances of any coreir definitions
  for (auto mpair : c->getNamespace("coreir")->getModules()) {
    recurse(mpair.second,mused,gused);
  }
  for (auto mpair : c->getNamespace("corebit")->getModules()) {
    recurse(mpair.second,mused,gused);
  }
  set<TypeGen*> tgused;
  for (auto gen : gused) {
    tgused.insert(gen->getTypeGen());
  }

  set<Module*> mErase;
  set<Generator*> gErase;
  set<TypeGen*> tgErase;
  for (auto npair : c->getNamespaces()) {
    auto nsname = npair.first;
    auto ns = npair.second;
    if (nocoreir && (nsname=="coreir" || nsname=="corebit")) {
      continue;
    }
    for (auto mpair : ns->getModules()) {
      if (mused.count(mpair.second)==0 && !mpair.second->isGenerated()) {
        mErase.insert(mpair.second);
      }
    }
    for (auto gpair : ns->getGenerators()) {
      if (gused.count(gpair.second)==0) {
        gErase.insert(gpair.second);
      }
    }
    for (auto tgpair : ns->getTypeGens()) {
      if (tgused.count(tgpair.second)==0) {
        tgErase.insert(tgpair.second);
      }
    }
  }

  //Need to erase in the order Modules, Generators, TypeGens
  for (auto m : mErase) {
    m->getNamespace()->eraseModule(m->getName());
  }
  for (auto g : gErase) {
    g->getNamespace()->eraseGenerator(g->getName());
  }
  for (auto tg : tgErase) {
    tg->getNamespace()->eraseTypeGen(tg->getName());
  }

  return (mErase.size() + gErase.size() + tgErase.size()) > 0;
  
}
