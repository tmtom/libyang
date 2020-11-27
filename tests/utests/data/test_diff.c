/**
 * @file test_diff.c
 * @author Radek Krejci <rkrejci@cesnet.cz>
 * @author Michal Vasko <mvasko@cesnet.cz>
 * @brief tests for lyd_diff()
 *
 * Copyright (c) 2020 CESNET, z.s.p.o.
 *
 * This source code is licensed under BSD 3-Clause License (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://opensource.org/licenses/BSD-3-Clause
 */

#include "libyang.h"
#include "tests/config.h"
#include "utests.h"


#define CHECK_PARSE_LYD_DIFF(INPUT_1, INPUT_2, OUT_MODEL) \
                assert_int_equal(LY_SUCCESS, lyd_diff_siblings(INPUT_1, INPUT_2, 0, &OUT_MODEL));\
                assert_non_null(OUT_MODEL)

#define TEST_DIFF_3(XML_1, XML_2, XML_3, DIFF_1, DIFF_2, MERGE) \
                { \
                    CONTEXT_CREATE_PATH(TESTS_DIR_MODULES_YANG);\
                    assert_non_null(ly_ctx_load_module(CONTEXT_GET, "ietf-netconf-acm", "2018-02-14", NULL));\
                    assert_int_equal(LY_SUCCESS, lys_parse_mem(CONTEXT_GET, schema, LYS_IN_YANG, NULL));\
    /*decladation*/\
                    struct lyd_node *model_1;\
                    struct lyd_node *model_2;\
                    struct lyd_node *model_3;\
    /*create*/\
                    CHECK_PARSE_LYD(XML_1, LYD_XML, LYD_PARSE_ONLY, 0, LY_SUCCESS, model_1);\
                    CHECK_PARSE_LYD(XML_2, LYD_XML, LYD_PARSE_ONLY, 0, LY_SUCCESS, model_2);\
                    CHECK_PARSE_LYD(XML_3, LYD_XML, LYD_PARSE_ONLY, 0, LY_SUCCESS, model_3);\
    /* diff1 */ \
                    struct lyd_node * diff1;\
                    CHECK_PARSE_LYD_DIFF(model_1, model_2, diff1); \
                    CHECK_LYD_STRING(diff1, DIFF_1, LYD_XML, LYD_PRINT_WITHSIBLINGS); \
                    assert_int_equal(lyd_diff_apply_all(&model_1, diff1), LY_SUCCESS); \
                    CHECK_LYD(model_1, model_2); \
    /* diff2 */ \
                    struct lyd_node * diff2;\
                    CHECK_PARSE_LYD_DIFF(model_2, model_3, diff2); \
                    CHECK_LYD_STRING(diff2, DIFF_2, LYD_XML, LYD_PRINT_WITHSIBLINGS); \
                    assert_int_equal(lyd_diff_apply_all(&model_2, diff2), LY_SUCCESS);\
                    CHECK_LYD(model_2, model_3);\
    /* merge */ \
                    assert_int_equal(lyd_diff_merge_all(&diff1, diff2, 0), LY_SUCCESS);\
                    CHECK_LYD_STRING(diff1, MERGE, LYD_XML, LYD_PRINT_WITHSIBLINGS); \
    /* CREAR ENV */ \
                    CHECK_FREE_LYD(model_1);\
                    CHECK_FREE_LYD(model_2);\
                    CHECK_FREE_LYD(model_3);\
                    CHECK_FREE_LYD(diff1);\
                    CHECK_FREE_LYD(diff2);\
                    CONTEXT_DESTROY;\
                }

