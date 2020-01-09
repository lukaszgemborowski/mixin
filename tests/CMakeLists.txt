add_executable(
    tests
    main.cxx
    basic.cxx
    tuple_select.cxx
    ability.cxx
)

target_include_directories(
    tests
    PRIVATE
    ${project-root}/3rd/catch2/
)

target_link_libraries(
    tests
    PRIVATE
    mixin
)
