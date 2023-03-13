; ModuleID = 'for.c'
source_filename = "for.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !8 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata i32* %2, metadata !13, metadata !DIExpression()), !dbg !15
  store i32 0, i32* %2, align 4, !dbg !15
  br label %3, !dbg !16

3:                                                ; preds = %10, %0
  %4 = load i32, i32* %2, align 4, !dbg !17
  %5 = icmp slt i32 %4, 10, !dbg !19
  br i1 %5, label %6, label %13, !dbg !20

6:                                                ; preds = %3
  %7 = load i32, i32* %2, align 4, !dbg !21
  %8 = add nsw i32 %7, 48, !dbg !23
  %9 = call i32 @putchar(i32 noundef %8), !dbg !24
  br label %10, !dbg !25

10:                                               ; preds = %6
  %11 = load i32, i32* %2, align 4, !dbg !26
  %12 = add nsw i32 %11, 1, !dbg !26
  store i32 %12, i32* %2, align 4, !dbg !26
  br label %3, !dbg !27, !llvm.loop !28

13:                                               ; preds = %3
  %14 = load i32, i32* %1, align 4, !dbg !31
  ret i32 %14, !dbg !31
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare dso_local i32 @putchar(i32 noundef) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 14.0.5 (Fedora 14.0.5-1.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "for.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/loop")
!2 = !{i32 7, !"Dwarf Version", i32 4}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 7, !"uwtable", i32 1}
!6 = !{i32 7, !"frame-pointer", i32 2}
!7 = !{!"clang version 14.0.5 (Fedora 14.0.5-1.fc36)"}
!8 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 3, type: !9, scopeLine: 3, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !12)
!9 = !DISubroutineType(types: !10)
!10 = !{!11}
!11 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!12 = !{}
!13 = !DILocalVariable(name: "i", scope: !14, file: !1, line: 4, type: !11)
!14 = distinct !DILexicalBlock(scope: !8, file: !1, line: 4, column: 2)
!15 = !DILocation(line: 4, column: 10, scope: !14)
!16 = !DILocation(line: 4, column: 6, scope: !14)
!17 = !DILocation(line: 4, column: 17, scope: !18)
!18 = distinct !DILexicalBlock(scope: !14, file: !1, line: 4, column: 2)
!19 = !DILocation(line: 4, column: 18, scope: !18)
!20 = !DILocation(line: 4, column: 2, scope: !14)
!21 = !DILocation(line: 5, column: 11, scope: !22)
!22 = distinct !DILexicalBlock(scope: !18, file: !1, line: 4, column: 28)
!23 = !DILocation(line: 5, column: 12, scope: !22)
!24 = !DILocation(line: 5, column: 3, scope: !22)
!25 = !DILocation(line: 6, column: 2, scope: !22)
!26 = !DILocation(line: 4, column: 24, scope: !18)
!27 = !DILocation(line: 4, column: 2, scope: !18)
!28 = distinct !{!28, !20, !29, !30}
!29 = !DILocation(line: 6, column: 2, scope: !14)
!30 = !{!"llvm.loop.mustprogress"}
!31 = !DILocation(line: 7, column: 1, scope: !8)
