# 单元测试：只链接 mediacore（C++ 逻辑），不需要启动 QML 界面
find_package(Qt6 REQUIRED COMPONENTS Test)

# 添加一个测试：tst_xxx.cpp -> 可执行文件 tst_xxx，并注册到 ctest
function(media_add_test name)
    qt_add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE mediacore Qt6::Test)
    # 测试用的 JSON 样例放在 tests/data/ 下，通过宏告诉测试代码它在哪
    target_compile_definitions(${name} PRIVATE
        TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
    add_test(NAME ${name} COMMAND ${name})
endfunction()

media_add_test(tst_mediajsonloader)
media_add_test(tst_mediafiltermodel)
media_add_test(tst_libraryviewmodel)
