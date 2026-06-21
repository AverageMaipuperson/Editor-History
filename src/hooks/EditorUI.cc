#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/UndoObject.hpp>
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/loader/Log.hpp>
#include <Geode/binding/SetIDPopup.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/utils/cocos.hpp>
#include "CCRadioMenu.h"
#include "popups/UndoObjectPopup.hpp"

class $modify(ToolsEditorUI, EditorUI)
{
    void onHistory(CCObject*)
    {
        UndoObjectPopup::create("Editor History")->show();
    }

    bool init(LevelEditorLayer* edit)
    {
        if(!EditorUI::init(edit)) return false;
        if(auto menu = this->querySelector("editor-buttons-menu"))
        {
            auto spr = EditorButtonSprite::createWithSprite("dt_history.png"_spr, 1.f, EditorBaseColor::Gray);
            auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ToolsEditorUI::onHistory));
            btn->setContentSize(ccp(40, 40));
            menu->addChild(btn);
            m_uiItems->addObject(btn);
            menu->updateLayout();
        }
        return true;
    }
};