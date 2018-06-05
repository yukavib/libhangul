#include <stdarg.h>
#include <stdlib.h>
#include <wchar.h>
#include <check.h>

#include "../hangul/hangul.h"

#define countof(x)  ((sizeof(x)) / (sizeof(x[0])))

static HangulInputContext* global_ic = NULL;

/* ic option을 바꾸면서 테스트하는걸 손쉽게 하기 위해서
 * static ic를 하나 두고 이것으로 테스트를 진행하게 코드를
 * 바꾼다. */
static HangulInputContext*
get_ic(const char* keyboard)
{
    if (global_ic == NULL) {
	global_ic = hangul_ic_new("2");
    }

    hangul_ic_select_keyboard(global_ic, keyboard);
    hangul_ic_reset(global_ic);

    return global_ic;
}

static void
set_ic_option(int option, bool value)
{
    HangulInputContext* ic = get_ic("2");
    hangul_ic_set_option(ic, option, value);
}

static bool
get_ic_option(int option)
{
    HangulInputContext* ic = get_ic("2");
    bool value = hangul_ic_get_option(ic, option);
    return value;
}

static bool
check_preedit_with_ic(HangulInputContext* ic, const char* input, const wchar_t* output)
{
    const char* p;
    const ucschar* preedit;
    int res;

    p = input;
    while (*p != '\0') {
	hangul_ic_process(ic, *p);
	p++;
    }

    preedit = hangul_ic_get_preedit_string(ic);

    res = wcscmp((const wchar_t*)preedit, output);

    return res == 0;
}

static bool
check_preedit(const char* keyboard, const char* input, const wchar_t* output)
{
    HangulInputContext* ic;
    int res;

    ic = get_ic(keyboard);

    res = check_preedit_with_ic(ic, input, output);;

    return res;
}

static bool
check_commit_with_ic(HangulInputContext* ic, const char* input, const wchar_t* output)
{
    const char* p;
    const ucschar* commit;
    int res;

    p = input;
    while (*p != '\0') {
	hangul_ic_process(ic, *p);
	p++;
    }

    commit = hangul_ic_get_commit_string(ic);

    res = wcscmp((const wchar_t*)commit, output);

    return res == 0;
}

static bool
check_commit(const char* keyboard, const char* input, const wchar_t* output)
{
    HangulInputContext* ic;
    int res;

    ic = get_ic(keyboard);

    res = check_commit_with_ic(ic, input, output);

    return res;
}

START_TEST(test_hangul_ic_process_2)
{
    /* ㄱㅏㅉ */
    fail_unless(check_commit("2", "rkW", L"가"));
    fail_unless(check_preedit("2", "rkW", L"ㅉ"));
    /* ㅂㅓㅆㅡ */
    fail_unless(check_commit("2", "qjTm", L"버"));
    fail_unless(check_preedit("2", "qjTm", L"쓰"));
    /* ㅁㅏㄹㄱㅗ */
    fail_unless(check_preedit("2", "akfr", L"맑"));
    fail_unless(check_commit("2", "akfrh", L"말"));
    fail_unless(check_preedit("2", "akfrh", L"고"));

    /* ㄱㅅㅏ*/
    fail_unless(check_preedit("2", "rt", L"ㄳ"));
    fail_unless(check_commit("2", "rtk", L"ㄱ"));
    fail_unless(check_preedit("2", "rtk", L"사"));

    /* backspace */
    fail_unless(check_preedit("2", "rkT\b", L"가"));
    fail_unless(check_preedit("2", "rt\bk", L"가"));
    fail_unless(check_preedit("2", "akfr\b", L"말"));
    fail_unless(check_preedit("2", "dnp\b", L"우"));
    fail_unless(check_preedit("2", "qqnpfr\b\b\b\b\b\b", L""));
    fail_unless(check_preedit("2", "Qnpfr\b\b\b\b\b", L""));
}
END_TEST

