; ModuleID = 'everything.c'
source_filename = "everything.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

%struct.person = type { [50 x i8], i32 }
%union.value = type { i32 }

@s1 = dso_local global [6 x i8] c"hello\00", align 1, !dbg !0
@a = dso_local global [5 x i32] [i32 1, i32 2, i32 3, i32 4, i32 5], align 16, !dbg !5
@c = dso_local global i8 0, align 1, !dbg !11
@uc = dso_local global i8 0, align 1, !dbg !14
@s = dso_local global i16 0, align 2, !dbg !17
@us = dso_local global i16 0, align 2, !dbg !20
@i = dso_local global i32 0, align 4, !dbg !23
@ui = dso_local global i32 0, align 4, !dbg !25
@l = dso_local global i64 0, align 8, !dbg !28
@ul = dso_local global i64 0, align 8, !dbg !31
@ll = dso_local global i64 0, align 8, !dbg !34
@ull = dso_local global i64 0, align 8, !dbg !37
@f = dso_local global float 0.000000e+00, align 4, !dbg !40
@d = dso_local global double 0.000000e+00, align 8, !dbg !43
@ld = dso_local global x86_fp80 0xK00000000000000000000, align 16, !dbg !46
@ip = dso_local global i32* null, align 8, !dbg !57
@.str = private unnamed_addr constant [6 x i8] c"hello\00", align 1
@cp = dso_local global i8* null, align 8, !dbg !62
@vp2 = dso_local global i8* null, align 8, !dbg !60
@func_ptr = dso_local global i32 (i32, i32)* null, align 8, !dbg !65
@p = dso_local global %struct.person zeroinitializer, align 4, !dbg !70
@.str.1 = private unnamed_addr constant [50 x i8] c"John Doe\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00", align 1
@v = dso_local global %union.value zeroinitializer, align 4, !dbg !79
@sub = dso_local global i32 (i32, i32)* null, align 8, !dbg !85
@vp = dso_local global i8* null, align 8, !dbg !49
@fp = dso_local global void (i32)* null, align 8, !dbg !52

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @add(i32 noundef %0, i32 noundef %1) #0 !dbg !96 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  call void @llvm.dbg.declare(metadata i32* %3, metadata !98, metadata !DIExpression()), !dbg !99
  store i32 %1, i32* %4, align 4
  call void @llvm.dbg.declare(metadata i32* %4, metadata !100, metadata !DIExpression()), !dbg !101
  %5 = load i32, i32* %3, align 4, !dbg !102
  %6 = load i32, i32* %4, align 4, !dbg !103
  %7 = add nsw i32 %5, %6, !dbg !104
  ret i32 %7, !dbg !105
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !106 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.person, align 4
  %3 = alloca %union.value, align 4
  store i32 0, i32* %1, align 4
  store i8 97, i8* @c, align 1, !dbg !109
  store i8 98, i8* @uc, align 1, !dbg !110
  store i16 32767, i16* @s, align 2, !dbg !111
  store i16 -1, i16* @us, align 2, !dbg !112
  store i32 2147483647, i32* @i, align 4, !dbg !113
  store i32 -1, i32* @ui, align 4, !dbg !114
  store i64 9223372036854775807, i64* @l, align 8, !dbg !115
  store i64 -1, i64* @ul, align 8, !dbg !116
  store i64 9223372036854775807, i64* @ll, align 8, !dbg !117
  store i64 -1, i64* @ull, align 8, !dbg !118
  store float 0x400921FA00000000, float* @f, align 4, !dbg !119
  store double 0x400921FB54442D18, double* @d, align 8, !dbg !120
  store x86_fp80 0xK4000C90FDAA22168C235, x86_fp80* @ld, align 16, !dbg !121
  store i32* null, i32** @ip, align 8, !dbg !122
  store i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str, i64 0, i64 0), i8** @cp, align 8, !dbg !123
  store i8* null, i8** @vp2, align 8, !dbg !124
  store i32 (i32, i32)* @add, i32 (i32, i32)** @func_ptr, align 8, !dbg !125
  %4 = getelementptr inbounds %struct.person, %struct.person* %2, i32 0, i32 0, !dbg !126
  %5 = bitcast [50 x i8]* %4 to i8*, !dbg !127
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 4 %5, i8* align 1 getelementptr inbounds ([50 x i8], [50 x i8]* @.str.1, i32 0, i32 0), i64 50, i1 false), !dbg !127
  %6 = getelementptr inbounds %struct.person, %struct.person* %2, i32 0, i32 1, !dbg !126
  store i32 30, i32* %6, align 4, !dbg !126
  %7 = bitcast %struct.person* %2 to i8*, !dbg !128
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 4 getelementptr inbounds (%struct.person, %struct.person* @p, i32 0, i32 0, i32 0), i8* align 4 %7, i64 56, i1 false), !dbg !128
  %8 = bitcast %union.value* %3 to i32*, !dbg !129
  store i32 42, i32* %8, align 4, !dbg !129
  %9 = bitcast %union.value* %3 to i8*, !dbg !130
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 4 bitcast (%union.value* @v to i8*), i8* align 4 %9, i64 4, i1 false), !dbg !130
  store i32 (i32, i32)* @add, i32 (i32, i32)** @sub, align 8, !dbg !131
  ret i32 0, !dbg !132
}

