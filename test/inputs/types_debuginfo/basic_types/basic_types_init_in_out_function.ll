; ModuleID = 'basic_types_init_in_out_function.c'
source_filename = "basic_types_init_in_out_function.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

@a = dso_local global i8 97, align 1, !dbg !0
@b = dso_local global i8 98, align 1, !dbg !5
@c = dso_local global i8 99, align 1, !dbg !8
@d = dso_local global i16 4, align 2, !dbg !11
@e = dso_local global i16 5, align 2, !dbg !14
@f = dso_local global i32 -5, align 4, !dbg !17
@g = dso_local global i32 5, align 4, !dbg !20
@h = dso_local global i64 -9, align 8, !dbg !23
@i = dso_local global i64 9, align 8, !dbg !26
@j = dso_local global i64 -256, align 8, !dbg !29
@k = dso_local global i64 265, align 8, !dbg !32
@l = dso_local global float -5.000000e-01, align 4, !dbg !35
@m = dso_local global double -9.000000e-01, align 8, !dbg !38
@n = dso_local global x86_fp80 0xKBFFEE666666666666800, align 16, !dbg !41

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !50 {
  %1 = alloca i32, align 4
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i16, align 2
  %6 = alloca i16, align 2
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i64, align 8
  %10 = alloca i64, align 8
  %11 = alloca i64, align 8
  %12 = alloca i64, align 8
  %13 = alloca float, align 4
  %14 = alloca double, align 8
  %15 = alloca x86_fp80, align 16
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata i8* %2, metadata !54, metadata !DIExpression()), !dbg !55
  store i8 111, i8* %2, align 1, !dbg !55
  call void @llvm.dbg.declare(metadata i8* %3, metadata !56, metadata !DIExpression()), !dbg !57
  store i8 112, i8* %3, align 1, !dbg !57
  call void @llvm.dbg.declare(metadata i8* %4, metadata !58, metadata !DIExpression()), !dbg !59
  store i8 114, i8* %4, align 1, !dbg !59
  call void @llvm.dbg.declare(metadata i16* %5, metadata !60, metadata !DIExpression()), !dbg !61
  store i16 5, i16* %5, align 2, !dbg !61
  call void @llvm.dbg.declare(metadata i16* %6, metadata !62, metadata !DIExpression()), !dbg !63
  store i16 6, i16* %6, align 2, !dbg !63
  call void @llvm.dbg.declare(metadata i32* %7, metadata !64, metadata !DIExpression()), !dbg !65
  store i32 -6, i32* %7, align 4, !dbg !65
  call void @llvm.dbg.declare(metadata i32* %8, metadata !66, metadata !DIExpression()), !dbg !67
  store i32 7, i32* %8, align 4, !dbg !67
  call void @llvm.dbg.declare(metadata i64* %9, metadata !68, metadata !DIExpression()), !dbg !69
  store i64 -10, i64* %9, align 8, !dbg !69
  call void @llvm.dbg.declare(metadata i64* %10, metadata !70, metadata !DIExpression()), !dbg !71
  store i64 8, i64* %10, align 8, !dbg !71
  call void @llvm.dbg.declare(metadata i64* %11, metadata !72, metadata !DIExpression()), !dbg !73
  store i64 -259, i64* %11, align 8, !dbg !73
  call void @llvm.dbg.declare(metadata i64* %12, metadata !74, metadata !DIExpression()), !dbg !75
  store i64 267, i64* %12, align 8, !dbg !75
  call void @llvm.dbg.declare(metadata float* %13, metadata !76, metadata !DIExpression()), !dbg !77
  store float 0xBFD99999A0000000, float* %13, align 4, !dbg !77
  call void @llvm.dbg.declare(metadata double* %14, metadata !78, metadata !DIExpression()), !dbg !79
  store double -1.000000e-01, double* %14, align 8, !dbg !79
  call void @llvm.dbg.declare(metadata x86_fp80* %15, metadata !80, metadata !DIExpression()), !dbg !81
  store x86_fp80 0xKBFFFF333333333333000, x86_fp80* %15, align 16, !dbg !81
  ret i32 1, !dbg !82
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!44, !45, !46, !47, !48}
!llvm.ident = !{!49}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "a", scope: !2, file: !3, line: 1, type: !7, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 14.0.5 (Fedora 14.0.5-2.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !4, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "basic_types_init_in_out_function.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/types_debuginfo/basic_types")
!4 = !{!0, !5, !8, !11, !14, !17, !20, !23, !26, !29, !32, !35, !38, !41}
!5 = !DIGlobalVariableExpression(var: !6, expr: !DIExpression())
!6 = distinct !DIGlobalVariable(name: "b", scope: !2, file: !3, line: 2, type: !7, isLocal: false, isDefinition: true)
!7 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!8 = !DIGlobalVariableExpression(var: !9, expr: !DIExpression())
!9 = distinct !DIGlobalVariable(name: "c", scope: !2, file: !3, line: 3, type: !10, isLocal: false, isDefinition: true)
!10 = !DIBasicType(name: "unsigned char", size: 8, encoding: DW_ATE_unsigned_char)
!11 = !DIGlobalVariableExpression(var: !12, expr: !DIExpression())
!12 = distinct !DIGlobalVariable(name: "d", scope: !2, file: !3, line: 4, type: !13, isLocal: false, isDefinition: true)
!13 = !DIBasicType(name: "short", size: 16, encoding: DW_ATE_signed)
!14 = !DIGlobalVariableExpression(var: !15, expr: !DIExpression())
!15 = distinct !DIGlobalVariable(name: "e", scope: !2, file: !3, line: 5, type: !16, isLocal: false, isDefinition: true)
!16 = !DIBasicType(name: "unsigned short", size: 16, encoding: DW_ATE_unsigned)
!17 = !DIGlobalVariableExpression(var: !18, expr: !DIExpression())
!18 = distinct !DIGlobalVariable(name: "f", scope: !2, file: !3, line: 6, type: !19, isLocal: false, isDefinition: true)
!19 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!20 = !DIGlobalVariableExpression(var: !21, expr: !DIExpression())
!21 = distinct !DIGlobalVariable(name: "g", scope: !2, file: !3, line: 7, type: !22, isLocal: false, isDefinition: true)
!22 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!23 = !DIGlobalVariableExpression(var: !24, expr: !DIExpression())
!24 = distinct !DIGlobalVariable(name: "h", scope: !2, file: !3, line: 8, type: !25, isLocal: false, isDefinition: true)
!25 = !DIBasicType(name: "long", size: 64, encoding: DW_ATE_signed)
!26 = !DIGlobalVariableExpression(var: !27, expr: !DIExpression())
!27 = distinct !DIGlobalVariable(name: "i", scope: !2, file: !3, line: 9, type: !28, isLocal: false, isDefinition: true)
!28 = !DIBasicType(name: "unsigned long", size: 64, encoding: DW_ATE_unsigned)
!29 = !DIGlobalVariableExpression(var: !30, expr: !DIExpression())
!30 = distinct !DIGlobalVariable(name: "j", scope: !2, file: !3, line: 10, type: !31, isLocal: false, isDefinition: true)
!31 = !DIBasicType(name: "long long", size: 64, encoding: DW_ATE_signed)
!32 = !DIGlobalVariableExpression(var: !33, expr: !DIExpression())
!33 = distinct !DIGlobalVariable(name: "k", scope: !2, file: !3, line: 11, type: !34, isLocal: false, isDefinition: true)
!34 = !DIBasicType(name: "unsigned long long", size: 64, encoding: DW_ATE_unsigned)
!35 = !DIGlobalVariableExpression(var: !36, expr: !DIExpression())
!36 = distinct !DIGlobalVariable(name: "l", scope: !2, file: !3, line: 13, type: !37, isLocal: false, isDefinition: true)
!37 = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
!38 = !DIGlobalVariableExpression(var: !39, expr: !DIExpression())
!39 = distinct !DIGlobalVariable(name: "m", scope: !2, file: !3, line: 14, type: !40, isLocal: false, isDefinition: true)
!40 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
!41 = !DIGlobalVariableExpression(var: !42, expr: !DIExpression())
!42 = distinct !DIGlobalVariable(name: "n", scope: !2, file: !3, line: 15, type: !43, isLocal: false, isDefinition: true)
!43 = !DIBasicType(name: "long double", size: 128, encoding: DW_ATE_float)
!44 = !{i32 7, !"Dwarf Version", i32 4}
!45 = !{i32 2, !"Debug Info Version", i32 3}
!46 = !{i32 1, !"wchar_size", i32 4}
!47 = !{i32 7, !"uwtable", i32 1}
!48 = !{i32 7, !"frame-pointer", i32 2}
!49 = !{!"clang version 14.0.5 (Fedora 14.0.5-2.fc36)"}
!50 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 16, type: !51, scopeLine: 16, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !53)
!51 = !DISubroutineType(types: !52)
!52 = !{!19}
!53 = !{}
!54 = !DILocalVariable(name: "o", scope: !50, file: !3, line: 17, type: !7)
!55 = !DILocation(line: 17, column: 7, scope: !50)
!56 = !DILocalVariable(name: "p", scope: !50, file: !3, line: 18, type: !7)
!57 = !DILocation(line: 18, column: 14, scope: !50)
!58 = !DILocalVariable(name: "r", scope: !50, file: !3, line: 19, type: !10)
!59 = !DILocation(line: 19, column: 23, scope: !50)
!60 = !DILocalVariable(name: "s", scope: !50, file: !3, line: 20, type: !13)
!61 = !DILocation(line: 20, column: 15, scope: !50)
!62 = !DILocalVariable(name: "t", scope: !50, file: !3, line: 21, type: !16)
!63 = !DILocation(line: 21, column: 24, scope: !50)
!64 = !DILocalVariable(name: "u", scope: !50, file: !3, line: 22, type: !19)
!65 = !DILocation(line: 22, column: 13, scope: !50)
!66 = !DILocalVariable(name: "v", scope: !50, file: !3, line: 23, type: !22)
!67 = !DILocation(line: 23, column: 18, scope: !50)
!68 = !DILocalVariable(name: "y", scope: !50, file: !3, line: 24, type: !25)
!69 = !DILocation(line: 24, column: 14, scope: !50)
!70 = !DILocalVariable(name: "x", scope: !50, file: !3, line: 25, type: !28)
!71 = !DILocation(line: 25, column: 23, scope: !50)
!72 = !DILocalVariable(name: "z", scope: !50, file: !3, line: 26, type: !31)
!73 = !DILocation(line: 26, column: 19, scope: !50)
!74 = !DILocalVariable(name: "q", scope: !50, file: !3, line: 27, type: !34)
!75 = !DILocation(line: 27, column: 28, scope: !50)
!76 = !DILocalVariable(name: "fl", scope: !50, file: !3, line: 29, type: !37)
!77 = !DILocation(line: 29, column: 15, scope: !50)
!78 = !DILocalVariable(name: "dm", scope: !50, file: !3, line: 30, type: !40)
!79 = !DILocation(line: 30, column: 16, scope: !50)
!80 = !DILocalVariable(name: "ldn", scope: !50, file: !3, line: 31, type: !43)
!81 = !DILocation(line: 31, column: 21, scope: !50)
!82 = !DILocation(line: 32, column: 2, scope: !50)
