/**
 * @file test_validation.c
 * @author: Radek Krejci <rkrejci@cesnet.cz>
 * @brief unit tests for functions from validation.c
 *
 * Copyright (c) 2020 CESNET, z.s.p.o.
 *
 * This source code is licensed under BSD 3-Clause License (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://opensource.org/licenses/BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>

#include "context.h"
#include "in.h"
#include "out.h"
#include "parser_data.h"
#include "printer_data.h"
#include "tests/config.h"
#include "tree_data_internal.h"
#include "tree_schema.h"
#include "utests.h"

const char *schema_a =
        "module a {\n"
        "    namespace urn:tests:a;\n"
        "    prefix a;\n"
        "    yang-version 1.1;\n"
        "\n"
        "    container cont {\n"
        "        leaf a {\n"
        "            when \"../../c = 'val_c'\";\n"
        "            type string;\n"
        "        }\n"
        "        leaf b {\n"
        "            type string;\n"
        "        }\n"
        "    }\n"
        "    leaf c {\n"
        "        when \"/cont/b = 'val_b'\";\n"
        "        type string;\n"
        "    }\n"
        "}";
const char *schema_b =
        "module b {\n"
        "    namespace urn:tests:b;\n"
        "    prefix b;\n"
        "    yang-version 1.1;\n"
        "\n"
        "    choice choic {\n"
        "        mandatory true;\n"
        "        leaf a {\n"
        "            type string;\n"
        "        }\n"
        "        case b {\n"
        "            leaf l {\n"
        "                type string;\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "    leaf c {\n"
        "        mandatory true;\n"
        "        type string;\n"
        "    }\n"
        "    leaf d {\n"
        "        type empty;\n"
        "    }\n"
        "}";
const char *schema_c =
        "module c {\n"
        "    namespace urn:tests:c;\n"
        "    prefix c;\n"
        "    yang-version 1.1;\n"
        "\n"
        "    choice choic {\n"
        "        leaf a {\n"
        "            type string;\n"
        "        }\n"
        "        case b {\n"
        "            leaf-list l {\n"
        "                min-elements 3;\n"
        "                type string;\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "    list lt {\n"
        "        max-elements 4;\n"
        "        key \"k\";\n"
        "        leaf k {\n"
        "            type string;\n"
        "        }\n"
        "    }\n"
        "    leaf d {\n"
        "        type empty;\n"
        "    }\n"
        "}";
const char *schema_d =
        "module d {\n"
        "    namespace urn:tests:d;\n"
        "    prefix d;\n"
        "    yang-version 1.1;\n"
        "\n"
        "    list lt {\n"
        "        key \"k\";\n"
        "        unique \"l1\";\n"
        "        leaf k {\n"
        "            type string;\n"
        "        }\n"
        "        leaf l1 {\n"
        "            type string;\n"
        "        }\n"
        "    }\n"
        "    list lt2 {\n"
        "        key \"k\";\n"
        "        unique \"cont/l2 l4\";\n"
        "        unique \"l5 l6\";\n"
        "        leaf k {\n"
        "            type string;\n"
        "        }\n"
        "        container cont {\n"
        "            leaf l2 {\n"
        "                type string;\n"
        "            }\n"
        "        }\n"
        "        leaf l4 {\n"
        "            type string;\n"
        "        }\n"
        "        leaf l5 {\n"
        "            type string;\n"
        "        }\n"
        "        leaf l6 {\n"
        "            type string;\n"
        "        }\n"
        "        list lt3 {\n"
        "            key \"kk\";\n"
        "            unique \"l3\";\n"
        "            leaf kk {\n"
        "                type string;\n"
        "            }\n"
        "            leaf l3 {\n"
        "                type string;\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "}";
const char *schema_e =
        "module e {\n"
        "    namespace urn:tests:e;\n"
        "    prefix e;\n"
        "    yang-version 1.1;\n"
        "\n"
        "    choice choic {\n"
        "        leaf a {\n"
        "            type string;\n"
        "        }\n"
        "        case b {\n"
        "            leaf-list l {\n"
        "                type string;\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "    list lt {\n"
        "        key \"k\";\n"
        "        leaf k {\n"
        "            type string;\n"
        "        }\n"
        "    }\n"
        "    leaf d {\n"
        "        type uint32;\n"
        "    }\n"
        "    leaf-list ll {\n"
        "        type string;\n"
        "    }\n"
        "    container cont {\n"
        "        list lt {\n"
        "            key \"k\";\n"
        "            leaf k {\n"
        "                type string;\n"
        "            }\n"
        "        }\n"
        "        leaf d {\n"
        "            type uint32;\n"
        "        }\n"
        "        leaf-list ll {\n"
        "            type string;\n"
        "        }\n"
        "        leaf-list ll2 {\n"
        "            type enumeration {\n"
        "                enum one;\n"
        "                enum two;\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "}";
const char *schema_f =
        "module f {\n"
        "    namespace urn:tests:f;\n"
        "    prefix f;\n"
        "    yang-version 1.1;\n"
        "\n"
        "    choice choic {\n"
        "        default \"c\";\n"
        "        leaf a {\n"
        "            type string;\n"
        "        }\n"
        "        case b {\n"
        "            leaf l {\n"
        "                type string;\n"
        "            }\n"
        "        }\n"
        "        case c {\n"
        "            leaf-list ll1 {\n"
        "                type string;\n"
        "                default \"def1\";\n"
        "                default \"def2\";\n"
        "                default \"def3\";\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "    leaf d {\n"
        "        type uint32;\n"
        "        default 15;\n"
        "    }\n"
        "    leaf-list ll2 {\n"
        "        type string;\n"
        "        default \"dflt1\";\n"
        "        default \"dflt2\";\n"
        "    }\n"
        "    container cont {\n"
        "        choice choic {\n"
        "            default \"c\";\n"
        "            leaf a {\n"
        "                type string;\n"
        "            }\n"
        "            case b {\n"
        "                leaf l {\n"
        "                    type string;\n"
        "                }\n"
        "            }\n"
        "            case c {\n"
        "                leaf-list ll1 {\n"
        "                    type string;\n"
        "                    default \"def1\";\n"
        "                    default \"def2\";\n"
        "                    default \"def3\";\n"
        "                }\n"
        "            }\n"
        "        }\n"
        "        leaf d {\n"
        "            type uint32;\n"
        "            default 15;\n"
        "        }\n"
        "        leaf-list ll2 {\n"
        "            type string;\n"
        "            default \"dflt1\";\n"
        "            default \"dflt2\";\n"
        "        }\n"
        "    }\n"
        "}";
const char *schema_g =
        "module g {\n"
        "    namespace urn:tests:g;\n"
        "    prefix g;\n"
        "    yang-version 1.1;\n"
        "\n"
        "    feature f1;\n"
        "    feature f2;\n"
        "    feature f3;\n"
        "\n"
        "    container cont {\n"
        "        if-feature \"f1\";\n"
        "        choice choic {\n"
        "            if-feature \"f2 or f3\";\n"
        "            leaf a {\n"
        "                type string;\n"
        "            }\n"
        "            case b {\n"
        "                if-feature \"f2 and f1\";\n"
        "                leaf l {\n"
        "                    type string;\n"
        "                }\n"
        "            }\n"
        "        }\n"
        "        leaf d {\n"
        "            type uint32;\n"
        "        }\n"
        "        container cont2 {\n"
        "            if-feature \"f2\";\n"
        "            leaf e {\n"
        "                type string;\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "}";
const char *schema_h =
        "module h {\n"
        "    namespace urn:tests:h;\n"
        "    prefix h;\n"
        "    yang-version 1.1;\n"
        "\n"
        "    container cont {\n"
        "        container cont2 {\n"
        "            config false;\n"
        "            leaf l {\n"
        "                type string;\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "}";
const char *schema_i =
        "module i {\n"
        "    namespace urn:tests:i;\n"
        "    prefix i;\n"
        "    yang-version 1.1;\n"
        "\n"
        "    container cont {\n"
        "        leaf l {\n"
        "            type string;\n"
        "        }\n"
        "        leaf l2 {\n"
        "            must \"../l = 'right'\";\n"
        "            type string;\n"
        "        }\n"
        "    }\n"
        "}";
const char *schema_j =
        "module j {\n"
        "    namespace urn:tests:j;\n"
        "    prefix j;\n"
        "    yang-version 1.1;\n"
        "\n"
        "    feature feat1;\n"
        "\n"
        "    container cont {\n"
        "        must \"false()\";\n"
        "        list l1 {\n"
        "            key \"k\";\n"
        "            leaf k {\n"
        "                type string;\n"
        "            }\n"
        "            action act {\n"
        "                if-feature feat1;\n"
        "                input {\n"
        "                    must \"../../lf1 = 'true'\";\n"
        "                    leaf lf2 {\n"
        "                        type leafref {\n"
        "                            path /lf3;\n"
        "                        }\n"
        "                    }\n"
        "                }\n"
        "                output {\n"
        "                    must \"../../lf1 = 'true2'\";\n"
        "                    leaf lf2 {\n"
        "                        type leafref {\n"
        "                            path /lf4;\n"
        "                        }\n"
        "                    }\n"
        "                }\n"
        "            }\n"
        "        }\n"
        "\n"
        "        leaf lf1 {\n"
        "            type string;\n"
        "        }\n"
        "    }\n"
        "\n"
        "    leaf lf3 {\n"
        "        type string;\n"
        "    }\n"
        "\n"
        "    leaf lf4 {\n"
        "        type string;\n"
        "    }\n"
        "}";

const char *feats[] = {"feat1", NULL};

#define CONTEXT_CREATE() \
                ly_set_log_clb(logger_null, 1);\
                CONTEXT_CREATE_PATH(TESTS_DIR_MODULES_YANG);\
                assert_int_equal(LY_SUCCESS, lys_parse_mem(CONTEXT_GET, schema_a, LYS_IN_YANG, NULL));\
                assert_non_null(ly_ctx_load_module(CONTEXT_GET, "ietf-netconf-with-defaults", "2011-06-01", NULL));\
                assert_int_equal(LY_SUCCESS, lys_parse_mem(CONTEXT_GET, schema_b, LYS_IN_YANG, NULL));\
                assert_int_equal(LY_SUCCESS, lys_parse_mem(CONTEXT_GET, schema_c, LYS_IN_YANG, NULL));\
                assert_int_equal(LY_SUCCESS, lys_parse_mem(CONTEXT_GET, schema_d, LYS_IN_YANG, NULL));\
                assert_int_equal(LY_SUCCESS, lys_parse_mem(CONTEXT_GET, schema_e, LYS_IN_YANG, NULL));\
                assert_int_equal(LY_SUCCESS, lys_parse_mem(CONTEXT_GET, schema_f, LYS_IN_YANG, NULL));\
                assert_int_equal(LY_SUCCESS, lys_parse_mem(CONTEXT_GET, schema_g, LYS_IN_YANG, NULL));\
                assert_int_equal(LY_SUCCESS, lys_parse_mem(CONTEXT_GET, schema_h, LYS_IN_YANG, NULL));\
                assert_int_equal(LY_SUCCESS, lys_parse_mem(CONTEXT_GET, schema_i, LYS_IN_YANG, NULL));\
                {\
                    struct ly_in *in;\
                    assert_int_equal(LY_SUCCESS, ly_in_new_memory(schema_j, &in));\
                    assert_int_equal(LY_SUCCESS, lys_parse(CONTEXT_GET, in, LYS_IN_YANG, feats, NULL));\
                    ly_in_free(in, 0);\
                }\


#define LYD_TREE_CREATE(INPUT, MODEL) \
                CHECK_PARSE_LYD_PARAM(INPUT, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_SUCCESS, MODEL)

#define LYD_TREE_CHECK_CHAR(IN_MODEL, TEXT) \
                CHECK_LYD_STRING_PARAM(IN_MODEL, TEXT, LYD_XML, LYD_PRINT_WITHSIBLINGS)

static void
test_when(void **state)
{
    (void) state;

    char *err_msg[1];
    char *err_path[1];
    const char *data;
    struct lyd_node *tree;
    int unsigned flag;

    CONTEXT_CREATE();
    data = "<c xmlns=\"urn:tests:a\">hey</c>";
    err_msg[0] = "When condition \"/cont/b = 'val_b'\" not satisfied.";
    err_path[0] = "/a:c";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    data = "<cont xmlns=\"urn:tests:a\"><b>val_b</b></cont><c xmlns=\"urn:tests:a\">hey</c>";
    LYD_TREE_CREATE(data, tree);
    flag = 0x5;
    CHECK_LYSC_NODE(tree->next->schema, NULL, 0, flag, 1, "c", 0, LYS_LEAF, 0, 0, NULL, 1);
    assert_int_equal(LYD_WHEN_TRUE, tree->next->flags);
    CHECK_FREE_LYD(tree);

    data = "<cont xmlns=\"urn:tests:a\"><a>val</a><b>val_b</b></cont><c xmlns=\"urn:tests:a\">val_c</c>";

    LYD_TREE_CREATE(data, tree);
    CHECK_LYSC_NODE(lyd_child(tree)->schema, NULL, 0, flag, 1, "a", 1, LYS_LEAF, 1, 0, NULL, 1);
    assert_int_equal(LYD_WHEN_TRUE, lyd_child(tree)->flags);
    CHECK_LYSC_NODE(tree->next->schema, NULL, 0, flag, 1, "c", 0, LYS_LEAF, 0, 0, NULL, 1);
    assert_int_equal(LYD_WHEN_TRUE, tree->next->flags);
    CHECK_FREE_LYD(tree);

    CONTEXT_DESTROY;
}

static void
test_mandatory(void **state)
{
    (void) state;

    CONTEXT_CREATE();

    char *err_msg[1];
    char *err_path[1];
    const char *data;
    struct lyd_node *tree;

    data = "<d xmlns=\"urn:tests:b\"/>";
    err_msg[0] = "Mandatory node \"choic\" instance does not exist.";
    err_path[0] = "/b:choic";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    data = "<l xmlns=\"urn:tests:b\">string</l><d xmlns=\"urn:tests:b\"/>";
    err_msg[0] = "Mandatory node \"c\" instance does not exist.";
    err_path[0] = "/b:c";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    data = "<a xmlns=\"urn:tests:b\">string</a>";
    err_msg[0] = "Mandatory node \"c\" instance does not exist.";
    err_path[0] = "/b:c";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    data = "<a xmlns=\"urn:tests:b\">string</a><c xmlns=\"urn:tests:b\">string2</c>";
    LYD_TREE_CREATE(data, tree);
    lyd_free_siblings(tree);

    CONTEXT_DESTROY;
}

static void
test_minmax(void **state)
{
    (void) state;

    char *err_msg[1];
    char *err_path[1];
    const char *data;
    struct lyd_node *tree;

    CONTEXT_CREATE();

    data = "<d xmlns=\"urn:tests:c\"/>";
    err_msg[0] = "Too few \"l\" instances.";
    err_path[0] = "/c:choic/b/l";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    data =
            "<l xmlns=\"urn:tests:c\">val1</l>"
            "<l xmlns=\"urn:tests:c\">val2</l>";
    err_msg[0] = "Too few \"l\" instances.";
    err_path[0] = "/c:choic/b/l";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    data =
            "<l xmlns=\"urn:tests:c\">val1</l>"
            "<l xmlns=\"urn:tests:c\">val2</l>"
            "<l xmlns=\"urn:tests:c\">val3</l>";
    LYD_TREE_CREATE(data, tree);
    CHECK_FREE_LYD(tree);

    data =
            "<l xmlns=\"urn:tests:c\">val1</l>"
            "<l xmlns=\"urn:tests:c\">val2</l>"
            "<l xmlns=\"urn:tests:c\">val3</l>"
            "<lt xmlns=\"urn:tests:c\"><k>val1</k></lt>"
            "<lt xmlns=\"urn:tests:c\"><k>val2</k></lt>"
            "<lt xmlns=\"urn:tests:c\"><k>val3</k></lt>"
            "<lt xmlns=\"urn:tests:c\"><k>val4</k></lt>"
            "<lt xmlns=\"urn:tests:c\"><k>val5</k></lt>";
    err_msg[0] = "Too many \"lt\" instances.";
    err_path[0] = "/c:lt";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    CONTEXT_DESTROY;
}

static void
test_unique(void **state)
{

    (void) state;
    char *err_msg[1];
    char *err_path[1];
    const char *data;
    struct lyd_node *tree;

    CONTEXT_CREATE();

    data =
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val1</k>\n"
            "    <l1>same</l1>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val2</k>\n"
            "</lt>";
    LYD_TREE_CREATE(data, tree);
    CHECK_FREE_LYD(tree);

    data =
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val1</k>\n"
            "    <l1>same</l1>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val2</k>\n"
            "    <l1>not-same</l1>\n"
            "</lt>";
    LYD_TREE_CREATE(data, tree);
    CHECK_FREE_LYD(tree);

    data =
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val1</k>\n"
            "    <l1>same</l1>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val2</k>\n"
            "    <l1>same</l1>\n"
            "</lt>";
    err_msg[0] = "Unique data leaf(s) \"l1\" not satisfied in \"/d:lt[k='val1']\" and \"/d:lt[k='val2']\".";
    err_path[0] = "/d:lt[k='val2']";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    /* now try with more instances */
    data =
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val1</k>\n"
            "    <l1>1</l1>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val2</k>\n"
            "    <l1>2</l1>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val3</k>\n"
            "    <l1>3</l1>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val4</k>\n"
            "    <l1>4</l1>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val5</k>\n"
            "    <l1>5</l1>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val6</k>\n"
            "    <l1>6</l1>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val7</k>\n"
            "    <l1>7</l1>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val8</k>\n"
            "    <l1>8</l1>\n"
            "</lt>";
    LYD_TREE_CREATE(data, tree);
    CHECK_FREE_LYD(tree);

    data =
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val1</k>\n"
            "    <l1>1</l1>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val2</k>\n"
            "    <l1>2</l1>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val3</k>\n"
            "    <l1>3</l1>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val4</k>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val5</k>\n"
            "    <l1>5</l1>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val6</k>\n"
            "    <l1>6</l1>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val7</k>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val8</k>\n"
            "</lt>";
    LYD_TREE_CREATE(data, tree);
    CHECK_FREE_LYD(tree);

    data =
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val1</k>\n"
            "    <l1>1</l1>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val2</k>\n"
            "    <l1>2</l1>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val3</k>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val4</k>\n"
            "    <l1>4</l1>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val5</k>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val6</k>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val7</k>\n"
            "    <l1>2</l1>\n"
            "</lt>\n"
            "<lt xmlns=\"urn:tests:d\">\n"
            "    <k>val8</k>\n"
            "    <l1>8</l1>\n"
            "</lt>";
    err_msg[0] = "Unique data leaf(s) \"l1\" not satisfied in \"/d:lt[k='val7']\" and \"/d:lt[k='val2']\".";
    err_path[0] = "/d:lt[k='val2']";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    CONTEXT_DESTROY;
}

