#include <unity.h>
#include <M5Unified.h>
#include "../../lib/display_handler.h"
#include "../../lib/page_manager.h"
#include "../../lib/pages/page_base.h"

DisplayHandler displayHandler;
PageManager pageManager;

// Test a simple page
class TestPage : public PageBase {
public:
    int setupCalled = 0;
    int loopCalled = 0;
    int cleanupCalled = 0;
    
    TestPage(DisplayHandler* disp) : PageBase(disp, "Test") {}
    
    void setup() override { 
        setupCalled++; 
        display->clearScreen();
    }
    
    void loop() override { 
        loopCalled++; 
    }
    
    void cleanup() override { 
        cleanupCalled++; 
    }
    
    const char* getName() override { return "Test"; }
};

TestPage testPage1(&displayHandler);
TestPage testPage2(&displayHandler);

void setUp(void) {}

void tearDown(void) {}

void test_page_manager_init() {
    pageManager.addPage(&testPage1);
    pageManager.addPage(&testPage2);
    
    TEST_ASSERT_EQUAL(2, pageManager.getPageCount());
}

void test_page_lifecycle() {
    pageManager.begin();
    
    // setup() must be called
    TEST_ASSERT_EQUAL(1, testPage1.setupCalled);
    
    // Simulate some "frames" / loops
    for(int i = 0; i < 5; i++) {
        pageManager.update();
    }
    
    TEST_ASSERT_EQUAL(5, testPage1.loopCalled);
}

void test_page_navigation() {
    pageManager.begin();
    
    int initialSetupCalls = testPage1.setupCalled;
    
    pageManager.nextPage();
    
    TEST_ASSERT_EQUAL(1, testPage1.cleanupCalled);
    
    TEST_ASSERT_EQUAL(1, testPage2.setupCalled);
}

void test_button_input() {
    pageManager.begin();
    
    // Simulate a click on button A
    M5.BtnA.setRawState(1, true);  // Press
    M5.update();
    
    pageManager.handleInput();
    
    // Check that the menu opens
    TEST_ASSERT_TRUE(testPage1.hasActiveMenu());
    
    M5.BtnA.setRawState(0, false);  // Release button
    M5.update();
}

void setup() {
    delay(2000);  // delay to open the serial monitor
    
    auto cfg = M5.config();
    M5.begin(cfg);
    M5.Display.setRotation(3);
    
    UNITY_BEGIN();
    
    RUN_TEST(test_page_manager_init);
    RUN_TEST(test_page_lifecycle);
    RUN_TEST(test_page_navigation);
    RUN_TEST(test_button_input);
    
    UNITY_END();
}

void loop() {
    // Tests done
    delay(1000);
}