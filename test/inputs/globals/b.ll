; ModuleID = 'init_by_func.c'
source_filename = "init_by_func.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

%struct.mystruct = type { i32 (i32)* }

@test = dso_local global %struct.mystruct { i32 (i32)* @square }, align 8, !dbg !0

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @square(i32 noundef %0) #0 !dbg !18 {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  call void @llvm.dbg.declare(metadata i32* %2, metadata !20, metadata !DIExpression()), !dbg !21
  %3 = load i32, i32* %2, align 4, !dbg !22
  %4 = load i32, i32* %2, align 4, !dbg !23
  %5 = mul nsw i32 %3, %4, !dbg !24
  ret i32 %5, !dbg !25
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main(i32 noundef %0, i8** noundef %1) #0 !dbg !26 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i8**, align 8
  %6 = alloca i8*, align 8
  %7 = alloca i64, align 8
  store i32 0, i32* %3, align 4
  store i32 %0, i32* %4, align 4
  call void @llvm.dbg.declare(metadata i32* %4, metadata !32, metadata !DIExpression()), !dbg !33
  store i8** %1, i8*** %5, align 8
  call void @llvm.dbg.declare(metadata i8*** %5, metadata !34, metadata !DIExpression()), !dbg !35
  %8 = load i32, i32* %4, align 4, !dbg !36
  %9 = icmp ne i32 %8, 2, !dbg !38
  br i1 %9, label %10, label %11, !dbg !39

10:                                               ; preds = %2
  store i32 -1, i32* %3, align 4, !dbg !40
  br label %26, !dbg !40

11:                                               ; preds = %2
  call void @llvm.dbg.declare(metadata i8** %6, metadata !42, metadata !DIExpression()), !dbg !43
  call void @llvm.dbg.declare(metadata i64* %7, metadata !44, metadata !DIExpression()), !dbg !46
  %12 = load i8**, i8*** %5, align 8, !dbg !47
  %13 = getelementptr inbounds i8*, i8** %12, i64 1, !dbg !47
  %14 = load i8*, i8** %13, align 8, !dbg !47
  %15 = call i64 @strtol(i8* noundef %14, i8** noundef %6, i32 noundef 10), !dbg !48
  store i64 %15, i64* %7, align 8, !dbg !46
  %16 = load i8*, i8** %6, align 8, !dbg !49
  %17 = load i8, i8* %16, align 1, !dbg !51
  %18 = sext i8 %17 to i32, !dbg !51
  %19 = icmp ne i32 %18, 0, !dbg !52
  br i1 %19, label %20, label %21, !dbg !53

20:                                               ; preds = %11
  store i32 -1, i32* %3, align 4, !dbg !54
  br label %26, !dbg !54

21:                                               ; preds = %11
  %22 = load i32 (i32)*, i32 (i32)** getelementptr inbounds (%struct.mystruct, %struct.mystruct* @test, i32 0, i32 0), align 8, !dbg !56
  %23 = load i64, i64* %7, align 8, !dbg !58
  %24 = trunc i64 %23 to i32, !dbg !58
  %25 = call i32 %22(i32 noundef %24), !dbg !59
  store i32 %25, i32* %3, align 4, !dbg !60
  br label %26, !dbg !60

26:                                               ; preds = %21, %20, %10
  %27 = load i32, i32* %3, align 4, !dbg !61
  ret i32 %27, !dbg !61
}

