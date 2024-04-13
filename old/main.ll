; ModuleID = 'start'
source_filename = "start"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-mingw32"

define i64 @main() local_unnamed_addr {
top:
  %0 = alloca [5 x {}*], align 8
  %.sub = getelementptr inbounds [5 x {}*], [5 x {}*]* %0, i64 0, i64 0
  store {}* inttoptr (i64 140711604883488 to {}*), {}** %.sub, align 8
  %1 = getelementptr inbounds [5 x {}*], [5 x {}*]* %0, i64 0, i64 1
  store {}* inttoptr (i64 140711620239248 to {}*), {}** %1, align 8
  %2 = getelementptr inbounds [5 x {}*], [5 x {}*]* %0, i64 0, i64 2
  store {}* inttoptr (i64 2458850081088 to {}*), {}** %2, align 8
  %3 = getelementptr inbounds [5 x {}*], [5 x {}*]* %0, i64 0, i64 3
  store {}* inttoptr (i64 2456742609024 to {}*), {}** %3, align 8
  %4 = getelementptr inbounds [5 x {}*], [5 x {}*]* %0, i64 0, i64 4
  store {}* inttoptr (i64 2456742609080 to {}*), {}** %4, align 8
  %5 = call nonnull {}* @ijl_invoke({}* inttoptr (i64 140711627441600 to {}*), {}** nonnull %.sub, i32 5, {}* inttoptr (i64 2458529195008 to {}*))
  %6 = bitcast {}* %5 to i64*
  %unbox = load i64, i64* %6, align 8, !tbaa !2, !alias.scope !8, !noalias !11
  ret i64 %unbox
}

declare nonnull {}* @ijl_invoke({}*, {}** nocapture readonly, i32, {}*) local_unnamed_addr

!llvm.module.flags = !{!0, !1}

!0 = !{i32 2, !"Dwarf Version", i32 4}
!1 = !{i32 2, !"Debug Info Version", i32 3}
!2 = !{!3, !3, i64 0}
!3 = !{!"jtbaa_immut", !4, i64 0}
!4 = !{!"jtbaa_value", !5, i64 0}
!5 = !{!"jtbaa_data", !6, i64 0}
!6 = !{!"jtbaa", !7, i64 0}
!7 = !{!"jtbaa"}
!8 = !{!9}
!9 = !{!"jnoalias_data", !10}
!10 = !{!"jnoalias"}
!11 = !{!12, !13, !14, !15}
!12 = !{!"jnoalias_gcframe", !10}
!13 = !{!"jnoalias_stack", !10}
!14 = !{!"jnoalias_typemd", !10}
!15 = !{!"jnoalias_const", !10}