static void
test_unique_nested(void **state)
{
    (void) state;

    char *err_msg[1];
    char *err_path[1];
    const char *data;
    struct lyd_node *tree;

    CONTEXT_CREATE();

    /* nested list uniquest are compared only with instances in the same parent list instance */
    data =
            "<lt2 xmlns=\"urn:tests:d\">\n"
            "    <k>val1</k>\n"
            "    <cont>\n"
            "        <l2>1</l2>\n"
            "    </cont>\n"
            "    <l4>1</l4>\n"
            "</lt2>\n"
            "<lt2 xmlns=\"urn:tests:d\">\n"
            "    <k>val2</k>\n"
            "    <cont>\n"
            "        <l2>2</l2>\n"
            "    </cont>\n"
            "    <l4>2</l4>\n"
            "    <lt3>\n"
            "        <kk>val1</kk>\n"
            "        <l3>1</l3>\n"
            "    </lt3>\n"
            "    <lt3>\n"
            "        <kk>val2</kk>\n"
            "        <l3>2</l3>\n"
            "    </lt3>\n"
            "</lt2>\n"
            "<lt2 xmlns=\"urn:tests:d\">\n"
            "    <k>val3</k>\n"
            "    <cont>\n"
            "        <l2>3</l2>\n"
            "    </cont>\n"
            "    <l4>3</l4>\n"
            "    <lt3>\n"
            "        <kk>val1</kk>\n"
            "        <l3>2</l3>\n"
            "    </lt3>\n"
            "</lt2>\n"
            "<lt2 xmlns=\"urn:tests:d\">\n"
            "    <k>val4</k>\n"
            "    <cont>\n"
            "        <l2>4</l2>\n"
            "    </cont>\n"
            "    <l4>4</l4>\n"
            "    <lt3>\n"
            "        <kk>val1</kk>\n"
            "        <l3>3</l3>\n"
            "    </lt3>\n"
            "</lt2>\n"
            "<lt2 xmlns=\"urn:tests:d\">\n"
            "    <k>val5</k>\n"
            "    <cont>\n"
            "        <l2>5</l2>\n"
            "    </cont>\n"
            "    <l4>5</l4>\n"
            "    <lt3>\n"
            "        <kk>val1</kk>\n"
            "        <l3>3</l3>\n"
            "    </lt3>\n"
            "</lt2>";
    LYD_TREE_CREATE(data, tree);
    CHECK_FREE_LYD(tree);

    data =
            "<lt2 xmlns=\"urn:tests:d\">\n"
            "    <k>val1</k>\n"
            "    <cont>\n"
            "        <l2>1</l2>\n"
            "    </cont>\n"
            "    <l4>1</l4>\n"
            "</lt2>\n"
            "<lt2 xmlns=\"urn:tests:d\">\n"
            "    <k>val2</k>\n"
            "    <cont>\n"
            "        <l2>2</l2>\n"
            "    </cont>\n"
            "    <lt3>\n"
            "        <kk>val1</kk>\n"
            "        <l3>1</l3>\n"
            "    </lt3>\n"
            "    <lt3>\n"
            "        <kk>val2</kk>\n"
            "        <l3>2</l3>\n"
            "    </lt3>\n"
            "    <lt3>\n"
            "        <kk>val3</kk>\n"
            "        <l3>1</l3>\n"
            "    </lt3>\n"
            "</lt2>\n"
            "<lt2 xmlns=\"urn:tests:d\">\n"
            "    <k>val3</k>\n"
            "    <cont>\n"
            "        <l2>3</l2>\n"
            "    </cont>\n"
            "    <l4>1</l4>\n"
            "    <lt3>\n"
            "        <kk>val1</kk>\n"
            "        <l3>2</l3>\n"
            "    </lt3>\n"
            "</lt2>\n"
            "<lt2 xmlns=\"urn:tests:d\">\n"
            "    <k>val4</k>\n"
            "    <cont>\n"
            "        <l2>4</l2>\n"
            "    </cont>\n"
            "    <lt3>\n"
            "        <kk>val1</kk>\n"
            "        <l3>3</l3>\n"
            "    </lt3>\n"
            "</lt2>\n"
            "<lt2 xmlns=\"urn:tests:d\">\n"
            "    <k>val5</k>\n"
            "    <cont>\n"
            "        <l2>5</l2>\n"
            "    </cont>\n"
            "    <lt3>\n"
            "        <kk>val1</kk>\n"
            "        <l3>3</l3>\n"
            "    </lt3>\n"
            "</lt2>";
    err_msg[0] = "Unique data leaf(s) \"l3\" not satisfied in"
            " \"/d:lt2[k='val2']/lt3[kk='val3']\" and"
            " \"/d:lt2[k='val2']/lt3[kk='val1']\".";
    err_path[0] = "/d:lt2[k='val2']/lt3[kk='val1']";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    data =
            "<lt2 xmlns=\"urn:tests:d\">\n"
            "    <k>val1</k>\n"
            "    <cont>\n"
            "        <l2>1</l2>\n"
            "    </cont>\n"
            "    <l4>1</l4>\n"
            "</lt2>\n"
            "<lt2 xmlns=\"urn:tests:d\">\n"
            "    <k>val2</k>\n"
            "    <cont>\n"
            "        <l2>2</l2>\n"
            "    </cont>\n"
            "    <l4>2</l4>\n"
            "</lt2>\n"
            "<lt2 xmlns=\"urn:tests:d\">\n"
            "    <k>val3</k>\n"
            "    <cont>\n"
            "        <l2>3</l2>\n"
            "    </cont>\n"
            "    <l4>3</l4>\n"
            "</lt2>\n"
            "<lt2 xmlns=\"urn:tests:d\">\n"
            "    <k>val4</k>\n"
            "    <cont>\n"
            "        <l2>2</l2>\n"
            "    </cont>\n"
            "    <l4>2</l4>\n"
            "</lt2>\n"
            "<lt2 xmlns=\"urn:tests:d\">\n"
            "    <k>val5</k>\n"
            "    <cont>\n"
            "        <l2>5</l2>\n"
            "    </cont>\n"
            "    <l4>5</l4>\n"
            "</lt2>";
    err_msg[0] = "Unique data leaf(s) \"cont/l2 l4\" not satisfied in \"/d:lt2[k='val4']\" and \"/d:lt2[k='val2']\".";
    err_path[0] = "/d:lt2[k='val2']";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    data =
            "<lt2 xmlns=\"urn:tests:d\">\n"
            "    <k>val1</k>\n"
            "    <cont>\n"
            "        <l2>1</l2>\n"
            "    </cont>\n"
            "    <l4>1</l4>\n"
            "    <l5>1</l5>\n"
            "    <l6>1</l6>\n"
            "</lt2>\n"
            "<lt2 xmlns=\"urn:tests:d\">\n"
            "    <k>val2</k>\n"
            "    <cont>\n"
            "        <l2>2</l2>\n"
            "    </cont>\n"
            "    <l4>1</l4>\n"
            "    <l5>1</l5>\n"
            "</lt2>\n"
            "<lt2 xmlns=\"urn:tests:d\">\n"
            "    <k>val3</k>\n"
            "    <cont>\n"
            "        <l2>3</l2>\n"
            "    </cont>\n"
            "    <l4>1</l4>\n"
            "    <l5>3</l5>\n"
            "    <l6>3</l6>\n"
            "</lt2>\n"
            "<lt2 xmlns=\"urn:tests:d\">\n"
            "    <k>val4</k>\n"
            "    <cont>\n"
            "        <l2>4</l2>\n"
            "    </cont>\n"
            "    <l4>1</l4>\n"
            "    <l6>1</l6>\n"
            "</lt2>\n"
            "<lt2 xmlns=\"urn:tests:d\">\n"
            "    <k>val5</k>\n"
            "    <cont>\n"
            "        <l2>5</l2>\n"
            "    </cont>\n"
            "    <l4>1</l4>\n"
            "    <l5>3</l5>\n"
            "    <l6>3</l6>\n"
            "</lt2>";
    err_msg[0] = "Unique data leaf(s) \"l5 l6\" not satisfied in \"/d:lt2[k='val5']\" and \"/d:lt2[k='val3']\".";
    err_path[0] = "/d:lt2[k='val3']";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    CONTEXT_DESTROY;
}

