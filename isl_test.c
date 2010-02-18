/*
 * Copyright 2008-2009 Katholieke Universiteit Leuven
 *
 * Use of this software is governed by the GNU LGPLv2.1 license
 *
 * Written by Sven Verdoolaege, K.U.Leuven, Departement
 * Computerwetenschappen, Celestijnenlaan 200A, B-3001 Leuven, Belgium
 */

#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include <isl_ctx.h>
#include <isl_set.h>
#include <isl_constraint.h>

static char *srcdir;

void test_read(struct isl_ctx *ctx)
{
	char filename[PATH_MAX];
	FILE *input;
	int n;
	struct isl_basic_set *bset1, *bset2;
	const char *str = "{[y]: Exists ( alpha : 2alpha = y)}";

	n = snprintf(filename, sizeof(filename),
			"%s/test_inputs/set.omega", srcdir);
	assert(n < sizeof(filename));
	input = fopen(filename, "r");
	assert(input);

	bset1 = isl_basic_set_read_from_file(ctx, input, 0);
	bset2 = isl_basic_set_read_from_str(ctx, str, 0);

	assert(isl_basic_set_is_equal(bset1, bset2) == 1);

	isl_basic_set_free(bset1);
	isl_basic_set_free(bset2);

	fclose(input);
}

/* Construct the basic set { [i] : 5 <= i <= N } */
void test_construction(struct isl_ctx *ctx)
{
	isl_int v;
	struct isl_dim *dim;
	struct isl_basic_set *bset;
	struct isl_constraint *c;

	isl_int_init(v);

	dim = isl_dim_set_alloc(ctx, 1, 1);
	bset = isl_basic_set_universe(dim);

	c = isl_inequality_alloc(isl_dim_copy(bset->dim));
	isl_int_set_si(v, -1);
	isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
	isl_int_set_si(v, 1);
	isl_constraint_set_coefficient(c, isl_dim_param, 0, v);
	bset = isl_basic_set_add_constraint(bset, c);

	c = isl_inequality_alloc(isl_dim_copy(bset->dim));
	isl_int_set_si(v, 1);
	isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
	isl_int_set_si(v, -5);
	isl_constraint_set_constant(c, v);
	bset = isl_basic_set_add_constraint(bset, c);

	isl_basic_set_free(bset);

	isl_int_clear(v);
}

void test_dim(struct isl_ctx *ctx)
{
	isl_map *map1, *map2;

	map1 = isl_map_read_from_str(ctx,
	    "[n] -> { [i] -> [j] : exists (a = [i/10] : i - 10a <= n ) }", -1);
	map1 = isl_map_add(map1, isl_dim_in, 1);
	map2 = isl_map_read_from_str(ctx,
	    "[n] -> { [i,k] -> [j] : exists (a = [i/10] : i - 10a <= n ) }", -1);
	assert(isl_map_is_equal(map1, map2));
	isl_map_free(map2);

	map1 = isl_map_project_out(map1, isl_dim_in, 0, 1);
	map2 = isl_map_read_from_str(ctx, "[n] -> { [i] -> [j] : n >= 0 }", -1);
	assert(isl_map_is_equal(map1, map2));

	isl_map_free(map1);
	isl_map_free(map2);
}

