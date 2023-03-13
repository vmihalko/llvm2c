; ModuleID = 'ptr-out-main.c'
source_filename = "ptr-out-main.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

@a = dso_local global i32** null, align 8, !dbg !0

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !14 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %2 = load i32**, i32*** @a, align 8, !dbg !18
  %3 = load i32*, i32** %2, align 8, !dbg !19
  store i32 10, i32* %3, align 4, !dbg !20
  %4 = load i32**, i32*** @a, align 8, !dbg !21
  %5 = load i32*, i32** %4, align 8, !dbg !22
  %6 = load i32, i32* %5, align 4, !dbg !23
  ret i32 %6, !dbg !24
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!8, !9, !10, !11, !12}
!llvm.ident = !{!13}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "a", scope: !2, file: !3, line: 1, type: !5, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 14.0.5 (Fedora 14.0.5-2.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !4, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "ptr-out-main.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/types_debuginfo/derived_types")
!4 = !{!0}
!5 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !6, size: 64)
!6 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !7, size: 64)
!7 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!8 = !{i32 7, !"Dwarf Version", i32 4}
!9 = !{i32 2, !"Debug Info Version", i32 3}
!10 = !{i32 1, !"wchar_size", i32 4}
!11 = !{i32 7, !"uwtable", i32 1}
!12 = !{i32 7, !"frame-pointer", i32 2}
!13 = !{!"clang version 14.0.5 (Fedora 14.0.5-2.fc36)"}
!14 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 2, type: !15, scopeLine: 2, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !17)
!15 = !DISubroutineType(types: !16)
!16 = !{!7}
!17 = !{}
!18 = !DILocation(line: 3, column: 11, scope: !14)
!19 = !DILocation(line: 3, column: 10, scope: !14)
!20 = !DILocation(line: 3, column: 13, scope: !14)
!21 = !DILocation(line: 4, column: 18, scope: !14)
!22 = !DILocation(line: 4, column: 17, scope: !14)
!23 = !DILocation(line: 4, column: 16, scope: !14)
!24 = !DILocation(line: 4, column: 9, scope: !14)
