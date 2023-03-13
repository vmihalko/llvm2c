; ModuleID = 'twoptr-in-main.c'
source_filename = "twoptr-in-main.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !8 {
  %1 = alloca i32, align 4
  %2 = alloca i32*, align 8
  %3 = alloca i32**, align 8
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata i32** %2, metadata !13, metadata !DIExpression()), !dbg !15
  call void @llvm.dbg.declare(metadata i32*** %3, metadata !16, metadata !DIExpression()), !dbg !18
  %4 = load i32*, i32** %2, align 8, !dbg !19
  store i32 10, i32* %4, align 4, !dbg !20
  %5 = load i32**, i32*** %3, align 8, !dbg !21
  %6 = load i32*, i32** %5, align 8, !dbg !22
  store i32 5, i32* %6, align 4, !dbg !23
  %7 = load i32*, i32** %2, align 8, !dbg !24
  %8 = load i32, i32* %7, align 4, !dbg !25
  %9 = load i32**, i32*** %3, align 8, !dbg !26
  %10 = load i32*, i32** %9, align 8, !dbg !27
  %11 = load i32, i32* %10, align 4, !dbg !28
  %12 = add nsw i32 %8, %11, !dbg !29
  ret i32 %12, !dbg !30
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 14.0.5 (Fedora 14.0.5-2.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "twoptr-in-main.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/types_debuginfo/derived_types")
!2 = !{i32 7, !"Dwarf Version", i32 4}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 7, !"uwtable", i32 1}
!6 = !{i32 7, !"frame-pointer", i32 2}
!7 = !{!"clang version 14.0.5 (Fedora 14.0.5-2.fc36)"}
!8 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 1, type: !9, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !12)
!9 = !DISubroutineType(types: !10)
!10 = !{!11}
!11 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!12 = !{}
!13 = !DILocalVariable(name: "a", scope: !8, file: !1, line: 2, type: !14)
!14 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64)
!15 = !DILocation(line: 2, column: 14, scope: !8)
!16 = !DILocalVariable(name: "b", scope: !8, file: !1, line: 3, type: !17)
!17 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !14, size: 64)
!18 = !DILocation(line: 3, column: 15, scope: !8)
!19 = !DILocation(line: 4, column: 10, scope: !8)
!20 = !DILocation(line: 4, column: 12, scope: !8)
!21 = !DILocation(line: 5, column: 4, scope: !8)
!22 = !DILocation(line: 5, column: 3, scope: !8)
!23 = !DILocation(line: 5, column: 6, scope: !8)
!24 = !DILocation(line: 6, column: 17, scope: !8)
!25 = !DILocation(line: 6, column: 16, scope: !8)
!26 = !DILocation(line: 6, column: 23, scope: !8)
!27 = !DILocation(line: 6, column: 22, scope: !8)
!28 = !DILocation(line: 6, column: 21, scope: !8)
!29 = !DILocation(line: 6, column: 19, scope: !8)
!30 = !DILocation(line: 6, column: 9, scope: !8)
