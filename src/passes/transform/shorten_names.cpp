#include "coreir.h"
#include "coreir/passes/transform/shorten_names.h"

using namespace std;
using namespace CoreIR;


void Passes::ShortenNames::initialize(int argc, char** argv) {
  
  
  auto c = this->getContext();
  //Create a new namespace for shorten
  
  //Find an unused namespace
  int n = 0;
  string nsname = "";
  do {
    nsname = "n" + to_string(n);
    if (!c->hasNamespace(nsname)) {
      break;
    }
    n++;
  } while(true);
  this->ns = c->newNamespace(nsname);
}

bool Passes::ShortenNames::finalize() {
  auto c = this->getContext();
  auto top = c->getTop();
  auto newTop = this->mmap[top];
  c->setTop(newTop);
  return true;
}

bool Passes::ShortenNames::runOnInstanceGraphNode(InstanceGraphNode& node) {
    auto c = getContext();
    Module* m = node.getModule();
    
    //Cannot change the name of a module with no def (for now)
    if (!m->hasDef()) {
      this->mmap[m] = m;
      return false;
    }
    
    //Create a new module with the same type
    string mname = "";
    if (m == c->getTop()) {
      mname = m->getName();
    }
    else {
      mname = "m" + to_string(this->mcnt++);
    }
    auto newmod = this->ns->newModuleDecl(mname, m->getType(), m->getModParams());
    auto newdef = newmod->newModuleDef();

    auto def = m->getDef();

    //Add all instances to new module
    map<string, string> imap;
    imap["self"] = "self";
    int i = 0;
    for (auto ipair : def->getInstances()) {
      auto iname = ipair.first;
      auto inst = ipair.second;
      string ireplace = "i"+to_string(i++); 
      imap[iname] = ireplace;
      auto imod = inst->getModuleRef();
      ASSERT(this->mmap.count(imod) > 0, "Internal error");
      auto mreplace = this->mmap[imod];
      //create new instance in the new def
      newdef->addInstance(ireplace,mreplace,inst->getModArgs());
    }

    //Make all connections in new def
    for (auto conn : def->getConnections()) {
      auto spa = conn.first->getSelectPath();
      auto spb = conn.second->getSelectPath();
      spa[0] = imap[spa[0]];
      spb[0] = imap[spb[0]];
      newdef->connect(spa, spb);
    }
    
    newmod->setDef(newdef);
    this->mmap[m] = newmod;
    return true;
}