static void
test_dup(void **state)
{
    (void) state;

    char *err_msg[1];
    char *err_path[1];
    const char *data;
    struct lyd_node *tree;

    CONTEXT_CREATE();

    data = "<d xmlns=\"urn:tests:e\">25</d><d xmlns=\"urn:tests:e\">50</d>";
    err_msg[0] = "Duplicate instance of \"d\".";
    err_path[0] = "/e:d";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    data = "<lt xmlns=\"urn:tests:e\"><k>A</k></lt><lt xmlns=\"urn:tests:e\"><k>B</k></lt><lt xmlns=\"urn:tests:e\"><k>A</k></lt>";
    err_msg[0] = "Duplicate instance of \"lt\".";
    err_path[0] = "/e:lt[k='A']";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    data = "<ll xmlns=\"urn:tests:e\">A</ll><ll xmlns=\"urn:tests:e\">B</ll><ll xmlns=\"urn:tests:e\">B</ll>";
    err_msg[0] = "Duplicate instance of \"ll\".";
    err_path[0] = "/e:ll[.='B']";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    data = "<cont xmlns=\"urn:tests:e\"></cont><cont xmlns=\"urn:tests:e\"/>";
    err_msg[0] = "Duplicate instance of \"cont\".";
    err_path[0] = "/e:cont";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    /* same tests again but using hashes */
    data = "<cont xmlns=\"urn:tests:e\"><d>25</d><d>50</d><ll>1</ll><ll>2</ll><ll>3</ll><ll>4</ll></cont>";
    err_msg[0] = "Duplicate instance of \"d\".";
    err_path[0] = "/e:cont/d";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    data = "<cont xmlns=\"urn:tests:e\"><ll>1</ll><ll>2</ll><ll>3</ll><ll>4</ll>"
            "<lt><k>a</k></lt><lt><k>b</k></lt><lt><k>c</k></lt><lt><k>d</k></lt><lt><k>c</k></lt></cont>";
    err_msg[0] = "Duplicate instance of \"lt\".";
    err_path[0] = "/e:cont/lt[k='c']";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    data = "<cont xmlns=\"urn:tests:e\"><ll>1</ll><ll>2</ll><ll>3</ll><ll>4</ll>"
            "<ll>a</ll><ll>b</ll><ll>c</ll><ll>d</ll><ll>d</ll></cont>";
    err_msg[0] = "Duplicate instance of \"ll\".";
    err_path[0] = "/e:cont/ll[.='d']";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    /* cases */
    data = "<l xmlns=\"urn:tests:e\">a</l><l xmlns=\"urn:tests:e\">b</l><l xmlns=\"urn:tests:e\">c</l><l xmlns=\"urn:tests:e\">b</l>";
    err_msg[0] = "Duplicate instance of \"l\".";
    err_path[0] = "/e:l[.='b']";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    data = "<l xmlns=\"urn:tests:e\">a</l><l xmlns=\"urn:tests:e\">b</l><l xmlns=\"urn:tests:e\">c</l><a xmlns=\"urn:tests:e\">aa</a>";
    err_msg[0] = "Data for both cases \"a\" and \"b\" exist.";
    err_path[0] = "/e:choic";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    CONTEXT_DESTROY;
}

