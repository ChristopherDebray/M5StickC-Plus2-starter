#ifndef PAGE_MANAGER_M5STICK_ADAPTER_H
#define PAGE_MANAGER_M5STICK_ADAPTER_H

#include <M5Unified.h>
#include <Arduino.h>
#include "../ports/page_manager_port.h"
#include "../pages/page_base.h"

#define MAX_PAGES 4

class PageManagerM5StickAdapter : public IPageManager {
public:
    PageManagerM5StickAdapter()
        : _pageCount(0), _currentPageIndex(-1), _currentPage(nullptr),
          _transitionInProgress(false), _lastBtnPWRState(false), _lastBtnBState(false) {}

    bool addPage(PageBase* page) override {
        if (_pageCount >= MAX_PAGES) return false;
        _pages[_pageCount++] = page;
        return true;
    }

    void begin() override {
        if (_pageCount > 0) goToPage(0);
    }

    void goToPage(int index) override {
        if (index < 0 || index >= _pageCount) return;
        if (index == _currentPageIndex) return;

        _transitionInProgress = true;

        if (_currentPage) {
            _currentPage->cleanup();
            _currentPage->setInitialized(false);
        }

        _currentPageIndex = index;
        _currentPage      = _pages[_currentPageIndex];
        _currentPage->setup();
        _currentPage->setInitialized(true);

        _transitionInProgress = false;
    }

    void nextPage() override {
        goToPage((_currentPageIndex + 1) % _pageCount);
    }

    void previousPage() override {
        goToPage((_currentPageIndex - 1 + _pageCount) % _pageCount);
    }

    void update() override {
        if (_currentPage && !_transitionInProgress) {
            _currentPage->loop();
        }
    }

    void handleInput() override {
        if (!_currentPage || _transitionInProgress) return;

        bool currentPWR = M5.BtnPWR.isPressed();
        bool currentB   = M5.BtnB.isPressed();

        if (!_currentPage->hasActiveMenu()) {
            if (currentPWR && !_lastBtnPWRState) {
                _lastBtnPWRState = currentPWR;
                _lastBtnBState   = currentB;
                previousPage();
                return;
            }
            if (currentB && !_lastBtnBState) {
                _lastBtnPWRState = currentPWR;
                _lastBtnBState   = currentB;
                nextPage();
                return;
            }
        }

        _lastBtnPWRState = currentPWR;
        _lastBtnBState   = currentB;
        _currentPage->handleInput();
    }

    int         getCurrentPageIndex() override { return _currentPageIndex; }
    int         getPageCount()        override { return _pageCount; }
    PageBase*   getCurrentPage()      override { return _currentPage; }
    const char* getCurrentPageName()  override {
        return _currentPage ? _currentPage->getName() : "None";
    }

private:
    PageBase* _pages[MAX_PAGES];
    int       _pageCount;
    int       _currentPageIndex;
    PageBase* _currentPage;
    bool      _transitionInProgress;
    bool      _lastBtnPWRState;
    bool      _lastBtnBState;
};

#endif
