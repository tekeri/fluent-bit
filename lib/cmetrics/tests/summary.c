/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  CMetrics
 *  ========
 *  Copyright 2021 Eduardo Silva <eduardo@calyptia.com>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <cmetrics/cmetrics.h>
#include <cmetrics/cmt_summary.h>
#include <cmetrics/cmt_encode_prometheus.h>

#include "cmt_tests.h"

static void prometheus_encode_test(struct cmt *cmt)
{
    cmt_sds_t buf;

    buf = cmt_encode_prometheus_create(cmt, CMT_FALSE);
    printf("\n%s\n", buf);
    cmt_encode_prometheus_destroy(buf);
}

void test_set_defaults()
{
    int ret;
    double val;
    double sum;
    uint64_t count;
    uint64_t ts;
    double q[6];
    double r[6];
    struct cmt *cmt;
    struct cmt_summary *s;

    cmt_initialize();

    /* Timestamp */
    ts = cmt_time_now();

    /* CMetrics context */
    cmt = cmt_create();
    TEST_CHECK(cmt != NULL);

    /* set quantiles, no labels */
    q[0] = 0.1;
    q[1] = 0.2;
    q[2] = 0.3;
    q[3] = 0.4;
    q[4] = 0.5;
    q[5] = 1.0;

    r[0] = 1;
    r[1] = 2;
    r[2] = 3;
    r[3] = 4;
    r[4] = 5;
    r[5] = 6;

    /* Create a gauge metric type */
    s = cmt_summary_create(cmt,
                           "k8s", "network", "load", "Network load",
                           6, q,
                           1, (char *[]) {"my_label"});
    TEST_CHECK(s != NULL);

    count = 10;
    sum   = 51.612894511314444;

    /* no quantiles, no labels */
    cmt_summary_set_default(s, ts, NULL, sum, count, 0, NULL);
    prometheus_encode_test(cmt);

    cmt_summary_set_default(s, ts, r, sum, count, 0, NULL);
    prometheus_encode_test(cmt);

    /* static label: register static label for the context */
    cmt_label_add(cmt, "static", "test");
    prometheus_encode_test(cmt);

    cmt_destroy(cmt);
}

TEST_LIST = {
    {"set_defaults", test_set_defaults},
    { 0 }
};
