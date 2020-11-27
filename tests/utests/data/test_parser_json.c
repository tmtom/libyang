/*
 * @file test_parser_xml.c
 * @author: Radek Krejci <rkrejci@cesnet.cz>
 * @brief unit tests for functions from parser_xml.c
 *
 * Copyright (c) 2019 CESNET, z.s.p.o.
 *
 * This source code is licensed under BSD 3-Clause License (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://opensource.org/licenses/BSD-3-Clause
 */

#include "context.h"
#include "in.h"
#include "out.h"
#include "parser_data.h"
#include "printer_data.h"
#include "tests/config.h"
#include "tree_data_internal.h"
#include "tree_schema.h"
#include "utests.h"

#define CONTEXT_CREATE \
                ly_set_log_clb(logger_null, 1);\
                CONTEXT_CREATE_PATH(TESTS_DIR_MODULES_YANG);\
                {\
                    const struct lys_module *mod;\
                    assert_non_null((mod = ly_ctx_load_module(CONTEXT_GET, "ietf-netconf", "2011-06-01", feats)));\
                }\
                assert_non_null(ly_ctx_load_module(CONTEXT_GET, "ietf-netconf-with-defaults", "2011-06-01", NULL));\
                assert_int_equal(LY_SUCCESS, lys_parse_mem(CONTEXT_GET, schema_a, LYS_IN_YANG, NULL))\


#define PARSER_CHECK_ERROR(INPUT, PARSE_OPTION, MODEL, RET_VAL, ERR_MESSAGE, ERR_PATH) \
                assert_int_equal(RET_VAL, lyd_parse_data_mem(CONTEXT_GET, data, LYD_JSON, PARSE_OPTION, LYD_VALIDATE_PRESENT, &MODEL));\
                CHECK_CTX_ERROR(CONTEXT_GET, ERR_MESSAGE, ERR_PATH);\
                assert_null(MODEL)

const char *schema_a = "module a {namespace urn:tests:a;prefix a;yang-version 1.1; import ietf-yang-metadata {prefix md;}"
        "md:annotation hint { type int8;}"
        "list l1 { key \"a b c\"; leaf a {type string;} leaf b {type string;} leaf c {type int16;} leaf d {type string;}}"
        "leaf foo { type string;}"
        "container c {"
        "    leaf x {type string;}"
        "    action act { input { leaf al {type string;} } output { leaf al {type uint8;} } }"
        "    notification n1 { leaf nl {type string;} }"
        "}"
        "container cp {presence \"container switch\"; leaf y {type string;} leaf z {type int8;}}"
        "anydata any {config false;}"
        "leaf-list ll1 { type uint8; }"
        "leaf foo2 { type string; default \"default-val\"; }"
        "leaf foo3 { type uint32; }"
        "notification n2;}";
const char *feats[] = {"writable-running", NULL};

