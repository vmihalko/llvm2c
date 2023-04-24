; ModuleID = 'arr5dim.c'
source_filename = "arr5dim.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

@a = dso_local global [1 x [2 x [3 x [4 x [5 x i32]]]]] zeroinitializer, align 16, !dbg !0

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !19 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %2 = load i32, i32* getelementptr inbounds ([1 x [2 x [3 x [4 x [5 x i32]]]]], [1 x [2 x [3 x [4 x [5 x i32]]]]]* @a, i64 0, i64 0, i64 0, i64 0, i64 0, i64 0), align 16, !dbg !23
  ret i32 %2, !dbg !24
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!13, !14, !15, !16, !17}
!llvm.ident = !{!18}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "a", scope: !2, file: !3, line: 1, type: !5, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 14.0.5 (Fedora 14.0.5-2.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !4, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "arr5dim.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/types_debuginfo/composite_types")
!4 = !{!0}
!5 = !DICompositeType(tag: DW_TAG_array_type, baseType: !6, size: 3840, elements: !7)
!6 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!7 = !{!8, !9, !10, !11, !12}
!8 = !DISubrange(count: 1)
!9 = !DISubrange(count: 2)
!10 = !DISubrange(count: 3)
!11 = !DISubrange(count: 4)
!12 = !DISubrange(count: 5)
!13 = !{i32 7, !"Dwarf Version", i32 4}
!14 = !{i32 2, !"Debug Info Version", i32 3}
!15 = !{i32 1, !"wchar_size", i32 4}
!16 = !{i32 7, !"uwtable", i32 1}
!17 = !{i32 7, !"frame-pointer", i32 2}
!18 = !{!"clang version 14.0.5 (Fedora 14.0.5-2.fc36)"}
!19 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 2, type: !20, scopeLine: 2, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !22)
!20 = !DISubroutineType(types: !21)
!21 = !{!6}
!22 = !{}
!23 = !DILocation(line: 3, column: 9, scope: !19)
!24 = !DILocation(line: 3, column: 2, scope: !19)
