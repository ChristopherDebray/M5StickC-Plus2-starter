#include <unity.h>
#include <M5Unified.h>
#include "../../lib/display_handler.h"
#include "../../lib/page_manager.h"
#include "../../lib/pages/page_base.h"

// ---------------------------------------------------------------------------
// Minimal stub page — tracks every lifecycle call for assertions
// ---------------------------------------------------------------------------
class StubPage : public PageBase {
public:
    int setupCalled   = 0;
    int loopCalled    = 0;
    int cleanupCalled = 0;
    const char* label;

    StubPage(DisplayHandler* disp, const char* name)
        : PageBase(disp, name), label(name) {}

    void setup()   override { setupCalled++;   }
    void loop()    override { loopCalled++;    }
    void cleanup() override { cleanupCalled++; }
    const char* getName() override { return label; }
};

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
DisplayHandler displayHandler;

// Each test constructs its own PageManager + StubPages on the stack so there
// is no shared state between tests.

void setUp(void)    {}
void tearDown(void) {}

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

// begin() with zero pages must not crash and leave index at -1
void test_begin_no_pages() {
    PageManager pm;
    pm.begin();
    TEST_ASSERT_EQUAL(-1, pm.getCurrentPageIndex());
    TEST_ASSERT_NULL(pm.getCurrentPage());
}

// addPage returns true until pool is full, then false
void test_add_page_overflow() {
    PageManager pm;
    StubPage p1(&displayHandler, "P1"), p2(&displayHandler, "P2"),
             p3(&displayHandler, "P3"), p4(&displayHandler, "P4"),
             p5(&displayHandler, "P5");

    TEST_ASSERT_TRUE(pm.addPage(&p1));
    TEST_ASSERT_TRUE(pm.addPage(&p2));
    TEST_ASSERT_TRUE(pm.addPage(&p3));
    TEST_ASSERT_TRUE(pm.addPage(&p4));
    TEST_ASSERT_FALSE(pm.addPage(&p5)); // MAX_PAGES == 4, must reject
    TEST_ASSERT_EQUAL(4, pm.getPageCount());
}

// begin() calls setup() on page 0 and marks it initialized
void test_begin_calls_setup_on_first_page() {
    PageManager pm;
    StubPage p1(&displayHandler, "P1"), p2(&displayHandler, "P2");
    pm.addPage(&p1);
    pm.addPage(&p2);
    pm.begin();

    TEST_ASSERT_EQUAL(1, p1.setupCalled);
    TEST_ASSERT_EQUAL(0, p2.setupCalled);
    TEST_ASSERT_TRUE(p1.isInitialized());
    TEST_ASSERT_EQUAL(0, pm.getCurrentPageIndex());
}

// goToPage out-of-range is a no-op
void test_goto_page_out_of_bounds() {
    PageManager pm;
    StubPage p1(&displayHandler, "P1");
    pm.addPage(&p1);
    pm.begin();

    int setupBefore   = p1.setupCalled;
    int cleanupBefore = p1.cleanupCalled;

    pm.goToPage(-1);
    pm.goToPage(5);

    TEST_ASSERT_EQUAL(setupBefore,   p1.setupCalled);   // no extra setup
    TEST_ASSERT_EQUAL(cleanupBefore, p1.cleanupCalled); // no spurious cleanup
    TEST_ASSERT_EQUAL(0, pm.getCurrentPageIndex());     // still on page 0
}

// goToPage with the current index is a no-op (no redundant setup/cleanup)
void test_goto_same_page_is_noop() {
    PageManager pm;
    StubPage p1(&displayHandler, "P1");
    pm.addPage(&p1);
    pm.begin();

    int setupBefore   = p1.setupCalled;
    int cleanupBefore = p1.cleanupCalled;

    pm.goToPage(0); // already on 0

    TEST_ASSERT_EQUAL(setupBefore,   p1.setupCalled);
    TEST_ASSERT_EQUAL(cleanupBefore, p1.cleanupCalled);
}