static void
test_leaf(void **state)
{
    (void) state;
    char *err_path[1];
    char *err_msg[1];
    const char *data = "{\"a:foo\":\"foo value\"}";
    struct lyd_node *tree;
    struct lyd_node_term *leaf;
    int unsigned flag;

    CONTEXT_CREATE;

    CHECK_PARSE_LYD(data, LYD_JSON, 0, LYD_VALIDATE_PRESENT, LY_SUCCESS, tree);
    flag = LYS_CONFIG_W | LYS_STATUS_CURR;
    CHECK_LYSC_NODE(tree->schema, NULL, 0, flag, 1, "foo", 1, LYS_LEAF, 0, 0, NULL, 0);
    leaf = (struct lyd_node_term *)tree;
    CHECK_LYD_VALUE(leaf->value, STRING, "foo value");

    flag = 0x205;
    CHECK_LYSC_NODE(tree->next->next->schema, NULL, 0, flag, 1, "foo2", 1, LYS_LEAF, 0, 0, NULL, 0);
    leaf = (struct lyd_node_term *)tree->next->next;

    CHECK_LYD_VALUE(leaf->value, STRING, "default-val");
    assert_true(leaf->flags & LYD_DEFAULT);

    CHECK_LYD_STRING(tree, data, LYD_JSON, LYD_PRINT_WITHSIBLINGS | LYD_PRINT_SHRINK);
    CHECK_FREE_LYD(tree);

    /* make foo2 explicit */
    data = "{\"a:foo2\":\"default-val\"}";
    CHECK_PARSE_LYD(data, LYD_JSON, 0, LYD_VALIDATE_PRESENT, LY_SUCCESS, tree);
    assert_non_null(tree);
    tree = tree->next;
    flag = 0x205;
    CHECK_LYSC_NODE(tree->schema, NULL, 0, flag, 1, "foo2", 1, LYS_LEAF, 0, 0, NULL, 0);
    leaf = (struct lyd_node_term *)tree;
    CHECK_LYD_VALUE(leaf->value, STRING, "default-val");
    assert_false(leaf->flags & LYD_DEFAULT);

    CHECK_LYD_STRING(tree, data, LYD_JSON, LYD_PRINT_WITHSIBLINGS | LYD_PRINT_SHRINK);
    CHECK_FREE_LYD(tree);

    /* parse foo2 but make it implicit */
    data = "{\"a:foo2\":\"default-val\",\"@a:foo2\":{\"ietf-netconf-with-defaults:default\":true}}";
    CHECK_PARSE_LYD(data, LYD_JSON, 0, LYD_VALIDATE_PRESENT, LY_SUCCESS, tree);
    assert_non_null(tree);
    tree = tree->next;
    CHECK_LYSC_NODE(tree->schema, NULL, 0, flag, 1, "foo2", 1, LYS_LEAF, 0, 0, NULL, 0);
    leaf = (struct lyd_node_term *)tree;
    CHECK_LYD_VALUE(leaf->value, STRING, "default-val");
    assert_true(leaf->flags & LYD_DEFAULT);

    /* TODO default values
    lyd_print_tree(out, tree, LYD_JSON, LYD_PRINT_SHRINK);
    assert_string_equal(printed, data);
    ly_out_reset(out);
    */
    CHECK_FREE_LYD(tree);

    /* multiple meatadata hint and unknown metadata xxx supposed to be skipped since it is from missing schema */
    data = "{\"@a:foo\":{\"a:hint\":1,\"a:hint\":2,\"x:xxx\":{\"value\":\"/x:no/x:yes\"}},\"a:foo\":\"xxx\"}";
    CHECK_PARSE_LYD(data, LYD_JSON, 0, LYD_VALIDATE_PRESENT, LY_SUCCESS, tree);
    flag = 0x5;
    CHECK_LYSC_NODE(tree->schema, NULL, 0, flag, 1, "foo", 1, LYS_LEAF, 0, 0, NULL, 0);
    CHECK_LYD_META(tree->meta, 1, "hint", 1, 1,  INT8, "1", 1);
    CHECK_LYD_META(tree->meta->next, 1, "hint", 0, 1,  INT8, "2", 2);
    assert_null(tree->meta->next->next);

    const char *result = "{\"a:foo\":\"xxx\",\"@a:foo\":{\"a:hint\":1,\"a:hint\":2}}";
    CHECK_LYD_STRING(tree, result, LYD_JSON, LYD_PRINT_SHRINK | LYD_PRINT_WITHSIBLINGS);
    CHECK_FREE_LYD(tree);

    err_msg[0] = "Unknown (or not implemented) YANG module \"x\" for metadata \"x:xxx\".";
    err_path[0] = "/a:foo";
    PARSER_CHECK_ERROR(data, LYD_PARSE_STRICT, tree, LY_EVALID, err_msg, err_path);

    /* missing referenced metadata node */
    data = "{\"@a:foo\" : { \"a:hint\" : 1 }}";
    err_msg[0] = "Missing JSON data instance to be coupled with @a:foo metadata.";
    err_path[0] = "/";
    PARSER_CHECK_ERROR(data, 0, tree, LY_EVALID, err_msg, err_path);

    /* missing namespace for meatadata*/
    data = "{\"a:foo\" : \"value\", \"@a:foo\" : { \"hint\" : 1 }}";
    err_msg[0] = "Metadata in JSON must be namespace-qualified, missing prefix for \"hint\".";
    err_path[0] = "/a:foo";
    PARSER_CHECK_ERROR(data, 0, tree, LY_EVALID, err_msg, err_path);

    CONTEXT_DESTROY;
}

