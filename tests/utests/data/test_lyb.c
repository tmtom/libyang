/**
 * @file test_lyb.c
 * @author Michal Vasko <mvasko@cesnet.cz>
 * @brief Cmocka tests for LYB binary data format.
 *
 * Copyright (c) 2020 CESNET, z.s.p.o.
 *
 * This source code is licensed under BSD 3-Clause License (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://opensource.org/licenses/BSD-3-Clause
 */

#include "hash_table.h"
#include "libyang.h"
#include "tests/config.h"
#include "utests.h"


static void
test_ietf_interfaces(void **state)
{
    (void) state;
    const char *data_xml =
            "<interfaces xmlns=\"urn:ietf:params:xml:ns:yang:ietf-interfaces\">\n"
            "    <interface>\n"
            "        <name>eth0</name>\n"
            "        <description>Ethernet 0</description>\n"
            "        <type xmlns:ianaift=\"urn:ietf:params:xml:ns:yang:iana-if-type\">ianaift:ethernetCsmacd</type>\n"
            "        <enabled>true</enabled>\n"
            "        <ipv4 xmlns=\"urn:ietf:params:xml:ns:yang:ietf-ip\">\n"
            "            <enabled>true</enabled>\n"
            "            <mtu>1500</mtu>\n"
            "            <address>\n"
            "                <ip>192.168.2.100</ip>\n"
            "                <prefix-length>24</prefix-length>\n"
            "            </address>\n"
            "        </ipv4>\n"
            "    </interface>\n"
            "    <interface>\n"
            "        <name>eth1</name>\n"
            "        <description>Ethernet 1</description>\n"
            "        <type xmlns:ianaift=\"urn:ietf:params:xml:ns:yang:iana-if-type\">ianaift:ethernetCsmacd</type>\n"
            "        <enabled>true</enabled>\n"
            "        <ipv4 xmlns=\"urn:ietf:params:xml:ns:yang:ietf-ip\">\n"
            "            <enabled>true</enabled>\n"
            "            <mtu>1500</mtu>\n"
            "            <address>\n"
            "                <ip>10.10.1.5</ip>\n"
            "                <prefix-length>16</prefix-length>\n"
            "            </address>\n"
            "        </ipv4>\n"
            "    </interface>\n"
            "    <interface>\n"
            "        <name>gigaeth0</name>\n"
            "        <description>GigabitEthernet 0</description>\n"
            "        <type xmlns:ianaift=\"urn:ietf:params:xml:ns:yang:iana-if-type\">ianaift:ethernetCsmacd</type>\n"
            "        <enabled>false</enabled>\n"
            "    </interface>\n"
            "</interfaces>\n";

    CONTEXT_CREATE_PATH(TESTS_DIR_MODULES_YANG);
    assert_non_null(ly_ctx_load_module(CONTEXT_GET, "ietf-ip", NULL, NULL));
    assert_non_null(ly_ctx_load_module(CONTEXT_GET, "iana-if-type", NULL, NULL));

    /* model_1 */
    struct lyd_node *model_1;

    CHECK_PARSE_LYD(data_xml, LYD_XML, LYD_PARSE_ONLY | LYD_PARSE_STRICT, 0, LY_SUCCESS, model_1);

    /* model_2 */
    char *xml_out;

    assert_int_equal(lyd_print_mem(&xml_out, model_1, LYD_LYB, LYD_PRINT_WITHSIBLINGS), 0);
    struct lyd_node *model_2;

    assert_int_equal(LY_SUCCESS, lyd_parse_data_mem(CONTEXT_GET, xml_out, LYD_LYB, LYD_PARSE_ONLY | LYD_PARSE_STRICT, 0, &model_2));
    assert_non_null(model_2);

    /* compare models */
    CHECK_LYD(model_1, model_2);

    /* clean */
    free(xml_out);
    CHECK_FREE_LYD(model_1);
    CHECK_FREE_LYD(model_2);
    CONTEXT_DESTROY;
}

