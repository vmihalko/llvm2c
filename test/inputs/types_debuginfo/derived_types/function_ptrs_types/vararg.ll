; ModuleID = 'vararg.c'
source_filename = "vararg.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

%struct.__va_list_tag = type { i32, i32, i8*, i8* }

@func_ptr = dso_local global i32 (i32, ...)* @print_args, align 8, !dbg !0

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @print_args(i32 noundef %0, ...) #0 !dbg !15 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca [1 x %struct.__va_list_tag], align 16
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  call void @llvm.dbg.declare(metadata i32* %2, metadata !17, metadata !DIExpression()), !dbg !18
  call void @llvm.dbg.declare(metadata i32* %3, metadata !19, metadata !DIExpression()), !dbg !20
  store i32 0, i32* %3, align 4, !dbg !20
  call void @llvm.dbg.declare(metadata [1 x %struct.__va_list_tag]* %4, metadata !21, metadata !DIExpression()), !dbg !36
  %7 = getelementptr inbounds [1 x %struct.__va_list_tag], [1 x %struct.__va_list_tag]* %4, i64 0, i64 0, !dbg !37
  %8 = bitcast %struct.__va_list_tag* %7 to i8*, !dbg !37
  call void @llvm.va_start(i8* %8), !dbg !37
  call void @llvm.dbg.declare(metadata i32* %5, metadata !38, metadata !DIExpression()), !dbg !40
  store i32 0, i32* %5, align 4, !dbg !40
  br label %9, !dbg !41

9:                                                ; preds = %35, %1
  %10 = load i32, i32* %5, align 4, !dbg !42
  %11 = load i32, i32* %2, align 4, !dbg !44
  %12 = icmp slt i32 %10, %11, !dbg !45
  br i1 %12, label %13, label %38, !dbg !46

13:                                               ; preds = %9
  call void @llvm.dbg.declare(metadata i32* %6, metadata !47, metadata !DIExpression()), !dbg !49
  %14 = getelementptr inbounds [1 x %struct.__va_list_tag], [1 x %struct.__va_list_tag]* %4, i64 0, i64 0, !dbg !50
  %15 = getelementptr inbounds %struct.__va_list_tag, %struct.__va_list_tag* %14, i32 0, i32 0, !dbg !50
  %16 = load i32, i32* %15, align 16, !dbg !50
  %17 = icmp ule i32 %16, 40, !dbg !50
  br i1 %17, label %18, label %24, !dbg !50

18:                                               ; preds = %13
  %19 = getelementptr inbounds %struct.__va_list_tag, %struct.__va_list_tag* %14, i32 0, i32 3, !dbg !50
  %20 = load i8*, i8** %19, align 16, !dbg !50
  %21 = getelementptr i8, i8* %20, i32 %16, !dbg !50
  %22 = bitcast i8* %21 to i32*, !dbg !50
  %23 = add i32 %16, 8, !dbg !50
  store i32 %23, i32* %15, align 16, !dbg !50
  br label %29, !dbg !50

24:                                               ; preds = %13
  %25 = getelementptr inbounds %struct.__va_list_tag, %struct.__va_list_tag* %14, i32 0, i32 2, !dbg !50
  %26 = load i8*, i8** %25, align 8, !dbg !50
  %27 = bitcast i8* %26 to i32*, !dbg !50
  %28 = getelementptr i8, i8* %26, i32 8, !dbg !50
  store i8* %28, i8** %25, align 8, !dbg !50
  br label %29, !dbg !50

29:                                               ; preds = %24, %18
  %30 = phi i32* [ %22, %18 ], [ %27, %24 ], !dbg !50
  %31 = load i32, i32* %30, align 4, !dbg !50
  store i32 %31, i32* %6, align 4, !dbg !49
  %32 = load i32, i32* %6, align 4, !dbg !51
  %33 = load i32, i32* %3, align 4, !dbg !52
  %34 = add nsw i32 %33, %32, !dbg !52
  store i32 %34, i32* %3, align 4, !dbg !52
  br label %35, !dbg !53

35:                                               ; preds = %29
  %36 = load i32, i32* %5, align 4, !dbg !54
  %37 = add nsw i32 %36, 1, !dbg !54
  store i32 %37, i32* %5, align 4, !dbg !54
  br label %9, !dbg !55, !llvm.loop !56

38:                                               ; preds = %9
  %39 = getelementptr inbounds [1 x %struct.__va_list_tag], [1 x %struct.__va_list_tag]* %4, i64 0, i64 0, !dbg !59
  %40 = bitcast %struct.__va_list_tag* %39 to i8*, !dbg !59
  call void @llvm.va_end(i8* %40), !dbg !59
  %41 = load i32, i32* %3, align 4, !dbg !60
  ret i32 %41, !dbg !61
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: nofree nosync nounwind willreturn
declare void @llvm.va_start(i8*) #2