static void
test_leaflist(void **state)
{
    (void) state;

    const char *data = "{\"a:ll1\":[10,11]}";
    struct lyd_node *tree;
    struct lyd_node_term *ll;
    int unsigned flag;

    CONTEXT_CREATE;

    CHECK_PARSE_LYD(data, LYD_JSON, 0, LYD_VALIDATE_PRESENT, LY_SUCCESS, tree);
    assert_non_null(tree);
    tree = tree->next;
    flag = LYS_CONFIG_W | LYS_STATUS_CURR | LYS_SET_RANGE;
    CHECK_LYSC_NODE(tree->schema, NULL, 0, flag, 1, "ll1", 1, LYS_LEAFLIST, 0, 0, NULL, 0);
    ll = (struct lyd_node_term *)tree;
    CHECK_LYD_VALUE(ll->value, UINT8, "10", 10);

    assert_non_null(tree->next);
    flag = 0x85;
    CHECK_LYSC_NODE(tree->next->schema, NULL, 0, flag, 1, "ll1", 1, LYS_LEAFLIST, 0, 0, NULL, 0);
    ll = (struct lyd_node_term *)tree->next;
    CHECK_LYD_VALUE(ll->value, UINT8, "11", 11);

    CHECK_LYD_STRING(tree, data, LYD_JSON, LYD_PRINT_WITHSIBLINGS | LYD_PRINT_SHRINK);
    CHECK_FREE_LYD(tree);

    /* simple metadata */
    data = "{\"a:ll1\":[10,11],\"@a:ll1\":[null,{\"a:hint\":2}]}";
    CHECK_PARSE_LYD(data, LYD_JSON, 0, LYD_VALIDATE_PRESENT, LY_SUCCESS, tree);
    assert_non_null(tree);
    tree = tree->next;
    flag = 0x85;
    CHECK_LYSC_NODE(tree->schema, NULL, 0, flag, 1, "ll1", 1, LYS_LEAFLIST, 0, 0, NULL, 0);
    ll = (struct lyd_node_term *)tree;
    CHECK_LYD_VALUE(ll->value, UINT8, "10", 10);
    assert_null(ll->meta);

    assert_non_null(tree->next);
    CHECK_LYSC_NODE(tree->next->schema, NULL, 0, flag, 1, "ll1", 1, LYS_LEAFLIST, 0, 0, NULL, 0);
    ll = (struct lyd_node_term *)tree->next;
    CHECK_LYD_VALUE(ll->value, UINT8, "11", 11);
    CHECK_LYD_META(ll->meta, 1, "hint", 0, 1,  INT8, "2", 2);
    assert_null(ll->meta->next);

    CHECK_LYD_STRING(tree, data, LYD_JSON, LYD_PRINT_WITHSIBLINGS | LYD_PRINT_SHRINK);
    CHECK_FREE_LYD(tree);

    /* multiple meatadata hint and unknown metadata xxx supposed to be skipped since it is from missing schema */
    data = "{\"@a:ll1\" : [{\"a:hint\" : 1, \"x:xxx\" :  { \"value\" : \"/x:no/x:yes\" }, \"a:hint\" : 10},null,{\"a:hint\" : 3}], \"a:ll1\" : [1,2,3]}";
    CHECK_PARSE_LYD(data, LYD_JSON, 0, LYD_VALIDATE_PRESENT, LY_SUCCESS, tree);
    assert_non_null(tree);
    tree = tree->next;
    CHECK_LYSC_NODE(tree->schema, NULL, 0, flag, 1, "ll1", 1, LYS_LEAFLIST, 0, 0, NULL, 0);
    ll = (struct lyd_node_term *)tree;
    CHECK_LYD_VALUE(ll->value, UINT8, "1", 1);
    CHECK_LYD_META(ll->meta, 1, "hint", 1, 1,  INT8, "1", 1);
    CHECK_LYD_META(ll->meta->next, 1, "hint", 0, 1,  INT8, "10", 10);

    assert_non_null(tree->next);
    CHECK_LYSC_NODE(tree->next->schema, NULL, 0, flag, 1, "ll1", 1, LYS_LEAFLIST, 0, 0, NULL, 0);
    ll = (struct lyd_node_term *)tree->next;
    CHECK_LYD_VALUE(ll->value, UINT8, "2", 2);
    assert_null(ll->meta);

    assert_non_null(tree->next->next);
    CHECK_LYSC_NODE(tree->next->next->schema, NULL, 0, flag, 1, "ll1", 1, LYS_LEAFLIST, 0, 0, NULL, 0);
    ll = (struct lyd_node_term *)tree->next->next;
    CHECK_LYD_VALUE(ll->value, UINT8, "3", 3);
    CHECK_LYD_META(ll->meta, 1, "hint", 0, 1,  INT8, "3", 3);
    assert_null(ll->meta->next);

    const char *result = "{\"a:ll1\":[1,2,3],\"@a:ll1\":[{\"a:hint\":1,\"a:hint\":10},null,{\"a:hint\":3}]}";
    CHECK_LYD_STRING(tree, result, LYD_JSON, LYD_PRINT_SHRINK | LYD_PRINT_WITHSIBLINGS)
    CHECK_FREE_LYD(tree);

    /* missing referenced metadata node */
    char *err_path[1];
    char *err_msg[1];

    data = "{\"@a:ll1\":[{\"a:hint\":1}]}";
    err_msg[0] = "Missing JSON data instance to be coupled with @a:ll1 metadata.";
    err_path[0] = "/";
    PARSER_CHECK_ERROR(data, 0, tree, LY_EVALID, err_msg, err_path);

    data = "{\"a:ll1\":[1],\"@a:ll1\":[{\"a:hint\":1},{\"a:hint\":2}]}";
    err_msg[0] = "Missing JSON data instance no. 2 of a:ll1 to be coupled with metadata.";
    err_path[0] = "/";
    PARSER_CHECK_ERROR(data, 0, tree, LY_EVALID, err_msg, err_path);

    data = "{\"@a:ll1\":[{\"a:hint\":1},{\"a:hint\":2},{\"a:hint\":3}],\"a:ll1\" : [1, 2]}";
    err_msg[0] = "Missing 3rd JSON data instance to be coupled with @a:ll1 metadata.";
    err_path[0] = "/";
    PARSER_CHECK_ERROR(data, 0, tree, LY_EVALID, err_msg, err_path);

    CONTEXT_DESTROY;
}