START_TEST(test_hangul_ic_process_2y)
{
    /* ㅎ     */
    fail_unless(check_preedit("2y", "g", L"ㅎ"));
    /*   ㅗ   */
    fail_unless(check_preedit("2y", "h", L"ㅗ"));
    /*     ㅌ */
    fail_unless(check_preedit("2y", "x", L"ㅌ"));
    /* ㅂㅇ   */
    fail_unless(check_preedit("2y", "qd", L"\x3178"));
    /* ᄼ     */
    fail_unless(check_preedit("2y", "Z", L"\x113c\x1160"));
    /* ᅐ     */
    fail_unless(check_preedit("2y", "V", L"\x1150\x1160"));
    /* ᅝ     */
    fail_unless(check_preedit("2y", "sg", L"\x115d\x1160"));

    /* ㄱㅏㅇ */
    fail_unless(check_preedit("2y", "rkd", L"강"));
    /* ㄹㅐ   */
    fail_unless(check_preedit("2y", "fo", L"래"));
    /* ㅎ. ㄴ */
    fail_unless(check_preedit("2y", "gKs", L"\x1112\x119e\x11ab"));
    /* ㅃㅇㅏㅁㅆ */
    fail_unless(check_preedit("2y", "QdhaT", L"\x112c\x1169\x11de"));
    /* ㅃㅇㅏㅁㅅㅅㅛ */
    fail_unless(check_commit("2y", "Qdhatty", L"\x112c\x1169\x11dd"));
    fail_unless(check_preedit("2y", "Qdhatty", L"쇼"));
    /* ㅃㅇㅏㅁㅆㅛ */
    fail_unless(check_commit("2y", "QdhaTy", L"\x112c\x1169\x11b7"));
    fail_unless(check_preedit("2y", "QdhaTy", L"쑈"));
    /* 옛이응 처리 */
    /* ㄱㅏㆁㆁ */
    fail_unless(check_preedit("2y", "rkDD", L"\x1100\x1161\x11ee"));
    /* ㄱㅏㆁㆁㅏ */
    fail_unless(check_commit("2y", "rkDDk", L"\x1100\x1161\x11f0"));
    fail_unless(check_preedit("2y", "rkDDk", L"\x114c\x1161"));


    bool val = get_ic_option(HANGUL_IC_OPTION_COMBI_ON_DOUBLE_STROKE);
    set_ic_option(HANGUL_IC_OPTION_COMBI_ON_DOUBLE_STROKE, true);

    /* ㅂㅂㅇㅏㅁㅅㅅ */
    fail_unless(check_preedit("2y", "qqdhatt", L"\x112c\x1169\x11de"));
    /* ㅂㅂㅇㅏㅁㅅㅅㅛ */
    fail_unless(check_commit("2y", "qqdhatty", L"\x112c\x1169\x11dd"));
    fail_unless(check_preedit("2y", "qqdhatty", L"쇼"));
    /* ㅂㅂㅇㅏㅁㅆㅛ */
    fail_unless(check_commit("2y", "qqdhaTy", L"\x112c\x1169\x11b7"));
    fail_unless(check_preedit("2y", "qqdhaTy", L"쑈"));

    /* ㅇㅇㅏㅇㅇㅏ */
    fail_unless(check_commit("2y", "ddkdd", L"\x1147\x1161\x11bc"));
    fail_unless(check_preedit("2y", "ddkdd", L"ㅇ"));

    /* ㅏㅏㅏㅏ */
    fail_unless(check_preedit("2y", "kkkk", L"\x115f\x11a2"));

    set_ic_option(HANGUL_IC_OPTION_COMBI_ON_DOUBLE_STROKE, val);
}
END_TEST

START_TEST(test_hangul_ic_process_3f)
{
    /* L V T  */
    /* ㅎ     */
    fail_unless(check_preedit("3f", "m", L"ㅎ"));
    /*   ㅗ   */
    fail_unless(check_preedit("3f", "v", L"ㅗ"));
    /*     ㅌ */
    fail_unless(check_preedit("3f", "W", L"ㅌ"));

    /* ㄱㅏㅇ */
    fail_unless(check_preedit("3f", "kfa", L"강"));
    /* ㄹㅐ   */
    fail_unless(check_preedit("3f", "yr", L"래"));
    /* ㄴ  ㅁ */
    fail_unless(check_preedit("3f", "hz", L"\x1102\x1160\x11b7"));
    /*   ㅜㅅ */ 
    fail_unless(check_preedit("3f", "tq", L"\x115f\x1165\x11ba"));
}
END_TEST

START_TEST(test_hangul_ic_process_3s)
{
    /* ㅎㅐㅅㅅ */
    fail_unless(check_preedit("3s", "mrqq", L"했"));
}
END_TEST

