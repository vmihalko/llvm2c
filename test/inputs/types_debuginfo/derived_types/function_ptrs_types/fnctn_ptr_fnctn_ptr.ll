; ModuleID = 'fnctn_ptr_fnctn_ptr.c'
source_filename = "fnctn_ptr_fnctn_ptr.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @myFunc(i32 (i32)* noundef %0) #0 !dbg !8 {
  %2 = alloca i32 (i32)*, align 8
  store i32 (i32)* %0, i32 (i32)** %2, align 8
  call void @llvm.dbg.declare(metadata i32 (i32)** %2, metadata !16, metadata !DIExpression()), !dbg !17
  %3 = load i32 (i32)*, i32 (i32)** %2, align 8, !dbg !18
  %4 = call i32 %3(i32 noundef 42), !dbg !18
  ret void, !dbg !19
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @printInt(i32 noundef %0) #0 !dbg !20 {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  call void @llvm.dbg.declare(metadata i32* %2, metadata !21, metadata !DIExpression()), !dbg !22
  %3 = load i32, i32* %2, align 4, !dbg !23
  ret i32 %3, !dbg !24
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !25 {
  %1 = alloca i32, align 4
  %2 = alloca void (i32 (i32)*)*, align 8
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata void (i32 (i32)*)** %2, metadata !28, metadata !DIExpression()), !dbg !33
  store void (i32 (i32)*)* @myFunc, void (i32 (i32)*)** %2, align 8, !dbg !33
  %3 = load void (i32 (i32)*)*, void (i32 (i32)*)** %2, align 8, !dbg !34
  call void %3(i32 (i32)* noundef @printInt), !dbg !34
  ret i32 0, !dbg !35
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 14.0.5 (Fedora 14.0.5-2.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "fnctn_ptr_fnctn_ptr.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/types_debuginfo/derived_types/function_ptrs_types")
!2 = !{i32 7, !"Dwarf Version", i32 4}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 7, !"uwtable", i32 1}
!6 = !{i32 7, !"frame-pointer", i32 2}
!7 = !{!"clang version 14.0.5 (Fedora 14.0.5-2.fc36)"}
!8 = distinct !DISubprogram(name: "myFunc", scope: !1, file: !1, line: 2, type: !9, scopeLine: 2, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!9 = !DISubroutineType(types: !10)
!10 = !{null, !11}
!11 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !12, size: 64)
!12 = !DISubroutineType(types: !13)
!13 = !{!14, !14}
!14 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!15 = !{}
!16 = !DILocalVariable(name: "funcPtr", arg: 1, scope: !8, file: !1, line: 2, type: !11)
!17 = !DILocation(line: 2, column: 19, scope: !8)
!18 = !DILocation(line: 4, column: 5, scope: !8)
!19 = !DILocation(line: 5, column: 1, scope: !8)
!20 = distinct !DISubprogram(name: "printInt", scope: !1, file: !1, line: 11, type: !12, scopeLine: 11, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!21 = !DILocalVariable(name: "num", arg: 1, scope: !20, file: !1, line: 11, type: !14)
!22 = !DILocation(line: 11, column: 18, scope: !20)
!23 = !DILocation(line: 12, column: 12, scope: !20)
!24 = !DILocation(line: 12, column: 5, scope: !20)
!25 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 15, type: !26, scopeLine: 15, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!26 = !DISubroutineType(types: !27)
!27 = !{!14}
!28 = !DILocalVariable(name: "myFuncPtr", scope: !25, file: !1, line: 17, type: !29)
!29 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !30, size: 64)
!30 = !DISubroutineType(types: !31)
!31 = !{null, !32}
!32 = !DIDerivedType(tag: DW_TAG_typedef, name: "funcPtrType", file: !1, line: 8, baseType: !11)
!33 = !DILocation(line: 17, column: 12, scope: !25)
!34 = !DILocation(line: 20, column: 5, scope: !25)
!35 = !DILocation(line: 22, column: 5, scope: !25)