static void
test_anydata(void **state)
{
    (void) state;

    const char *data;
    struct lyd_node *tree;
    int unsigned flag;

    CONTEXT_CREATE;

    data = "{\"a:any\":{\"x:element1\":{\"element2\":\"/a:some/a:path\",\"list\":[{},{\"key\":\"a\"}]}}}";
    CHECK_PARSE_LYD(data, LYD_JSON, 0, LYD_VALIDATE_PRESENT, LY_SUCCESS, tree);
    assert_non_null(tree);
    tree = tree->next;
    flag = LYS_SET_ENUM | LYS_CONFIG_R | LYS_YIN_ARGUMENT;
    CHECK_LYSC_NODE(tree->schema, NULL, 0, flag, 1, "any", 1, LYS_ANYDATA, 0, 0, NULL, 0);
    CHECK_LYD_STRING(tree, data, LYD_JSON, LYD_PRINT_WITHSIBLINGS | LYD_PRINT_SHRINK);
    CHECK_FREE_LYD(tree);

    CONTEXT_DESTROY;
}

static void
test_list(void **state)
{
    (void) state;

    const char *data = "{\"a:l1\":[{\"a\":\"one\",\"b\":\"one\",\"c\":1}]}";
    struct lyd_node *tree, *iter;
    struct lyd_node_inner *list;
    struct lyd_node_term *leaf;
    int unsigned flag;

    CONTEXT_CREATE;

    /* check hashes */
    CHECK_PARSE_LYD(data, LYD_JSON, 0, LYD_VALIDATE_PRESENT, LY_SUCCESS, tree);
    flag = LYS_CONFIG_W | LYS_STATUS_CURR | LYS_SET_RANGE;
    CHECK_LYSC_NODE(tree->schema, NULL, 0, flag, 1, "l1", 1, LYS_LIST, 0, 0, NULL, 0);
    list = (struct lyd_node_inner *)tree;
    LY_LIST_FOR(list->child, iter) {
        assert_int_not_equal(0, iter->hash);
    }

    CHECK_LYD_STRING(tree, data, LYD_JSON, LYD_PRINT_WITHSIBLINGS | LYD_PRINT_SHRINK);
    CHECK_FREE_LYD(tree);

    /* missing keys */
    char *err_path[1];
    char *err_msg[1];

    data = "{ \"a:l1\": [ {\"c\" : 1, \"b\" : \"b\"}]}";
    err_msg[0] = "List instance is missing its key \"a\".";
    err_path[0] = "/a:l1[b='b'][c='1']";
    PARSER_CHECK_ERROR(data, 0, tree, LY_EVALID, err_msg, err_path);

    data = "{ \"a:l1\": [ {\"a\" : \"a\"}]}";
    err_msg[0] = "List instance is missing its key \"b\".";
    err_path[0] = "/a:l1[a='a']";
    PARSER_CHECK_ERROR(data, 0, tree, LY_EVALID, err_msg, err_path);

    data = "{ \"a:l1\": [ {\"b\" : \"b\", \"a\" : \"a\"}]}";
    err_msg[0] = "List instance is missing its key \"c\".";
    err_path[0] = "/a:l1[a='a'][b='b']";
    PARSER_CHECK_ERROR(data, 0, tree, LY_EVALID, err_msg, err_path);

    /* key duplicate */
    data = "{ \"a:l1\": [ {\"c\" : 1, \"b\" : \"b\", \"a\" : \"a\", \"c\" : 1}]}";
    err_msg[0] = "Duplicate instance of \"c\".";
    err_path[0] = "/a:l1[a='a'][b='b'][c='1'][c='1']/c";
    PARSER_CHECK_ERROR(data, 0, tree, LY_EVALID, err_msg, err_path);

    /* keys order, in contrast to XML, JSON accepts keys in any order even in strict mode */
    data = "{ \"a:l1\": [ {\"d\" : \"d\", \"a\" : \"a\", \"c\" : 1, \"b\" : \"b\"}]}";
    CHECK_PARSE_LYD(data, LYD_JSON, 0, LYD_VALIDATE_PRESENT, LY_SUCCESS, tree);
    flag = 0x85;
    CHECK_LYSC_NODE(tree->schema, NULL, 0, flag, 1, "l1", 1, LYS_LIST, 0, 0, NULL, 0);
    list = (struct lyd_node_inner *)tree;
    assert_non_null(leaf = (struct lyd_node_term *)list->child);
    flag = 0x105;
    CHECK_LYSC_NODE(leaf->schema, NULL, 0, flag, 1, "a", 1, LYS_LEAF, 1, 0, NULL, 0);
    assert_non_null(leaf = (struct lyd_node_term *)leaf->next);
    CHECK_LYSC_NODE(leaf->schema, NULL, 0, flag, 1, "b", 1, LYS_LEAF, 1, 0, NULL, 0);
    assert_non_null(leaf = (struct lyd_node_term *)leaf->next);
    CHECK_LYSC_NODE(leaf->schema, NULL, 0, flag, 1, "c", 1, LYS_LEAF, 1, 0, NULL, 0);
    assert_non_null(leaf = (struct lyd_node_term *)leaf->next);
    flag = 0x5;
    CHECK_LYSC_NODE(leaf->schema, NULL, 0, flag, 1, "d", 0, LYS_LEAF, 1, 0, NULL, 0);
    CHECK_CTX_ERROR_NONE(CONTEXT_GET);

    const char *result = "{\"a:l1\":[{\"a\":\"a\",\"b\":\"b\",\"c\":1,\"d\":\"d\"}]}";
    CHECK_LYD_STRING(tree, result, LYD_JSON, LYD_PRINT_SHRINK | LYD_PRINT_WITHSIBLINGS);
    CHECK_FREE_LYD(tree);

    /*  */
    data = "{\"a:l1\":[{\"c\":1,\"b\":\"b\",\"a\":\"a\"}]}";
    CHECK_PARSE_LYD(data, LYD_JSON, LYD_PARSE_STRICT, LYD_VALIDATE_PRESENT, LY_SUCCESS, tree);
    flag = 0x85;
    CHECK_LYSC_NODE(tree->schema, NULL, 0, flag, 1, "l1", 1, LYS_LIST, 0, 0, NULL, 0);
    list = (struct lyd_node_inner *)tree;
    flag = 0x105;
    assert_non_null(leaf = (struct lyd_node_term *)list->child);
    CHECK_LYSC_NODE(leaf->schema, NULL, 0, flag, 1, "a", 1, LYS_LEAF, 1, 0, NULL, 0);
    assert_non_null(leaf = (struct lyd_node_term *)leaf->next);
    CHECK_LYSC_NODE(leaf->schema, NULL, 0, flag, 1, "b", 1, LYS_LEAF, 1, 0, NULL, 0);
    assert_non_null(leaf = (struct lyd_node_term *)leaf->next);
    CHECK_LYSC_NODE(leaf->schema, NULL, 0, flag, 1, "c", 1, LYS_LEAF, 1, 0, NULL, 0);
    CHECK_CTX_ERROR_NONE(CONTEXT_GET);

    result = "{\"a:l1\":[{\"a\":\"a\",\"b\":\"b\",\"c\":1}]}";
    CHECK_LYD_STRING(tree, result, LYD_JSON, LYD_PRINT_SHRINK | LYD_PRINT_WITHSIBLINGS);
    CHECK_FREE_LYD(tree);

    data = "{\"a:cp\":{\"@\":{\"a:hint\":1}}}";
    CHECK_PARSE_LYD(data, LYD_JSON, 0, LYD_VALIDATE_PRESENT, LY_SUCCESS, tree);
    assert_non_null(tree);
    tree = tree->next;
    flag = 0x85;
    CHECK_LYSC_NODE(tree->schema, NULL, 0, flag, 1, "cp", 1, LYS_CONTAINER, 0, 0, NULL, 0);
    CHECK_LYD_META(tree->meta, 1, "hint", 0, 1,  INT8, "1", 1);
    assert_null(tree->meta->next);

    CHECK_LYD_STRING(tree, data, LYD_JSON, LYD_PRINT_WITHSIBLINGS | LYD_PRINT_SHRINK);
    CHECK_FREE_LYD(tree);

    CONTEXT_DESTROY;
}

