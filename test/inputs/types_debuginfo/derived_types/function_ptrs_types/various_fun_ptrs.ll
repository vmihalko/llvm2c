; ModuleID = 'various_fun_ptrs.c'
source_filename = "various_fun_ptrs.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

@func_ptr = dso_local global i32 (i32, i32)* null, align 8, !dbg !0
@intintint = dso_local global i32 (i32, i32)* null, align 8, !dbg !5
@voidint = dso_local global void (i32)* null, align 8, !dbg !11
@voidvoid = dso_local global void ()* null, align 8, !dbg !16
@pintvoid = dso_local global i32* ()* null, align 8, !dbg !21
@ppppintvoid = dso_local global i32**** ()* null, align 8, !dbg !27
@intppppchar = dso_local global i32 (i8****)* null, align 8, !dbg !35
@pintirray = dso_local global i32* (i32*)* null, align 8, !dbg !45

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @add(i32 noundef %0, i32 noundef %1) #0 !dbg !56 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  call void @llvm.dbg.declare(metadata i32* %3, metadata !58, metadata !DIExpression()), !dbg !59
  store i32 %1, i32* %4, align 4
  call void @llvm.dbg.declare(metadata i32* %4, metadata !60, metadata !DIExpression()), !dbg !61
  %5 = load i32, i32* %3, align 4, !dbg !62
  %6 = load i32, i32* %4, align 4, !dbg !63
  %7 = add nsw i32 %5, %6, !dbg !64
  ret i32 %7, !dbg !65
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !66 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 (i32, i32)* @add, i32 (i32, i32)** @func_ptr, align 8, !dbg !69
  %2 = load i32 (i32, i32)*, i32 (i32, i32)** @func_ptr, align 8, !dbg !70
  %3 = call i32 %2(i32 noundef 2, i32 noundef 3), !dbg !71
  ret i32 %3, !dbg !72
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!50, !51, !52, !53, !54}
!llvm.ident = !{!55}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "func_ptr", scope: !2, file: !3, line: 1, type: !7, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 14.0.5 (Fedora 14.0.5-2.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !4, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "various_fun_ptrs.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/types_debuginfo/derived_types/function_ptrs_types")
!4 = !{!0, !5, !11, !16, !21, !27, !35, !45}
!5 = !DIGlobalVariableExpression(var: !6, expr: !DIExpression())
!6 = distinct !DIGlobalVariable(name: "intintint", scope: !2, file: !3, line: 2, type: !7, isLocal: false, isDefinition: true)
!7 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !8, size: 64)
!8 = !DISubroutineType(types: !9)
!9 = !{!10, !10, !10}
!10 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!11 = !DIGlobalVariableExpression(var: !12, expr: !DIExpression())
!12 = distinct !DIGlobalVariable(name: "voidint", scope: !2, file: !3, line: 3, type: !13, isLocal: false, isDefinition: true)
!13 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !14, size: 64)
!14 = !DISubroutineType(types: !15)
!15 = !{null, !10}
!16 = !DIGlobalVariableExpression(var: !17, expr: !DIExpression())
!17 = distinct !DIGlobalVariable(name: "voidvoid", scope: !2, file: !3, line: 4, type: !18, isLocal: false, isDefinition: true)
!18 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !19, size: 64)
!19 = !DISubroutineType(types: !20)
!20 = !{null}
!21 = !DIGlobalVariableExpression(var: !22, expr: !DIExpression())
!22 = distinct !DIGlobalVariable(name: "pintvoid", scope: !2, file: !3, line: 5, type: !23, isLocal: false, isDefinition: true)
!23 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !24, size: 64)
!24 = !DISubroutineType(types: !25)
!25 = !{!26}
!26 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !10, size: 64)
!27 = !DIGlobalVariableExpression(var: !28, expr: !DIExpression())
!28 = distinct !DIGlobalVariable(name: "ppppintvoid", scope: !2, file: !3, line: 6, type: !29, isLocal: false, isDefinition: true)
!29 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !30, size: 64)
!30 = !DISubroutineType(types: !31)
!31 = !{!32}
!32 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !33, size: 64)
!33 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !34, size: 64)
!34 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !26, size: 64)
!35 = !DIGlobalVariableExpression(var: !36, expr: !DIExpression())
!36 = distinct !DIGlobalVariable(name: "intppppchar", scope: !2, file: !3, line: 7, type: !37, isLocal: false, isDefinition: true)
!37 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !38, size: 64)
!38 = !DISubroutineType(types: !39)
!39 = !{!10, !40}
!40 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !41, size: 64)
!41 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !42, size: 64)
!42 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !43, size: 64)
!43 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !44, size: 64)
!44 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!45 = !DIGlobalVariableExpression(var: !46, expr: !DIExpression())
!46 = distinct !DIGlobalVariable(name: "pintirray", scope: !2, file: !3, line: 8, type: !47, isLocal: false, isDefinition: true)
!47 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !48, size: 64)
!48 = !DISubroutineType(types: !49)
!49 = !{!26, !26}
!50 = !{i32 7, !"Dwarf Version", i32 4}
!51 = !{i32 2, !"Debug Info Version", i32 3}
!52 = !{i32 1, !"wchar_size", i32 4}
!53 = !{i32 7, !"uwtable", i32 1}
!54 = !{i32 7, !"frame-pointer", i32 2}
!55 = !{!"clang version 14.0.5 (Fedora 14.0.5-2.fc36)"}
!56 = distinct !DISubprogram(name: "add", scope: !3, file: !3, line: 12, type: !8, scopeLine: 12, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !57)
!57 = !{}
!58 = !DILocalVariable(name: "a", arg: 1, scope: !56, file: !3, line: 12, type: !10)
!59 = !DILocation(line: 12, column: 13, scope: !56)
!60 = !DILocalVariable(name: "b", arg: 2, scope: !56, file: !3, line: 12, type: !10)
!61 = !DILocation(line: 12, column: 20, scope: !56)
!62 = !DILocation(line: 13, column: 12, scope: !56)
!63 = !DILocation(line: 13, column: 16, scope: !56)
!64 = !DILocation(line: 13, column: 14, scope: !56)
!65 = !DILocation(line: 13, column: 5, scope: !56)
!66 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 16, type: !67, scopeLine: 16, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !57)
!67 = !DISubroutineType(types: !68)
!68 = !{!10}
!69 = !DILocation(line: 17, column: 11, scope: !66)
!70 = !DILocation(line: 18, column: 11, scope: !66)
!71 = !DILocation(line: 18, column: 9, scope: !66)
!72 = !DILocation(line: 18, column: 2, scope: !66)
