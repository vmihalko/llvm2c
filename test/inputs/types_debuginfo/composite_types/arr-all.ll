; ModuleID = 'arr-all.c'
source_filename = "arr-all.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

@global_arrayl = dso_local global [5 x i32] [i32 1, i32 2, i32 3, i32 4, i32 5], align 16, !dbg !0
@global_array = dso_local global [3 x [5 x i32]] [[5 x i32] [i32 1, i32 2, i32 3, i32 0, i32 0], [5 x i32] [i32 4, i32 5, i32 6, i32 0, i32 0], [5 x i32] [i32 7, i32 8, i32 9, i32 0, i32 0]], align 16, !dbg !5
@__const.main.local_array = private unnamed_addr constant [3 x i32] [i32 10, i32 20, i32 30], align 4
@__const.main.local_arlray = private unnamed_addr constant [3 x [5 x i32]] [[5 x i32] [i32 10, i32 11, i32 12, i32 0, i32 0], [5 x i32] [i32 13, i32 14, i32 15, i32 0, i32 0], [5 x i32] [i32 16, i32 17, i32 18, i32 0, i32 0]], align 16

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !20 {
  %1 = alloca i32, align 4
  %2 = alloca [3 x i32], align 4
  %3 = alloca [3 x [5 x i32]], align 16
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata [3 x i32]* %2, metadata !24, metadata !DIExpression()), !dbg !27
  %4 = bitcast [3 x i32]* %2 to i8*, !dbg !27
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 4 %4, i8* align 4 bitcast ([3 x i32]* @__const.main.local_array to i8*), i64 12, i1 false), !dbg !27
  call void @llvm.dbg.declare(metadata [3 x [5 x i32]]* %3, metadata !28, metadata !DIExpression()), !dbg !29
  %5 = bitcast [3 x [5 x i32]]* %3 to i8*, !dbg !29
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %5, i8* align 16 bitcast ([3 x [5 x i32]]* @__const.main.local_arlray to i8*), i64 60, i1 false), !dbg !29
  ret i32 0, !dbg !30
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: argmemonly nofree nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { argmemonly nofree nounwind willreturn }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!14, !15, !16, !17, !18}
!llvm.ident = !{!19}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "global_arrayl", scope: !2, file: !3, line: 4, type: !12, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 14.0.5 (Fedora 14.0.5-2.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !4, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "arr-all.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/types_debuginfo/composite_types")
!4 = !{!0, !5}
!5 = !DIGlobalVariableExpression(var: !6, expr: !DIExpression())
!6 = distinct !DIGlobalVariable(name: "global_array", scope: !2, file: !3, line: 6, type: !7, isLocal: false, isDefinition: true)
!7 = !DICompositeType(tag: DW_TAG_array_type, baseType: !8, size: 480, elements: !9)
!8 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!9 = !{!10, !11}
!10 = !DISubrange(count: 3)
!11 = !DISubrange(count: 5)
!12 = !DICompositeType(tag: DW_TAG_array_type, baseType: !8, size: 160, elements: !13)
!13 = !{!11}
!14 = !{i32 7, !"Dwarf Version", i32 4}
!15 = !{i32 2, !"Debug Info Version", i32 3}
!16 = !{i32 1, !"wchar_size", i32 4}
!17 = !{i32 7, !"uwtable", i32 1}
!18 = !{i32 7, !"frame-pointer", i32 2}
!19 = !{!"clang version 14.0.5 (Fedora 14.0.5-2.fc36)"}
!20 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 12, type: !21, scopeLine: 12, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !23)
!21 = !DISubroutineType(types: !22)
!22 = !{!8}
!23 = !{}
!24 = !DILocalVariable(name: "local_array", scope: !20, file: !3, line: 14, type: !25)
!25 = !DICompositeType(tag: DW_TAG_array_type, baseType: !8, size: 96, elements: !26)
!26 = !{!10}
!27 = !DILocation(line: 14, column: 9, scope: !20)
!28 = !DILocalVariable(name: "local_arlray", scope: !20, file: !3, line: 16, type: !7)
!29 = !DILocation(line: 16, column: 9, scope: !20)
!30 = !DILocation(line: 21, column: 5, scope: !20)