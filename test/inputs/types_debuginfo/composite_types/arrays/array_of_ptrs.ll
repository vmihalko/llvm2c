; ModuleID = 'array_of_ptrs.c'
source_filename = "array_of_ptrs.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !8 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca [1 x [2 x i32*]], align 16
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata i32* %2, metadata !13, metadata !DIExpression()), !dbg !14
  store i32 5, i32* %2, align 4, !dbg !14
  call void @llvm.dbg.declare(metadata [1 x [2 x i32*]]* %3, metadata !15, metadata !DIExpression()), !dbg !21
  %4 = getelementptr inbounds [1 x [2 x i32*]], [1 x [2 x i32*]]* %3, i64 0, i64 0, !dbg !22
  %5 = getelementptr inbounds [2 x i32*], [2 x i32*]* %4, i64 0, i64 0, !dbg !23
  store i32* %2, i32** %5, align 8, !dbg !23
  %6 = getelementptr inbounds i32*, i32** %5, i64 1, !dbg !23
  store i32* %2, i32** %6, align 8, !dbg !23
  %7 = getelementptr inbounds [1 x [2 x i32*]], [1 x [2 x i32*]]* %3, i64 0, i64 0, !dbg !24
  %8 = getelementptr inbounds [2 x i32*], [2 x i32*]* %7, i64 0, i64 1, !dbg !24
  %9 = load i32*, i32** %8, align 8, !dbg !24
  %10 = load i32, i32* %9, align 4, !dbg !25
  %11 = sub nsw i32 %10, 5, !dbg !26
  ret i32 %11, !dbg !27
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 14.0.5 (Fedora 14.0.5-2.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "array_of_ptrs.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/types_debuginfo/composite_types")
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
!13 = !DILocalVariable(name: "a", scope: !8, file: !1, line: 2, type: !11)
!14 = !DILocation(line: 2, column: 6, scope: !8)
!15 = !DILocalVariable(name: "array_of_ptr", scope: !8, file: !1, line: 3, type: !16)
!16 = !DICompositeType(tag: DW_TAG_array_type, baseType: !17, size: 128, elements: !18)
!17 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64)
!18 = !{!19, !20}
!19 = !DISubrange(count: 1)
!20 = !DISubrange(count: 2)
!21 = !DILocation(line: 3, column: 7, scope: !8)
!22 = !DILocation(line: 3, column: 28, scope: !8)
!23 = !DILocation(line: 3, column: 29, scope: !8)
!24 = !DILocation(line: 4, column: 11, scope: !8)
!25 = !DILocation(line: 4, column: 10, scope: !8)
!26 = !DILocation(line: 4, column: 31, scope: !8)
!27 = !DILocation(line: 4, column: 2, scope: !8)