const char *schema =
        "module defaults {\n"
        "    yang-version 1.1;\n"
        "    namespace \"urn:libyang:tests:defaults\";\n"
        "    prefix df;\n"
        "\n"
        "    feature unhide;\n"
        "\n"
        "    typedef defint32 {\n"
        "        type int32;\n"
        "        default \"42\";\n"
        "    }\n"
        "\n"
        "    leaf hiddenleaf {\n"
        "        if-feature \"unhide\";\n"
        "        type int32;\n"
        "        default \"42\";\n"
        "    }\n"
        "\n"
        "    container df {\n"
        "        leaf foo {\n"
        "            type defint32;\n"
        "        }\n"
        "\n"
        "        leaf hiddenleaf {\n"
        "            if-feature \"unhide\";\n"
        "            type int32;\n"
        "            default \"42\";\n"
        "        }\n"
        "\n"
        "        container bar {\n"
        "            presence \"\";\n"
        "            leaf hi {\n"
        "                type int32;\n"
        "                default \"42\";\n"
        "            }\n"
        "\n"
        "            leaf ho {\n"
        "                type int32;\n"
        "                mandatory true;\n"
        "            }\n"
        "        }\n"
        "\n"
        "        leaf-list llist {\n"
        "            type defint32;\n"
        "            ordered-by user;\n"
        "        }\n"
        "\n"
        "        leaf-list dllist {\n"
        "            type uint8;\n"
        "            default \"1\";\n"
        "            default \"2\";\n"
        "            default \"3\";\n"
        "        }\n"
        "\n"
        "        list list {\n"
        "            key \"name\";\n"
        "            leaf name {\n"
        "                type string;\n"
        "            }\n"
        "\n"
        "            leaf value {\n"
        "                type int32;\n"
        "                default \"42\";\n"
        "            }\n"
        "        }\n"
        "\n"
        "        choice select {\n"
        "            default \"a\";\n"
        "            case a {\n"
        "                choice a {\n"
        "                    leaf a1 {\n"
        "                        type int32;\n"
        "                        default \"42\";\n"
        "                    }\n"
        "\n"
        "                    leaf a2 {\n"
        "                        type int32;\n"
        "                        default \"24\";\n"
        "                    }\n"
        "                }\n"
        "            }\n"
        "\n"
        "            leaf b {\n"
        "                type string;\n"
        "            }\n"
        "\n"
        "            container c {\n"
        "                presence \"\";\n"
        "                leaf x {\n"
        "                    type int32;\n"
        "                    default \"42\";\n"
        "                }\n"
        "            }\n"
        "        }\n"
        "\n"
        "        choice select2 {\n"
        "            default \"s2b\";\n"
        "            leaf s2a {\n"
        "                type int32;\n"
        "                default \"42\";\n"
        "            }\n"
        "\n"
        "            case s2b {\n"
        "                choice s2b {\n"
        "                    default \"b1\";\n"
        "                    case b1 {\n"
        "                        leaf b1_1 {\n"
        "                            type int32;\n"
        "                            default \"42\";\n"
        "                        }\n"
        "\n"
        "                        leaf b1_2 {\n"
        "                            type string;\n"
        "                        }\n"
        "\n"
        "                        leaf b1_status {\n"
        "                            type int32;\n"
        "                            default \"42\";\n"
        "                            config false;\n"
        "                        }\n"
        "                    }\n"
        "\n"
        "                    leaf b2 {\n"
        "                        type int32;\n"
        "                        default \"42\";\n"
        "                    }\n"
        "                }\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "\n"
        "    container hidden {\n"
        "        leaf foo {\n"
        "            type int32;\n"
        "            default \"42\";\n"
        "        }\n"
        "\n"
        "        leaf baz {\n"
        "            type int32;\n"
        "            default \"42\";\n"
        "        }\n"
        "\n"
        "        leaf papa {\n"
        "            type int32;\n"
        "            default \"42\";\n"
        "            config false;\n"
        "        }\n"
        "    }\n"
        "\n"
        "    rpc rpc1 {\n"
        "        input {\n"
        "            leaf inleaf1 {\n"
        "                type string;\n"
        "            }\n"
        "\n"
        "            leaf inleaf2 {\n"
        "                type string;\n"
        "                default \"def1\";\n"
        "            }\n"
        "        }\n"
        "\n"
        "        output {\n"
        "            leaf outleaf1 {\n"
        "                type string;\n"
        "                default \"def2\";\n"
        "            }\n"
        "\n"
        "            leaf outleaf2 {\n"
        "                type string;\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "\n"
        "    notification notif {\n"
        "        leaf ntfleaf1 {\n"
        "            type string;\n"
        "            default \"def3\";\n"
        "        }\n"
        "\n"
        "        leaf ntfleaf2 {\n"
        "            type string;\n"
        "        }\n"
        "    }\n"
        "}\n";