static void
test_container(void **state)
{
    (void) state;

    const char *data = "{\"a:c\":{}}";
    struct lyd_node *tree;
    struct lyd_node_inner *cont;
    int unsigned flag;

    CONTEXT_CREATE;

    CHECK_PARSE_LYD(data, LYD_JSON, 0, LYD_VALIDATE_PRESENT, LY_SUCCESS, tree);
    flag = 0x5;
    CHECK_LYSC_NODE(tree->schema, NULL, 0, flag, 1, "c", 1, LYS_CONTAINER, 0, 0, NULL, 0);
    cont = (struct lyd_node_inner *)tree;
    assert_true(cont->flags & LYD_DEFAULT);

    CHECK_LYD_STRING(tree, "{}", LYD_JSON, LYD_PRINT_SHRINK | LYD_PRINT_WITHSIBLINGS);
    CHECK_FREE_LYD(tree);

    data = "{\"a:cp\":{}}";
    CHECK_PARSE_LYD(data, LYD_JSON, 0, LYD_VALIDATE_PRESENT, LY_SUCCESS, tree);
    assert_non_null(tree);
    tree = tree->next;
    flag = 0x85;
    CHECK_LYSC_NODE(tree->schema, NULL, 0, flag, 1, "cp", 1, LYS_CONTAINER, 0, 0, NULL, 0);
    cont = (struct lyd_node_inner *)tree;
    assert_false(cont->flags & LYD_DEFAULT);

    CHECK_LYD_STRING(tree, data, LYD_JSON, LYD_PRINT_WITHSIBLINGS | LYD_PRINT_SHRINK);
    CHECK_FREE_LYD(tree);

    CONTEXT_DESTROY;
}

