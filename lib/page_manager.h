#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H

#include <M5Unified.h>
#include <Arduino.h>
#include "./pages/page_base.h"

#define MAX_PAGES 4

class PageManager {
private:
    PageBase* pages[MAX_PAGES];
    int pageCount;
    int currentPageIndex;
    PageBase* currentPage;

    bool transitionInProgress;

    // Manual rising-edge tracking for PWR and B buttons.
    // wasPressed() is unreliable on these two buttons (gets stuck internally).
    bool lastBtnPWRState;
    bool lastBtnBState;

public:
    PageManager() {
        pageCount = 0;
        currentPageIndex = -1;
        currentPage = nullptr;
        transitionInProgress = false;
        lastBtnPWRState = false;
        lastBtnBState = false;
    }

    // Register a new page to the pool
    bool addPage(PageBase* page) {
        if (pageCount >= MAX_PAGES) {
            return false;
        }
        pages[pageCount] = page;
        pageCount++;
        return true;
    }
    
    // Initialize with first page
    void begin() {
        if (pageCount > 0) {
            goToPage(0);
        }
    }
    
    // Switch to a specific page by index
    void goToPage(int index) {
        if (index < 0 || index >= pageCount) {
            return;
        }
        
        if (index == currentPageIndex) {
            return; // Already on this page
        }
        
        transitionInProgress = true;
        
        // Cleanup current page
        if (currentPage != nullptr) {
            currentPage->cleanup();
            currentPage->setInitialized(false);
        }
        
        // Switch to new page
        currentPageIndex = index;
        currentPage = pages[currentPageIndex];
        
        // Setup new page
        currentPage->setup();
        currentPage->setInitialized(true);
        
        transitionInProgress = false;
    }
    
    void nextPage() {
        int nextIndex = (currentPageIndex + 1) % pageCount;
        goToPage(nextIndex);
    }
    
    void previousPage() {
        int prevIndex = (currentPageIndex - 1 + pageCount) % pageCount;
        goToPage(prevIndex);
    }
    
    // Update current page
    void update() {
        if (currentPage != nullptr && !transitionInProgress) {
            currentPage->loop();
        }
    }

    // Handle inputs - delegates to current page or handles page navigation.
    // PWR = previous page, B = next page (only when no menu is active).
    // Rising-edge detection used for PWR and B — wasPressed() is buggy on those.
    void handleInput() {
        if (currentPage == nullptr || transitionInProgress) return;

        bool currentPWRState = M5.BtnPWR.isPressed();
        bool currentBState   = M5.BtnB.isPressed();

        if (!currentPage->hasActiveMenu()) {
            // PWR rising edge → previous page
            if (currentPWRState && !lastBtnPWRState) {
                lastBtnPWRState = currentPWRState;
                lastBtnBState   = currentBState;
                previousPage();
                return;
            }

            // B rising edge → next page
            if (currentBState && !lastBtnBState) {
                lastBtnPWRState = currentPWRState;
                lastBtnBState   = currentBState;
                nextPage();
                return;
            }
        }

        // Always update state before delegating so the page's own
        // handleBasicInputInteractions() sees a consistent button state.
        lastBtnPWRState = currentPWRState;
        lastBtnBState   = currentBState;

        // Let the current page handle the input
        currentPage->handleInput();
    }
    
    // Getters
    int getCurrentPageIndex() { return currentPageIndex; }
    int getPageCount() { return pageCount; }
    PageBase* getCurrentPage() { return currentPage; }
    const char* getCurrentPageName() { 
        return currentPage ? currentPage->getName() : "None"; 
    }
};

#endif