static void
test_invalid(void **state)
{
    (void) state;
    const char *xml = "<df xmlns=\"urn:libyang:tests:defaults\"><foo>42</foo></df>";

    CONTEXT_CREATE_PATH(TESTS_DIR_MODULES_YANG);
    assert_non_null(ly_ctx_load_module(CONTEXT_GET, "ietf-netconf-acm", "2018-02-14", NULL));
    assert_int_equal(LY_SUCCESS, lys_parse_mem(CONTEXT_GET, schema, LYS_IN_YANG, NULL));

    struct lyd_node *model_1;

    CHECK_PARSE_LYD(xml, LYD_XML, LYD_PARSE_ONLY, 0, LY_SUCCESS, model_1);

    struct lyd_node *diff = NULL;

    assert_int_equal(lyd_diff_siblings(model_1, lyd_child(model_1), 0, &diff), LY_EINVAL);
    assert_int_equal(lyd_diff_siblings(NULL, NULL, 0, NULL), LY_EINVAL);

    CHECK_FREE_LYD(model_1);
    CHECK_FREE_LYD(diff);
    CONTEXT_DESTROY;
}

static void
test_same(void **state)
{
    (void) state;
    const char *xml =
            "<nacm xmlns=\"urn:ietf:params:xml:ns:yang:ietf-netconf-acm\">\n"
            "  <enable-nacm>true</enable-nacm>\n"
            "  <read-default>permit</read-default>\n"
            "  <write-default>deny</write-default>\n"
            "  <exec-default>permit</exec-default>\n"
            "  <enable-external-groups>true</enable-external-groups>\n"
            "</nacm><df xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <foo>42</foo><b1_1>42</b1_1>\n"
            "</df><hidden xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <foo>42</foo><baz>42</baz></hidden>\n";

    CONTEXT_CREATE_PATH(TESTS_DIR_MODULES_YANG);
    assert_non_null(ly_ctx_load_module(CONTEXT_GET, "ietf-netconf-acm", "2018-02-14", NULL));
    assert_int_equal(LY_SUCCESS, lys_parse_mem(CONTEXT_GET, schema, LYS_IN_YANG, NULL));

    struct lyd_node *model_1;
    struct lyd_node *model_2;

    CHECK_PARSE_LYD(xml, LYD_XML, LYD_PARSE_ONLY, 0, LY_SUCCESS, model_1);
    CHECK_PARSE_LYD(xml, LYD_XML, LYD_PARSE_ONLY, 0, LY_SUCCESS, model_2);

    struct lyd_node *diff = NULL;

    assert_int_equal(lyd_diff_siblings(model_1, model_2, 0, &diff), LY_SUCCESS);
    assert_null(diff);
    assert_int_equal(lyd_diff_apply_all(&model_1, diff), LY_SUCCESS);
    CHECK_LYD(model_1, model_2);

    CHECK_FREE_LYD(model_1);
    CHECK_FREE_LYD(model_2);
    CHECK_FREE_LYD(diff);
    CONTEXT_DESTROY;
}

