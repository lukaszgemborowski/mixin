add_executable(
    tests
    main.cxx)

target_include_directories(
    tests
    PRIVATE
    ${project-root}/3rd/catch2/
)