static void
test_origin(void **state)
{
    (void) state;
    const char *origin_yang =
            "module test-origin {"
            "   namespace \"urn:test-origin\";"
            "   prefix to;"
            "   import ietf-origin {"
            "       prefix or;"
            "   }"
            ""
            "   container cont {"
            "       leaf leaf1 {"
            "           type string;"
            "       }"
            "       leaf leaf2 {"
            "           type string;"
            "       }"
            "       leaf leaf3 {"
            "           type uint8;"
            "       }"
            "   }"
            "}";
    const char *data_xml =
            "<cont xmlns=\"urn:test-origin\">\n"
            "  <leaf1 xmlns:or=\"urn:ietf:params:xml:ns:yang:ietf-origin\" or:origin=\"or:default\">value1</leaf1>\n"
            "  <leaf2>value2</leaf2>\n"
            "  <leaf3 xmlns:or=\"urn:ietf:params:xml:ns:yang:ietf-origin\" or:origin=\"or:system\">125</leaf3>\n"
            "</cont>\n";

    CONTEXT_CREATE_PATH(TESTS_DIR_MODULES_YANG);
    assert_int_equal(LY_SUCCESS, lys_parse_mem(CONTEXT_GET, origin_yang, LYS_IN_YANG, NULL));
    lys_set_implemented(ly_ctx_get_module_latest(CONTEXT_GET, "ietf-origin"), NULL);

    /* model_1 */
    struct lyd_node *model_1;

    CHECK_PARSE_LYD(data_xml, LYD_XML, LYD_PARSE_ONLY | LYD_PARSE_STRICT, 0, LY_SUCCESS, model_1);

    /* model_2 */
    char *xml_out;

    assert_int_equal(lyd_print_mem(&xml_out, model_1, LYD_LYB, LYD_PRINT_WITHSIBLINGS), 0);
    struct lyd_node *model_2;

    assert_int_equal(LY_SUCCESS, lyd_parse_data_mem(CONTEXT_GET, xml_out, LYD_LYB, LYD_PARSE_ONLY | LYD_PARSE_STRICT, 0, &model_2));
    assert_non_null(model_2);

    /* compare models */
    CHECK_LYD(model_1, model_2);

    /* clean */
    free(xml_out);
    CHECK_FREE_LYD(model_1);
    CHECK_FREE_LYD(model_2);
    CONTEXT_DESTROY;
}