START_TEST(test_hangul_ic_process_romaja)
{
    HangulInputContext* ic;
    const ucschar* preedit;
    const ucschar* commit;

    // romaja keyboard test
    ic = hangul_ic_new("ro");

    // basic test: han produces 한
    hangul_ic_process(ic, 'h');
    hangul_ic_process(ic, 'a');
    hangul_ic_process(ic, 'n');

    preedit = hangul_ic_get_preedit_string(ic);
    commit = hangul_ic_get_commit_string(ic);
    fail_unless(preedit[0] == L'한');
    fail_unless(commit[0] == 0);

    hangul_ic_reset(ic);

    // insert ㅇ when a syllable is not started with consonant
    hangul_ic_process(ic, 'a');

    preedit = hangul_ic_get_preedit_string(ic);
    commit = hangul_ic_get_commit_string(ic);
    fail_unless(preedit[0] == L'아');
    fail_unless(commit[0] == 0);

    // remove correctly when automatically ㅇ was inserted
    hangul_ic_backspace(ic);

    preedit = hangul_ic_get_preedit_string(ic);
    commit = hangul_ic_get_commit_string(ic);
    fail_unless(preedit[0] == 0);
    fail_unless(commit[0] == 0);

    // append ㅡ when a syllable is not ended with vowel
    hangul_ic_process(ic, 't');
    hangul_ic_process(ic, 't');

    preedit = hangul_ic_get_preedit_string(ic);
    commit = hangul_ic_get_commit_string(ic);
    fail_unless(preedit[0] == 0x314c); // ㅌ
    fail_unless(commit[0] == L'트');

    // ng makes trailing ㅇ
    hangul_ic_reset(ic);
    hangul_ic_process(ic, 'g');
    hangul_ic_process(ic, 'a');
    hangul_ic_process(ic, 'n');
    hangul_ic_process(ic, 'g');

    preedit = hangul_ic_get_preedit_string(ic);
    commit = hangul_ic_get_commit_string(ic);
    fail_unless(preedit[0] == L'강'); // 강
    fail_unless(commit[0] == 0);

    // gangi makes 강이
    hangul_ic_process(ic, 'i');

    preedit = hangul_ic_get_preedit_string(ic);
    commit = hangul_ic_get_commit_string(ic);
    fail_unless(preedit[0] == L'이');
    fail_unless(commit[0] == L'강');  // 강

    // nanG makes 난ㄱ
    // uppercase makes new syllable
    hangul_ic_process(ic, 'n');
    hangul_ic_process(ic, 'a');
    hangul_ic_process(ic, 'n');
    hangul_ic_process(ic, 'G');

    preedit = hangul_ic_get_preedit_string(ic);
    commit = hangul_ic_get_commit_string(ic);
    fail_unless(preedit[0] == 0x3131); // ㄱ
    fail_unless(commit[0] == L'난');  // 난

    // special operation for x
    // x generate ㅈ for leading consonant
    hangul_ic_reset(ic);
    hangul_ic_process(ic, 'x');
    hangul_ic_process(ic, 'x');

    preedit = hangul_ic_get_preedit_string(ic);
    commit = hangul_ic_get_commit_string(ic);
    fail_unless(preedit[0] == 0x3148); // 지
    fail_unless(commit[0] == L'즈');

    hangul_ic_reset(ic);
    hangul_ic_process(ic, 'x');
    hangul_ic_process(ic, 'y');

    preedit = hangul_ic_get_preedit_string(ic);
    commit = hangul_ic_get_commit_string(ic);
    fail_unless(preedit[0] == L'지'); // 지
    fail_unless(commit[0] == 0x0);

    // x generate ㄱㅅ for trailing consonant
    // and ㅅ will be transferred to next syllable when next input
    // character is vowel.
    hangul_ic_reset(ic);
    hangul_ic_process(ic, 's');
    hangul_ic_process(ic, 'e');
    hangul_ic_process(ic, 'x');
    hangul_ic_process(ic, 'y');

    preedit = hangul_ic_get_preedit_string(ic);
    commit = hangul_ic_get_commit_string(ic);
    fail_unless(preedit[0] == L'시'); // 시
    fail_unless(commit[0] == L'섹');  // 섹
    
    hangul_ic_delete(ic);
}
END_TEST

START_TEST(test_hangul_ic_auto_reorder)
{
    bool val = get_ic_option(HANGUL_IC_OPTION_AUTO_REORDER);

    set_ic_option(HANGUL_IC_OPTION_AUTO_REORDER, true);
    fail_unless(check_preedit("2", "rk", L"가"));
    fail_unless(check_preedit("2", "kr", L"가"));

    set_ic_option(HANGUL_IC_OPTION_AUTO_REORDER, false);
    fail_unless(check_preedit("2", "rk", L"가"));
    fail_unless(check_commit("2", "kr", L"ㅏ"));
    fail_unless(check_preedit("2", "kr", L"ㄱ"));

    set_ic_option(HANGUL_IC_OPTION_AUTO_REORDER, true);
    fail_unless(check_preedit("3f", "kf", L"가"));
    fail_unless(check_preedit("3f", "fk", L"가"));

    set_ic_option(HANGUL_IC_OPTION_AUTO_REORDER, false);
    fail_unless(check_preedit("3f", "kf", L"가"));
    fail_unless(check_commit("3f", "fk", L"ㅏ"));
    fail_unless(check_preedit("3f", "fk", L"ㄱ"));

    set_ic_option(HANGUL_IC_OPTION_AUTO_REORDER, val);
}
END_TEST