// Transition: cleanup old page, setup new page, update initialized flags
void test_page_transition_lifecycle() {
    PageManager pm;
    StubPage p1(&displayHandler, "P1"), p2(&displayHandler, "P2");
    pm.addPage(&p1);
    pm.addPage(&p2);
    pm.begin(); // p1 active

    pm.goToPage(1);

    TEST_ASSERT_EQUAL(1, p1.cleanupCalled);
    TEST_ASSERT_FALSE(p1.isInitialized());

    TEST_ASSERT_EQUAL(1, p2.setupCalled);
    TEST_ASSERT_TRUE(p2.isInitialized());
    TEST_ASSERT_EQUAL(1, pm.getCurrentPageIndex());
}

// nextPage() advances and wraps around to 0
void test_next_page_wraps() {
    PageManager pm;
    StubPage p1(&displayHandler, "P1"), p2(&displayHandler, "P2");
    pm.addPage(&p1);
    pm.addPage(&p2);
    pm.begin(); // index 0

    pm.nextPage();
    TEST_ASSERT_EQUAL(1, pm.getCurrentPageIndex());

    pm.nextPage(); // wrap
    TEST_ASSERT_EQUAL(0, pm.getCurrentPageIndex());
}

// previousPage() goes back and wraps from 0 to last
void test_previous_page_wraps() {
    PageManager pm;
    StubPage p1(&displayHandler, "P1"), p2(&displayHandler, "P2");
    pm.addPage(&p1);
    pm.addPage(&p2);
    pm.begin(); // index 0

    pm.previousPage(); // wrap to 1
    TEST_ASSERT_EQUAL(1, pm.getCurrentPageIndex());

    pm.previousPage(); // back to 0
    TEST_ASSERT_EQUAL(0, pm.getCurrentPageIndex());
}

// update() drives loop() on the current page only
void test_update_calls_loop() {
    PageManager pm;
    StubPage p1(&displayHandler, "P1"), p2(&displayHandler, "P2");
    pm.addPage(&p1);
    pm.addPage(&p2);
    pm.begin();

    for (int i = 0; i < 3; i++) pm.update();

    TEST_ASSERT_EQUAL(3, p1.loopCalled);
    TEST_ASSERT_EQUAL(0, p2.loopCalled); // inactive page must not loop
}

// update() with no current page must not crash
void test_update_no_page_no_crash() {
    PageManager pm;
    pm.update(); // should not crash
}

// getCurrentPageName() returns "None" when no page registered
void test_get_page_name_no_page() {
    PageManager pm;
    TEST_ASSERT_EQUAL_STRING("None", pm.getCurrentPageName());
}

// getCurrentPageName() returns the name of the active page
void test_get_page_name_active_page() {
    PageManager pm;
    StubPage p1(&displayHandler, "Clock"), p2(&displayHandler, "Settings");
    pm.addPage(&p1);
    pm.addPage(&p2);
    pm.begin();

    TEST_ASSERT_EQUAL_STRING("Clock", pm.getCurrentPageName());

    pm.nextPage();
    TEST_ASSERT_EQUAL_STRING("Settings", pm.getCurrentPageName());
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------
void setup() {
    delay(2000); // wait for serial monitor

    auto cfg = M5.config();
    M5.begin(cfg);
    M5.Display.setRotation(3);

    UNITY_BEGIN();

    RUN_TEST(test_begin_no_pages);
    RUN_TEST(test_add_page_overflow);
    RUN_TEST(test_begin_calls_setup_on_first_page);
    RUN_TEST(test_goto_page_out_of_bounds);
    RUN_TEST(test_goto_same_page_is_noop);
    RUN_TEST(test_page_transition_lifecycle);
    RUN_TEST(test_next_page_wraps);
    RUN_TEST(test_previous_page_wraps);
    RUN_TEST(test_update_calls_loop);
    RUN_TEST(test_update_no_page_no_crash);
    RUN_TEST(test_get_page_name_no_page);
    RUN_TEST(test_get_page_name_active_page);

    UNITY_END();
}

void loop() {
    delay(1000);
}