static void
test_empty1(void **state)
{
    (void) state;
    const char *xml_in =
            "<df xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <foo>42</foo>\n"
            "  <b1_1>42</b1_1>\n"
            "</df>\n"
            "<hidden xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <foo>42</foo>\n"
            "  <baz>42</baz>\n"
            "</hidden>\n";

    CONTEXT_CREATE_PATH(TESTS_DIR_MODULES_YANG);
    assert_non_null(ly_ctx_load_module(CONTEXT_GET, "ietf-netconf-acm", "2018-02-14", NULL));
    assert_int_equal(LY_SUCCESS, lys_parse_mem(CONTEXT_GET, schema, LYS_IN_YANG, NULL));

    struct lyd_node *model_1 = NULL;
    struct lyd_node *model_2;

    CHECK_PARSE_LYD(xml_in, LYD_XML, LYD_PARSE_ONLY, 0, LY_SUCCESS, model_2);

    struct lyd_node *diff;

    CHECK_PARSE_LYD_DIFF(model_1, model_2, diff);
    const char *result = "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"create\">\n"
            "  <foo>42</foo>\n"
            "  <b1_1>42</b1_1>\n"
            "</df>\n"
            "<hidden xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"create\">\n"
            "  <foo>42</foo>\n"
            "  <baz>42</baz>\n"
            "</hidden>\n";
    CHECK_LYD_STRING(diff, result, LYD_XML, LYD_PRINT_WITHSIBLINGS);
    assert_int_equal(lyd_diff_apply_all(&model_1, diff), LY_SUCCESS);
    CHECK_LYD(model_1, model_2);

    CHECK_FREE_LYD(model_1);
    CHECK_FREE_LYD(model_2);
    CHECK_FREE_LYD(diff);
    CONTEXT_DESTROY;
}

static void
test_empty2(void **state)
{
    (void) state;
    const char *xml = "<df xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <foo>42</foo>\n"
            "  <b1_1>42</b1_1>\n"
            "</df><hidden xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <foo>42</foo>\n"
            "  <baz>42</baz>\n"
            "</hidden>\n";

    CONTEXT_CREATE_PATH(TESTS_DIR_MODULES_YANG);
    assert_non_null(ly_ctx_load_module(CONTEXT_GET, "ietf-netconf-acm", "2018-02-14", NULL));
    assert_int_equal(LY_SUCCESS, lys_parse_mem(CONTEXT_GET, schema, LYS_IN_YANG, NULL));

    struct lyd_node *model_1;

    CHECK_PARSE_LYD(xml, LYD_XML, LYD_PARSE_ONLY, 0, LY_SUCCESS, model_1);

    struct lyd_node *diff;

    CHECK_PARSE_LYD_DIFF(model_1, NULL, diff);
    const char *result = "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"delete\">\n"
            "  <foo>42</foo>\n"
            "  <b1_1>42</b1_1>\n"
            "</df>\n"
            "<hidden xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"delete\">\n"
            "  <foo>42</foo>\n"
            "  <baz>42</baz>\n"
            "</hidden>\n";
    CHECK_LYD_STRING(diff, result, LYD_XML, LYD_PRINT_WITHSIBLINGS);

    assert_int_equal(lyd_diff_apply_all(&model_1, diff), LY_SUCCESS);
    assert_ptr_equal(model_1, NULL);

    CHECK_FREE_LYD(diff);
    CHECK_FREE_LYD(model_1);
    CONTEXT_DESTROY;
}

static void
test_empty_nested(void **state)
{
    (void) state;
    const char *xml = "<df xmlns=\"urn:libyang:tests:defaults\"><foo>42</foo></df>";

    CONTEXT_CREATE_PATH(TESTS_DIR_MODULES_YANG);
    assert_non_null(ly_ctx_load_module(CONTEXT_GET, "ietf-netconf-acm", "2018-02-14", NULL));
    assert_int_equal(LY_SUCCESS, lys_parse_mem(CONTEXT_GET, schema, LYS_IN_YANG, NULL));

    struct lyd_node *model_1;

    CHECK_PARSE_LYD(xml, LYD_XML, LYD_PARSE_ONLY, 0, LY_SUCCESS, model_1);

    struct lyd_node *diff = NULL;

    assert_int_equal(lyd_diff_siblings(NULL, NULL, 0, &diff), LY_SUCCESS);
    assert_null(diff);

    struct lyd_node *diff1;

    CHECK_PARSE_LYD_DIFF(NULL, lyd_child(model_1), diff1);
    const char *result = "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <foo yang:operation=\"create\">42</foo>\n"
            "</df>\n";
    CHECK_LYD_STRING(diff1, result, LYD_XML, LYD_PRINT_WITHSIBLINGS);

    struct lyd_node *diff2;

    CHECK_PARSE_LYD_DIFF(lyd_child(model_1), NULL, diff2);
    result = "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <foo yang:operation=\"delete\">42</foo>\n"
            "</df>\n";
    CHECK_LYD_STRING(diff2, result, LYD_XML, LYD_PRINT_WITHSIBLINGS);

    CHECK_FREE_LYD(model_1);
    CHECK_FREE_LYD(diff1);
    CHECK_FREE_LYD(diff2);
    CONTEXT_DESTROY;
}

