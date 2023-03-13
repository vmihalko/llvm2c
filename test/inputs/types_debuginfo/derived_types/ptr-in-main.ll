; ModuleID = 'ptr-in-main.c'
source_filename = "ptr-in-main.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !8 {
  %1 = alloca i32, align 4
  %2 = alloca i32**, align 8
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata i32*** %2, metadata !13, metadata !DIExpression()), !dbg !16
  %3 = load i32**, i32*** %2, align 8, !dbg !17
  %4 = load i32*, i32** %3, align 8, !dbg !18
  store i32 10, i32* %4, align 4, !dbg !19
  %5 = load i32**, i32*** %2, align 8, !dbg !20
  %6 = load i32*, i32** %5, align 8, !dbg !21
  %7 = load i32, i32* %6, align 4, !dbg !22
  ret i32 %7, !dbg !23
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 14.0.5 (Fedora 14.0.5-2.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "ptr-in-main.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/types_debuginfo/derived_types")
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
!14 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !15, size: 64)
!15 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64)
!16 = !DILocation(line: 2, column: 15, scope: !8)
!17 = !DILocation(line: 3, column: 11, scope: !8)
!18 = !DILocation(line: 3, column: 10, scope: !8)
!19 = !DILocation(line: 3, column: 13, scope: !8)
!20 = !DILocation(line: 4, column: 18, scope: !8)
!21 = !DILocation(line: 4, column: 17, scope: !8)
!22 = !DILocation(line: 4, column: 16, scope: !8)
!23 = !DILocation(line: 4, column: 9, scope: !8)