declare dso_local i64 @strtol(i8* noundef, i8** noundef, i32 noundef) #2

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!12, !13, !14, !15, !16}
!llvm.ident = !{!17}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "test", scope: !2, file: !3, line: 10, type: !5, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 14.0.5 (Fedora 14.0.5-1.fc36)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !4, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "init_by_func.c", directory: "/home/vmihalko/DIPLO/llvm2c/test/inputs/globals")
!4 = !{!0}
!5 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "mystruct", file: !3, line: 6, size: 64, elements: !6)
!6 = !{!7}
!7 = !DIDerivedType(tag: DW_TAG_member, name: "myfunc", scope: !5, file: !3, line: 7, baseType: !8, size: 64)
!8 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !9, size: 64)
!9 = !DISubroutineType(types: !10)
!10 = !{!11, !11}
!11 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!12 = !{i32 7, !"Dwarf Version", i32 4}
!13 = !{i32 2, !"Debug Info Version", i32 3}
!14 = !{i32 1, !"wchar_size", i32 4}
!15 = !{i32 7, !"uwtable", i32 1}
!16 = !{i32 7, !"frame-pointer", i32 2}
!17 = !{!"clang version 14.0.5 (Fedora 14.0.5-1.fc36)"}
!18 = distinct !DISubprogram(name: "square", scope: !3, file: !3, line: 2, type: !9, scopeLine: 2, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !19)
!19 = !{}
!20 = !DILocalVariable(name: "x", arg: 1, scope: !18, file: !3, line: 2, type: !11)
!21 = !DILocation(line: 2, column: 16, scope: !18)
!22 = !DILocation(line: 3, column: 12, scope: !18)
!23 = !DILocation(line: 3, column: 16, scope: !18)
!24 = !DILocation(line: 3, column: 14, scope: !18)
!25 = !DILocation(line: 3, column: 5, scope: !18)
!26 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 12, type: !27, scopeLine: 13, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !19)
!27 = !DISubroutineType(types: !28)
!28 = !{!11, !11, !29}
!29 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !30, size: 64)
!30 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !31, size: 64)
!31 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!32 = !DILocalVariable(name: "argc", arg: 1, scope: !26, file: !3, line: 12, type: !11)
!33 = !DILocation(line: 12, column: 14, scope: !26)
!34 = !DILocalVariable(name: "argv", arg: 2, scope: !26, file: !3, line: 12, type: !29)
!35 = !DILocation(line: 12, column: 26, scope: !26)
!36 = !DILocation(line: 14, column: 9, scope: !37)
!37 = distinct !DILexicalBlock(scope: !26, file: !3, line: 14, column: 9)
!38 = !DILocation(line: 14, column: 14, scope: !37)
!39 = !DILocation(line: 14, column: 9, scope: !26)
!40 = !DILocation(line: 15, column: 9, scope: !41)
!41 = distinct !DILexicalBlock(scope: !37, file: !3, line: 14, column: 20)
!42 = !DILocalVariable(name: "p", scope: !26, file: !3, line: 18, type: !30)
!43 = !DILocation(line: 18, column: 8, scope: !26)
!44 = !DILocalVariable(name: "num", scope: !26, file: !3, line: 19, type: !45)
!45 = !DIBasicType(name: "long", size: 64, encoding: DW_ATE_signed)
!46 = !DILocation(line: 19, column: 7, scope: !26)
!47 = !DILocation(line: 19, column: 20, scope: !26)
!48 = !DILocation(line: 19, column: 13, scope: !26)
!49 = !DILocation(line: 21, column: 7, scope: !50)
!50 = distinct !DILexicalBlock(scope: !26, file: !3, line: 21, column: 6)
!51 = !DILocation(line: 21, column: 6, scope: !50)
!52 = !DILocation(line: 21, column: 9, scope: !50)
!53 = !DILocation(line: 21, column: 6, scope: !26)
!54 = !DILocation(line: 22, column: 3, scope: !55)
!55 = distinct !DILexicalBlock(scope: !50, file: !3, line: 21, column: 18)
!56 = !DILocation(line: 24, column: 15, scope: !57)
!57 = distinct !DILexicalBlock(scope: !50, file: !3, line: 23, column: 9)
!58 = !DILocation(line: 24, column: 22, scope: !57)
!59 = !DILocation(line: 24, column: 10, scope: !57)
!60 = !DILocation(line: 24, column: 3, scope: !57)
!61 = !DILocation(line: 26, column: 1, scope: !26)