static void
test_statements(void **state)
{
    (void) state;
    const char *links_yang =
            "module links {\n"
            "    yang-version 1.1;\n"
            "    namespace \"urn:module2\";\n"
            "    prefix mod2;\n"
            "\n"
            "    identity just-another-identity;\n"
            "\n"
            "    leaf one-leaf {\n"
            "        type string;\n"
            "    }\n"
            "\n"
            "    list list-for-augment {\n"
            "        key keyleaf;\n"
            "\n"
            "        leaf keyleaf {\n"
            "            type string;\n"
            "        }\n"
            "\n"
            "        leaf just-leaf {\n"
            "            type int32;\n"
            "        }\n"
            "    }\n"
            "\n"
            "    leaf rleaf {\n"
            "        type string;\n"
            "    }\n"
            "\n"
            "    leaf-list llist {\n"
            "        type string;\n"
            "        min-elements 0;\n"
            "        max-elements 100;\n"
            "        ordered-by user;\n"
            "    }\n"
            "\n"
            "    grouping rgroup {\n"
            "        leaf rg1 {\n"
            "            type string;\n"
            "        }\n"
            "\n"
            "        leaf rg2 {\n"
            "            type string;\n"
            "        }\n"
            "    }\n"
            "}\n";

    const char *statements_yang =
            "module statements {\n"
            "    namespace \"urn:module\";\n"
            "    prefix mod;\n"
            "    yang-version 1.1;\n"
            "\n"
            "    import links {\n"
            "        prefix mod2;\n"
            "    }\n"
            "\n"
            "    identity random-identity {\n"
            "        base \"mod2:just-another-identity\";\n"
            "        base \"another-identity\";\n"
            "    }\n"
            "\n"
            "    identity another-identity {\n"
            "        base \"mod2:just-another-identity\";\n"
            "    }\n"
            "\n"
            "    typedef percent {\n"
            "        type uint8 {\n"
            "            range \"0 .. 100\";\n"
            "        }\n"
            "        units percent;\n"
            "    }\n"
            "\n"
            "    container ice-cream-shop {\n"
            "        container employees {\n"
            "            list employee {\n"
            "                config true;\n"
            "                key id;\n"
            "                unique name;\n"
            "                min-elements 0;\n"
            "                max-elements 100;\n"
            "\n"
            "                leaf id {\n"
            "                    type uint64;\n"
            "                    mandatory true;\n"
            "                }\n"
            "\n"
            "                leaf name {\n"
            "                    type string;\n"
            "                }\n"
            "\n"
            "                leaf age {\n"
            "                    type uint32;\n"
            "                }\n"
            "            }\n"
            "        }\n"
            "    }\n"
            "\n"
            "    container random {\n"
            "        choice switch {\n"
            "            case a {\n"
            "                leaf aleaf {\n"
            "                    type string;\n"
            "                    default aaa;\n"
            "                }\n"
            "            }\n"
            "\n"
            "            case c {\n"
            "                leaf cleaf {\n"
            "                    type string;\n"
            "                }\n"
            "            }\n"
            "        }\n"
            "\n"
            "        anyxml xml-data;\n"
            "        anydata any-data;\n"
            "        leaf-list leaflist {\n"
            "            type string;\n"
            "            min-elements 0;\n"
            "            max-elements 20;\n"
            "            ordered-by system;\n"
            "        }\n"
            "\n"
            "        grouping group {\n"
            "            leaf g1 {\n"
            "                mandatory false;\n"
            "                type percent;\n"
            "            }\n"
            "\n"
            "            leaf g2 {\n"
            "                type string;\n"
            "            }\n"
            "        }\n"
            "\n"
            "        uses group;\n"
            "        uses mod2:rgroup;\n"
            "\n"
            "        leaf lref {\n"
            "            type leafref {\n"
            "                path \"/mod2:one-leaf\";\n"
            "            }\n"
            "        }\n"
            "\n"
            "        leaf iref {\n"
            "            type identityref {\n"
            "                base \"mod2:just-another-identity\";\n"
            "            }\n"
            "        }\n"
            "    }\n"
            "\n"
            "    augment \"/random\" {\n"
            "        leaf aug-leaf {\n"
            "            type string;\n"
            "        }\n"
            "    }\n"
            "}\n";

    const char *data_xml =
            "<ice-cream-shop xmlns=\"urn:module\">\n"
            "  <employees>\n"
            "    <employee>\n"
            "      <id>0</id>\n"
            "      <name>John Doe</name>\n"
            "      <age>28</age>\n"
            "    </employee>\n"
            "    <employee>\n"
            "      <id>1</id>\n"
            "      <name>Dohn Joe</name>\n"
            "      <age>20</age>\n"
            "    </employee>\n"
            "  </employees>\n"
            "</ice-cream-shop>\n"
            "<one-leaf xmlns=\"urn:module2\">reference leaf</one-leaf>\n"
            "<random xmlns=\"urn:module\">\n"
            "  <aleaf>string</aleaf>\n"
            "  <xml-data><anyxml>data</anyxml></xml-data>\n"
            "  <any-data><data>any data</data></any-data>\n"
            "  <leaflist>l0</leaflist>\n"
            "  <leaflist>l1</leaflist>\n"
            "  <leaflist>l2</leaflist>\n"
            "  <g1>40</g1>\n"
            "  <g2>string</g2>\n"
            "  <aug-leaf>string</aug-leaf>\n"
            "  <rg1>string</rg1>\n"
            "  <rg2>string</rg2>\n"
            "  <lref>reference leaf</lref>\n"
            "  <iref>random-identity</iref>\n"
            "</random>\n";

    CONTEXT_CREATE_PATH(TESTS_DIR_MODULES_YANG);
    assert_int_equal(LY_SUCCESS, lys_parse_mem(CONTEXT_GET, links_yang, LYS_IN_YANG, NULL));
    assert_int_equal(LY_SUCCESS, lys_parse_mem(CONTEXT_GET, statements_yang, LYS_IN_YANG, NULL));

    /* model_1 */
    struct lyd_node *model_1;

    CHECK_PARSE_LYD(data_xml, LYD_XML, LYD_PARSE_ONLY | LYD_PARSE_STRICT, 0, LY_SUCCESS, model_1);

    /* model_2 */
    char *xml_out;

    assert_int_equal(lyd_print_mem(&xml_out, model_1, LYD_LYB, LYD_PRINT_WITHSIBLINGS), 0);
    struct lyd_node *model_2;

    assert_int_equal(LY_SUCCESS, lyd_parse_data_mem(CONTEXT_GET, xml_out, LYD_LYB, LYD_PARSE_ONLY | LYD_PARSE_STRICT, 0, &model_2));
    assert_non_null(model_2);

    /* compare models */
    CHECK_LYD(model_1, model_2);

    /* clean */
    free(xml_out);
    CHECK_FREE_LYD(model_1);
    CHECK_FREE_LYD(model_2);
    CONTEXT_DESTROY;
}

