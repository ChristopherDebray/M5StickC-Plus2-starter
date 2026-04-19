#ifndef PAGE_MANAGER_DEPS_H
#define PAGE_MANAGER_DEPS_H

#include "../ports/page_manager_port.h"
#include "../adapters/page_manager_m5stick_adapter.h"

inline IPageManager* getM5StickPageManager() {
    return new PageManagerM5StickAdapter();
}

#endif
