// All intrinsics start with "llvm.rarch.".
let TargetPrefix = "rarch" in {
 
 def int_rarch_drawcircle: ClangBuiltin<"__builtin_rarch_draw_circle">,
   Intrinsic<[], [llvm_anyptr_ty], [IntrNoMem, IntrHasSideEffects]>;
 
 def int_rarch_flush: ClangBuiltin<"__builtin_rarch_flush">,
   Intrinsic<[], [], [IntrNoMem, IntrHasSideEffects]>;
 
 def int_rarch_fillscreen: ClangBuiltin<"__builtin_rarch_fill">,
   Intrinsic<[], [llvm_i64_ty], [IntrNoMem, IntrHasSideEffects]>;

 } // TargetPrefix = "rarch"