START_TEST(test_hangul_ic_combi_on_double_stroke)
{
    bool val = get_ic_option(HANGUL_IC_OPTION_COMBI_ON_DOUBLE_STROKE);

    set_ic_option(HANGUL_IC_OPTION_COMBI_ON_DOUBLE_STROKE, true);
    fail_unless(check_preedit("2", "rrkrr", L"깎"));
    fail_unless(check_preedit("2", "rrkrrk", L"가"));

    /* ㅂㅓㅅㅅ */
    fail_unless(check_preedit("2", "qjtt", L"벘"));
    fail_unless(check_commit("2", "qjttm", L"벗"));
    fail_unless(check_preedit("2", "qjttm", L"스"));

    fail_unless(check_preedit("2", "rktt", L"갔"));
    fail_unless(check_preedit("2", "rktt\b", L"갓"));

    set_ic_option(HANGUL_IC_OPTION_COMBI_ON_DOUBLE_STROKE, false);
    fail_unless(check_commit("2", "rr", L"ㄱ"));
    fail_unless(check_preedit("2", "rr", L"ㄱ"));
    fail_unless(check_preedit("2", "rrk", L"가"));
    fail_unless(check_preedit("2", "rrkr", L"각"));
    fail_unless(check_commit("2", "rrkrr", L"각"));
    fail_unless(check_preedit("2", "rrkrr", L"ㄱ"));
    fail_unless(check_preedit("2", "rrkrrk", L"가"));

    /* ㅂㅓㅅㅅ */
    fail_unless(check_commit("2", "qjtt", L"벗"));
    fail_unless(check_preedit("2", "qjtt", L"ㅅ"));

    set_ic_option(HANGUL_IC_OPTION_COMBI_ON_DOUBLE_STROKE, val);
}
END_TEST

START_TEST(test_hangul_ic_non_choseong_combi)
{
    bool val = get_ic_option(HANGUL_IC_OPTION_NON_CHOSEONG_COMBI);
    set_ic_option(HANGUL_IC_OPTION_NON_CHOSEONG_COMBI, true);
    fail_unless(check_preedit("2", "rt", L"ㄳ"));
    fail_unless(check_commit("2", "rtk", L"ㄱ"));
    fail_unless(check_preedit("2", "rtk", L"사"));

    set_ic_option(HANGUL_IC_OPTION_NON_CHOSEONG_COMBI, false);
    fail_unless(check_commit("2", "rt", L"ㄱ"));
    fail_unless(check_preedit("2", "rt", L"ㅅ"));

    set_ic_option(HANGUL_IC_OPTION_NON_CHOSEONG_COMBI, val);
END_TEST
}