void test_div(struct isl_ctx *ctx)
{
	isl_int v;
	int pos;
	struct isl_dim *dim;
	struct isl_div *div;
	struct isl_basic_set *bset;
	struct isl_constraint *c;

	isl_int_init(v);

	/* test 1 */
	dim = isl_dim_set_alloc(ctx, 0, 1);
	bset = isl_basic_set_universe(dim);

	c = isl_equality_alloc(isl_dim_copy(bset->dim));
	isl_int_set_si(v, -1);
	isl_constraint_set_constant(c, v);
	isl_int_set_si(v, 1);
	isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
	div = isl_div_alloc(isl_dim_copy(bset->dim));
	c = isl_constraint_add_div(c, div, &pos);
	isl_int_set_si(v, 3);
	isl_constraint_set_coefficient(c, isl_dim_div, pos, v);
	bset = isl_basic_set_add_constraint(bset, c);

	c = isl_equality_alloc(isl_dim_copy(bset->dim));
	isl_int_set_si(v, 1);
	isl_constraint_set_constant(c, v);
	isl_int_set_si(v, -1);
	isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
	div = isl_div_alloc(isl_dim_copy(bset->dim));
	c = isl_constraint_add_div(c, div, &pos);
	isl_int_set_si(v, 3);
	isl_constraint_set_coefficient(c, isl_dim_div, pos, v);
	bset = isl_basic_set_add_constraint(bset, c);

	assert(bset->n_div == 1);
	isl_basic_set_free(bset);

	/* test 2 */
	dim = isl_dim_set_alloc(ctx, 0, 1);
	bset = isl_basic_set_universe(dim);

	c = isl_equality_alloc(isl_dim_copy(bset->dim));
	isl_int_set_si(v, 1);
	isl_constraint_set_constant(c, v);
	isl_int_set_si(v, -1);
	isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
	div = isl_div_alloc(isl_dim_copy(bset->dim));
	c = isl_constraint_add_div(c, div, &pos);
	isl_int_set_si(v, 3);
	isl_constraint_set_coefficient(c, isl_dim_div, pos, v);
	bset = isl_basic_set_add_constraint(bset, c);

	c = isl_equality_alloc(isl_dim_copy(bset->dim));
	isl_int_set_si(v, -1);
	isl_constraint_set_constant(c, v);
	isl_int_set_si(v, 1);
	isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
	div = isl_div_alloc(isl_dim_copy(bset->dim));
	c = isl_constraint_add_div(c, div, &pos);
	isl_int_set_si(v, 3);
	isl_constraint_set_coefficient(c, isl_dim_div, pos, v);
	bset = isl_basic_set_add_constraint(bset, c);

	assert(bset->n_div == 1);
	isl_basic_set_free(bset);

	/* test 3 */
	dim = isl_dim_set_alloc(ctx, 0, 1);
	bset = isl_basic_set_universe(dim);

	c = isl_equality_alloc(isl_dim_copy(bset->dim));
	isl_int_set_si(v, 1);
	isl_constraint_set_constant(c, v);
	isl_int_set_si(v, -1);
	isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
	div = isl_div_alloc(isl_dim_copy(bset->dim));
	c = isl_constraint_add_div(c, div, &pos);
	isl_int_set_si(v, 3);
	isl_constraint_set_coefficient(c, isl_dim_div, pos, v);
	bset = isl_basic_set_add_constraint(bset, c);

	c = isl_equality_alloc(isl_dim_copy(bset->dim));
	isl_int_set_si(v, -3);
	isl_constraint_set_constant(c, v);
	isl_int_set_si(v, 1);
	isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
	div = isl_div_alloc(isl_dim_copy(bset->dim));
	c = isl_constraint_add_div(c, div, &pos);
	isl_int_set_si(v, 4);
	isl_constraint_set_coefficient(c, isl_dim_div, pos, v);
	bset = isl_basic_set_add_constraint(bset, c);

	assert(bset->n_div == 1);
	isl_basic_set_free(bset);

	/* test 4 */
	dim = isl_dim_set_alloc(ctx, 0, 1);
	bset = isl_basic_set_universe(dim);

	c = isl_equality_alloc(isl_dim_copy(bset->dim));
	isl_int_set_si(v, 2);
	isl_constraint_set_constant(c, v);
	isl_int_set_si(v, -1);
	isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
	div = isl_div_alloc(isl_dim_copy(bset->dim));
	c = isl_constraint_add_div(c, div, &pos);
	isl_int_set_si(v, 3);
	isl_constraint_set_coefficient(c, isl_dim_div, pos, v);
	bset = isl_basic_set_add_constraint(bset, c);

	c = isl_equality_alloc(isl_dim_copy(bset->dim));
	isl_int_set_si(v, -1);
	isl_constraint_set_constant(c, v);
	isl_int_set_si(v, 1);
	isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
	div = isl_div_alloc(isl_dim_copy(bset->dim));
	c = isl_constraint_add_div(c, div, &pos);
	isl_int_set_si(v, 6);
	isl_constraint_set_coefficient(c, isl_dim_div, pos, v);
	bset = isl_basic_set_add_constraint(bset, c);

	assert(isl_basic_set_is_empty(bset));
	isl_basic_set_free(bset);

	/* test 5 */
	dim = isl_dim_set_alloc(ctx, 0, 2);
	bset = isl_basic_set_universe(dim);

	c = isl_equality_alloc(isl_dim_copy(bset->dim));
	isl_int_set_si(v, -1);
	isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
	div = isl_div_alloc(isl_dim_copy(bset->dim));
	c = isl_constraint_add_div(c, div, &pos);
	isl_int_set_si(v, 3);
	isl_constraint_set_coefficient(c, isl_dim_div, pos, v);
	bset = isl_basic_set_add_constraint(bset, c);

	c = isl_equality_alloc(isl_dim_copy(bset->dim));
	isl_int_set_si(v, 1);
	isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
	isl_int_set_si(v, -3);
	isl_constraint_set_coefficient(c, isl_dim_set, 1, v);
	bset = isl_basic_set_add_constraint(bset, c);

	assert(bset->n_div == 0);
	isl_basic_set_free(bset);

	/* test 6 */
	dim = isl_dim_set_alloc(ctx, 0, 2);
	bset = isl_basic_set_universe(dim);

	c = isl_equality_alloc(isl_dim_copy(bset->dim));
	isl_int_set_si(v, -1);
	isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
	div = isl_div_alloc(isl_dim_copy(bset->dim));
	c = isl_constraint_add_div(c, div, &pos);
	isl_int_set_si(v, 6);
	isl_constraint_set_coefficient(c, isl_dim_div, pos, v);
	bset = isl_basic_set_add_constraint(bset, c);

	c = isl_equality_alloc(isl_dim_copy(bset->dim));
	isl_int_set_si(v, 1);
	isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
	isl_int_set_si(v, -3);
	isl_constraint_set_coefficient(c, isl_dim_set, 1, v);
	bset = isl_basic_set_add_constraint(bset, c);

	assert(bset->n_div == 1);
	isl_basic_set_free(bset);

	/* test 7 */
	/* This test is a bit tricky.  We set up an equality
	 *		a + 3b + 3c = 6 e0
	 * Normalization of divs creates _two_ divs
	 *		a = 3 e0
	 *		c - b - e0 = 2 e1
	 * Afterwards e0 is removed again because it has coefficient -1
	 * and we end up with the original equality and div again.
	 * Perhaps we can avoid the introduction of this temporary div.
	 */
	dim = isl_dim_set_alloc(ctx, 0, 3);
	bset = isl_basic_set_universe(dim);

	c = isl_equality_alloc(isl_dim_copy(bset->dim));
	isl_int_set_si(v, -1);
	isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
	isl_int_set_si(v, -3);
	isl_constraint_set_coefficient(c, isl_dim_set, 1, v);
	isl_int_set_si(v, -3);
	isl_constraint_set_coefficient(c, isl_dim_set, 2, v);
	div = isl_div_alloc(isl_dim_copy(bset->dim));
	c = isl_constraint_add_div(c, div, &pos);
	isl_int_set_si(v, 6);
	isl_constraint_set_coefficient(c, isl_dim_div, pos, v);
	bset = isl_basic_set_add_constraint(bset, c);

	assert(bset->n_div == 1);
	isl_basic_set_free(bset);

	/* test 8 */
	dim = isl_dim_set_alloc(ctx, 0, 4);
	bset = isl_basic_set_universe(dim);

	c = isl_equality_alloc(isl_dim_copy(bset->dim));
	isl_int_set_si(v, -1);
	isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
	isl_int_set_si(v, -3);
	isl_constraint_set_coefficient(c, isl_dim_set, 1, v);
	isl_int_set_si(v, -3);
	isl_constraint_set_coefficient(c, isl_dim_set, 3, v);
	div = isl_div_alloc(isl_dim_copy(bset->dim));
	c = isl_constraint_add_div(c, div, &pos);
	isl_int_set_si(v, 6);
	isl_constraint_set_coefficient(c, isl_dim_div, pos, v);
	bset = isl_basic_set_add_constraint(bset, c);

	c = isl_equality_alloc(isl_dim_copy(bset->dim));
	isl_int_set_si(v, -1);
	isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
	isl_int_set_si(v, 1);
	isl_constraint_set_coefficient(c, isl_dim_set, 2, v);
	isl_int_set_si(v, 1);
	isl_constraint_set_constant(c, v);
	bset = isl_basic_set_add_constraint(bset, c);

	assert(bset->n_div == 1);
	isl_basic_set_free(bset);

	/* test 9 */
	dim = isl_dim_set_alloc(ctx, 0, 2);
	bset = isl_basic_set_universe(dim);

	c = isl_equality_alloc(isl_dim_copy(bset->dim));
	isl_int_set_si(v, 1);
	isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
	isl_int_set_si(v, -1);
	isl_constraint_set_coefficient(c, isl_dim_set, 1, v);
	div = isl_div_alloc(isl_dim_copy(bset->dim));
	c = isl_constraint_add_div(c, div, &pos);
	isl_int_set_si(v, -2);
	isl_constraint_set_coefficient(c, isl_dim_div, pos, v);
	bset = isl_basic_set_add_constraint(bset, c);

	c = isl_equality_alloc(isl_dim_copy(bset->dim));
	isl_int_set_si(v, -1);
	isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
	div = isl_div_alloc(isl_dim_copy(bset->dim));
	c = isl_constraint_add_div(c, div, &pos);
	isl_int_set_si(v, 3);
	isl_constraint_set_coefficient(c, isl_dim_div, pos, v);
	isl_int_set_si(v, 2);
	isl_constraint_set_constant(c, v);
	bset = isl_basic_set_add_constraint(bset, c);

	bset = isl_basic_set_fix_si(bset, isl_dim_set, 0, 2);

	assert(!isl_basic_set_is_empty(bset));

	isl_basic_set_free(bset);

	/* test 10 */
	dim = isl_dim_set_alloc(ctx, 0, 2);
	bset = isl_basic_set_universe(dim);

	c = isl_equality_alloc(isl_dim_copy(bset->dim));
	isl_int_set_si(v, 1);
	isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
	div = isl_div_alloc(isl_dim_copy(bset->dim));
	c = isl_constraint_add_div(c, div, &pos);
	isl_int_set_si(v, -2);
	isl_constraint_set_coefficient(c, isl_dim_div, pos, v);
	bset = isl_basic_set_add_constraint(bset, c);

	bset = isl_basic_set_fix_si(bset, isl_dim_set, 0, 2);

	isl_basic_set_free(bset);

	isl_int_clear(v);
}

