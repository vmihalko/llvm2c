; ModuleID = 'function-ptr.c'
source_filename = "function-ptr.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

@func_ptr = dso_local global i32 (i32, i32)* null, align 8, !dbg !0

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @add(i32 noundef %0, i32 noundef %1) #0 !dbg !15 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  call void @llvm.dbg.declare(metadata i32* %3, metadata !17, metadata !DIExpression()), !dbg !18
  store i32 %1, i32* %4, align 4
  call void @llvm.dbg.declare(metadata i32* %4, metadata !19, metadata !DIExpression()), !dbg !20
  %5 = load i32, i32* %3, align 4, !dbg !21
  %6 = load i32, i32* %4, align 4, !dbg !22
  %7 = add nsw i32 %5, %6, !dbg !23
  ret i32 %7, !dbg !24
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !25 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 (i32, i32)* @add, i32 (i32, i32)** @func_ptr, align 8, !dbg !28
  %2 = load i32 (i32, i32)*, i32 (i32, i32)** @func_ptr, align 8, !dbg !29
  %3 = call i32 %2(i32 noundef 2, i32 noundef 3), !dbg !30
  ret i32 %3, !dbg !31
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!9, !10, !11, !12, !13}
!llvm.ident = !{!14}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "func_ptr", scope: !2, file: !3, line: 1, type: !5, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 14.0.5 (Fedora 14.0.5-2.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !4, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "function-ptr.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/types_debuginfo/derived_types")
!4 = !{!0}
!5 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !6, size: 64)
!6 = !DISubroutineType(types: !7)
!7 = !{!8, !8, !8}
!8 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!9 = !{i32 7, !"Dwarf Version", i32 4}
!10 = !{i32 2, !"Debug Info Version", i32 3}
!11 = !{i32 1, !"wchar_size", i32 4}
!12 = !{i32 7, !"uwtable", i32 1}
!13 = !{i32 7, !"frame-pointer", i32 2}
!14 = !{!"clang version 14.0.5 (Fedora 14.0.5-2.fc36)"}
!15 = distinct !DISubprogram(name: "add", scope: !3, file: !3, line: 4, type: !6, scopeLine: 4, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !16)
!16 = !{}
!17 = !DILocalVariable(name: "a", arg: 1, scope: !15, file: !3, line: 4, type: !8)
!18 = !DILocation(line: 4, column: 13, scope: !15)
!19 = !DILocalVariable(name: "b", arg: 2, scope: !15, file: !3, line: 4, type: !8)
!20 = !DILocation(line: 4, column: 20, scope: !15)
!21 = !DILocation(line: 5, column: 12, scope: !15)
!22 = !DILocation(line: 5, column: 16, scope: !15)
!23 = !DILocation(line: 5, column: 14, scope: !15)
!24 = !DILocation(line: 5, column: 5, scope: !15)
!25 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 8, type: !26, scopeLine: 8, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !16)
!26 = !DISubroutineType(types: !27)
!27 = !{!8}
!28 = !DILocation(line: 9, column: 11, scope: !25)
!29 = !DILocation(line: 10, column: 11, scope: !25)
!30 = !DILocation(line: 10, column: 9, scope: !25)
!31 = !DILocation(line: 10, column: 2, scope: !25)
