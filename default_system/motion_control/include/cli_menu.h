/*
 * SEMES CONFIDENTIAL
 *
 * Copyright (c) 2017 SEMES Co.,Ltd.
 * All Rights Reserved.
 *
 * NOTICE: All information contained herein is, and remains the property
 * of SEMES and its suppliers, if any. The intellectual and technical concepts
 * contained herein are proprietary to SEMES and its suppliers and may be
 * covered by S.Korea and Foreign Patents, patents in process, and are
 * protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material is
 * strictly forbidden unless prior written permission is obtained from SEMES.
 */


#ifndef __SEP_CLI_MENU_H__
#define __SEP_CLI_MENU_H__
#include <stdio.h>
#include <string.h>
#include <list>
#include <string>
#include <memory>
#include "log.h"

namespace sephi {

    enum CLI_MENU_ENTRY_ARG {
        NO_FN_ARG = ('N' << 24) | ('A' << 16) | ('R' << 8) | 'G',
    };
    using std::string;
    using std::unique_ptr;
    using std::make_unique;

    /// Command-line-interface 메뉴 항목 클래스
    /**
     * \see cli_menu_test.cpp
     */
    template<class Provider>
    struct CliMenuEntry {
        string title;        ///< 메뉴에 표시할 타이틀
        int selectKey;      ///< 입력 메뉴가 선택됐는 지 확인하기 위한 키 값
        void *arg;          ///< OnSelect 시의 인자
        int (Provider::*onSelect)(void *);  ///< 메뉴 선택시에 실행하게 될 함수
        int (Provider::*onDisplay)(void *);  ///< 메뉴 Refresh때 실행할 함수
        std::list<unique_ptr<CliMenuEntry>> submenu;
        CliMenuEntry *jump_to; ///< 선택하면 default로 다음 실행할 메뉴
    public:
        CliMenuEntry(const char *titlemenu, int key, int (Provider::*onSelectFn)(void *) = NULL,
                     void *fn_arg = (void *) NO_FN_ARG, CliMenuEntry<Provider> *jumpmenu = NULL,
                     int (Provider::*onDisplayFn)(void *) = NULL) {
            title = titlemenu;
            selectKey = key;
            onSelect = onSelectFn;
            arg = fn_arg;
            jump_to = jumpmenu;
            onDisplay = onDisplayFn;
        }

        ~CliMenuEntry() {
        }
    };

    /// CLI에서 메뉴가 선택 되었을 때 수행해야 할 기능을 제공하는 기능 구현 helper class
    struct ICliMenuProvider {
        char line_buffer[1024];

        char *getLine(char *instream) {
            if (instream) while (isspace(*instream)) instream += 1;
            // reuse input stream or get input
            if (instream && strlen(instream) > 0) {
                printf("%s\n", instream);
                strncpy(line_buffer, instream, 1024);
                return line_buffer;
            } else {
                return fgets(line_buffer, 1024, stdin);
            }
        }

        virtual bool intialize() { return true; } // override these if required
        virtual bool finalize() { return true; }
    };

    /// 메뉴 Application class
    /**
     * \see ICliMenuProvider, CliMenu
     *
     */
    template<class Provider, int MAX_MENU_DEPTH = 10>
    class CliMenu {
        const int KEY_EXIT_, KEY_ROOT_, KEY_UP_;
        CliMenuEntry<Provider> *menu_stack_[MAX_MENU_DEPTH];
        int stack_top_;
        bool initialized_;
    protected:
        Provider *owner;
        char line_buffer[1024];
        std::list<unique_ptr<CliMenuEntry<Provider>>> menu_list;

        int push(CliMenuEntry<Provider> *entry) {
            SEP_REQUIRE_RET(stack_top_ < MAX_MENU_DEPTH - 1, -1);
            menu_stack_[++stack_top_] = entry;
            return 0;
        }

        int pop() {
            SEP_REQUIRE_RET(stack_top_ > 0, -1);
            menu_stack_[stack_top_--] = NULL;
            return 0;
        }

        int popAll() {
            stack_top_ = 0;
            return 0;
        }

        int printAndGetInput() {
            CliMenuEntry<Provider> *menu = menu_stack_[stack_top_];

            printf("   >> %s <<\n", menu->title.c_str());
            printf("-----------------------------------------------------------\n");
            if (menu->onDisplay) {
                if ((owner->*(menu->onDisplay))(NULL) != 0) {
                    printf("-----------------------------------------------------------\n\n");
                    return 1; // to upper menu
                }
                printf("-----------------------------------------------------------\n");
            }

            for (auto &submenu :  menu->submenu) {
                printf("    %c.%s\n", submenu->selectKey, submenu->title.c_str());
            }
            printf("-- X: eXit, U: to Upper, R: to Root -----------------------\n");
            printf(" choice?");
            fflush(stdout);

            return (fgets(line_buffer, sizeof(line_buffer), stdin) == NULL) ? KEY_EXIT_ : line_buffer[0];
        }

        CliMenuEntry<Provider> *selectMenu(int key) {
            CliMenuEntry<Provider> *menu = menu_stack_[stack_top_];

            for (auto &submenu : menu->submenu) {
                if (key == submenu->selectKey) return submenu.get();
            }
            return NULL;
        }

    public:
        CliMenu(Provider *prg) : KEY_EXIT_('X'), KEY_ROOT_('R'), KEY_UP_('U'), stack_top_(-1), owner(prg) {
            memset(menu_stack_, 0, sizeof(menu_stack_));
            line_buffer[0] = 0;
            initialized_ = owner->initialize();
        }

        virtual ~CliMenu() { owner->finalize(); }

        virtual int initializeMenu() = 0; ///< 메뉴를 loading 하도록

        int runMenu() {
            SEP_REQUIRE_RET(initialized_, -1);
            CliMenuEntry<Provider> *menu;
            int inkey;
            do {
                // 화면에 메뉴항목 출력 및 선택 입력
                inkey = 0;
                inkey = printAndGetInput();

                if (inkey == 1) {
                    SEP_REQUIRE_RET(pop() == 0, -1);
                    continue;
                }
                // echo to print
                printf("%c\n\n", inkey);

                // 입력 받은 키값에 따라 분기
                if (inkey == KEY_ROOT_) { popAll(); }
                else if (inkey == KEY_UP_) { pop(); }
                else if (inkey == KEY_EXIT_) { }
                else {
                    menu = selectMenu(inkey);
                    if (menu != NULL) {
                        if (menu->onSelect) { // run menu, line buffer를 사용할 수 있게 넘겨줌.
                            if ((owner->*(menu->onSelect))(
                                    (menu->arg != (void *) NO_FN_ARG) ? menu->arg : line_buffer + 1) != 0) {
                                //실행 실패시 menu 유지
                                SEP_TRE("Fail to execute onSelect!\n");
                                SEP_REQUIRE_RET(pop() == 0, -1);
                                continue;
                            }
                        }
                        if (menu->jump_to) {
                            if (menu->jump_to == menu_stack_[stack_top_ - 1]) // upper로 jump하는 경우,
                                pop();
                            else if (menu->jump_to == menu_stack_[stack_top_]) // 자기 자신으로 jump하는 경우
                                ;
                            else
                                push(menu->jump_to); //jump가 설정 됐을 경우 jump
                        } else if (!menu->submenu.empty()) { // 하위 메뉴가 있으면 down

                            push(menu);
                        }

                    }
                }
            } while (inkey != KEY_EXIT_);
            return 0;
        }
    };

} // end of sephi

#endif //__SEP_CLI_MENU_H__
