; ModuleID = 'fnctn_args_types.c'
source_filename = "fnctn_args_types.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

%struct.person = type { [50 x i8], i32 }

@s1 = dso_local global [6 x i8] c"hello\00", align 1, !dbg !0
@a = dso_local global [5 x i32] [i32 1, i32 2, i32 3, i32 4, i32 5], align 16, !dbg !5
@__const.main.p = private unnamed_addr constant %struct.person { [50 x i8] c"John Doe\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00", i32 30 }, align 4
@p = dso_local global %struct.person zeroinitializer, align 4, !dbg !11

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @add(i32 noundef %0, i32 noundef %1) #0 !dbg !30 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  call void @llvm.dbg.declare(metadata i32* %3, metadata !34, metadata !DIExpression()), !dbg !35
  store i32 %1, i32* %4, align 4
  call void @llvm.dbg.declare(metadata i32* %4, metadata !36, metadata !DIExpression()), !dbg !37
  %5 = load i32, i32* %3, align 4, !dbg !38
  %6 = load i32, i32* %4, align 4, !dbg !39
  %7 = add nsw i32 %5, %6, !dbg !40
  ret i32 %7, !dbg !41
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !42 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.person, align 8
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata %struct.person* %2, metadata !45, metadata !DIExpression()), !dbg !46
  %3 = bitcast %struct.person* %2 to i8*, !dbg !46
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 4 %3, i8* align 4 getelementptr inbounds (%struct.person, %struct.person* @__const.main.p, i32 0, i32 0, i32 0), i64 56, i1 false), !dbg !46
  call void @my_function(i8 noundef signext 97, i8 noundef zeroext 98, i16 noundef signext 32767, i16 noundef zeroext -1, i32 noundef 2147483647, i32 noundef -1, i64 noundef 9223372036854775807, i64 noundef -1, i64 noundef 9223372036854775807, i64 noundef -1, float noundef 0x400921FA00000000, double noundef 0x400921FB54442D18, x86_fp80 noundef 0xK4000C90FDAA22168C235, i8* noundef null, void (i32)* noundef null, i32* noundef null, i32 (i32, i32)* noundef null, i8* noundef getelementptr inbounds ([6 x i8], [6 x i8]* @s1, i64 0, i64 0), i32* noundef getelementptr inbounds ([5 x i32], [5 x i32]* @a, i64 0, i64 0), %struct.person* noundef byval(%struct.person) align 8 %2), !dbg !47
  ret i32 0, !dbg !48
}

; Function Attrs: argmemonly nofree nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #2

declare dso_local void @my_function(i8 noundef signext, i8 noundef zeroext, i16 noundef signext, i16 noundef zeroext, i32 noundef, i32 noundef, i64 noundef, i64 noundef, i64 noundef, i64 noundef, float noundef, double noundef, x86_fp80 noundef, i8* noundef, void (i32)* noundef, i32* noundef, i32 (i32, i32)* noundef, i8* noundef, i32* noundef, %struct.person* noundef byval(%struct.person) align 8) #3

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { argmemonly nofree nounwind willreturn }
attributes #3 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!24, !25, !26, !27, !28}
!llvm.ident = !{!29}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "s1", scope: !2, file: !3, line: 5, type: !21, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 14.0.5 (Fedora 14.0.5-2.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !4, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "fnctn_args_types.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/types_debuginfo/function_args_types")
!4 = !{!0, !5, !11}
!5 = !DIGlobalVariableExpression(var: !6, expr: !DIExpression())
!6 = distinct !DIGlobalVariable(name: "a", scope: !2, file: !3, line: 6, type: !7, isLocal: false, isDefinition: true)
!7 = !DICompositeType(tag: DW_TAG_array_type, baseType: !8, size: 160, elements: !9)
!8 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!9 = !{!10}
!10 = !DISubrange(count: 5)
!11 = !DIGlobalVariableExpression(var: !12, expr: !DIExpression())
!12 = distinct !DIGlobalVariable(name: "p", scope: !2, file: !3, line: 13, type: !13, isLocal: false, isDefinition: true)
!13 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "person", file: !3, line: 9, size: 448, elements: !14)
!14 = !{!15, !20}
!15 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !13, file: !3, line: 10, baseType: !16, size: 400)
!16 = !DICompositeType(tag: DW_TAG_array_type, baseType: !17, size: 400, elements: !18)
!17 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!18 = !{!19}
!19 = !DISubrange(count: 50)
!20 = !DIDerivedType(tag: DW_TAG_member, name: "age", scope: !13, file: !3, line: 11, baseType: !8, size: 32, offset: 416)
!21 = !DICompositeType(tag: DW_TAG_array_type, baseType: !17, size: 48, elements: !22)
!22 = !{!23}
!23 = !DISubrange(count: 6)
!24 = !{i32 7, !"Dwarf Version", i32 4}
!25 = !{i32 2, !"Debug Info Version", i32 3}
!26 = !{i32 1, !"wchar_size", i32 4}
!27 = !{i32 7, !"uwtable", i32 1}
!28 = !{i32 7, !"frame-pointer", i32 2}
!29 = !{!"clang version 14.0.5 (Fedora 14.0.5-2.fc36)"}
!30 = distinct !DISubprogram(name: "add", scope: !3, file: !3, line: 22, type: !31, scopeLine: 22, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !33)
!31 = !DISubroutineType(types: !32)
!32 = !{!8, !8, !8}
!33 = !{}
!34 = !DILocalVariable(name: "a", arg: 1, scope: !30, file: !3, line: 22, type: !8)
!35 = !DILocation(line: 22, column: 13, scope: !30)
!36 = !DILocalVariable(name: "b", arg: 2, scope: !30, file: !3, line: 22, type: !8)
!37 = !DILocation(line: 22, column: 20, scope: !30)
!38 = !DILocation(line: 23, column: 12, scope: !30)
!39 = !DILocation(line: 23, column: 16, scope: !30)
!40 = !DILocation(line: 23, column: 14, scope: !30)
!41 = !DILocation(line: 23, column: 5, scope: !30)
!42 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 32, type: !43, scopeLine: 32, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !33)
!43 = !DISubroutineType(types: !44)
!44 = !{!8}
!45 = !DILocalVariable(name: "p", scope: !42, file: !3, line: 33, type: !13)
!46 = !DILocation(line: 33, column: 19, scope: !42)
!47 = !DILocation(line: 35, column: 5, scope: !42)
!48 = !DILocation(line: 50, column: 5, scope: !42)
