#ifndef PAGE_MANAGER_PORT_H
#define PAGE_MANAGER_PORT_H

class PageBase;

class IPageManager {
public:
    virtual ~IPageManager() = default;

    virtual bool addPage(PageBase* page) = 0;
    virtual void begin() = 0;
    virtual void goToPage(int index) = 0;
    virtual void nextPage() = 0;
    virtual void previousPage() = 0;
    virtual void update() = 0;
    virtual void handleInput() = 0;

    virtual int getCurrentPageIndex() = 0;
    virtual int getPageCount() = 0;
    virtual PageBase* getCurrentPage() = 0;
    virtual const char* getCurrentPageName() = 0;
};

#endif