static void
test_opaq(void **state)
{
    (void) state;

    const char *data;
    struct lyd_node *tree;
    char *err_msg[1];
    char *err_path[1];

    CONTEXT_CREATE;

    /* invalid value, no flags */
    data = "{\"a:foo3\":[null]}";
    err_msg[0] = "Invalid non-number-encoded uint32 value \"\".";
    err_path[0] = "/a:foo3";
    PARSER_CHECK_ERROR(data, 0, tree, LY_EVALID, err_msg, err_path);

    /* opaq flag */
    CHECK_PARSE_LYD(data, LYD_JSON, LYD_PARSE_OPAQ, LYD_VALIDATE_PRESENT, LY_SUCCESS, tree);
    CHECK_LYD_NODE_OPAQ((struct lyd_node_opaq *)tree, 0, 0, LY_PREF_JSON, "foo3", 0, 0, NULL,  0,  "");
    CHECK_LYD_STRING(tree, data, LYD_JSON, LYD_PRINT_WITHSIBLINGS | LYD_PRINT_SHRINK);
    CHECK_FREE_LYD(tree);

    /* missing key, no flags */
    data = "{\"a:l1\":[{\"a\":\"val_a\",\"b\":\"val_b\",\"d\":\"val_d\"}]}";
    err_msg[0] = "List instance is missing its key \"c\".";
    err_path[0] = "/a:l1[a='val_a'][b='val_b']";
    PARSER_CHECK_ERROR(data, 0, tree, LY_EVALID, err_msg, err_path);

    /* opaq flag */
    CHECK_PARSE_LYD(data, LYD_JSON, LYD_PARSE_OPAQ, LYD_VALIDATE_PRESENT, LY_SUCCESS, tree);
    CHECK_LYD_NODE_OPAQ((struct lyd_node_opaq *)tree, 0, 0x1, LY_PREF_JSON, "l1", 0, 0, NULL,  0,  "");
    CHECK_LYD_STRING(tree, data, LYD_JSON, LYD_PRINT_WITHSIBLINGS | LYD_PRINT_SHRINK);
    CHECK_FREE_LYD(tree);

    /* invalid key, no flags */
    data = "{\"a:l1\":[{\"a\":\"val_a\",\"b\":\"val_b\",\"c\":\"val_c\"}]}";
    err_msg[0] = "Invalid non-number-encoded int16 value \"val_c\".";
    err_path[0] = "/a:l1/c";
    PARSER_CHECK_ERROR(data, 0, tree, LY_EVALID, err_msg, err_path);

    /* opaq flag */
    CHECK_PARSE_LYD(data, LYD_JSON, LYD_PARSE_OPAQ, LYD_VALIDATE_PRESENT, LY_SUCCESS, tree);
    CHECK_LYD_NODE_OPAQ((struct lyd_node_opaq *)tree, 0, 0x1, LY_PREF_JSON, "l1", 0, 0, NULL,  0,  "");
    CHECK_LYD_STRING(tree, data, LYD_JSON, LYD_PRINT_WITHSIBLINGS | LYD_PRINT_SHRINK);
    CHECK_FREE_LYD(tree);

    data = "{\"a:l1\":[{\"a\":\"val_a\",\"b\":\"val_b\",\"c\":{\"val\":\"val_c\"}}]}";
    CHECK_PARSE_LYD(data, LYD_JSON, LYD_PARSE_OPAQ, LYD_VALIDATE_PRESENT, LY_SUCCESS, tree);
    CHECK_LYD_NODE_OPAQ((struct lyd_node_opaq *)tree, 0, 0x1, LY_PREF_JSON, "l1", 0, 0, NULL,  0,  "");
    CHECK_LYD_STRING(tree, data, LYD_JSON, LYD_PRINT_WITHSIBLINGS | LYD_PRINT_SHRINK);
    CHECK_FREE_LYD(tree);

    data = "{\"a:l1\":[{\"a\":\"val_a\",\"b\":\"val_b\"}]}";
    CHECK_PARSE_LYD(data, LYD_JSON, LYD_PARSE_OPAQ, LYD_VALIDATE_PRESENT, LY_SUCCESS, tree);
    CHECK_LYD_NODE_OPAQ((struct lyd_node_opaq *)tree, 0, 0x1, LY_PREF_JSON, "l1", 0, 0, NULL,  0,  "");
    CHECK_LYD_STRING(tree, data, LYD_JSON, LYD_PRINT_WITHSIBLINGS | LYD_PRINT_SHRINK);
    CHECK_FREE_LYD(tree);

    CONTEXT_DESTROY;
}

