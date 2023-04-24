; ModuleID = 'array_of_fnctn_ptrs.c'
source_filename = "array_of_fnctn_ptrs.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

@__const.main.function_array = private unnamed_addr constant [3 x i32 (...)*] [i32 (...)* bitcast (i32 ()* @fun to i32 (...)*), i32 (...)* bitcast (i32 ()* @fun to i32 (...)*), i32 (...)* bitcast (i32 ()* @fun to i32 (...)*)], align 16

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @fun() #0 !dbg !8 {
  ret i32 0, !dbg !13
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !14 {
  %1 = alloca i32, align 4
  %2 = alloca [3 x i32 (...)*], align 16
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata [3 x i32 (...)*]* %2, metadata !15, metadata !DIExpression()), !dbg !22
  %3 = bitcast [3 x i32 (...)*]* %2 to i8*, !dbg !22
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %3, i8* align 16 bitcast ([3 x i32 (...)*]* @__const.main.function_array to i8*), i64 24, i1 false), !dbg !22
  %4 = getelementptr inbounds [3 x i32 (...)*], [3 x i32 (...)*]* %2, i64 0, i64 2, !dbg !23
  %5 = load i32 (...)*, i32 (...)** %4, align 16, !dbg !23
  %6 = call i32 (...) %5(), !dbg !24
  ret i32 %6, !dbg !25
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: argmemonly nofree nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { argmemonly nofree nounwind willreturn }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 14.0.5 (Fedora 14.0.5-2.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "array_of_fnctn_ptrs.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/types_debuginfo/composite_types")
!2 = !{i32 7, !"Dwarf Version", i32 4}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 7, !"uwtable", i32 1}
!6 = !{i32 7, !"frame-pointer", i32 2}
!7 = !{!"clang version 14.0.5 (Fedora 14.0.5-2.fc36)"}
!8 = distinct !DISubprogram(name: "fun", scope: !1, file: !1, line: 1, type: !9, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !12)
!9 = !DISubroutineType(types: !10)
!10 = !{!11}
!11 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!12 = !{}
!13 = !DILocation(line: 2, column: 2, scope: !8)
!14 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 5, type: !9, scopeLine: 5, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !12)
!15 = !DILocalVariable(name: "function_array", scope: !14, file: !1, line: 6, type: !16)
!16 = !DICompositeType(tag: DW_TAG_array_type, baseType: !17, size: 192, elements: !20)
!17 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !18, size: 64)
!18 = !DISubroutineType(types: !19)
!19 = !{!11, null}
!20 = !{!21}
!21 = !DISubrange(count: 3)
!22 = !DILocation(line: 6, column: 8, scope: !14)
!23 = !DILocation(line: 7, column: 11, scope: !14)
!24 = !DILocation(line: 7, column: 9, scope: !14)
!25 = !DILocation(line: 7, column: 2, scope: !14)
