add_executable(
    tests
    main.cxx
    basic.cxx
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