void test_application_case(struct isl_ctx *ctx, const char *name)
{
	char filename[PATH_MAX];
	FILE *input;
	int n;
	struct isl_basic_set *bset1, *bset2;
	struct isl_basic_map *bmap;

	n = snprintf(filename, sizeof(filename),
			"%s/test_inputs/%s.omega", srcdir, name);
	assert(n < sizeof(filename));
	input = fopen(filename, "r");
	assert(input);

	bset1 = isl_basic_set_read_from_file(ctx, input, 0);
	bmap = isl_basic_map_read_from_file(ctx, input, 0);

	bset1 = isl_basic_set_apply(bset1, bmap);

	bset2 = isl_basic_set_read_from_file(ctx, input, 0);

	assert(isl_basic_set_is_equal(bset1, bset2) == 1);

	isl_basic_set_free(bset1);
	isl_basic_set_free(bset2);

	fclose(input);
}

void test_application(struct isl_ctx *ctx)
{
	test_application_case(ctx, "application");
	test_application_case(ctx, "application2");
}

void test_affine_hull_case(struct isl_ctx *ctx, const char *name)
{
	char filename[PATH_MAX];
	FILE *input;
	int n;
	struct isl_basic_set *bset1, *bset2;

	n = snprintf(filename, sizeof(filename),
			"%s/test_inputs/%s.polylib", srcdir, name);
	assert(n < sizeof(filename));
	input = fopen(filename, "r");
	assert(input);

	bset1 = isl_basic_set_read_from_file(ctx, input, 0);
	bset2 = isl_basic_set_read_from_file(ctx, input, 0);

	bset1 = isl_basic_set_affine_hull(bset1);

	assert(isl_basic_set_is_equal(bset1, bset2) == 1);

	isl_basic_set_free(bset1);
	isl_basic_set_free(bset2);

	fclose(input);
}

