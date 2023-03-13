; ModuleID = 'noinit-global_decom.c'
source_filename = "noinit-global_decom.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

@__const_main_source = dso_local global [7 x i8] c"noinit\00", align 1, !dbg !0
@dest = dso_local global [7 x i8] zeroinitializer, align 1, !dbg !8

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !19 {
  %1 = alloca i32, align 4
  %2 = alloca [7 x i8], align 1
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata [7 x i8]* %2, metadata !24, metadata !DIExpression()), !dbg !25
  br label %3, !dbg !26

3:                                                ; preds = %0
  call void @llvm.dbg.label(metadata !27), !dbg !28
  %4 = bitcast [7 x i8]* %2 to i8*, !dbg !29
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 1 %4, i8* align 1 getelementptr inbounds ([7 x i8], [7 x i8]* @__const_main_source, i64 0, i64 0), i64 7, i1 false), !dbg !29
  %5 = getelementptr inbounds [7 x i8], [7 x i8]* %2, i64 0, i64 0, !dbg !30
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 1 getelementptr inbounds ([7 x i8], [7 x i8]* @dest, i64 0, i64 0), i8* align 1 %5, i64 7, i1 false), !dbg !31
  ret i32 0, !dbg !32
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.label(metadata) #1

; Function Attrs: argmemonly nofree nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { argmemonly nofree nounwind willreturn }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!13, !14, !15, !16, !17}
!llvm.ident = !{!18}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "__const_main_source", scope: !2, file: !3, line: 5, type: !10, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 14.0.5 (Fedora 14.0.5-1.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, retainedTypes: !4, globals: !7, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "noinit-global_decom.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/arrays")
!4 = !{!5}
!5 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !6, size: 64)
!6 = !DIBasicType(name: "unsigned char", size: 8, encoding: DW_ATE_unsigned_char)
!7 = !{!0, !8}
!8 = !DIGlobalVariableExpression(var: !9, expr: !DIExpression())
!9 = distinct !DIGlobalVariable(name: "dest", scope: !2, file: !3, line: 6, type: !10, isLocal: false, isDefinition: true)
!10 = !DICompositeType(tag: DW_TAG_array_type, baseType: !6, size: 56, elements: !11)
!11 = !{!12}
!12 = !DISubrange(count: 7)
!13 = !{i32 7, !"Dwarf Version", i32 4}
!14 = !{i32 2, !"Debug Info Version", i32 3}
!15 = !{i32 1, !"wchar_size", i32 4}
!16 = !{i32 7, !"uwtable", i32 1}
!17 = !{i32 7, !"frame-pointer", i32 2}
!18 = !{!"clang version 14.0.5 (Fedora 14.0.5-1.fc36)"}
!19 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 8, type: !20, scopeLine: 8, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !23)
!20 = !DISubroutineType(types: !21)
!21 = !{!22}
!22 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!23 = !{}
!24 = !DILocalVariable(name: "source", scope: !19, file: !3, line: 9, type: !10)
!25 = !DILocation(line: 9, column: 19, scope: !19)
!26 = !DILocation(line: 9, column: 5, scope: !19)
!27 = !DILabel(scope: !19, name: "block0", file: !3, line: 10)
!28 = !DILocation(line: 10, column: 5, scope: !19)
!29 = !DILocation(line: 11, column: 5, scope: !19)
!30 = !DILocation(line: 12, column: 26, scope: !19)
!31 = !DILocation(line: 12, column: 5, scope: !19)
!32 = !DILocation(line: 13, column: 5, scope: !19)
