//
// Created by 정순용 on 2017. 4. 26..
//


#include "cli_menu.h"
#include <stdlib.h>
using namespace sephi;

class TestCUI : public  ICliMenuProvider
{
public:
    /// 리소스를 초기화 한다.
    bool initialize() {return true;}
    /// 리소스를 해제한다.
    bool finalize(){ return true;}

    /// int값을 출력해준다.
    int printInt(void *) {
        printf("Print integer double value : %d \n\n", rand());
        return 0;
    }
    /// double값을 출력해준다
    int printDouble(void *) {
        printf("Print random double value : %lf \n\n", rand()*0.00001);
        return 0;
    }
    /// string을 출력해준다
    int printString(void*) {
        printf("Test CLI Menu!!\n\n");
        return 0;
    }

    // 입력을 받아 출력해 준다.
    int getInt(void* arg) {
        char *in = getLine((char*) arg);
        int i = atoi(in);
        printf("integer = %d\n", i);
        return 0;
    }


};

/// TestMenu를 위한 MenuEntry
typedef CliMenuEntry<TestCUI> TestMenuEntry;
#define MENU_ITEM(...)  make_unique<CliMenuEntry<TestCUI>>(__VA_ARGS__)

/// 프로파일 CUI 메뉴 implementation
/**
    CliMenu를 상속받아 구현하였다.
*/
class TestMenu : public CliMenu<TestCUI>
{
public:
    TestMenu(TestCUI* owner_) : CliMenu<TestCUI>(owner_){
        initializeMenu();
    }
    /// 메뉴 항목을 초기화 한다.
    virtual int initializeMenu() {
        menu_list.push_back( MENU_ITEM("CLI Test Menu", 0));
        auto& topmenu = *menu_list.front();
        topmenu.submenu.push_back(MENU_ITEM("Print int value", 'i', &TestCUI::printInt, (void*)NO_FN_ARG, &topmenu));
        topmenu.submenu.push_back(MENU_ITEM("Print double value", 'd',  &TestCUI::printDouble, (void*)NO_FN_ARG, &topmenu));
        topmenu.submenu.push_back(MENU_ITEM("Print String value", 's',  &TestCUI::printString, (void*)NO_FN_ARG, &topmenu));
        topmenu.submenu.push_back(MENU_ITEM("Input menu", 'I'));
        auto &input_menu = topmenu.submenu.back()->submenu;
        input_menu.push_back(MENU_ITEM("Get integer", 'i', &TestCUI::getInt));

        return push(&topmenu);
    }
};


int main()
{
    TestCUI cui;
    TestMenu menu(&cui);

    menu.runMenu();
    return 0;
}