static void
test_defaults(void **state)
{
    (void) state;

    struct lyd_node *tree, *node, *diff;
    const struct lys_module *mod;
    const char *str;

    CONTEXT_CREATE();
    mod = ly_ctx_get_module_latest(CONTEXT_GET, "f");

    /* get defaults */
    tree = NULL;
    assert_int_equal(lyd_validate_module(&tree, mod, 0, &diff), LY_SUCCESS);
    assert_non_null(tree);
    assert_non_null(diff);

    /* check all defaults exist */
    str = "<ll1 xmlns=\"urn:tests:f\" xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">def1</ll1>\n"
            "<ll1 xmlns=\"urn:tests:f\" xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">def2</ll1>\n"
            "<ll1 xmlns=\"urn:tests:f\" xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">def3</ll1>\n"
            "<d xmlns=\"urn:tests:f\" xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">15</d>\n"
            "<ll2 xmlns=\"urn:tests:f\" xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">dflt1</ll2>\n"
            "<ll2 xmlns=\"urn:tests:f\" xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">dflt2</ll2>\n"
            "<cont xmlns=\"urn:tests:f\">\n"
            "  <ll1 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">def1</ll1>\n"
            "  <ll1 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">def2</ll1>\n"
            "  <ll1 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">def3</ll1>\n"
            "  <d xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">15</d>\n"
            "  <ll2 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">dflt1</ll2>\n"
            "  <ll2 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">dflt2</ll2>\n"
            "</cont>\n";
    CHECK_LYD_STRING_PARAM(tree, str, LYD_XML, LYD_PRINT_WD_IMPL_TAG | LYD_PRINT_WITHSIBLINGS);

    /* check diff */
    str = "<ll1 xmlns=\"urn:tests:f\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"create\">def1</ll1>\n"
            "<ll1 xmlns=\"urn:tests:f\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"create\">def2</ll1>\n"
            "<ll1 xmlns=\"urn:tests:f\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"create\">def3</ll1>\n"
            "<d xmlns=\"urn:tests:f\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"create\">15</d>\n"
            "<ll2 xmlns=\"urn:tests:f\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"create\">dflt1</ll2>\n"
            "<ll2 xmlns=\"urn:tests:f\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"create\">dflt2</ll2>\n"
            "<cont xmlns=\"urn:tests:f\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"create\">\n"
            "  <ll1 yang:operation=\"create\">def1</ll1>\n"
            "  <ll1 yang:operation=\"create\">def2</ll1>\n"
            "  <ll1 yang:operation=\"create\">def3</ll1>\n"
            "  <d yang:operation=\"create\">15</d>\n"
            "  <ll2 yang:operation=\"create\">dflt1</ll2>\n"
            "  <ll2 yang:operation=\"create\">dflt2</ll2>\n"
            "</cont>\n";
    CHECK_LYD_STRING_PARAM(diff, str, LYD_XML, LYD_PRINT_WD_ALL | LYD_PRINT_WITHSIBLINGS);
    CHECK_FREE_LYD(diff);

    /* create another explicit case and validate */
    assert_int_equal(lyd_new_term(NULL, mod, "l", "value", 0, &node), LY_SUCCESS);
    assert_int_equal(lyd_insert_sibling(tree, node, &tree), LY_SUCCESS);
    assert_int_equal(lyd_validate_all(&tree, CONTEXT_GET, LYD_VALIDATE_PRESENT, &diff), LY_SUCCESS);

    /* check data tree */
    str = "<l xmlns=\"urn:tests:f\">value</l>\n"
            "<d xmlns=\"urn:tests:f\" xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">15</d>\n"
            "<ll2 xmlns=\"urn:tests:f\" xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">dflt1</ll2>\n"
            "<ll2 xmlns=\"urn:tests:f\" xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">dflt2</ll2>\n"
            "<cont xmlns=\"urn:tests:f\">\n"
            "  <ll1 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">def1</ll1>\n"
            "  <ll1 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">def2</ll1>\n"
            "  <ll1 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">def3</ll1>\n"
            "  <d xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">15</d>\n"
            "  <ll2 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">dflt1</ll2>\n"
            "  <ll2 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">dflt2</ll2>\n"
            "</cont>\n";
    CHECK_LYD_STRING_PARAM(tree, str, LYD_XML, LYD_PRINT_WD_IMPL_TAG | LYD_PRINT_WITHSIBLINGS);

    /* check diff */
    str = "<ll1 xmlns=\"urn:tests:f\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"delete\">def1</ll1>\n"
            "<ll1 xmlns=\"urn:tests:f\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"delete\">def2</ll1>\n"
            "<ll1 xmlns=\"urn:tests:f\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"delete\">def3</ll1>\n";
    CHECK_LYD_STRING_PARAM(diff, str, LYD_XML, LYD_PRINT_WD_ALL | LYD_PRINT_WITHSIBLINGS);
    CHECK_FREE_LYD(diff);

    /* create explicit leaf-list and leaf and validate */
    assert_int_equal(lyd_new_term(NULL, mod, "d", "15", 0, &node), LY_SUCCESS);
    assert_int_equal(lyd_insert_sibling(tree, node, &tree), LY_SUCCESS);
    assert_int_equal(lyd_new_term(NULL, mod, "ll2", "dflt2", 0, &node), LY_SUCCESS);
    assert_int_equal(lyd_insert_sibling(tree, node, &tree), LY_SUCCESS);
    assert_int_equal(lyd_validate_all(&tree, CONTEXT_GET, LYD_VALIDATE_PRESENT, &diff), LY_SUCCESS);

    /* check data tree */
    str = "<l xmlns=\"urn:tests:f\">value</l>\n"
            "<d xmlns=\"urn:tests:f\">15</d>\n"
            "<ll2 xmlns=\"urn:tests:f\">dflt2</ll2>\n"
            "<cont xmlns=\"urn:tests:f\">\n"
            "  <ll1 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">def1</ll1>\n"
            "  <ll1 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">def2</ll1>\n"
            "  <ll1 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">def3</ll1>\n"
            "  <d xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">15</d>\n"
            "  <ll2 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">dflt1</ll2>\n"
            "  <ll2 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">dflt2</ll2>\n"
            "</cont>\n";
    CHECK_LYD_STRING_PARAM(tree, str, LYD_XML, LYD_PRINT_WD_IMPL_TAG | LYD_PRINT_WITHSIBLINGS);

    /* check diff */
    str = "<d xmlns=\"urn:tests:f\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"delete\">15</d>\n"
            "<ll2 xmlns=\"urn:tests:f\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"delete\">dflt1</ll2>\n"
            "<ll2 xmlns=\"urn:tests:f\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"delete\">dflt2</ll2>\n";
    CHECK_LYD_STRING_PARAM(diff, str, LYD_XML, LYD_PRINT_WD_ALL | LYD_PRINT_WITHSIBLINGS);
    CHECK_FREE_LYD(diff);

    /* create first explicit container, which should become implicit */
    assert_int_equal(lyd_new_inner(NULL, mod, "cont", 0, &node), LY_SUCCESS);
    assert_int_equal(lyd_insert_sibling(tree, node, &tree), LY_SUCCESS);
    assert_int_equal(lyd_validate_all(&tree, CONTEXT_GET, LYD_VALIDATE_PRESENT, &diff), LY_SUCCESS);

    /* check data tree */
    str = "<l xmlns=\"urn:tests:f\">value</l>\n"
            "<d xmlns=\"urn:tests:f\">15</d>\n"
            "<ll2 xmlns=\"urn:tests:f\">dflt2</ll2>\n"
            "<cont xmlns=\"urn:tests:f\">\n"
            "  <ll1 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">def1</ll1>\n"
            "  <ll1 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">def2</ll1>\n"
            "  <ll1 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">def3</ll1>\n"
            "  <d xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">15</d>\n"
            "  <ll2 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">dflt1</ll2>\n"
            "  <ll2 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">dflt2</ll2>\n"
            "</cont>\n";
    CHECK_LYD_STRING_PARAM(tree, str, LYD_XML, LYD_PRINT_WD_IMPL_TAG | LYD_PRINT_WITHSIBLINGS);
    /* check diff */
    assert_null(diff);

    /* create second explicit container, which should become implicit, so the first tree node should be removed */
    assert_int_equal(lyd_new_inner(NULL, mod, "cont", 0, &node), LY_SUCCESS);
    assert_int_equal(lyd_insert_sibling(tree, node, &tree), LY_SUCCESS);
    assert_int_equal(lyd_validate_all(&tree, CONTEXT_GET, LYD_VALIDATE_PRESENT, &diff), LY_SUCCESS);

    /* check data tree */
    str = "<l xmlns=\"urn:tests:f\">value</l>\n"
            "<d xmlns=\"urn:tests:f\">15</d>\n"
            "<ll2 xmlns=\"urn:tests:f\">dflt2</ll2>\n"
            "<cont xmlns=\"urn:tests:f\">\n"
            "  <ll1 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">def1</ll1>\n"
            "  <ll1 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">def2</ll1>\n"
            "  <ll1 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">def3</ll1>\n"
            "  <d xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">15</d>\n"
            "  <ll2 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">dflt1</ll2>\n"
            "  <ll2 xmlns:ncwd=\"urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults\" ncwd:default=\"true\">dflt2</ll2>\n"
            "</cont>\n";
    CHECK_LYD_STRING_PARAM(tree, str, LYD_XML, LYD_PRINT_WD_IMPL_TAG | LYD_PRINT_WITHSIBLINGS);
    /* check diff */
    assert_null(diff);

    /* similar changes for nested defaults */
    assert_int_equal(lyd_new_term(tree->prev, NULL, "ll1", "def3", 0, NULL), LY_SUCCESS);
    assert_int_equal(lyd_new_term(tree->prev, NULL, "d", "5", 0, NULL), LY_SUCCESS);
    assert_int_equal(lyd_new_term(tree->prev, NULL, "ll2", "non-dflt", 0, NULL), LY_SUCCESS);
    assert_int_equal(lyd_validate_all(&tree, CONTEXT_GET, LYD_VALIDATE_PRESENT, &diff), LY_SUCCESS);

    /* check data tree */
    str = "<l xmlns=\"urn:tests:f\">value</l>\n"
            "<d xmlns=\"urn:tests:f\">15</d>\n"
            "<ll2 xmlns=\"urn:tests:f\">dflt2</ll2>\n"
            "<cont xmlns=\"urn:tests:f\">\n"
            "  <ll1>def3</ll1>\n"
            "  <d>5</d>\n"
            "  <ll2>non-dflt</ll2>\n"
            "</cont>\n";
    LYD_TREE_CHECK_CHAR(tree, str);

    /* check diff */
    str = "<cont xmlns=\"urn:tests:f\" xmlns:yang=\"urn:ietf:params:xml:ns:yang:1\" yang:operation=\"none\">\n"
            "  <ll1 yang:operation=\"delete\">def1</ll1>\n"
            "  <ll1 yang:operation=\"delete\">def2</ll1>\n"
            "  <ll1 yang:operation=\"delete\">def3</ll1>\n"
            "  <d yang:operation=\"delete\">15</d>\n"
            "  <ll2 yang:operation=\"delete\">dflt1</ll2>\n"
            "  <ll2 yang:operation=\"delete\">dflt2</ll2>\n"
            "</cont>\n";
    CHECK_LYD_STRING_PARAM(diff, str, LYD_XML, LYD_PRINT_WD_ALL | LYD_PRINT_WITHSIBLINGS);
    CHECK_FREE_LYD(diff);
    CHECK_FREE_LYD(tree);

    CONTEXT_DESTROY;
}

