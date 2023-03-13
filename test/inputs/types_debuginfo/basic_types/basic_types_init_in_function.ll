; ModuleID = 'basic_types_init_in_function.c'
source_filename = "basic_types_init_in_function.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !8 {
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
  call void @llvm.dbg.declare(metadata i8* %2, metadata !13, metadata !DIExpression()), !dbg !15
  store i8 97, i8* %2, align 1, !dbg !15
  call void @llvm.dbg.declare(metadata i8* %3, metadata !16, metadata !DIExpression()), !dbg !17
  store i8 98, i8* %3, align 1, !dbg !17
  call void @llvm.dbg.declare(metadata i8* %4, metadata !18, metadata !DIExpression()), !dbg !20
  store i8 99, i8* %4, align 1, !dbg !20
  call void @llvm.dbg.declare(metadata i16* %5, metadata !21, metadata !DIExpression()), !dbg !23
  store i16 4, i16* %5, align 2, !dbg !23
  call void @llvm.dbg.declare(metadata i16* %6, metadata !24, metadata !DIExpression()), !dbg !26
  store i16 5, i16* %6, align 2, !dbg !26
  call void @llvm.dbg.declare(metadata i32* %7, metadata !27, metadata !DIExpression()), !dbg !28
  store i32 -5, i32* %7, align 4, !dbg !28
  call void @llvm.dbg.declare(metadata i32* %8, metadata !29, metadata !DIExpression()), !dbg !31
  store i32 5, i32* %8, align 4, !dbg !31
  call void @llvm.dbg.declare(metadata i64* %9, metadata !32, metadata !DIExpression()), !dbg !34
  store i64 -9, i64* %9, align 8, !dbg !34
  call void @llvm.dbg.declare(metadata i64* %10, metadata !35, metadata !DIExpression()), !dbg !37
  store i64 9, i64* %10, align 8, !dbg !37
  call void @llvm.dbg.declare(metadata i64* %11, metadata !38, metadata !DIExpression()), !dbg !40
  store i64 -256, i64* %11, align 8, !dbg !40
  call void @llvm.dbg.declare(metadata i64* %12, metadata !41, metadata !DIExpression()), !dbg !43
  store i64 265, i64* %12, align 8, !dbg !43
  call void @llvm.dbg.declare(metadata float* %13, metadata !44, metadata !DIExpression()), !dbg !46
  store float -5.000000e-01, float* %13, align 4, !dbg !46
  call void @llvm.dbg.declare(metadata double* %14, metadata !47, metadata !DIExpression()), !dbg !49
  store double -9.000000e-01, double* %14, align 8, !dbg !49
  call void @llvm.dbg.declare(metadata x86_fp80* %15, metadata !50, metadata !DIExpression()), !dbg !52
  store x86_fp80 0xKBFFEE666666666666800, x86_fp80* %15, align 16, !dbg !52
  ret i32 1, !dbg !53
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 14.0.5 (Fedora 14.0.5-2.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "basic_types_init_in_function.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/types_debuginfo/basic_types")
!2 = !{i32 7, !"Dwarf Version", i32 4}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 7, !"uwtable", i32 1}
!6 = !{i32 7, !"frame-pointer", i32 2}
!7 = !{!"clang version 14.0.5 (Fedora 14.0.5-2.fc36)"}
!8 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 1, type: !9, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !12)
!9 = !DISubroutineType(types: !10)
!10 = !{!11}
!11 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!12 = !{}
!13 = !DILocalVariable(name: "a", scope: !8, file: !1, line: 2, type: !14)
!14 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!15 = !DILocation(line: 2, column: 7, scope: !8)
!16 = !DILocalVariable(name: "b", scope: !8, file: !1, line: 3, type: !14)
!17 = !DILocation(line: 3, column: 7, scope: !8)
!18 = !DILocalVariable(name: "c", scope: !8, file: !1, line: 4, type: !19)
!19 = !DIBasicType(name: "unsigned char", size: 8, encoding: DW_ATE_unsigned_char)
!20 = !DILocation(line: 4, column: 16, scope: !8)
!21 = !DILocalVariable(name: "d", scope: !8, file: !1, line: 5, type: !22)
!22 = !DIBasicType(name: "short", size: 16, encoding: DW_ATE_signed)
!23 = !DILocation(line: 5, column: 8, scope: !8)
!24 = !DILocalVariable(name: "e", scope: !8, file: !1, line: 6, type: !25)
!25 = !DIBasicType(name: "unsigned short", size: 16, encoding: DW_ATE_unsigned)
!26 = !DILocation(line: 6, column: 17, scope: !8)
!27 = !DILocalVariable(name: "f", scope: !8, file: !1, line: 7, type: !11)
!28 = !DILocation(line: 7, column: 6, scope: !8)
!29 = !DILocalVariable(name: "g", scope: !8, file: !1, line: 8, type: !30)
!30 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!31 = !DILocation(line: 8, column: 11, scope: !8)
!32 = !DILocalVariable(name: "h", scope: !8, file: !1, line: 9, type: !33)
!33 = !DIBasicType(name: "long", size: 64, encoding: DW_ATE_signed)
!34 = !DILocation(line: 9, column: 7, scope: !8)
!35 = !DILocalVariable(name: "i", scope: !8, file: !1, line: 10, type: !36)
!36 = !DIBasicType(name: "unsigned long", size: 64, encoding: DW_ATE_unsigned)
!37 = !DILocation(line: 10, column: 16, scope: !8)
!38 = !DILocalVariable(name: "j", scope: !8, file: !1, line: 11, type: !39)
!39 = !DIBasicType(name: "long long", size: 64, encoding: DW_ATE_signed)
!40 = !DILocation(line: 11, column: 12, scope: !8)
!41 = !DILocalVariable(name: "k", scope: !8, file: !1, line: 12, type: !42)
!42 = !DIBasicType(name: "unsigned long long", size: 64, encoding: DW_ATE_unsigned)
!43 = !DILocation(line: 12, column: 21, scope: !8)
!44 = !DILocalVariable(name: "l", scope: !8, file: !1, line: 14, type: !45)
!45 = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
!46 = !DILocation(line: 14, column: 8, scope: !8)
!47 = !DILocalVariable(name: "m", scope: !8, file: !1, line: 15, type: !48)
!48 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
!49 = !DILocation(line: 15, column: 9, scope: !8)
!50 = !DILocalVariable(name: "n", scope: !8, file: !1, line: 16, type: !51)
!51 = !DIBasicType(name: "long double", size: 128, encoding: DW_ATE_float)
!52 = !DILocation(line: 16, column: 14, scope: !8)
!53 = !DILocation(line: 17, column: 2, scope: !8)
