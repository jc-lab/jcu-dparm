if (NOT TARGET jcu-random)
    FetchContent_Declare(
            jcu_random
            GIT_REPOSITORY https://github.com/jc-lab/jcu-random.git
            GIT_TAG v0.0.1
    )

    FetchContent_GetProperties(jcu_random)

    if (NOT jcu_random_POPULATED)
        FetchContent_Populate(jcu_random)
        add_subdirectory(${jcu_random_SOURCE_DIR} ${jcu_random_BINARY_DIR})
    endif ()
endif ()