void test_affine_hull(struct isl_ctx *ctx)
{
	test_affine_hull_case(ctx, "affine2");
	test_affine_hull_case(ctx, "affine");
	test_affine_hull_case(ctx, "affine3");
}

void test_convex_hull_case(struct isl_ctx *ctx, const char *name)
{
	char filename[PATH_MAX];
	FILE *input;
	int n;
	struct isl_basic_set *bset1, *bset2;
	struct isl_set *set;

	n = snprintf(filename, sizeof(filename),
			"%s/test_inputs/%s.polylib", srcdir, name);
	assert(n < sizeof(filename));
	input = fopen(filename, "r");
	assert(input);

	bset1 = isl_basic_set_read_from_file(ctx, input, 0);
	bset2 = isl_basic_set_read_from_file(ctx, input, 0);

	set = isl_basic_set_union(bset1, bset2);
	bset1 = isl_set_convex_hull(set);

	bset2 = isl_basic_set_read_from_file(ctx, input, 0);

	assert(isl_basic_set_is_equal(bset1, bset2) == 1);

	isl_basic_set_free(bset1);
	isl_basic_set_free(bset2);

	fclose(input);
}

void test_convex_hull(struct isl_ctx *ctx)
{
	test_convex_hull_case(ctx, "convex0");
	test_convex_hull_case(ctx, "convex1");
	test_convex_hull_case(ctx, "convex2");
	test_convex_hull_case(ctx, "convex3");
	test_convex_hull_case(ctx, "convex4");
	test_convex_hull_case(ctx, "convex5");
	test_convex_hull_case(ctx, "convex6");
	test_convex_hull_case(ctx, "convex7");
	test_convex_hull_case(ctx, "convex8");
	test_convex_hull_case(ctx, "convex9");
	test_convex_hull_case(ctx, "convex10");
	test_convex_hull_case(ctx, "convex11");
	test_convex_hull_case(ctx, "convex12");
	test_convex_hull_case(ctx, "convex13");
	test_convex_hull_case(ctx, "convex14");
}