; Function Attrs: nofree nosync nounwind willreturn
declare void @llvm.va_end(i8*) #2

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !62 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %2 = load i32 (i32, ...)*, i32 (i32, ...)** @func_ptr, align 8, !dbg !65
  %3 = call i32 (i32, ...) %2(i32 noundef 3, i32 noundef 1, i32 noundef 2, i32 noundef 3), !dbg !66
  ret i32 %3, !dbg !67
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { nofree nosync nounwind willreturn }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!9, !10, !11, !12, !13}
!llvm.ident = !{!14}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "func_ptr", scope: !2, file: !3, line: 15, type: !5, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 14.0.5 (Fedora 14.0.5-2.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !4, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "vararg.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/types_debuginfo/derived_types/function_ptrs_types")
!4 = !{!0}
!5 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !6, size: 64)
!6 = !DISubroutineType(types: !7)
!7 = !{!8, !8, null}
!8 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!9 = !{i32 7, !"Dwarf Version", i32 4}
!10 = !{i32 2, !"Debug Info Version", i32 3}
!11 = !{i32 1, !"wchar_size", i32 4}
!12 = !{i32 7, !"uwtable", i32 1}
!13 = !{i32 7, !"frame-pointer", i32 2}
!14 = !{!"clang version 14.0.5 (Fedora 14.0.5-2.fc36)"}
!15 = distinct !DISubprogram(name: "print_args", scope: !3, file: !3, line: 3, type: !6, scopeLine: 3, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !16)
!16 = !{}
!17 = !DILocalVariable(name: "count", arg: 1, scope: !15, file: !3, line: 3, type: !8)
!18 = !DILocation(line: 3, column: 20, scope: !15)
!19 = !DILocalVariable(name: "r", scope: !15, file: !3, line: 4, type: !8)
!20 = !DILocation(line: 4, column: 9, scope: !15)
!21 = !DILocalVariable(name: "args", scope: !15, file: !3, line: 5, type: !22)
!22 = !DIDerivedType(tag: DW_TAG_typedef, name: "va_list", file: !23, line: 14, baseType: !24)
!23 = !DIFile(filename: "/usr/lib64/clang/14.0.5/include/stdarg.h", directory: "")
!24 = !DIDerivedType(tag: DW_TAG_typedef, name: "__builtin_va_list", file: !3, baseType: !25)
!25 = !DICompositeType(tag: DW_TAG_array_type, baseType: !26, size: 192, elements: !34)
!26 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "__va_list_tag", size: 192, elements: !27)
!27 = !{!28, !30, !31, !33}
!28 = !DIDerivedType(tag: DW_TAG_member, name: "gp_offset", scope: !26, file: !3, line: 5, baseType: !29, size: 32)
!29 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!30 = !DIDerivedType(tag: DW_TAG_member, name: "fp_offset", scope: !26, file: !3, line: 5, baseType: !29, size: 32, offset: 32)
!31 = !DIDerivedType(tag: DW_TAG_member, name: "overflow_arg_area", scope: !26, file: !3, line: 5, baseType: !32, size: 64, offset: 64)
!32 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!33 = !DIDerivedType(tag: DW_TAG_member, name: "reg_save_area", scope: !26, file: !3, line: 5, baseType: !32, size: 64, offset: 128)
!34 = !{!35}
!35 = !DISubrange(count: 1)
!36 = !DILocation(line: 5, column: 13, scope: !15)
!37 = !DILocation(line: 6, column: 5, scope: !15)
!38 = !DILocalVariable(name: "i", scope: !39, file: !3, line: 7, type: !8)
!39 = distinct !DILexicalBlock(scope: !15, file: !3, line: 7, column: 5)
!40 = !DILocation(line: 7, column: 14, scope: !39)
!41 = !DILocation(line: 7, column: 10, scope: !39)
!42 = !DILocation(line: 7, column: 21, scope: !43)
!43 = distinct !DILexicalBlock(scope: !39, file: !3, line: 7, column: 5)
!44 = !DILocation(line: 7, column: 25, scope: !43)
!45 = !DILocation(line: 7, column: 23, scope: !43)
!46 = !DILocation(line: 7, column: 5, scope: !39)
!47 = !DILocalVariable(name: "arg", scope: !48, file: !3, line: 8, type: !8)
!48 = distinct !DILexicalBlock(scope: !43, file: !3, line: 7, column: 37)
!49 = !DILocation(line: 8, column: 13, scope: !48)
!50 = !DILocation(line: 8, column: 19, scope: !48)
!51 = !DILocation(line: 9, column: 7, scope: !48)
!52 = !DILocation(line: 9, column: 4, scope: !48)
!53 = !DILocation(line: 10, column: 5, scope: !48)
!54 = !DILocation(line: 7, column: 33, scope: !43)
!55 = !DILocation(line: 7, column: 5, scope: !43)
!56 = distinct !{!56, !46, !57, !58}
!57 = !DILocation(line: 10, column: 5, scope: !39)
!58 = !{!"llvm.loop.mustprogress"}
!59 = !DILocation(line: 11, column: 5, scope: !15)
!60 = !DILocation(line: 12, column: 12, scope: !15)
!61 = !DILocation(line: 12, column: 5, scope: !15)
!62 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 17, type: !63, scopeLine: 17, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !16)
!63 = !DISubroutineType(types: !64)
!64 = !{!8}
!65 = !DILocation(line: 18, column: 11, scope: !62)
!66 = !DILocation(line: 18, column: 9, scope: !62)
!67 = !DILocation(line: 18, column: 2, scope: !62)