static void
test_leaf(void **state)
{
    (void) state;
    const char *xml1 =
            "<df xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <foo>42</foo>\n"
            "</df>\n"
            "<hidden xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <foo>42</foo>\n"
            "  <baz>42</baz>\n"
            "</hidden>\n";
    const char *xml2 =
            "<df xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <foo>41</foo>\n"
            "  <b1_1>42</b1_1>\n"
            "</df>\n";
    const char *xml3 =
            "<df xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <foo>40</foo>\n"
            "</df>\n"
            "<hidden xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <foo>40</foo>\n"
            "</hidden>\n";
    const char *out_diff_1 =
            "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <foo yang:operation=\"replace\" yang:orig-default=\"false\" yang:orig-value=\"42\">41</foo>\n"
            "  <b1_1 yang:operation=\"create\">42</b1_1>\n"
            "</df>\n"
            "<hidden xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"delete\">\n"
            "  <foo>42</foo>\n"
            "  <baz>42</baz>\n"
            "</hidden>\n";

    const char *out_diff_2 = "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <foo yang:operation=\"replace\" yang:orig-default=\"false\" yang:orig-value=\"41\">40</foo>\n"
            "  <b1_1 yang:operation=\"delete\">42</b1_1>\n"
            "</df>\n"
            "<hidden xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"create\">\n"
            "  <foo>40</foo>\n"
            "</hidden>\n";

    const char *out_merge =
            "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <foo yang:operation=\"replace\" yang:orig-default=\"false\" yang:orig-value=\"42\">40</foo>\n"
            "</df>\n"
            "<hidden xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <foo yang:operation=\"replace\" yang:orig-value=\"42\" yang:orig-default=\"false\">40</foo>\n"
            "  <baz yang:operation=\"delete\">42</baz>\n"
            "</hidden>\n";

    TEST_DIFF_3(xml1, xml2, xml3, out_diff_1, out_diff_2, out_merge);
}

static void
test_list(void **state)
{
    (void) state;
    const char *xml1 = "<df xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <list>\n"
            "    <name>a</name>\n"
            "    <value>1</value>\n"
            "  </list>\n"
            "  <list>\n"
            "    <name>b</name>\n"
            "    <value>2</value>\n"
            "  </list>\n"
            "</df>\n";
    const char *xml2 = "<df xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <list>\n"
            "    <name>b</name>\n"
            "    <value>-2</value>\n"
            "  </list>\n"
            "  <list>\n"
            "    <name>c</name>\n"
            "    <value>3</value>\n"
            "  </list>\n"
            "</df>\n";
    const char *xml3 = "<df xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <list>\n"
            "    <name>b</name>\n"
            "    <value>-2</value>\n"
            "  </list>\n"
            "  <list>\n"
            "    <name>a</name>\n"
            "    <value>2</value>\n"
            "  </list>\n"
            "</df>\n";

    const char *out_diff_1 =
            "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <list yang:operation=\"delete\">\n"
            "    <name>a</name>\n"
            "    <value>1</value>\n"
            "  </list>\n"
            "  <list yang:operation=\"none\">\n"
            "    <name>b</name>\n"
            "    <value yang:operation=\"replace\" yang:orig-default=\"false\" yang:orig-value=\"2\">-2</value>\n"
            "  </list>\n"
            "  <list yang:operation=\"create\">\n"
            "    <name>c</name>\n"
            "    <value>3</value>\n"
            "  </list>\n"
            "</df>\n";
    const char *out_diff_2 =
            "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <list yang:operation=\"delete\">\n"
            "    <name>c</name>\n"
            "    <value>3</value>\n"
            "  </list>\n"
            "  <list yang:operation=\"create\">\n"
            "    <name>a</name>\n"
            "    <value>2</value>\n"
            "  </list>\n"
            "</df>\n";
    const char *out_merge =
            "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <list yang:operation=\"none\">\n"
            "    <name>a</name>\n"
            "    <value yang:operation=\"replace\" yang:orig-value=\"1\" yang:orig-default=\"false\">2</value>\n"
            "  </list>\n"
            "  <list yang:operation=\"none\">\n"
            "    <name>b</name>\n"
            "    <value yang:operation=\"replace\" yang:orig-default=\"false\" yang:orig-value=\"2\">-2</value>\n"
            "  </list>\n"
            "</df>\n";

    TEST_DIFF_3(xml1, xml2, xml3, out_diff_1, out_diff_2, out_merge);
}