void test_gist_case(struct isl_ctx *ctx, const char *name)
{
	char filename[PATH_MAX];
	FILE *input;
	int n;
	struct isl_basic_set *bset1, *bset2;

	n = snprintf(filename, sizeof(filename),
			"%s/test_inputs/%s.polylib", srcdir, name);
	assert(n < sizeof(filename));
	input = fopen(filename, "r");
	assert(input);

	bset1 = isl_basic_set_read_from_file(ctx, input, 0);
	bset2 = isl_basic_set_read_from_file(ctx, input, 0);

	bset1 = isl_basic_set_gist(bset1, bset2);

	bset2 = isl_basic_set_read_from_file(ctx, input, 0);

	assert(isl_basic_set_is_equal(bset1, bset2) == 1);

	isl_basic_set_free(bset1);
	isl_basic_set_free(bset2);

	fclose(input);
}

void test_gist(struct isl_ctx *ctx)
{
	test_gist_case(ctx, "gist1");
}

void test_coalesce(struct isl_ctx *ctx)
{
	struct isl_set *set, *set2;

	set = isl_set_read_from_str(ctx,
		"{[x,y]: x >= 0 & x <= 10 & y >= 0 & y <= 10 or "
		       "y >= x & x >= 2 & 5 >= y }", -1);
	set = isl_set_coalesce(set);
	assert(set && set->n == 1);
	isl_set_free(set);

	set = isl_set_read_from_str(ctx,
		"{[x,y]: y >= 0 & 2x + y <= 30 & y <= 10 & x >= 0 or "
		       "x + y >= 10 & y <= x & x + y <= 20 & y >= 0}", -1);
	set = isl_set_coalesce(set);
	assert(set && set->n == 1);
	isl_set_free(set);

	set = isl_set_read_from_str(ctx,
		"{[x,y]: y >= 0 & 2x + y <= 30 & y <= 10 & x >= 0 or "
		       "x + y >= 10 & y <= x & x + y <= 19 & y >= 0}", -1);
	set = isl_set_coalesce(set);
	assert(set && set->n == 2);
	isl_set_free(set);

	set = isl_set_read_from_str(ctx,
		"{[x,y]: y >= 0 & x <= 5 & y <= x or "
		       "y >= 0 & x >= 6 & x <= 10 & y <= x}", -1);
	set = isl_set_coalesce(set);
	assert(set && set->n == 1);
	isl_set_free(set);

	set = isl_set_read_from_str(ctx,
		"{[x,y]: y >= 0 & x <= 5 & y <= x or "
		       "y >= 0 & x >= 7 & x <= 10 & y <= x}", -1);
	set = isl_set_coalesce(set);
	assert(set && set->n == 2);
	isl_set_free(set);

	set = isl_set_read_from_str(ctx,
		"{[x,y]: y >= 0 & x <= 5 & y <= x or "
		       "y >= 0 & x >= 6 & x <= 10 & y + 1 <= x}", -1);
	set = isl_set_coalesce(set);
	assert(set && set->n == 2);
	isl_set_free(set);

	set = isl_set_read_from_str(ctx,
		"{[x,y]: y >= 0 & x <= 5 & y <= x or "
		       "y >= 0 & x = 6 & y <= 6}", -1);
	set = isl_set_coalesce(set);
	assert(set && set->n == 1);
	isl_set_free(set);

	set = isl_set_read_from_str(ctx,
		"{[x,y]: y >= 0 & x <= 5 & y <= x or "
		       "y >= 0 & x = 7 & y <= 6}", -1);
	set = isl_set_coalesce(set);
	assert(set && set->n == 2);
	isl_set_free(set);

	set = isl_set_read_from_str(ctx,
		"{[x,y]: y >= 0 & x <= 5 & y <= x or "
		       "y >= 0 & x = 6 & y <= 5}", -1);
	set = isl_set_coalesce(set);
	assert(set && set->n == 1);
	set2 = isl_set_read_from_str(ctx,
		"{[x,y]: y >= 0 & x <= 5 & y <= x or "
		       "y >= 0 & x = 6 & y <= 5}", -1);
	assert(isl_set_is_equal(set, set2));
	isl_set_free(set);
	isl_set_free(set2);

	set = isl_set_read_from_str(ctx,
		"{[x,y]: y >= 0 & x <= 5 & y <= x or "
		       "y >= 0 & x = 6 & y <= 7}", -1);
	set = isl_set_coalesce(set);
	assert(set && set->n == 2);
	isl_set_free(set);

	set = isl_set_read_from_str(ctx,
		"[n] -> { [i] : i = 1 and n >= 2 or 2 <= i and i <= n }", -1);
	set = isl_set_coalesce(set);
	assert(set && set->n == 1);
	set2 = isl_set_read_from_str(ctx,
		"[n] -> { [i] : i = 1 and n >= 2 or 2 <= i and i <= n }", -1);
	assert(isl_set_is_equal(set, set2));
	isl_set_free(set);
	isl_set_free(set2);

	set = isl_set_read_from_str(ctx,
		"{[x,y] : x >= 0 and y >= 0 or 0 <= y and y <= 5 and x = -1}", -1);
	set = isl_set_coalesce(set);
	set2 = isl_set_read_from_str(ctx,
		"{[x,y] : x >= 0 and y >= 0 or 0 <= y and y <= 5 and x = -1}", -1);
	assert(isl_set_is_equal(set, set2));
	isl_set_free(set);
	isl_set_free(set2);

	set = isl_set_read_from_str(ctx,
		"[n] -> { [i] : 1 <= i and i <= n - 1 or "
				"2 <= i and i <= n }", -1);
	set = isl_set_coalesce(set);
	assert(set && set->n == 1);
	set2 = isl_set_read_from_str(ctx,
		"[n] -> { [i] : 1 <= i and i <= n - 1 or "
				"2 <= i and i <= n }", -1);
	assert(isl_set_is_equal(set, set2));
	isl_set_free(set);
	isl_set_free(set2);
}

