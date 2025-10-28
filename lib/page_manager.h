#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H

#include <M5StickCPlus2.h>
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
    
public:
    PageManager() {
        pageCount = 0;
        currentPageIndex = -1;
        currentPage = nullptr;
        transitionInProgress = false;
    }
    
    // Register a new page to the pull
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
    
    // Getters
    int getCurrentPageIndex() { return currentPageIndex; }
    int getPageCount() { return pageCount; }
    PageBase* getCurrentPage() { return currentPage; }
    const char* getCurrentPageName() { 
        return currentPage ? currentPage->getName() : "None"; 
    }
};

#endif