static void
test_state(void **state)
{
    (void) state;

    char *err_msg[1];
    char *err_path[1];
    const char *data;
    struct lyd_node *tree;

    CONTEXT_CREATE();

    data =
            "<cont xmlns=\"urn:tests:h\">\n"
            "  <cont2>\n"
            "    <l>val</l>\n"
            "  </cont2>\n"
            "</cont>\n";
    err_msg[0] = "Invalid state data node \"cont2\" found.";
    err_path[0] = "/h:cont/cont2";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, LYD_PARSE_ONLY | LYD_PARSE_NO_STATE, 0, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    CHECK_PARSE_LYD_PARAM(data, LYD_XML, LYD_PARSE_ONLY, 0, LY_SUCCESS, tree);
    assert_int_equal(LY_EVALID, lyd_validate_all(&tree, NULL, LYD_VALIDATE_PRESENT | LYD_VALIDATE_NO_STATE, NULL));
    err_msg[0] = "Invalid state data node \"cont2\" found.";
    err_path[0] = "/h:cont/cont2";
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    CHECK_FREE_LYD(tree);

    CONTEXT_DESTROY;
}

static void
test_must(void **state)
{
    (void) state;
    char *err_msg[1];
    char *err_path[1];
    const char *data;
    struct lyd_node *tree;

    CONTEXT_CREATE();

    data = "<cont xmlns=\"urn:tests:i\">\n"
            "  <l>wrong</l>\n"
            "  <l2>val</l2>\n"
            "</cont>\n";
    err_msg[0] = "Must condition \"../l = 'right'\" not satisfied.";
    err_path[0] = "/i:cont/l2";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, 0, LYD_VALIDATE_PRESENT, LY_EVALID, tree);
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    data = "<cont xmlns=\"urn:tests:i\">\n"
            "  <l>right</l>\n"
            "  <l2>val</l2>\n"
            "</cont>\n";
    LYD_TREE_CREATE(data, tree);
    CHECK_FREE_LYD(tree);

    CONTEXT_DESTROY;
}

