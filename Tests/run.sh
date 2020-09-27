echo "\nStarting testing...\n"

./XML-parser.out test_1_ok_example_from_web.xml
./XML-parser.out test_2_ok_example_from_web.xml
./XML-parser.out test_3_ok_example_from_web.xml
./XML-parser.out test_4_ok_example_from_web.xml
./XML-parser.out test_5_ok_example_from_web.xml
./XML-parser.out test_6_ok_example_from_web.xml
./XML-parser.out test_7_ok_example_from_web.xml
./XML-parser.out test_8_ok_example_from_web.xml
./XML-parser.out test_9_ok_example_from_web.xml
./XML-parser.out test_10_ok_example_from_web.xml
./XML-parser.out test_11_fail_improperly_nested.xml
./XML-parser.out test_12_fail_not_closed_tags.xml
./XML-parser.out test_13_fail_tag_name_error_not_closed_comment.xml
./XML-parser.out test_14_ok_just_one_root.xml
./XML-parser.out test_15_fail_single_open_tag.xml
./XML-parser.out test_16_fail_single_close_tag.xml
./XML-parser.out test_17_fail_double_root.xml
./XML-parser.out test_18_fail_empty.xml
./XML-parser.out test_19_fail_unescaped_char_tag_have_no_pair.xml
./XML-parser.out test_20_fail_malformatted_comment.xml

echo "Tests are completed.\n"