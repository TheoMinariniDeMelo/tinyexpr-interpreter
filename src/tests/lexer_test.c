#include "../lexer.h"
#include "../../includes/munit/munit.h"

static MunitResult
test_init_empty_lexer(const MunitParameter params[], void* data){
    Lexer* lx;
    const char* str = "";
    lexer_init(lx, str);

    munit_assert_not_null(lx);
    munit_assert_null(lx->src);
    munit_assert(lx->pos == 0);
    munit_assert(lx->line == 0);
    munit_assert(lx->col == 0);
    return MUNIT_OK;
}


static MunitTest test_suite_tests[] = {
    {(char*) "/lexer/init_empty_lexer", test_init_empty_lexer, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

static const MunitSuite test_suite = {
    (char*) "", test_suite_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)])
{
    return munit_suite_main(&test_suite, (void*) "tinyexpr", argc, argv);
}
