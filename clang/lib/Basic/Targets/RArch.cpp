#include "RArch.h"
#include "clang/Basic/Builtins.h"
#include "clang/Basic/MacroBuilder.h"

using namespace clang;
using namespace clang::targets;

void RArchTargetInfo::getTargetDefines(const LangOptions &Opts,
                                     MacroBuilder &Builder) const {
  Builder.defineMacro("__rarch__");
}

llvm::SmallVector<Builtin::InfosShard> RArchTargetInfo::getTargetBuiltins() const {
  return {};
}