static void
test_rpc(void **state)
{
    (void) state;

    const char *data;
    struct ly_in *in;
    struct lyd_node *tree, *op;
    const struct lyd_node *node;
    int unsigned flag;

    CONTEXT_CREATE;

    data = "{\"ietf-netconf:rpc\":{\"edit-config\":{"
            "\"target\":{\"running\":[null]},"
            "\"config\":{\"a:cp\":{\"z\":[null],\"@z\":{\"ietf-netconf:operation\":\"replace\"}},"
            "\"a:l1\":[{\"@\":{\"ietf-netconf:operation\":\"replace\"},\"a\":\"val_a\",\"b\":\"val_b\",\"c\":\"val_c\"}]}"
            "}}}";
    assert_int_equal(LY_SUCCESS, ly_in_new_memory(data, &in));
    assert_int_equal(LY_SUCCESS, lyd_parse_rpc(CONTEXT_GET, in, LYD_JSON, &tree, &op));
    ly_in_free(in, 0);

    assert_non_null(op);
    const char *dsc = "The <edit-config> operation loads all or part of a specified\n"
            "configuration to the specified target configuration.";
    const char *ref = "RFC 6241, Section 7.2";

    CHECK_LYSC_ACTION((struct lysc_action *)op->schema, dsc, 0, LYS_STATUS_CURR,
            1, 0, 0, 1, "edit-config", LYS_RPC,
            0, 0, 0, 0, 0, ref, 0);

    CHECK_LYD_NODE_OPAQ((struct lyd_node_opaq *)tree, 0, 0x1, LY_PREF_JSON, "rpc", 0, 0, NULL,  0,  "");
    /* TODO support generic attributes in JSON ?
    assert_non_null(((struct lyd_node_opaq *)tree)->attr);
    */

    node = lyd_child(tree);
    CHECK_LYSC_ACTION((struct lysc_action *)node->schema, dsc, 0, LYS_STATUS_CURR,
            1, 0, 0, 1, "edit-config", LYS_RPC,
            0, 0, 0, 0, 0, ref, 0);
    node = lyd_child(node)->next;
    flag = 0x5;
    CHECK_LYSC_NODE(node->schema, "Inline Config content.", 0, flag, 1, "config", 0, LYS_ANYXML, 1, 0, NULL, 0);

    node = ((struct lyd_node_any *)node)->value.tree;
    flag = 0x85;
    CHECK_LYSC_NODE(node->schema, NULL, 0, flag, 1, "cp", 1, LYS_CONTAINER, 0, 0, NULL, 0);
    node = lyd_child(node);
    /* z has no value */
    CHECK_LYD_NODE_OPAQ((struct lyd_node_opaq *)node, 0x1, 0, LY_PREF_JSON, "z", 0, 0, NULL,  0,  "");
    node = node->parent->next;
    /* l1 key c has invalid value so it is at the end */
    CHECK_LYD_NODE_OPAQ((struct lyd_node_opaq *)node, 0x1, 0x1, LY_PREF_JSON, "l1", 0, 0, NULL,  0,  "");

    CHECK_LYD_STRING(tree, data, LYD_JSON, LYD_PRINT_WITHSIBLINGS | LYD_PRINT_SHRINK);
    CHECK_FREE_LYD(tree);

    /* wrong namespace, element name, whatever... */
    /* TODO */
    CONTEXT_DESTROY;
}

static void
test_action(void **state)
{
    *state = test_action;

    const char *data;
    struct ly_in *in;
    struct lyd_node *tree, *op;
    const struct lyd_node *node;

    CONTEXT_CREATE;

    data = "{\"ietf-netconf:rpc\":{\"yang:action\":{\"a:c\":{\"act\":{\"al\":\"value\"}}}}}";
    assert_int_equal(LY_SUCCESS, ly_in_new_memory(data, &in));
    assert_int_equal(LY_SUCCESS, lyd_parse_rpc(CONTEXT_GET, in, LYD_JSON, &tree, &op));
    ly_in_free(in, 0);

    assert_non_null(op);
    CHECK_LYSC_ACTION((struct lysc_action *)op->schema, NULL, 0, LYS_STATUS_CURR,
            1, 0, 0, 1, "act", LYS_ACTION,
            1, 0, 0, 1, 0, NULL, 0);

    CHECK_LYD_NODE_OPAQ((struct lyd_node_opaq *)tree, 0, 0x1, LY_PREF_JSON, "rpc", 0, 0, NULL,  0,  "");
    node = lyd_child(tree);
    CHECK_LYD_NODE_OPAQ((struct lyd_node_opaq *)node, 0, 0x1, LY_PREF_JSON, "action", 0, 0, NULL,  0,  "");

    CHECK_LYD_STRING(tree, data, LYD_JSON, LYD_PRINT_WITHSIBLINGS | LYD_PRINT_SHRINK);
    CHECK_FREE_LYD(tree);

    /* wrong namespace, element name, whatever... */
    /* TODO */
    CONTEXT_DESTROY;
}