#if 0
static void
test_types(void **state)
{
    struct state *st = (*state);
    int ret;

    ly_ctx_set_searchdir(st->ctx, TESTS_DIR "/data/files");
    assert_non_null(ly_ctx_load_module(st->ctx, "types", NULL));

    st->dt1 = lyd_parse_path(st->ctx, TESTS_DIR "/data/files/types.xml", LYD_XML, LYD_OPT_CONFIG);
    assert_ptr_not_equal(st->dt1, NULL);

    ret = lyd_print_mem(&st->mem, st->dt1, LYD_LYB, LYP_WITHSIBLINGS);
    assert_int_equal(ret, 0);

    st->dt2 = lyd_parse_mem(st->ctx, st->mem, LYD_LYB, LYD_OPT_CONFIG | LYD_OPT_STRICT);
    assert_ptr_not_equal(st->dt2, NULL);

    check_data_tree(st->dt1, st->dt2);
}

static void
test_annotations(void **state)
{
    struct state *st = (*state);
    int ret;

    ly_ctx_set_searchdir(st->ctx, TESTS_DIR "/data/files");
    assert_non_null(ly_ctx_load_module(st->ctx, "annotations", NULL));

    st->dt1 = lyd_parse_path(st->ctx, TESTS_DIR "/data/files/annotations.xml", LYD_XML, LYD_OPT_CONFIG);
    assert_ptr_not_equal(st->dt1, NULL);

    ret = lyd_print_mem(&st->mem, st->dt1, LYD_LYB, LYP_WITHSIBLINGS);
    assert_int_equal(ret, 0);

    st->dt2 = lyd_parse_mem(st->ctx, st->mem, LYD_LYB, LYD_OPT_CONFIG | LYD_OPT_STRICT);
    assert_ptr_not_equal(st->dt2, NULL);

    check_data_tree(st->dt1, st->dt2);
}