static void
test_userord_llist(void **state)
{
    (void) state;
    const char *xml1 =
            "<df xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <llist>1</llist>\n"
            "  <llist>2</llist>\n"
            "  <llist>3</llist>\n"
            "  <llist>4</llist>\n"
            "  <llist>5</llist>\n"
            "</df>\n";
    const char *xml2 =
            "<df xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <llist>1</llist>\n"
            "  <llist>4</llist>\n"
            "  <llist>3</llist>\n"
            "  <llist>2</llist>\n"
            "  <llist>5</llist>\n"
            "</df>\n";
    const char *xml3 =
            "<df xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <llist>5</llist>\n"
            "  <llist>4</llist>\n"
            "  <llist>3</llist>\n"
            "  <llist>2</llist>\n"
            "</df>\n";

    const char *out_diff_1 =
            "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <llist yang:operation=\"replace\" yang:orig-default=\"false\" yang:orig-value=\"3\" yang:value=\"1\">4</llist>\n"
            "  <llist yang:operation=\"replace\" yang:orig-default=\"false\" yang:orig-value=\"2\" yang:value=\"4\">3</llist>\n"
            "</df>\n";
    const char *out_diff_2 =
            "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <llist yang:operation=\"delete\" yang:orig-value=\"\">1</llist>\n"
            "  <llist yang:operation=\"replace\" yang:orig-default=\"false\" yang:orig-value=\"2\" yang:value=\"\">5</llist>\n"
            "</df>\n";
    const char *out_merge =
            "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <llist yang:operation=\"replace\" yang:orig-default=\"false\" yang:orig-value=\"3\" yang:value=\"1\">4</llist>\n"
            "  <llist yang:operation=\"replace\" yang:orig-default=\"false\" yang:orig-value=\"2\" yang:value=\"4\">3</llist>\n"
            "  <llist yang:orig-value=\"\" yang:operation=\"delete\">1</llist>\n"
            "  <llist yang:orig-default=\"false\" yang:orig-value=\"2\" yang:value=\"\" yang:operation=\"replace\">5</llist>\n"
            "</df>\n";

    TEST_DIFF_3(xml1, xml2, xml3, out_diff_1, out_diff_2, out_merge);
}

