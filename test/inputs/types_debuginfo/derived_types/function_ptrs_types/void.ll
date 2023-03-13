; ModuleID = 'void.c'
source_filename = "void.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

@voidInt = dso_local global void (i32)* null, align 8, !dbg !0

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @foo(i32 noundef %0) #0 !dbg !15 {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  call void @llvm.dbg.declare(metadata i32* %2, metadata !17, metadata !DIExpression()), !dbg !18
  %3 = load i32, i32* %2, align 4, !dbg !19
  %4 = add nsw i32 %3, 1, !dbg !19
  store i32 %4, i32* %2, align 4, !dbg !19
  ret void, !dbg !20
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !21 {
  store void (i32)* @foo, void (i32)** @voidInt, align 8, !dbg !24
  ret i32 0, !dbg !25
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!9, !10, !11, !12, !13}
!llvm.ident = !{!14}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "voidInt", scope: !2, file: !3, line: 1, type: !5, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 14.0.5 (Fedora 14.0.5-2.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !4, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "void.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/types_debuginfo/derived_types/function_ptrs_types")
!4 = !{!0}
!5 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !6, size: 64)
!6 = !DISubroutineType(types: !7)
!7 = !{null, !8}
!8 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!9 = !{i32 7, !"Dwarf Version", i32 4}
!10 = !{i32 2, !"Debug Info Version", i32 3}
!11 = !{i32 1, !"wchar_size", i32 4}
!12 = !{i32 7, !"uwtable", i32 1}
!13 = !{i32 7, !"frame-pointer", i32 2}
!14 = !{!"clang version 14.0.5 (Fedora 14.0.5-2.fc36)"}
!15 = distinct !DISubprogram(name: "foo", scope: !3, file: !3, line: 3, type: !6, scopeLine: 3, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !16)
!16 = !{}
!17 = !DILocalVariable(name: "a", arg: 1, scope: !15, file: !3, line: 3, type: !8)
!18 = !DILocation(line: 3, column: 14, scope: !15)
!19 = !DILocation(line: 4, column: 3, scope: !15)
!20 = !DILocation(line: 5, column: 1, scope: !15)
!21 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 7, type: !22, scopeLine: 7, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !16)
!22 = !DISubroutineType(types: !23)
!23 = !{!8}
!24 = !DILocation(line: 8, column: 10, scope: !21)
!25 = !DILocation(line: 9, column: 1, scope: !21)