static void
test_action(void **state)
{
    (void) state;

    const char *data;
    char *err_msg[1];
    char *err_path[1];
    struct ly_in *in;
    struct lyd_node *tree, *op_tree;

    CONTEXT_CREATE();

    data =
            "<cont xmlns=\"urn:tests:j\">\n"
            "  <l1>\n"
            "    <k>val1</k>\n"
            "    <act>\n"
            "      <lf2>target</lf2>\n"
            "    </act>\n"
            "  </l1>\n"
            "</cont>\n";
    assert_int_equal(LY_SUCCESS, ly_in_new_memory(data, &in));
    assert_int_equal(LY_SUCCESS, lyd_parse_rpc(CONTEXT_GET, in, LYD_XML, &op_tree, NULL));
    assert_non_null(op_tree);

    /* missing leafref */
    assert_int_equal(LY_EVALID, lyd_validate_op(op_tree, NULL, LYD_VALIDATE_OP_RPC, NULL));
    err_msg[0] = "Invalid leafref value \"target\" - no target instance \"/lf3\" with the same value.";
    err_path[0] = "/j:cont/l1[k='val1']/act/lf2";
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);
    ly_in_free(in, 0);

    data = "<cont xmlns=\"urn:tests:j\">\n"
            "  <lf1>not true</lf1>\n"
            "</cont>\n"
            "<lf3 xmlns=\"urn:tests:j\">target</lf3>\n";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, LYD_PARSE_ONLY, 0, LY_SUCCESS, tree);

    /* input must false */
    assert_int_equal(LY_EVALID, lyd_validate_op(op_tree, tree, LYD_VALIDATE_OP_RPC, NULL));
    err_msg[0] = "Must condition \"../../lf1 = 'true'\" not satisfied.";
    err_path[0] = "/j:cont/l1[k='val1']/act";
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    CHECK_FREE_LYD(tree);
    data = "<cont xmlns=\"urn:tests:j\">\n"
            "  <lf1>true</lf1>\n"
            "</cont>\n"
            "<lf3 xmlns=\"urn:tests:j\">target</lf3>\n";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, LYD_PARSE_ONLY, 0, LY_SUCCESS, tree);

    /* success */
    assert_int_equal(LY_SUCCESS, lyd_validate_op(op_tree, tree, LYD_VALIDATE_OP_RPC, NULL));

    lyd_free_tree(op_tree);
    lyd_free_siblings(tree);

    CONTEXT_DESTROY;
}