void test_closure(struct isl_ctx *ctx)
{
	isl_set *dom;
	isl_map *up, *right;
	isl_map *map, *map2;
	int exact;

	/* COCOA example 1 */
	map = isl_map_read_from_str(ctx,
		"[n,k] -> { [i,j] -> [i2,j2] : i2 = i + 1 and j2 = j + 1 and "
			"1 <= i and i < n and 1 <= j and j < n or "
			"i2 = i + 1 and j2 = j - 1 and "
			"1 <= i and i < n and 2 <= j and j <= n }", -1);
	map = isl_map_power(map, 1, &exact);
	assert(exact);
	isl_map_free(map);

	/* COCOA example 1 */
	map = isl_map_read_from_str(ctx,
		"[n] -> { [i,j] -> [i2,j2] : i2 = i + 1 and j2 = j + 1 and "
			"1 <= i and i < n and 1 <= j and j < n or "
			"i2 = i + 1 and j2 = j - 1 and "
			"1 <= i and i < n and 2 <= j and j <= n }", -1);
	map = isl_map_transitive_closure(map, &exact);
	assert(exact);
	map2 = isl_map_read_from_str(ctx,
		"[n] -> { [i,j] -> [i2,j2] : exists (k1,k2,k : "
			"1 <= i and i < n and 1 <= j and j <= n and "
			"2 <= i2 and i2 <= n and 1 <= j2 and j2 <= n and "
			"i2 = i + k1 + k2 and j2 = j + k1 - k2 and "
			"k1 >= 0 and k2 >= 0 and k1 + k2 = k and k >= 1 )}", -1);
	assert(isl_map_is_equal(map, map2));
	isl_map_free(map2);
	isl_map_free(map);

	map = isl_map_read_from_str(ctx,
		"[n] -> { [x] -> [y] : y = x + 1 and 0 <= x and x <= n and "
				     " 0 <= y and y <= n }", -1);
	map = isl_map_transitive_closure(map, &exact);
	map2 = isl_map_read_from_str(ctx,
		"[n] -> { [x] -> [y] : y > x and 0 <= x and x <= n and "
				     " 0 <= y and y <= n }", -1);
	assert(isl_map_is_equal(map, map2));
	isl_map_free(map2);
	isl_map_free(map);

	/* COCOA example 2 */
	map = isl_map_read_from_str(ctx,
		"[n] -> { [i,j] -> [i2,j2] : i2 = i + 2 and j2 = j + 2 and "
			"1 <= i and i < n - 1 and 1 <= j and j < n - 1 or "
			"i2 = i + 2 and j2 = j - 2 and "
			"1 <= i and i < n - 1 and 3 <= j and j <= n }", -1);
	map = isl_map_transitive_closure(map, &exact);
	assert(exact);
	map2 = isl_map_read_from_str(ctx,
		"[n] -> { [i,j] -> [i2,j2] : exists (k1,k2,k : "
			"1 <= i and i < n - 1 and 1 <= j and j <= n and "
			"3 <= i2 and i2 <= n and 1 <= j2 and j2 <= n and "
			"i2 = i + 2 k1 + 2 k2 and j2 = j + 2 k1 - 2 k2 and "
			"k1 >= 0 and k2 >= 0 and k1 + k2 = k and k >= 1) }", -1);
	assert(isl_map_is_equal(map, map2));
	isl_map_free(map);
	isl_map_free(map2);

	/* COCOA Fig.2 left */
	map = isl_map_read_from_str(ctx,
		"[n] -> { [i,j] -> [i2,j2] : i2 = i + 2 and j2 = j and "
			"i <= 2 j - 3 and i <= n - 2 and j <= 2 i - 1 and "
			"j <= n or "
			"i2 = i and j2 = j + 2 and i <= 2 j - 1 and i <= n and "
			"j <= 2 i - 3 and j <= n - 2 or "
			"i2 = i + 1 and j2 = j + 1 and i <= 2 j - 1 and "
			"i <= n - 1 and j <= 2 i - 1 and j <= n - 1 }", -1);
	map = isl_map_transitive_closure(map, &exact);
	assert(exact);
	isl_map_free(map);

	/* COCOA Fig.2 right */
	map = isl_map_read_from_str(ctx,
		"[n,k] -> { [i,j] -> [i2,j2] : i2 = i + 3 and j2 = j and "
			"i <= 2 j - 4 and i <= n - 3 and j <= 2 i - 1 and "
			"j <= n or "
			"i2 = i and j2 = j + 3 and i <= 2 j - 1 and i <= n and "
			"j <= 2 i - 4 and j <= n - 3 or "
			"i2 = i + 1 and j2 = j + 1 and i <= 2 j - 1 and "
			"i <= n - 1 and j <= 2 i - 1 and j <= n - 1 }", -1);
	map = isl_map_power(map, 1, &exact);
	assert(exact);
	isl_map_free(map);

	/* COCOA Fig.2 right */
	map = isl_map_read_from_str(ctx,
		"[n] -> { [i,j] -> [i2,j2] : i2 = i + 3 and j2 = j and "
			"i <= 2 j - 4 and i <= n - 3 and j <= 2 i - 1 and "
			"j <= n or "
			"i2 = i and j2 = j + 3 and i <= 2 j - 1 and i <= n and "
			"j <= 2 i - 4 and j <= n - 3 or "
			"i2 = i + 1 and j2 = j + 1 and i <= 2 j - 1 and "
			"i <= n - 1 and j <= 2 i - 1 and j <= n - 1 }", -1);
	map = isl_map_transitive_closure(map, &exact);
	assert(exact);
	map2 = isl_map_read_from_str(ctx,
		"[n] -> { [i,j] -> [i2,j2] : exists (k1,k2,k3,k : "
			"i <= 2 j - 1 and i <= n and j <= 2 i - 1 and "
			"j <= n and 3 + i + 2 j <= 3 n and "
			"3 + 2 i + j <= 3n and i2 <= 2 j2 -1 and i2 <= n and "
			"i2 <= 3 j2 - 4 and j2 <= 2 i2 -1 and j2 <= n and "
			"13 + 4 j2 <= 11 i2 and i2 = i + 3 k1 + k3 and "
			"j2 = j + 3 k2 + k3 and k1 >= 0 and k2 >= 0 and "
			"k3 >= 0 and k1 + k2 + k3 = k and k > 0) }", -1);
	assert(isl_map_is_equal(map, map2));
	isl_map_free(map2);
	isl_map_free(map);

	/* COCOA Fig.1 right */
	dom = isl_set_read_from_str(ctx,
		"{ [x,y] : x >= 0 and -2 x + 3 y >= 0 and x <= 3 and "
			"2 x - 3 y + 3 >= 0 }", -1);
	right = isl_map_read_from_str(ctx,
		"{ [x,y] -> [x2,y2] : x2 = x + 1 and y2 = y }", -1);
	up = isl_map_read_from_str(ctx,
		"{ [x,y] -> [x2,y2] : x2 = x and y2 = y + 1 }", -1);
	right = isl_map_intersect_domain(right, isl_set_copy(dom));
	right = isl_map_intersect_range(right, isl_set_copy(dom));
	up = isl_map_intersect_domain(up, isl_set_copy(dom));
	up = isl_map_intersect_range(up, dom);
	map = isl_map_union(up, right);
	map = isl_map_transitive_closure(map, &exact);
	assert(!exact);
	isl_map_free(map);

	/* COCOA Theorem 1 counter example */
	map = isl_map_read_from_str(ctx,
		"{ [i,j] -> [i2,j2] : i = 0 and 0 <= j and j <= 1 and "
			"i2 = 1 and j2 = j or "
			"i = 0 and j = 0 and i2 = 0 and j2 = 1 }", -1);
	map = isl_map_transitive_closure(map, &exact);
	assert(exact);
	isl_map_free(map);

	map = isl_map_read_from_str(ctx,
		"[m,n] -> { [i,j] -> [i2,j2] : i2 = i and j2 = j + 2 and "
			"1 <= i,i2 <= n and 1 <= j,j2 <= m or "
			"i2 = i + 1 and 3 <= j2 - j <= 4 and "
			"1 <= i,i2 <= n and 1 <= j,j2 <= m }", -1);
	map = isl_map_transitive_closure(map, &exact);
	assert(exact);
	isl_map_free(map);

	/* Kelly et al 1996, fig 12 */
	map = isl_map_read_from_str(ctx,
		"[n] -> { [i,j] -> [i2,j2] : i2 = i and j2 = j + 1 and "
			"1 <= i,j,j+1 <= n or "
			"j = n and j2 = 1 and i2 = i + 1 and "
			"1 <= i,i+1 <= n }", -1);
	map = isl_map_transitive_closure(map, &exact);
	assert(exact);
	map2 = isl_map_read_from_str(ctx,
		"[n] -> { [i,j] -> [i2,j2] : 1 <= j < j2 <= n and "
			"1 <= i <= n and i = i2 or "
			"1 <= i < i2 <= n and 1 <= j <= n and "
			"1 <= j2 <= n }", -1);
	assert(isl_map_is_equal(map, map2));
	isl_map_free(map2);
	isl_map_free(map);

	/* Omega's closure4 */
	map = isl_map_read_from_str(ctx,
		"[m,n] -> { [x,y] -> [x2,y2] : x2 = x and y2 = y + 1 and "
			"1 <= x,y <= 10 or "
			"x2 = x + 1 and y2 = y and "
			"1 <= x <= 20 && 5 <= y <= 15 }", -1);
	map = isl_map_transitive_closure(map, &exact);
	assert(exact);
	isl_map_free(map);

	map = isl_map_read_from_str(ctx,
		"[n] -> { [x] -> [y]: 1 <= n <= y - x <= 10 }", -1);
	map = isl_map_transitive_closure(map, &exact);
	assert(!exact);
	map2 = isl_map_read_from_str(ctx,
		"[n] -> { [x] -> [y] : 1 <= n <= 10 and y >= n + x }", -1);
	assert(isl_map_is_equal(map, map2));
	isl_map_free(map);
	isl_map_free(map2);
}

int main()
{
	struct isl_ctx *ctx;

	srcdir = getenv("srcdir");
	assert(srcdir);

	ctx = isl_ctx_alloc();
	test_read(ctx);
	test_construction(ctx);
	test_dim(ctx);
	test_div(ctx);
	test_application(ctx);
	test_affine_hull(ctx);
	test_convex_hull(ctx);
	test_gist(ctx);
	test_coalesce(ctx);
	test_closure(ctx);
	isl_ctx_free(ctx);
	return 0;
}