static void
test_similar_annot_names(void **state)
{
    struct state *st = (*state);
    int ret;

    ly_ctx_set_searchdir(st->ctx, TESTS_DIR "/data/files");
    assert_non_null(ly_ctx_load_module(st->ctx, "annotations", NULL));

    st->dt1 = lyd_parse_path(st->ctx, TESTS_DIR "/data/files/similar-annot-names.xml", LYD_XML, LYD_OPT_CONFIG);
    assert_ptr_not_equal(st->dt1, NULL);

    ret = lyd_print_mem(&st->mem, st->dt1, LYD_LYB, LYP_WITHSIBLINGS);
    assert_int_equal(ret, 0);

    st->dt2 = lyd_parse_mem(st->ctx, st->mem, LYD_LYB, LYD_OPT_CONFIG | LYD_OPT_STRICT);
    assert_ptr_not_equal(st->dt2, NULL);

    check_data_tree(st->dt1, st->dt2);
}

static void
test_many_child_annot(void **state)
{
    struct state *st = (*state);
    int ret;

    ly_ctx_set_searchdir(st->ctx, TESTS_DIR "/data/files");
    assert_non_null(ly_ctx_load_module(st->ctx, "annotations", NULL));

    st->dt1 = lyd_parse_path(st->ctx, TESTS_DIR "/data/files/many-childs-annot.xml", LYD_XML, LYD_OPT_CONFIG);
    assert_ptr_not_equal(st->dt1, NULL);

    ret = lyd_print_mem(&st->mem, st->dt1, LYD_LYB, LYP_WITHSIBLINGS);
    assert_int_equal(ret, 0);

    st->dt2 = lyd_parse_mem(st->ctx, st->mem, LYD_LYB, LYD_OPT_CONFIG | LYD_OPT_STRICT);
    assert_ptr_not_equal(st->dt2, NULL);

    check_data_tree(st->dt1, st->dt2);
}

static void
test_union(void **state)
{
    struct state *st = (*state);
    int ret;

    ly_ctx_set_searchdir(st->ctx, TESTS_DIR "/data/files");
    assert_non_null(ly_ctx_load_module(st->ctx, "union", NULL));

    st->dt1 = lyd_parse_path(st->ctx, TESTS_DIR "/data/files/union.xml", LYD_XML, LYD_OPT_CONFIG);
    assert_ptr_not_equal(st->dt1, NULL);

    ret = lyd_print_mem(&st->mem, st->dt1, LYD_LYB, LYP_WITHSIBLINGS);
    assert_int_equal(ret, 0);

    st->dt2 = lyd_parse_mem(st->ctx, st->mem, LYD_LYB, LYD_OPT_CONFIG | LYD_OPT_STRICT);
    assert_ptr_not_equal(st->dt2, NULL);

    check_data_tree(st->dt1, st->dt2);
}

static void
test_union2(void **state)
{
    struct state *st = (*state);
    int ret;

    ly_ctx_set_searchdir(st->ctx, TESTS_DIR "/data/files");
    assert_non_null(ly_ctx_load_module(st->ctx, "statements", NULL));

    st->dt1 = lyd_parse_path(st->ctx, TESTS_DIR "/data/files/union2.xml", LYD_XML, LYD_OPT_CONFIG);
    assert_ptr_not_equal(st->dt1, NULL);

    ret = lyd_print_mem(&st->mem, st->dt1, LYD_LYB, LYP_WITHSIBLINGS);
    assert_int_equal(ret, 0);

    st->dt2 = lyd_parse_mem(st->ctx, st->mem, LYD_LYB, LYD_OPT_CONFIG | LYD_OPT_STRICT);
    assert_ptr_not_equal(st->dt2, NULL);

    check_data_tree(st->dt1, st->dt2);
}

static void
test_collisions(void **state)
{
    struct state *st = (*state);
    int ret;

    ly_ctx_set_searchdir(st->ctx, TESTS_DIR "/data/files");
    assert_non_null(ly_ctx_load_module(st->ctx, "annotations", NULL));

    st->dt1 = lyd_parse_path(st->ctx, TESTS_DIR "/data/files/collisions.xml", LYD_XML, LYD_OPT_CONFIG);
    assert_ptr_not_equal(st->dt1, NULL);

    ret = lyd_print_mem(&st->mem, st->dt1, LYD_LYB, LYP_WITHSIBLINGS);
    assert_int_equal(ret, 0);

    st->dt2 = lyd_parse_mem(st->ctx, st->mem, LYD_LYB, LYD_OPT_CONFIG | LYD_OPT_STRICT);
    assert_ptr_not_equal(st->dt2, NULL);

    check_data_tree(st->dt1, st->dt2);
}

