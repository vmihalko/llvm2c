; ModuleID = 'l.c'
source_filename = "l.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !8 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata i32* %2, metadata !13, metadata !DIExpression()), !dbg !14
  store i32 5, i32* %2, align 4, !dbg !14
  %3 = load i32, i32* %2, align 4, !dbg !15
  %4 = icmp ne i32 %3, 0, !dbg !15
  br i1 %4, label %5, label %13, !dbg !17

5:                                                ; preds = %0
  br label %6, !dbg !18

6:                                                ; preds = %9, %5
  %7 = load i32, i32* %2, align 4, !dbg !20
  %8 = icmp ne i32 %7, 0, !dbg !18
  br i1 %8, label %9, label %12, !dbg !18

9:                                                ; preds = %6
  %10 = load i32, i32* %2, align 4, !dbg !21
  %11 = add nsw i32 %10, -1, !dbg !21
  store i32 %11, i32* %2, align 4, !dbg !21
  br label %6, !dbg !18, !llvm.loop !22

12:                                               ; preds = %6
  br label %14, !dbg !24

13:                                               ; preds = %0
  store i32 3, i32* %2, align 4, !dbg !25
  br label %14

14:                                               ; preds = %13, %12
  %15 = load i32, i32* %2, align 4, !dbg !27
  ret i32 %15, !dbg !28
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 14.0.5 (Fedora 14.0.5-1.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "l.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/loop")
!2 = !{i32 7, !"Dwarf Version", i32 4}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 7, !"uwtable", i32 1}
!6 = !{i32 7, !"frame-pointer", i32 2}
!7 = !{!"clang version 14.0.5 (Fedora 14.0.5-1.fc36)"}
!8 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 1, type: !9, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !12)
!9 = !DISubroutineType(types: !10)
!10 = !{!11}
!11 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!12 = !{}
!13 = !DILocalVariable(name: "a", scope: !8, file: !1, line: 2, type: !11)
!14 = !DILocation(line: 2, column: 6, scope: !8)
!15 = !DILocation(line: 3, column: 7, scope: !16)
!16 = distinct !DILexicalBlock(scope: !8, file: !1, line: 3, column: 7)
!17 = !DILocation(line: 3, column: 7, scope: !8)
!18 = !DILocation(line: 4, column: 3, scope: !19)
!19 = distinct !DILexicalBlock(scope: !16, file: !1, line: 3, column: 11)
!20 = !DILocation(line: 4, column: 10, scope: !19)
!21 = !DILocation(line: 5, column: 5, scope: !19)
!22 = distinct !{!22, !18, !21, !23}
!23 = !{!"llvm.loop.mustprogress"}
!24 = !DILocation(line: 6, column: 2, scope: !19)
!25 = !DILocation(line: 7, column: 5, scope: !26)
!26 = distinct !DILexicalBlock(scope: !16, file: !1, line: 6, column: 9)
!27 = !DILocation(line: 9, column: 9, scope: !8)
!28 = !DILocation(line: 9, column: 2, scope: !8)
