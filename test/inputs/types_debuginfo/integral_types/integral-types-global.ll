; ModuleID = 'integral-types-global.c'
source_filename = "integral-types-global.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

@c = dso_local global i8 0, align 1, !dbg !0
@uc = dso_local global i8 0, align 1, !dbg !5
@s = dso_local global i16 0, align 2, !dbg !8
@us = dso_local global i16 0, align 2, !dbg !11
@i = dso_local global i32 0, align 4, !dbg !14
@ui = dso_local global i32 0, align 4, !dbg !17
@l = dso_local global i64 0, align 8, !dbg !20
@ul = dso_local global i64 0, align 8, !dbg !23
@ll = dso_local global i64 0, align 8, !dbg !26
@ull = dso_local global i64 0, align 8, !dbg !29

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !39 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i8 97, i8* @c, align 1, !dbg !43
  store i8 98, i8* @uc, align 1, !dbg !44
  store i16 32767, i16* @s, align 2, !dbg !45
  store i16 -1, i16* @us, align 2, !dbg !46
  store i32 2147483647, i32* @i, align 4, !dbg !47
  store i32 -1, i32* @ui, align 4, !dbg !48
  store i64 9223372036854775807, i64* @l, align 8, !dbg !49
  store i64 -1, i64* @ul, align 8, !dbg !50
  store i64 9223372036854775807, i64* @ll, align 8, !dbg !51
  store i64 -1, i64* @ull, align 8, !dbg !52
  ret i32 0, !dbg !53
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!33, !34, !35, !36, !37}
!llvm.ident = !{!38}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "c", scope: !2, file: !3, line: 2, type: !32, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 14.0.5 (Fedora 14.0.5-2.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !4, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "integral-types-global.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/types_debuginfo/integral_types")
!4 = !{!0, !5, !8, !11, !14, !17, !20, !23, !26, !29}
!5 = !DIGlobalVariableExpression(var: !6, expr: !DIExpression())
!6 = distinct !DIGlobalVariable(name: "uc", scope: !2, file: !3, line: 3, type: !7, isLocal: false, isDefinition: true)
!7 = !DIBasicType(name: "unsigned char", size: 8, encoding: DW_ATE_unsigned_char)
!8 = !DIGlobalVariableExpression(var: !9, expr: !DIExpression())
!9 = distinct !DIGlobalVariable(name: "s", scope: !2, file: !3, line: 4, type: !10, isLocal: false, isDefinition: true)
!10 = !DIBasicType(name: "short", size: 16, encoding: DW_ATE_signed)
!11 = !DIGlobalVariableExpression(var: !12, expr: !DIExpression())
!12 = distinct !DIGlobalVariable(name: "us", scope: !2, file: !3, line: 5, type: !13, isLocal: false, isDefinition: true)
!13 = !DIBasicType(name: "unsigned short", size: 16, encoding: DW_ATE_unsigned)
!14 = !DIGlobalVariableExpression(var: !15, expr: !DIExpression())
!15 = distinct !DIGlobalVariable(name: "i", scope: !2, file: !3, line: 6, type: !16, isLocal: false, isDefinition: true)
!16 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!17 = !DIGlobalVariableExpression(var: !18, expr: !DIExpression())
!18 = distinct !DIGlobalVariable(name: "ui", scope: !2, file: !3, line: 7, type: !19, isLocal: false, isDefinition: true)
!19 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!20 = !DIGlobalVariableExpression(var: !21, expr: !DIExpression())
!21 = distinct !DIGlobalVariable(name: "l", scope: !2, file: !3, line: 8, type: !22, isLocal: false, isDefinition: true)
!22 = !DIBasicType(name: "long", size: 64, encoding: DW_ATE_signed)
!23 = !DIGlobalVariableExpression(var: !24, expr: !DIExpression())
!24 = distinct !DIGlobalVariable(name: "ul", scope: !2, file: !3, line: 9, type: !25, isLocal: false, isDefinition: true)
!25 = !DIBasicType(name: "unsigned long", size: 64, encoding: DW_ATE_unsigned)
!26 = !DIGlobalVariableExpression(var: !27, expr: !DIExpression())
!27 = distinct !DIGlobalVariable(name: "ll", scope: !2, file: !3, line: 10, type: !28, isLocal: false, isDefinition: true)
!28 = !DIBasicType(name: "long long", size: 64, encoding: DW_ATE_signed)
!29 = !DIGlobalVariableExpression(var: !30, expr: !DIExpression())
!30 = distinct !DIGlobalVariable(name: "ull", scope: !2, file: !3, line: 11, type: !31, isLocal: false, isDefinition: true)
!31 = !DIBasicType(name: "unsigned long long", size: 64, encoding: DW_ATE_unsigned)
!32 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!33 = !{i32 7, !"Dwarf Version", i32 4}
!34 = !{i32 2, !"Debug Info Version", i32 3}
!35 = !{i32 1, !"wchar_size", i32 4}
!36 = !{i32 7, !"uwtable", i32 1}
!37 = !{i32 7, !"frame-pointer", i32 2}
!38 = !{!"clang version 14.0.5 (Fedora 14.0.5-2.fc36)"}
!39 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 13, type: !40, scopeLine: 13, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !42)
!40 = !DISubroutineType(types: !41)
!41 = !{!16}
!42 = !{}
!43 = !DILocation(line: 14, column: 7, scope: !39)
!44 = !DILocation(line: 15, column: 8, scope: !39)
!45 = !DILocation(line: 16, column: 7, scope: !39)
!46 = !DILocation(line: 17, column: 8, scope: !39)
!47 = !DILocation(line: 18, column: 7, scope: !39)
!48 = !DILocation(line: 19, column: 8, scope: !39)
!49 = !DILocation(line: 20, column: 7, scope: !39)
!50 = !DILocation(line: 21, column: 8, scope: !39)
!51 = !DILocation(line: 22, column: 8, scope: !39)
!52 = !DILocation(line: 23, column: 9, scope: !39)
!53 = !DILocation(line: 24, column: 5, scope: !39)
