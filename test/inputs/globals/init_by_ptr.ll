; ModuleID = 'init_by_ptr.c'
source_filename = "init_by_ptr.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

%struct.mystruct = type { i32*, i32 }

@test = dso_local global %struct.mystruct { i32* bitcast (i8* getelementptr (i8, i8* bitcast (%struct.mystruct* @test to i8*), i64 8) to i32*), i32 5 }, align 8, !dbg !0

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !17 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %2 = load i32*, i32** getelementptr inbounds (%struct.mystruct, %struct.mystruct* @test, i32 0, i32 0), align 8, !dbg !21
  %3 = load i32, i32* %2, align 4, !dbg !22
  ret i32 %3, !dbg !23
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!11, !12, !13, !14, !15}
!llvm.ident = !{!16}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "test", scope: !2, file: !3, line: 6, type: !5, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 14.0.5 (Fedora 14.0.5-1.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !4, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "init_by_ptr.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/globals")
!4 = !{!0}
!5 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "mystruct", file: !3, line: 1, size: 128, elements: !6)
!6 = !{!7, !10}
!7 = !DIDerivedType(tag: DW_TAG_member, name: "ptr_to_a", scope: !5, file: !3, line: 2, baseType: !8, size: 64)
!8 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !9, size: 64)
!9 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!10 = !DIDerivedType(tag: DW_TAG_member, name: "a", scope: !5, file: !3, line: 3, baseType: !9, size: 32, offset: 64)
!11 = !{i32 7, !"Dwarf Version", i32 4}
!12 = !{i32 2, !"Debug Info Version", i32 3}
!13 = !{i32 1, !"wchar_size", i32 4}
!14 = !{i32 7, !"uwtable", i32 1}
!15 = !{i32 7, !"frame-pointer", i32 2}
!16 = !{!"clang version 14.0.5 (Fedora 14.0.5-1.fc36)"}
!17 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 8, type: !18, scopeLine: 9, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !20)
!18 = !DISubroutineType(types: !19)
!19 = !{!9}
!20 = !{}
!21 = !DILocation(line: 10, column: 16, scope: !17)
!22 = !DILocation(line: 10, column: 9, scope: !17)
!23 = !DILocation(line: 10, column: 2, scope: !17)