static void
test_userord_llist2(void **state)
{
    (void) state;
    const char *xml1 =
            "<df xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <llist>1</llist>\n"
            "  <list><name>a</name><value>1</value></list>\n"
            "  <llist>2</llist>\n"
            "  <llist>3</llist>\n"
            "  <llist>4</llist>\n"
            "</df>\n";
    const char *xml2 =
            "<df xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <llist>1</llist>\n"
            "  <list><name>a</name><value>1</value></list>\n"
            "  <llist>2</llist>\n"
            "  <llist>4</llist>\n"
            "  <llist>3</llist>\n"
            "</df>\n";
    const char *xml3 =
            "<df xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <llist>4</llist>\n"
            "  <llist>1</llist>\n"
            "  <list><name>a</name><value>1</value></list>\n"
            "  <llist>3</llist>\n"
            "</df>\n";

    const char *out_diff_1 =
            "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <llist yang:operation=\"replace\" yang:orig-default=\"false\" yang:orig-value=\"3\" yang:value=\"2\">4</llist>\n"
            "</df>\n";
    const char *out_diff_2 =
            "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <llist yang:operation=\"delete\" yang:orig-value=\"1\">2</llist>\n"
            "  <llist yang:operation=\"replace\" yang:orig-default=\"false\" yang:orig-value=\"1\" yang:value=\"\">4</llist>\n"
            "</df>\n";
    const char *out_merge =
            "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <llist yang:operation=\"replace\" yang:orig-default=\"false\" yang:orig-value=\"3\" yang:value=\"\">4</llist>\n"
            "  <llist yang:orig-value=\"1\" yang:operation=\"delete\">2</llist>\n"
            "</df>\n";

    TEST_DIFF_3(xml1, xml2, xml3, out_diff_1, out_diff_2, out_merge);
}

static void
test_userord_mix(void **state)
{
    (void) state;
    const char *xml1 =
            "<df xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <llist>1</llist>\n"
            "  <llist>2</llist>\n"
            "  <llist>3</llist>\n"
            "</df>\n";
    const char *xml2 =
            "<df xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <llist>3</llist>\n"
            "  <llist>1</llist>\n"
            "</df>\n";
    const char *xml3 =
            "<df xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <llist>1</llist>\n"
            "  <llist>4</llist>\n"
            "  <llist>3</llist>\n"
            "</df>\n";

    const char *out_diff_1 =
            "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <llist yang:operation=\"delete\" yang:orig-value=\"1\">2</llist>\n"
            "  <llist yang:operation=\"replace\" yang:orig-default=\"false\" yang:orig-value=\"1\" yang:value=\"\">3</llist>\n"
            "</df>\n";
    const char *out_diff_2 =
            "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <llist yang:operation=\"replace\" yang:orig-default=\"false\" yang:orig-value=\"3\" yang:value=\"\">1</llist>\n"
            "  <llist yang:operation=\"create\" yang:value=\"1\">4</llist>\n"
            "</df>\n";
    const char *out_merge =
            "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <llist yang:operation=\"delete\" yang:orig-value=\"1\">2</llist>\n"
            "  <llist yang:operation=\"replace\" yang:orig-default=\"false\" yang:orig-value=\"1\" yang:value=\"\">3</llist>\n"
            "  <llist yang:orig-default=\"false\" yang:orig-value=\"3\" yang:value=\"\" yang:operation=\"replace\">1</llist>\n"
            "  <llist yang:value=\"1\" yang:operation=\"create\">4</llist>\n"
            "</df>\n";

    TEST_DIFF_3(xml1, xml2, xml3, out_diff_1, out_diff_2, out_merge);
}

