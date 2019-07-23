Adding a new pass checklist:

- [ ] create a new `.cpp` file here in `parser/` folder
- [ ] name the file in `lowerCamelCase`
- [ ] create a non-static function with the same name as the file (without `.cpp`)
- [ ] add the function declaration to `passes.h`
- [ ] add a new item into `../core/PassType.h` and clarify dependencies of your pass on other passes
- [ ] call the pass from `ProgramParser.cpp`
