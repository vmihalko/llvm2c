; ModuleID = 'basic_types_init_out_function.c'
source_filename = "basic_types_init_out_function.c"
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
@n = dso_local global x86_fp80 0xKBFFCC28F5C28F5C29000, align 16, !dbg !41

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !50 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  ret i32 1, !dbg !54
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!44, !45, !46, !47, !48}
!llvm.ident = !{!49}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "a", scope: !2, file: !3, line: 1, type: !7, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 14.0.5 (Fedora 14.0.5-2.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !4, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "basic_types_init_out_function.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/types_debuginfo/basic_types")
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
!54 = !DILocation(line: 17, column: 2, scope: !50)