static void
test_wd(void **state)
{
    (void) state;
    const struct lys_module *mod;
    const char *xml2 = "<df xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <foo>41</foo>\n"
            "  <dllist>4</dllist>\n"
            "</df>\n";
    const char *xml3 = "<df xmlns=\"urn:libyang:tests:defaults\">\n"
            "  <foo>42</foo>\n"
            "  <dllist>4</dllist>\n"
            "  <dllist>1</dllist>\n"
            "</df>\n";

    CONTEXT_CREATE_PATH(TESTS_DIR_MODULES_YANG);
    assert_non_null(ly_ctx_load_module(CONTEXT_GET, "ietf-netconf-acm", "2018-02-14", NULL));
    assert_int_equal(LY_SUCCESS, lys_parse_mem(CONTEXT_GET, schema, LYS_IN_YANG, NULL));
    mod = ly_ctx_get_module_implemented(CONTEXT_GET, "defaults");
    assert_non_null(mod);

    struct lyd_node *model_1 = NULL;

    assert_int_equal(lyd_validate_module(&model_1, mod, 0, NULL), LY_SUCCESS);
    assert_ptr_not_equal(model_1, NULL);

    struct lyd_node *model_2;
    struct lyd_node *model_3;

    CHECK_PARSE_LYD(xml2, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_SUCCESS, model_2);
    CHECK_PARSE_LYD(xml3, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_SUCCESS, model_3);

    /* diff1 */
    struct lyd_node *diff1 = NULL;

    assert_int_equal(lyd_diff_siblings(model_1, model_2, LYD_DIFF_DEFAULTS, &diff1), LY_SUCCESS);
    assert_non_null(diff1);

    const char *diff1_out_1 =
            "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <foo yang:operation=\"replace\" yang:orig-default=\"true\" yang:orig-value=\"42\">41</foo>\n"
            "  <dllist yang:operation=\"delete\">1</dllist>\n"
            "  <dllist yang:operation=\"delete\">2</dllist>\n"
            "  <dllist yang:operation=\"delete\">3</dllist>\n"
            "  <dllist yang:operation=\"create\">4</dllist>\n"
            "</df>\n";

    CHECK_LYD_STRING(diff1, diff1_out_1, LYD_XML, LYD_PRINT_WITHSIBLINGS | LYD_PRINT_WD_ALL);
    assert_int_equal(lyd_diff_apply_all(&model_1, diff1), LY_SUCCESS);
    CHECK_LYD(model_1, model_2);

    /* diff2 */
    struct lyd_node *diff2;

    assert_int_equal(lyd_diff_siblings(model_2, model_3, LYD_DIFF_DEFAULTS, &diff2), LY_SUCCESS);
    assert_non_null(diff2);
    const char *result = "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <foo yang:operation=\"replace\" yang:orig-default=\"false\" yang:orig-value=\"41\">42</foo>\n"
            "  <dllist yang:operation=\"create\">1</dllist>\n"
            "</df>\n";
    CHECK_LYD_STRING(diff2, result, LYD_XML, LYD_PRINT_WITHSIBLINGS);

    assert_int_equal(lyd_diff_apply_all(&model_2, diff2), LY_SUCCESS);
    CHECK_LYD(model_2, model_3);

    /* merge */
    assert_int_equal(lyd_diff_merge_all(&diff1, diff2, 0), LY_SUCCESS);

    const char *diff1_out_2 =
            "<df xmlns=\"urn:libyang:tests:defaults\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <foo yang:orig-default=\"true\" yang:operation=\"none\">42</foo>\n"
            "  <dllist yang:operation=\"none\" yang:orig-default=\"true\">1</dllist>\n"
            "  <dllist yang:operation=\"delete\">2</dllist>\n"
            "  <dllist yang:operation=\"delete\">3</dllist>\n"
            "  <dllist yang:operation=\"create\">4</dllist>\n"
            "</df>\n";

    CHECK_LYD_STRING(diff1, diff1_out_2, LYD_XML, LYD_PRINT_WITHSIBLINGS | LYD_PRINT_WD_ALL);

    CHECK_FREE_LYD(model_1);
    CHECK_FREE_LYD(model_2);
    CHECK_FREE_LYD(model_3);
    CHECK_FREE_LYD(diff1);
    CHECK_FREE_LYD(diff2);
    CONTEXT_DESTROY;
}

int
main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_invalid),
        cmocka_unit_test(test_same),
        cmocka_unit_test(test_empty1),
        cmocka_unit_test(test_empty2),
        cmocka_unit_test(test_empty_nested),
        cmocka_unit_test(test_leaf),
        cmocka_unit_test(test_list),
        cmocka_unit_test(test_userord_llist),
        cmocka_unit_test(test_userord_llist2),
        cmocka_unit_test(test_userord_mix),
        cmocka_unit_test(test_wd),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