static void
test_notification(void **state)
{
    *state = test_notification;

    const char *data;
    struct ly_in *in;
    struct lyd_node *tree, *ntf;
    const struct lyd_node *node;
    int unsigned flag;

    CONTEXT_CREATE;

    data = "{\"ietf-restconf:notification\":{\"eventTime\":\"2037-07-08T00:01:00Z\",\"a:c\":{\"n1\":{\"nl\":\"value\"}}}}";
    assert_int_equal(LY_SUCCESS, ly_in_new_memory(data, &in));
    assert_int_equal(LY_SUCCESS, lyd_parse_notif(CONTEXT_GET, in, LYD_JSON, &tree, &ntf));
    ly_in_free(in, 0);

    assert_non_null(ntf);
    CHECK_LYSC_NOTIF((struct lysc_notif *)ntf->schema, 1, NULL, 0, 0x4, 1, 0, "n1", 1, 0, NULL, 0);

    CHECK_LYD_NODE_OPAQ((struct lyd_node_opaq *)tree, 0, 0x1, LY_PREF_JSON, "notification", 0, 0, NULL,  0,  "");
    node = lyd_child(tree);
    CHECK_LYD_NODE_OPAQ((struct lyd_node_opaq *)node, 0, 0, LY_PREF_JSON, "eventTime", 0, 0, NULL,  0,  "2037-07-08T00:01:00Z");
    node = node->next;
    flag = 0x5;
    CHECK_LYSC_NODE(node->schema, NULL, 0, flag, 1, "c", 1, LYS_CONTAINER, 0, 0, NULL, 0);

    CHECK_LYD_STRING(tree, data, LYD_JSON, LYD_PRINT_WITHSIBLINGS | LYD_PRINT_SHRINK);
    CHECK_FREE_LYD(tree);

    /* top-level notif without envelope */
    data = "{\"a:n2\":{}}";
    assert_int_equal(LY_SUCCESS, ly_in_new_memory(data, &in));
    assert_int_equal(LY_SUCCESS, lyd_parse_notif(CONTEXT_GET, in, LYD_JSON, &tree, &ntf));
    ly_in_free(in, 0);

    assert_non_null(ntf);
    CHECK_LYSC_NOTIF((struct lysc_notif *)ntf->schema, 0, NULL, 0, 0x4, 1, 0, "n2", 0, 0, NULL, 0);

    assert_non_null(tree);
    assert_ptr_equal(ntf, tree);

    CHECK_LYD_STRING(tree, data, LYD_JSON, LYD_PRINT_WITHSIBLINGS | LYD_PRINT_SHRINK);
    CHECK_FREE_LYD(tree);

    CONTEXT_DESTROY;
    /* wrong namespace, element name, whatever... */
    /* TODO */
}

static void
test_reply(void **state)
{
    (void) state;

    const char *data;
    struct ly_in *in;
    struct lyd_node *request, *tree, *op;
    const struct lyd_node *node;
    int unsigned flag;

    CONTEXT_CREATE;

    data = "{\"a:c\":{\"act\":{\"al\":\"value\"}}}";
    assert_int_equal(LY_SUCCESS, ly_in_new_memory(data, &in));
    assert_int_equal(LY_SUCCESS, lyd_parse_rpc(CONTEXT_GET, in, LYD_JSON, &request, NULL));
    ly_in_free(in, 0);

    data = "{\"ietf-netconf:rpc-reply\":{\"a:al\":25}}";
    assert_int_equal(LY_SUCCESS, ly_in_new_memory(data, &in));
    assert_int_equal(LY_SUCCESS, lyd_parse_reply(request, in, LYD_JSON, &tree, &op));
    ly_in_free(in, 0);
    CHECK_FREE_LYD(request);

    assert_non_null(op);
    CHECK_LYSC_ACTION((struct lysc_action *)op->schema, NULL, 0, LYS_STATUS_CURR,
            1, 0, 0, 1, "act", LYS_ACTION,
            1, 0, 0, 1, 0, NULL, 0);
    node = lyd_child(op);
    flag = 0x6;
    CHECK_LYSC_NODE(node->schema, NULL, 0, flag, 1, "al", 0, LYS_LEAF, 1, 0, NULL, 0);

    CHECK_LYD_NODE_OPAQ((struct lyd_node_opaq *)tree, 0, 0x1, LY_PREF_JSON, "rpc-reply", 0, 0, NULL,  0,  "");
    node = lyd_child(tree);
    flag = 0x5;
    CHECK_LYSC_NODE(node->schema, NULL, 0, flag, 1, "c", 1, LYS_CONTAINER, 0, 0, NULL, 0);

    /* TODO print only rpc-reply node and then output subtree */
    const char *result = "{\"a:al\":25}"; 
    CHECK_LYD_STRING(lyd_child(op), result, LYD_JSON, LYD_PRINT_SHRINK | LYD_PRINT_WITHSIBLINGS);
    result = "{\"a:c\":{\"act\":{\"al\":25}}}";
    CHECK_LYD_STRING(lyd_child(tree), result, LYD_JSON, LYD_PRINT_SHRINK | LYD_PRINT_WITHSIBLINGS);
    CHECK_FREE_LYD(tree);

    /* wrong namespace, element name, whatever... */
    /* TODO */
    CONTEXT_DESTROY;
}

int
main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_leaf),
        cmocka_unit_test(test_leaflist),
        cmocka_unit_test(test_anydata),
        cmocka_unit_test(test_list),
        cmocka_unit_test(test_container),
        cmocka_unit_test(test_opaq),
        cmocka_unit_test(test_rpc),
        cmocka_unit_test(test_action),
        cmocka_unit_test(test_notification),
        cmocka_unit_test(test_reply),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