; Function Attrs: argmemonly nofree nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { argmemonly nofree nounwind willreturn }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!90, !91, !92, !93, !94}
!llvm.ident = !{!95}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "s1", scope: !2, file: !3, line: 33, type: !87, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 14.0.5 (Fedora 14.0.5-2.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !4, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "everything.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/types_debuginfo")
!4 = !{!0, !5, !11, !14, !17, !20, !23, !25, !28, !31, !34, !37, !40, !43, !46, !49, !52, !57, !60, !62, !65, !70, !79, !85}
!5 = !DIGlobalVariableExpression(var: !6, expr: !DIExpression())
!6 = distinct !DIGlobalVariable(name: "a", scope: !2, file: !3, line: 34, type: !7, isLocal: false, isDefinition: true)
!7 = !DICompositeType(tag: DW_TAG_array_type, baseType: !8, size: 160, elements: !9)
!8 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!9 = !{!10}
!10 = !DISubrange(count: 5)
!11 = !DIGlobalVariableExpression(var: !12, expr: !DIExpression())
!12 = distinct !DIGlobalVariable(name: "c", scope: !2, file: !3, line: 2, type: !13, isLocal: false, isDefinition: true)
!13 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!14 = !DIGlobalVariableExpression(var: !15, expr: !DIExpression())
!15 = distinct !DIGlobalVariable(name: "uc", scope: !2, file: !3, line: 3, type: !16, isLocal: false, isDefinition: true)
!16 = !DIBasicType(name: "unsigned char", size: 8, encoding: DW_ATE_unsigned_char)
!17 = !DIGlobalVariableExpression(var: !18, expr: !DIExpression())
!18 = distinct !DIGlobalVariable(name: "s", scope: !2, file: !3, line: 4, type: !19, isLocal: false, isDefinition: true)
!19 = !DIBasicType(name: "short", size: 16, encoding: DW_ATE_signed)
!20 = !DIGlobalVariableExpression(var: !21, expr: !DIExpression())
!21 = distinct !DIGlobalVariable(name: "us", scope: !2, file: !3, line: 5, type: !22, isLocal: false, isDefinition: true)
!22 = !DIBasicType(name: "unsigned short", size: 16, encoding: DW_ATE_unsigned)
!23 = !DIGlobalVariableExpression(var: !24, expr: !DIExpression())
!24 = distinct !DIGlobalVariable(name: "i", scope: !2, file: !3, line: 6, type: !8, isLocal: false, isDefinition: true)
!25 = !DIGlobalVariableExpression(var: !26, expr: !DIExpression())
!26 = distinct !DIGlobalVariable(name: "ui", scope: !2, file: !3, line: 7, type: !27, isLocal: false, isDefinition: true)
!27 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!28 = !DIGlobalVariableExpression(var: !29, expr: !DIExpression())
!29 = distinct !DIGlobalVariable(name: "l", scope: !2, file: !3, line: 8, type: !30, isLocal: false, isDefinition: true)
!30 = !DIBasicType(name: "long", size: 64, encoding: DW_ATE_signed)
!31 = !DIGlobalVariableExpression(var: !32, expr: !DIExpression())
!32 = distinct !DIGlobalVariable(name: "ul", scope: !2, file: !3, line: 9, type: !33, isLocal: false, isDefinition: true)
!33 = !DIBasicType(name: "unsigned long", size: 64, encoding: DW_ATE_unsigned)
!34 = !DIGlobalVariableExpression(var: !35, expr: !DIExpression())
!35 = distinct !DIGlobalVariable(name: "ll", scope: !2, file: !3, line: 10, type: !36, isLocal: false, isDefinition: true)
!36 = !DIBasicType(name: "long long", size: 64, encoding: DW_ATE_signed)
!37 = !DIGlobalVariableExpression(var: !38, expr: !DIExpression())
!38 = distinct !DIGlobalVariable(name: "ull", scope: !2, file: !3, line: 11, type: !39, isLocal: false, isDefinition: true)
!39 = !DIBasicType(name: "unsigned long long", size: 64, encoding: DW_ATE_unsigned)
!40 = !DIGlobalVariableExpression(var: !41, expr: !DIExpression())
!41 = distinct !DIGlobalVariable(name: "f", scope: !2, file: !3, line: 14, type: !42, isLocal: false, isDefinition: true)
!42 = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
!43 = !DIGlobalVariableExpression(var: !44, expr: !DIExpression())
!44 = distinct !DIGlobalVariable(name: "d", scope: !2, file: !3, line: 15, type: !45, isLocal: false, isDefinition: true)
!45 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
!46 = !DIGlobalVariableExpression(var: !47, expr: !DIExpression())
!47 = distinct !DIGlobalVariable(name: "ld", scope: !2, file: !3, line: 16, type: !48, isLocal: false, isDefinition: true)
!48 = !DIBasicType(name: "long double", size: 128, encoding: DW_ATE_float)
!49 = !DIGlobalVariableExpression(var: !50, expr: !DIExpression())
!50 = distinct !DIGlobalVariable(name: "vp", scope: !2, file: !3, line: 19, type: !51, isLocal: false, isDefinition: true)
!51 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!52 = !DIGlobalVariableExpression(var: !53, expr: !DIExpression())
!53 = distinct !DIGlobalVariable(name: "fp", scope: !2, file: !3, line: 20, type: !54, isLocal: false, isDefinition: true)
!54 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !55, size: 64)
!55 = !DISubroutineType(types: !56)
!56 = !{null, !8}
!57 = !DIGlobalVariableExpression(var: !58, expr: !DIExpression())
!58 = distinct !DIGlobalVariable(name: "ip", scope: !2, file: !3, line: 27, type: !59, isLocal: false, isDefinition: true)
!59 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !8, size: 64)
!60 = !DIGlobalVariableExpression(var: !61, expr: !DIExpression())
!61 = distinct !DIGlobalVariable(name: "vp2", scope: !2, file: !3, line: 28, type: !51, isLocal: false, isDefinition: true)
!62 = !DIGlobalVariableExpression(var: !63, expr: !DIExpression())
!63 = distinct !DIGlobalVariable(name: "cp", scope: !2, file: !3, line: 29, type: !64, isLocal: false, isDefinition: true)
!64 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !13, size: 64)
!65 = !DIGlobalVariableExpression(var: !66, expr: !DIExpression())
!66 = distinct !DIGlobalVariable(name: "func_ptr", scope: !2, file: !3, line: 30, type: !67, isLocal: false, isDefinition: true)
!67 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !68, size: 64)
!68 = !DISubroutineType(types: !69)
!69 = !{!8, !8, !8}
!70 = !DIGlobalVariableExpression(var: !71, expr: !DIExpression())
!71 = distinct !DIGlobalVariable(name: "p", scope: !2, file: !3, line: 41, type: !72, isLocal: false, isDefinition: true)
!72 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "person", file: !3, line: 37, size: 448, elements: !73)
!73 = !{!74, !78}
!74 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !72, file: !3, line: 38, baseType: !75, size: 400)
!75 = !DICompositeType(tag: DW_TAG_array_type, baseType: !13, size: 400, elements: !76)
!76 = !{!77}
!77 = !DISubrange(count: 50)
!78 = !DIDerivedType(tag: DW_TAG_member, name: "age", scope: !72, file: !3, line: 39, baseType: !8, size: 32, offset: 416)
!79 = !DIGlobalVariableExpression(var: !80, expr: !DIExpression())
!80 = distinct !DIGlobalVariable(name: "v", scope: !2, file: !3, line: 48, type: !81, isLocal: false, isDefinition: true)
!81 = distinct !DICompositeType(tag: DW_TAG_union_type, name: "value", file: !3, line: 44, size: 32, elements: !82)
!82 = !{!83, !84}
!83 = !DIDerivedType(tag: DW_TAG_member, name: "i", scope: !81, file: !3, line: 45, baseType: !8, size: 32)
!84 = !DIDerivedType(tag: DW_TAG_member, name: "f", scope: !81, file: !3, line: 46, baseType: !42, size: 32)
!85 = !DIGlobalVariableExpression(var: !86, expr: !DIExpression())
!86 = distinct !DIGlobalVariable(name: "sub", scope: !2, file: !3, line: 54, type: !67, isLocal: false, isDefinition: true)
!87 = !DICompositeType(tag: DW_TAG_array_type, baseType: !13, size: 48, elements: !88)
!88 = !{!89}
!89 = !DISubrange(count: 6)
!90 = !{i32 7, !"Dwarf Version", i32 4}
!91 = !{i32 2, !"Debug Info Version", i32 3}
!92 = !{i32 1, !"wchar_size", i32 4}
!93 = !{i32 7, !"uwtable", i32 1}
!94 = !{i32 7, !"frame-pointer", i32 2}
!95 = !{!"clang version 14.0.5 (Fedora 14.0.5-2.fc36)"}
!96 = distinct !DISubprogram(name: "add", scope: !3, file: !3, line: 51, type: !68, scopeLine: 51, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !97)
!97 = !{}
!98 = !DILocalVariable(name: "a", arg: 1, scope: !96, file: !3, line: 51, type: !8)
!99 = !DILocation(line: 51, column: 13, scope: !96)
!100 = !DILocalVariable(name: "b", arg: 2, scope: !96, file: !3, line: 51, type: !8)
!101 = !DILocation(line: 51, column: 20, scope: !96)
!102 = !DILocation(line: 52, column: 12, scope: !96)
!103 = !DILocation(line: 52, column: 16, scope: !96)
!104 = !DILocation(line: 52, column: 14, scope: !96)
!105 = !DILocation(line: 52, column: 5, scope: !96)
!106 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 56, type: !107, scopeLine: 56, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !97)
!107 = !DISubroutineType(types: !108)
!108 = !{!8}
!109 = !DILocation(line: 58, column: 7, scope: !106)
!110 = !DILocation(line: 59, column: 8, scope: !106)
!111 = !DILocation(line: 60, column: 7, scope: !106)
!112 = !DILocation(line: 61, column: 8, scope: !106)
!113 = !DILocation(line: 62, column: 7, scope: !106)
!114 = !DILocation(line: 63, column: 8, scope: !106)
!115 = !DILocation(line: 64, column: 7, scope: !106)
!116 = !DILocation(line: 65, column: 8, scope: !106)
!117 = !DILocation(line: 66, column: 8, scope: !106)
!118 = !DILocation(line: 67, column: 9, scope: !106)
!119 = !DILocation(line: 68, column: 7, scope: !106)
!120 = !DILocation(line: 69, column: 7, scope: !106)
!121 = !DILocation(line: 70, column: 8, scope: !106)
!122 = !DILocation(line: 72, column: 8, scope: !106)
!123 = !DILocation(line: 73, column: 8, scope: !106)
!124 = !DILocation(line: 74, column: 9, scope: !106)
!125 = !DILocation(line: 75, column: 14, scope: !106)
!126 = !DILocation(line: 76, column: 24, scope: !106)
!127 = !DILocation(line: 76, column: 25, scope: !106)
!128 = !DILocation(line: 76, column: 9, scope: !106)
!129 = !DILocation(line: 77, column: 22, scope: !106)
!130 = !DILocation(line: 77, column: 9, scope: !106)
!131 = !DILocation(line: 78, column: 9, scope: !106)
!132 = !DILocation(line: 80, column: 5, scope: !106)
