; ModuleID = 'pointers-global-various.c'
source_filename = "pointers-global-various.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

@ip = dso_local global i32* null, align 8, !dbg !0
@.str = private unnamed_addr constant [6 x i8] c"hello\00", align 1
@cp = dso_local global i8* null, align 8, !dbg !8
@vp2 = dso_local global i8* null, align 8, !dbg !5
@func_ptr = dso_local global i32 (i32, i32)* null, align 8, !dbg !12

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @add(i32 noundef %0, i32 noundef %1) #0 !dbg !25 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  call void @llvm.dbg.declare(metadata i32* %3, metadata !27, metadata !DIExpression()), !dbg !28
  store i32 %1, i32* %4, align 4
  call void @llvm.dbg.declare(metadata i32* %4, metadata !29, metadata !DIExpression()), !dbg !30
  %5 = load i32, i32* %3, align 4, !dbg !31
  %6 = load i32, i32* %4, align 4, !dbg !32
  %7 = add nsw i32 %5, %6, !dbg !33
  ret i32 %7, !dbg !34
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !35 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32* null, i32** @ip, align 8, !dbg !38
  store i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str, i64 0, i64 0), i8** @cp, align 8, !dbg !39
  store i8* null, i8** @vp2, align 8, !dbg !40
  store i32 (i32, i32)* @add, i32 (i32, i32)** @func_ptr, align 8, !dbg !41
  ret i32 0, !dbg !42
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!19, !20, !21, !22, !23}
!llvm.ident = !{!24}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "ip", scope: !2, file: !3, line: 2, type: !18, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 14.0.5 (Fedora 14.0.5-2.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !4, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "pointers-global-various.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/types_debuginfo/derived_types")
!4 = !{!0, !5, !8, !12}
!5 = !DIGlobalVariableExpression(var: !6, expr: !DIExpression())
!6 = distinct !DIGlobalVariable(name: "vp2", scope: !2, file: !3, line: 3, type: !7, isLocal: false, isDefinition: true)
!7 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!8 = !DIGlobalVariableExpression(var: !9, expr: !DIExpression())
!9 = distinct !DIGlobalVariable(name: "cp", scope: !2, file: !3, line: 4, type: !10, isLocal: false, isDefinition: true)
!10 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64)
!11 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!12 = !DIGlobalVariableExpression(var: !13, expr: !DIExpression())
!13 = distinct !DIGlobalVariable(name: "func_ptr", scope: !2, file: !3, line: 5, type: !14, isLocal: false, isDefinition: true)
!14 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !15, size: 64)
!15 = !DISubroutineType(types: !16)
!16 = !{!17, !17, !17}
!17 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!18 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !17, size: 64)
!19 = !{i32 7, !"Dwarf Version", i32 4}
!20 = !{i32 2, !"Debug Info Version", i32 3}
!21 = !{i32 1, !"wchar_size", i32 4}
!22 = !{i32 7, !"uwtable", i32 1}
!23 = !{i32 7, !"frame-pointer", i32 2}
!24 = !{!"clang version 14.0.5 (Fedora 14.0.5-2.fc36)"}
!25 = distinct !DISubprogram(name: "add", scope: !3, file: !3, line: 8, type: !15, scopeLine: 8, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !26)
!26 = !{}
!27 = !DILocalVariable(name: "a", arg: 1, scope: !25, file: !3, line: 8, type: !17)
!28 = !DILocation(line: 8, column: 13, scope: !25)
!29 = !DILocalVariable(name: "b", arg: 2, scope: !25, file: !3, line: 8, type: !17)
!30 = !DILocation(line: 8, column: 20, scope: !25)
!31 = !DILocation(line: 9, column: 12, scope: !25)
!32 = !DILocation(line: 9, column: 16, scope: !25)
!33 = !DILocation(line: 9, column: 14, scope: !25)
!34 = !DILocation(line: 9, column: 5, scope: !25)
!35 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 12, type: !36, scopeLine: 12, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !26)
!36 = !DISubroutineType(types: !37)
!37 = !{!17}
!38 = !DILocation(line: 14, column: 8, scope: !35)
!39 = !DILocation(line: 15, column: 8, scope: !35)
!40 = !DILocation(line: 16, column: 9, scope: !35)
!41 = !DILocation(line: 17, column: 14, scope: !35)
!42 = !DILocation(line: 18, column: 5, scope: !35)