START_TEST(test_syllable_iterator)
{
    ucschar str[] = {
	// L L V V T T
	0x1107, 0x1107, 0x116e, 0x1166, 0x11af, 0x11a8,          // 0
	// L V T
	0x1108, 0x1170, 0x11b0,                                  // 6
	// L L V V T T M
	0x1107, 0x1107, 0x116e, 0x1166, 0x11af, 0x11a8, 0x302E,  // 9
	// L V T M
	0x1108, 0x1170, 0x11b0, 0x302F,                          // 16
	// Lf V
	0x115f, 0x1161,                                          // 20
	// L Vf
	0x110c, 0x1160,                                          // 22
	// L LVT T
	0x1107, 0xbc14, 0x11a8,                                  // 24
	// L LV T
	0x1100, 0xac00, 0x11a8,                                  // 27
	// LVT
	0xc00d,                                                  // 30 
	// other
	'a',                                                     // 31
	0                                                        // 32
    };

    const ucschar* begin = str;
    const ucschar* end = str + countof(str) - 1;
    const ucschar* s = str;

    s = hangul_syllable_iterator_next(s, end);
    fail_unless(s - str == 6,
		"error: next syllable: L L V V T T");

    s = hangul_syllable_iterator_next(s, end);
    fail_unless(s - str == 9,
		"error: next syllable: L V T");

    s = hangul_syllable_iterator_next(s, end);
    fail_unless(s - str == 16,
		"error: next syllable: L L V V T T M");

    s = hangul_syllable_iterator_next(s, end);
    fail_unless(s - str == 20,
		"error: next syllable: L V T M");

    s = hangul_syllable_iterator_next(s, end);
    fail_unless(s - str == 22,
		"error: next syllable: Lf V");

    s = hangul_syllable_iterator_next(s, end);
    fail_unless(s - str == 24,
		"error: next syllable: L Vf");

    s = hangul_syllable_iterator_next(s, end);
    fail_unless(s - str == 27,
		"error: next syllable: L LVT T");

    s = hangul_syllable_iterator_next(s, end);
    fail_unless(s - str == 30,
		"error: next syllable: L LV T");

    s = hangul_syllable_iterator_next(s, end);
    fail_unless(s - str == 31,
		"error: next syllable: LVT");

    s = hangul_syllable_iterator_next(s, end);
    fail_unless(s - str == 32,
		"error: next syllable: other");

    s = end;
    s = hangul_syllable_iterator_prev(s, begin);
    fail_unless(s - str == 31,
		"error: prev syllable: other");

    s = hangul_syllable_iterator_prev(s, begin);
    fail_unless(s - str == 30,
		"error: prev syllable: LVT");

    s = hangul_syllable_iterator_prev(s, begin);
    fail_unless(s - str == 27,
		"error: prev syllable: L LV T");

    s = hangul_syllable_iterator_prev(s, begin);
    fail_unless(s - str == 24,
		"error: prev syllable: L LVT T");

    s = hangul_syllable_iterator_prev(s, begin);
    fail_unless(s - str == 22,
		"error: prev syllable: L Vf");

    s = hangul_syllable_iterator_prev(s, begin);
    fail_unless(s - str == 20,
		"error: prev syllable: Lf V");

    s = hangul_syllable_iterator_prev(s, begin);
    fail_unless(s - str == 16,
		"error: prev syllable: L V T M");

    s = hangul_syllable_iterator_prev(s, begin);
    fail_unless(s - str == 9,
		"error: prev syllable: L L V V T T M");

    s = hangul_syllable_iterator_prev(s, begin);
    fail_unless(s - str == 6,
		"error: prev syllable: L V T");

    s = hangul_syllable_iterator_prev(s, begin);
    fail_unless(s - str == 0,
		"error: prev syllable: L L V V T T");
}
END_TEST

START_TEST(test_hangul_keyboard)
{
    const char* id;
    const char* name;
    unsigned int n;
    unsigned int i;

    n = hangul_keyboard_list_get_count();
    fail_unless(n != 0,
		"error: there is no hangul keyboard");

    for (i = 0; i < n; ++i) {
	id = hangul_keyboard_list_get_keyboard_id(i);
	fail_unless(id != NULL,
		    "error: keyboard id == NULL");
    }

    for (i = 0; i < n; ++i) {
	name = hangul_keyboard_list_get_keyboard_name(i);
	fail_unless(name != NULL,
		    "error: keyboard id == NULL");
    }

    HangulKeyboard* keyboard;
    fail_unless(
        (keyboard = hangul_keyboard_new_from_file(TEST_SOURCE_DIR "/recursive.xml")) != NULL
    );
    hangul_keyboard_delete(keyboard);
}
END_TEST

START_TEST(test_hangul_jamo_to_cjamo)
{
    fail_unless(
	0x3183 == hangul_jamo_to_cjamo(0x11f2)
    );
    fail_unless(
	0x316f == hangul_jamo_to_cjamo(0xa971)
    );
    fail_unless(
	0x3149 == hangul_jamo_to_cjamo(0xd7f9)
    );
}
END_TEST

Suite* libhangul_suite()
{
    Suite* s = suite_create("libhangul");

    TCase* hangul = tcase_create("hangul");
    tcase_add_test(hangul, test_hangul_ic_process_2);
    tcase_add_test(hangul, test_hangul_ic_process_2y);
    tcase_add_test(hangul, test_hangul_ic_process_3f);
    tcase_add_test(hangul, test_hangul_ic_process_3s);
    tcase_add_test(hangul, test_hangul_ic_process_romaja);
    tcase_add_test(hangul, test_hangul_ic_auto_reorder);
    tcase_add_test(hangul, test_hangul_ic_combi_on_double_stroke);
    tcase_add_test(hangul, test_hangul_ic_non_choseong_combi);
    tcase_add_test(hangul, test_syllable_iterator);
    tcase_add_test(hangul, test_hangul_keyboard);
    tcase_add_test(hangul, test_hangul_jamo_to_cjamo);
    suite_add_tcase(s, hangul);

    return s;
}

int main()
{
    hangul_init();

    int number_failed;
    Suite* s = libhangul_suite();
    SRunner* sr = srunner_create(s);

    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);

    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    hangul_ic_delete(global_ic);

    hangul_fini();

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
