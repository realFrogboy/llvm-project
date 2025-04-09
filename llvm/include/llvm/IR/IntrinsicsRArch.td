let TargetPrefix = "sim" in {  // All intrinsics start with "llvm.sim.".

def int_draw_circle:
 	Intrinsic<[], [llvm_ptr_ty], [IntrNoMem, IntrHasSideEffects]>;

def int_flush:
 	Intrinsic<[], [], [IntrNoMem, IntrHasSideEffects]>;

def int_fillscreen:
 	Intrinsic<[], [], [IntrNoMem, IntrHasSideEffects]>;

} // TargetPrefix = "sim"
