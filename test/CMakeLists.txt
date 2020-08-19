set(PROJECT_PREFIX jcu-dparm-)

set(CRYPTO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src/crypto)
set(CRYPTO_TEST_TARGET ${PROJECT_PREFIX}crypto_test)
add_executable(${CRYPTO_TEST_TARGET} ${CMAKE_CURRENT_SOURCE_DIR}/crypto_test.cc
        ${CRYPTO_SRC_DIR}/hash.cc
        ${CRYPTO_SRC_DIR}/hash.h
        ${CRYPTO_SRC_DIR}/hash_sha_1.cc
        ${CRYPTO_SRC_DIR}/hash_sha_1.h
        ${CRYPTO_SRC_DIR}/pbkdf2.cc
        ${CRYPTO_SRC_DIR}/pbkdf2.h
        )
target_link_libraries(${CRYPTO_TEST_TARGET}
        PRIVATE
        gtest
        gmock
        gtest_main
        )
add_test(NAME ${CRYPTO_TEST_TARGET}-gtest COMMAND ${CRYPTO_TEST_TARGET})

