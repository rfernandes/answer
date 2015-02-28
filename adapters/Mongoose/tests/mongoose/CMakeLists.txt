add_test(NAME mongoose COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/mongoose.sh $<TARGET_FILE:answer_mongoose> $<TARGET_FILE_DIR:BasicService>)

set_tests_properties(mongoose PROPERTIES TIMEOUT 8)