static void
test_reply(void **state)
{
    (void) state;
    char *err_msg[1];
    char *err_path[1];
    const char *data;
    struct ly_in *in;
    struct lyd_node *tree, *op_tree, *request;

    CONTEXT_CREATE();

    data =
            "<cont xmlns=\"urn:tests:j\">\n"
            "  <l1>\n"
            "    <k>val1</k>\n"
            "    <act>\n"
            "      <lf2>target</lf2>\n"
            "    </act>\n"
            "  </l1>\n"
            "</cont>\n";
    assert_int_equal(LY_SUCCESS, ly_in_new_memory(data, &in));
    assert_int_equal(LY_SUCCESS, lyd_parse_rpc(CONTEXT_GET, in, LYD_XML, &request, NULL));
    assert_non_null(request);
    ly_in_free(in, 0);

    data = "<lf2 xmlns=\"urn:tests:j\">target</lf2>";
    assert_int_equal(LY_SUCCESS, ly_in_new_memory(data, &in));
    assert_int_equal(LY_SUCCESS, lyd_parse_reply(request, in, LYD_XML, &op_tree, NULL));
    lyd_free_all(request);
    assert_non_null(op_tree);
    ly_in_free(in, 0);

    /* missing leafref */
    assert_int_equal(LY_EVALID, lyd_validate_op(op_tree, NULL, LYD_VALIDATE_OP_REPLY, NULL));
    err_msg[0] = "Invalid leafref value \"target\" - no target instance \"/lf4\" with the same value.";
    err_path[0] = "/j:cont/l1[k='val1']/act/lf2";
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    data = "<cont xmlns=\"urn:tests:j\">\n"
            "  <lf1>not true</lf1>\n"
            "</cont>\n"
            "<lf4 xmlns=\"urn:tests:j\">target</lf4>\n";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, LYD_PARSE_ONLY, 0, LY_SUCCESS, tree);

    /* input must false */
    assert_int_equal(LY_EVALID, lyd_validate_op(op_tree, tree, LYD_VALIDATE_OP_REPLY, NULL));
    err_msg[0] = "Must condition \"../../lf1 = 'true2'\" not satisfied.";
    err_path[0] = "/j:cont/l1[k='val1']/act";
    CHECK_CTX_ERROR(CONTEXT_GET, err_msg, err_path);

    CHECK_FREE_LYD(tree);
    data = "<cont xmlns=\"urn:tests:j\">\n"
            "  <lf1>true2</lf1>\n"
            "</cont>\n"
            "<lf4 xmlns=\"urn:tests:j\">target</lf4>\n";
    CHECK_PARSE_LYD_PARAM(data, LYD_XML, LYD_PARSE_ONLY, 0, LY_SUCCESS, tree);

    /* success */
    assert_int_equal(LY_SUCCESS, lyd_validate_op(op_tree, tree, LYD_VALIDATE_OP_REPLY, NULL));

    lyd_free_tree(op_tree);
    CHECK_FREE_LYD(tree);

    CONTEXT_DESTROY;
}

int
main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_when),
        cmocka_unit_test(test_mandatory),
        cmocka_unit_test(test_minmax),
        cmocka_unit_test(test_unique),
        cmocka_unit_test(test_unique_nested),
        cmocka_unit_test(test_dup),
        cmocka_unit_test(test_defaults),
        cmocka_unit_test(test_state),
        cmocka_unit_test(test_must),
        cmocka_unit_test(test_action),
        cmocka_unit_test(test_reply),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
