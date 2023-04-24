; ModuleID = 'sstruct.c'
source_filename = "sstruct.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

%struct.person = type { [50 x i32], i32 }

@a = dso_local global %struct.person zeroinitializer, align 4, !dbg !0

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !19 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.person, align 4
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata %struct.person* %2, metadata !23, metadata !DIExpression()), !dbg !24
  ret i32 0, !dbg !25
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!13, !14, !15, !16, !17}
!llvm.ident = !{!18}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "a", scope: !2, file: !3, line: 5, type: !5, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 14.0.5 (Fedora 14.0.5-2.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !4, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "sstruct.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/types_debuginfo/composite_types/structs")
!4 = !{!0}
!5 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "person", file: !3, line: 1, size: 1632, elements: !6)
!6 = !{!7, !12}
!7 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !5, file: !3, line: 2, baseType: !8, size: 1600)
!8 = !DICompositeType(tag: DW_TAG_array_type, baseType: !9, size: 1600, elements: !10)
!9 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!10 = !{!11}
!11 = !DISubrange(count: 50)
!12 = !DIDerivedType(tag: DW_TAG_member, name: "age", scope: !5, file: !3, line: 3, baseType: !9, size: 32, offset: 1600)
!13 = !{i32 7, !"Dwarf Version", i32 4}
!14 = !{i32 2, !"Debug Info Version", i32 3}
!15 = !{i32 1, !"wchar_size", i32 4}
!16 = !{i32 7, !"uwtable", i32 1}
!17 = !{i32 7, !"frame-pointer", i32 2}
!18 = !{!"clang version 14.0.5 (Fedora 14.0.5-2.fc36)"}
!19 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 6, type: !20, scopeLine: 6, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !22)
!20 = !DISubroutineType(types: !21)
!21 = !{!9}
!22 = !{}
!23 = !DILocalVariable(name: "b", scope: !19, file: !3, line: 7, type: !5)
!24 = !DILocation(line: 7, column: 19, scope: !19)
!25 = !DILocation(line: 8, column: 2, scope: !19)