static void
test_anydata(void **state)
{
    struct state *st = (*state);
    const struct lys_module *mod;
    int ret;
    const char *test_anydata =
            "module test-anydata {"
            "   namespace \"urn:test-anydata\";"
            "   prefix ya;"
            ""
            "   container cont {"
            "       anydata ntf;"
            "   }"
            "}";

    assert_non_null(ly_ctx_load_module(st->ctx, "ietf-netconf-notifications", NULL));

    st->dt1 = lyd_parse_path(st->ctx, TESTS_DIR "/data/files/ietf-netconf-notifications.json", LYD_JSON, LYD_OPT_NOTIF | LYD_OPT_TRUSTED, NULL);
    assert_ptr_not_equal(st->dt1, NULL);

/* get notification in LYB format to set as anydata content */
    ret = lyd_print_mem(&st->mem, st->dt1, LYD_LYB, LYP_WITHSIBLINGS);
    assert_int_equal(ret, 0);

    lyd_free_withsiblings(st->dt1);
    st->dt1 = NULL;

/* now comes the real test, test anydata */
    mod = lys_parse_mem(st->ctx, test_anydata, LYS_YANG);
    assert_non_null(mod);

    st->dt1 = lyd_new(NULL, mod, "cont");
    assert_non_null(st->dt1);

    assert_non_null(lyd_new_anydata(st->dt1, NULL, "ntf", st->mem, LYD_ANYDATA_LYBD));
    st->mem = NULL;

    ret = lyd_print_mem(&st->mem, st->dt1, LYD_LYB, LYP_WITHSIBLINGS);
    assert_int_equal(ret, 0);

    ret = lyd_validate(&st->dt1, LYD_OPT_CONFIG, NULL);
    assert_int_equal(ret, 0);

    st->dt2 = lyd_parse_mem(st->ctx, st->mem, LYD_LYB, LYD_OPT_CONFIG | LYD_OPT_STRICT);
    assert_ptr_not_equal(st->dt2, NULL);

    check_data_tree(st->dt1, st->dt2);

/* and also test the embedded notification itself */
    free(st->mem);
    ret = lyd_lyb_data_length(((struct lyd_node_anydata *)st->dt1->child)->value.mem);
    st->mem = malloc(ret);
    memcpy(st->mem, ((struct lyd_node_anydata *)st->dt1->child)->value.mem, ret);

    lyd_free_withsiblings(st->dt2);
    st->dt2 = lyd_parse_mem(st->ctx, st->mem, LYD_LYB, LYD_OPT_NOTIF | LYD_OPT_STRICT | LYD_OPT_NOEXTDEPS, NULL);
    assert_ptr_not_equal(st->dt2, NULL);

/* parse the JSON again for this comparison */
    lyd_free_withsiblings(st->dt1);
    st->dt1 = lyd_parse_path(st->ctx, TESTS_DIR "/data/files/ietf-netconf-notifications.json", LYD_JSON, LYD_OPT_NOTIF | LYD_OPT_TRUSTED, NULL);
    assert_ptr_not_equal(st->dt1, NULL);

    check_data_tree(st->dt1, st->dt2);
}

