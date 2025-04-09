#include "RArch.h"
#include "clang/Basic/Builtins.h"
#include "clang/Basic/MacroBuilder.h"
#include "clang/Basic/TargetBuiltins.h"

using namespace clang;
using namespace clang::targets;

static constexpr llvm::StringTable BuiltinStrings =
    CLANG_BUILTIN_STR_TABLE_START
#define BUILTIN CLANG_BUILTIN_STR_TABLE
#define TARGET_BUILTIN CLANG_TARGET_BUILTIN_STR_TABLE
#define TARGET_HEADER_BUILTIN CLANG_TARGET_HEADER_BUILTIN_STR_TABLE
#include "clang/Basic/BuiltinsRArch.def"
;

static constexpr int NumRArchBuiltins = clang::RArch::LastTSBuiltin - Builtin::FirstTSBuiltin;

static constexpr auto BuiltinInfos = Builtin::MakeInfos<NumRArchBuiltins>({
#define BUILTIN CLANG_BUILTIN_ENTRY
#define LANGBUILTIN CLANG_LANGBUILTIN_ENTRY
#define LIBBUILTIN CLANG_LIBBUILTIN_ENTRY
#define TARGET_BUILTIN CLANG_TARGET_BUILTIN_ENTRY
#define TARGET_HEADER_BUILTIN CLANG_TARGET_HEADER_BUILTIN_ENTRY
#include "clang/Basic/BuiltinsRArch.def"
});

void RArchTargetInfo::getTargetDefines(const LangOptions &Opts,
                                     MacroBuilder &Builder) const {
  Builder.defineMacro("__rarch__");
}

llvm::SmallVector<Builtin::InfosShard> RArchTargetInfo::getTargetBuiltins() const {
  return {{&BuiltinStrings, BuiltinInfos}};
}