static void
test_submodule_feature(void **state)
{
    struct state *st = (*state);
    const struct lys_module *mod;
    int ret;

    ly_ctx_set_searchdir(st->ctx, TESTS_DIR "/data/files");
    mod = ly_ctx_load_module(st->ctx, "feature-submodule-main", NULL);
    assert_non_null(mod);
    assert_int_equal(lys_features_enable(mod, "test-submodule-feature"), 0);

    st->dt1 = lyd_parse_path(st->ctx, TESTS_DIR "/data/files/test-submodule-feature.json", LYD_JSON, LYD_OPT_CONFIG);
    assert_ptr_not_equal(st->dt1, NULL);

    ret = lyd_print_mem(&st->mem, st->dt1, LYD_LYB, LYP_WITHSIBLINGS);
    assert_int_equal(ret, 0);

    st->dt2 = lyd_parse_mem(st->ctx, st->mem, LYD_LYB, LYD_OPT_CONFIG | LYD_OPT_STRICT);
    assert_ptr_not_equal(st->dt2, NULL);

    check_data_tree(st->dt1, st->dt2);
}

static void
test_coliding_augments(void **state)
{
    struct state *st = (*state);
    int ret;

    ly_ctx_set_searchdir(st->ctx, TESTS_DIR "/data/files");
    assert_non_null(ly_ctx_load_module(st->ctx, "augment-target", NULL));
    assert_non_null(ly_ctx_load_module(st->ctx, "augment0", NULL));
    assert_non_null(ly_ctx_load_module(st->ctx, "augment1", NULL));

    st->dt1 = lyd_parse_path(st->ctx, TESTS_DIR "/data/files/augment.xml", LYD_XML, LYD_OPT_CONFIG);
    assert_ptr_not_equal(st->dt1, NULL);

    ret = lyd_print_mem(&st->mem, st->dt1, LYD_LYB, LYP_WITHSIBLINGS);
    assert_int_equal(ret, 0);

    st->dt2 = lyd_parse_mem(st->ctx, st->mem, LYD_LYB, LYD_OPT_CONFIG | LYD_OPT_STRICT);
    assert_ptr_not_equal(st->dt2, NULL);

    check_data_tree(st->dt1, st->dt2);
}

static void
test_leafrefs(void **state)
{
    struct state *st = (*state);
    int ret;

    ly_ctx_set_searchdir(st->ctx, TESTS_DIR "/data/files");
    assert_non_null(ly_ctx_load_module(st->ctx, "leafrefs2", NULL));

    st->dt1 = lyd_parse_path(st->ctx, TESTS_DIR "/data/files/leafrefs2.json", LYD_JSON, LYD_OPT_CONFIG | LYD_OPT_STRICT);
    assert_ptr_not_equal(st->dt1, NULL);

    ret = lyd_print_mem(&st->mem, st->dt1, LYD_LYB, LYP_WITHSIBLINGS);
    assert_int_equal(ret, 0);

    st->dt2 = lyd_parse_mem(st->ctx, st->mem, LYD_LYB, LYD_OPT_CONFIG | LYD_OPT_STRICT);
    assert_ptr_not_equal(st->dt2, NULL);

    check_data_tree(st->dt1, st->dt2);
}

#endif

int
main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_ietf_interfaces),
        cmocka_unit_test(test_origin),
        cmocka_unit_test(test_statements),
        /*cmocka_unit_test_setup_teardown(test_types, setup_f, teardown_f),
        cmocka_unit_test_setup_teardown(test_annotations, setup_f, teardown_f),
        cmocka_unit_test_setup_teardown(test_similar_annot_names, setup_f, teardown_f),
        cmocka_unit_test_setup_teardown(test_many_child_annot, setup_f, teardown_f),
        cmocka_unit_test_setup_teardown(test_union, setup_f, teardown_f),
        cmocka_unit_test_setup_teardown(test_union2, setup_f, teardown_f),
        cmocka_unit_test_setup_teardown(test_collisions, setup_f, teardown_f),
        cmocka_unit_test_setup_teardown(test_anydata, setup_f, teardown_f),
        cmocka_unit_test_setup_teardown(test_submodule_feature, setup_f, teardown_f),
        cmocka_unit_test_setup_teardown(test_coliding_augments, setup_f, teardown_f),
        cmocka_unit_test_setup_teardown(test_leafrefs, setup_f, teardown_f),*/
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
