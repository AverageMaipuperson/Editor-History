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
using namespace geode::prelude;
using namespace cocos2d;
using namespace geode;

class UndoFilterPopup : public Popup {
    public:
    CCRadioMenu* m_menu;

    static UndoFilterPopup* create(std::string const& text)
    {
        auto ret = new UndoFilterPopup();

        if (ret && ret->init(240.f, 100.f, text))
        {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }

    CCMenuItemToggler* toggler(char const* spr1, char const* spr2) {
        auto offSpr = CCSprite::createWithSpriteFrameName(spr2);
        auto onSpr = CCSprite::createWithSpriteFrameName(spr1);

        auto off = CCMenuItemSprite::create(offSpr, offSpr, nullptr, nullptr);
        auto on  = CCMenuItemSprite::create(onSpr,  onSpr,  nullptr, nullptr);

        return CCMenuItemToggler::create(off, on, this, NULL);
    }

    bool init(float width, float height, std::string const& text)
    {   
        if (!geode::Popup::init(width, height, "GJ_square01.png")) return false;

        auto arr = CCArrayExt<CCMenuItemToggler*>(CCArray::create());

        arr.push_back(toggler("GJ_checkOn_001.png", "GJ_checkOff_001.png"));
        arr.push_back(toggler("GJ_checkOn_001.png", "GJ_checkOff_001.png"));
        arr.push_back(toggler("GJ_checkOn_001.png", "GJ_checkOff_001.png"));

        m_menu = CCRadioMenu::create(arr);
        m_menu->alignItemsHorizontallyWithPadding(5.f);
        m_menu->setPosition(ccp(m_mainLayer->getContentSize().width / 2, m_mainLayer->getContentSize().height / 2));
        m_mainLayer->addChild(m_menu);

        this->setTitle(text.c_str());
        return true;
    }

    virtual void onClose(cocos2d::CCObject* sender) override;
};

class UndoObjectPopup : public Popup, FLAlertLayerProtocol, SetIDPopupDelegate {
    
    public:
    EditorUI* m_ui;
    LevelEditorLayer* m_lel;
    int m_page;
    std::vector<cocos2d::extension::CCScale9Sprite*> m_cells;
    CCMenu* m_arrowMenu;
    std::vector<UndoObject*> m_undoList = {};
    ScrollLayer* m_scrollLayer;
    CCLabelBMFont* m_pageLabel;
    static int m_filter;

    static UndoObjectPopup* create(std::string const& text)
    {
        auto ret = new UndoObjectPopup();

        if (ret && ret->init(440.f, 280.f, text))
        {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }

    protected:
    void createCellFromOffset(int offset, UndoObject* undo, int id)
    {
        auto winSize = CCDirector::get()->getWinSize();
        std::string command;
        int objID = 0;
        int count = 0;

        auto scale = cocos2d::extension::CCScale9Sprite::create("square02_small.png", CCRectMake(0,0,40,40));
        scale->setOpacity(127);
        scale->setAnchorPoint({0,1});
        scale->setContentSize({m_mainLayer->getContentSize().width - 80, 40});
        // scale->setPosition({30, m_mainLayer->getContentSize().height - 45 * offset - 50});
        scale->setTag((int)undo->m_command);

        auto numLabel = CCLabelBMFont::create(
            fmt::format("{}", id + 1).c_str(), 
            "goldFont.fnt"
        );
        numLabel->setPosition(ccp(20, 20));
        numLabel->setScale(0.75f);
        scale->addChild(numLabel);

        if (undo->m_objects) 
        {
            count = undo->m_objects->count();
            if (count > 0) 
            {
                auto firstObj = static_cast<GameObject*>(undo->m_objects->objectAtIndex(0));
                if (firstObj) objID = firstObj->m_objectID;
            }

        } else if (undo->m_objectCopy) 
        {
            count = 1;
            objID = undo->m_objectCopy->m_object->m_objectID;

        }

        auto plural = (count > 1);
        switch ((int)undo->m_command) 
        {
            case 2: command = plural ? fmt::format("Created {} objects", count) : "Created 1 object"; break;
            case 3: command = plural ? fmt::format("Pasted {} objects", count) : "Pasted 1 object"; break;
            case 1: command = plural ? fmt::format("Deleted {} objects", count) : "Deleted 1 object"; break;
            case 5: command = plural ? fmt::format("Transformed {} objects", count) : "Transformed 1 object"; break;
            case 4: command = fmt::format("Deleted {} objects", count); break;
            case 6: command = plural ? fmt::format("De-selected {} objects", count) : "Selected objects"; break;
            default: command = fmt::format("Command {}: {} objects", (int)undo->m_command, count); break;

        }

        auto cmdLabel = CCLabelBMFont::create(
            command.c_str(), 
            "bigFont.fnt"
        );
        cmdLabel->setScale(.5f);
        cmdLabel->setPosition(ccp(scale->getContentSize().width / 2, 20));
        scale->addChild(cmdLabel);
        auto spr = CCSprite::createWithSpriteFrameName("GJ_undoBtn_001.png");
        spr->setScale(.75f);
        auto btn = CCMenuItemExt::createSpriteExtra(
            spr, 
            [this, offset, id](CCObject* sender)
            {
                geode::createQuickPopup(
                    "Undo Action",
                    "Are you sure you want to <cr>undo</c> this action? <cy>it can lead to bugs if you're not careful.</c>",
                    "Cancel",
                    "Undo",
                    [this, offset, id](FLAlertLayer* layer, bool btn2)
                    {
                        if (btn2) this->undo(offset, id); 
                    }
                );
            }
        );
        btn->setPosition(ccp(scale->getContentSize().width - 20, 20));

        auto menu = CCMenu::create(btn, NULL);
        menu->ignoreAnchorPointForPosition(false);
        scale->addChild(menu);

        spr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        spr->setScale(.75f);
        btn = CCMenuItemSpriteExtra::create(spr, undo, menu_selector(UndoObjectPopup::moreInfo));
        btn->setPosition(scale->getContentSize());
        menu = CCMenu::create(btn, NULL);
        menu->setPosition(ccp(0,0));
        if (Mod::get()->getSettingValue<bool>("info-btn")) scale->addChild(menu);

        m_cells.push_back(scale);

        m_scrollLayer->m_contentLayer->addChild(scale);

    }

    void changePage(int mod) 
    {
        int off = 0;

        for (auto cell : m_cells) 
        {
            if (cell) cell->removeFromParentAndCleanup(true);
        }

        m_cells.clear();

        m_page += mod;
        
        m_pageLabel->setString(utils::numToString(m_page).c_str());

        // thanks to erymanthus
        auto layout = ColumnLayout::create()
        ->setAxisReverse(true)
        ->setAxisAlignment(geode::AxisAlignment::End)
        ->setAutoGrowAxis(280.f)
        ->setGap(5.f);
        m_scrollLayer->m_contentLayer->setLayout(layout);

        for (unsigned int i = (m_page - 1) * 10 ; i < m_undoList.size() ; i++)
        {
            if (i >= m_page * 10) break;
            auto undo = m_undoList[i];
            if (undo) this->createCellFromOffset(off, undo, i);
            off++;
        }

        m_scrollLayer->m_contentLayer->updateLayout();

        auto node = static_cast<CCMenuItemSpriteExtra*>(m_arrowMenu->getChildByTag(1002));
        auto cond = (m_page * 10 < m_undoList.size());
        node->setEnabled(cond);
        node->setOpacity(cond ? 255 : 127);

        node = static_cast<CCMenuItemSpriteExtra*>(m_arrowMenu->getChildByTag(1001));
        cond = (m_page > 1);
        node->setEnabled(cond);
        node->setOpacity(cond ? 255 : 127);

        m_scrollLayer->scrollToTop();
    }

    void nextPage(CCObject*)
    {
        changePage(1);
    }

    void prevPage(CCObject*)
    {
        changePage(-1);
    }

    bool init(float width, float height, std::string const& text) 
    {
        if (!geode::Popup::init(width, height, "GJ_square01.png")) return false;

        m_ui = EditorUI::get();
        m_lel = m_ui->m_editorLayer;
        m_page = 1;

        auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
        int off = 0;

        m_scrollLayer = ScrollLayer::create(
            CCSize(400, 230),
            true,
            true
        );

        m_mainLayer->addChild(m_scrollLayer);
        m_scrollLayer->setPosition(ccp(40, 8));

        auto undoObjects = m_lel->m_undoObjects;
        m_undoList.reserve(undoObjects->count());

        for (unsigned int i = 0; i < undoObjects->count(); ++i) 
        {
            auto* obj = undoObjects->objectAtIndex(i);
            if (auto* undoObj = static_cast<UndoObject*>(obj))
            {
                m_undoList.push_back(undoObj);
            }
        }

        if (m_undoList.size() < 1) 
        {
            auto emptyLabel = CCLabelBMFont::create(
            "No History :(", 
            "bigFont.fnt"
            );
            emptyLabel->setOpacity(127);
            emptyLabel->setPosition({220, 140});
            m_mainLayer->addChild(emptyLabel);

        }

        auto menu = CCMenu::create();
        auto spr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(UndoObjectPopup::prevPage));

        btn->setTag(1001);
        btn->setEnabled(false);
        btn->setOpacity(127);
        menu->addChild(btn);

        spr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
        spr->setFlipX(true);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(UndoObjectPopup::nextPage));
        btn->setTag(1002);

        if (m_undoList.size() <= 10)
        {
            btn->setEnabled(false);
            btn->setOpacity(127);
        }

        menu->addChild(btn);
        menu->ignoreAnchorPointForPosition(false);
        menu->alignItemsHorizontallyWithPadding(780.f);
        menu->setScale(0.5f);
        menu->setAnchorPoint(ccp(0, 0.5f));
        m_arrowMenu = menu;
        menu->setPosition(220, 230);
        m_mainLayer->addChild(menu, 100);

        spr = CCSprite::create("GJ_button_02.png");
        m_pageLabel = CCLabelBMFont::create("1", "bigFont.fnt");
        m_pageLabel->setPosition(ccp(spr->getContentSize().width / 2, spr->getContentSize().height / 2));
        m_pageLabel->setScale(0.75f);
        spr->addChild(m_pageLabel);
        spr->setScale(.7f);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(UndoObjectPopup::onPage));
        menu = CCMenu::create(btn, NULL);
        menu->setPosition(ccp(m_size.width - 20, m_size.height / 2 + 60));

        spr = CCSprite::create("dt_filter.png"_spr);
        spr->setScale(.7f);
        spr = ButtonSprite::create(spr, 0, false, 0, "GJ_button_01.png", 1.f);
        spr->setScale(.7f);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(UndoObjectPopup::onFilter));
        menu->addChild(btn);
        menu->alignItemsVerticallyWithPadding(5.f);
        m_mainLayer->addChild(menu);

        this->setTitle(text.c_str());
        changePage(0);

        return true;
    }   

    /* void onUndo(CCObject* obj)
    {
        auto btn = (CCMenuItemSpriteExtra*)obj;
        auto offset = reinterpret_cast<uintptr_t>(btn->getUserData());

        auto alert = FLAlertLayer::create(
            this,
            "Undo Action",
            "Are you sure you want to <cr>undo</c> this action? <cy>it can lead to bugs if you're not careful.</c>",
            "Cancel",
            "Undo"
        );
        alert->setTag(btn->getTag());
        alert->setUserData((void*)offset);
        alert->show();
    } */

    void undo(int off, int id)
    {
        auto undoObjects = m_lel->m_undoObjects;
        int idx = static_cast<int>(undoObjects->count()) - 1;
        if (off < 0 || off > idx) return;

        auto obj = static_cast<UndoObject*>(undoObjects->objectAtIndex(off));
        if (!obj) return;
        obj->retain();

        undoObjects->exchangeObjectAtIndex(off, idx);
        m_ui->undoLastAction(m_lel);
        obj->release();

        onClose(nullptr);
    }


    /* void FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
    {
        if(btn2) 
        {
            auto offset = reinterpret_cast<uintptr_t>(layer->getUserData());
            undo((int)offset, layer->getTag());
        }
    } */

    void onPage(CCObject* obj)
    {  
        // i had to bruteforce this because my page calculations suck and im lazy
        auto popup = SetIDPopup::create(
            m_page, 
            1, 
            std::ceil(m_undoList.size() / 10) + 1, 
            "Go To Page",
            "Go",
            true,
            1,
            0,
            true,
            true
        );
        popup->m_delegate = this;
        popup->show();
        
        // simple fix for arrows not being rotated in some android devices for some reason
        if (auto menu = popup->m_mainLayer->getChildByID("main-menu"))
        {
            if (auto btn = static_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("left-arrow-button")))
            {
                btn->getNormalImage()->setRotation(0.f);
            }

            if (auto btn = static_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("right-arrow-button")))
            {
                btn->getNormalImage()->setRotation(0.f);
            }
        }
    }

    void setIDPopupClosed(SetIDPopup* popup, int val)
    {
        changePage(val - m_page);
    }

    void moreInfo(CCObject* obj)
    {
        auto undo = (UndoObject*)obj;
        auto t = undo->m_transformState;

        FLAlertLayer::create(
            "Info",
            fmt::format("<cy>Command:</c> {} \n <cp>Transform State:</c> scaleX: {}, scaleY: {}, angleX: {}, angleY: {}, skewX: {}, skewY: {}, tRotation: {}, tPositionX: {}, tPositionY: {} \n <cr>Undo Transform</c>: {} \n <cg>Redo</c>: {}", (int)cast::union_cast<float>(undo->m_command), t.m_scaleX, t.m_scaleY, t.m_angleX, t.m_angleY, t.m_skewX, t.m_skewY, t.m_transformRotation, t.m_transformPosition.x, t.m_transformPosition.y, undo->m_undoTransform, undo->m_redo).c_str(),
            "OK"
        )->show();
    }

    void onFilter(CCObject*)
    {
        auto popup = UndoFilterPopup::create("Filter");
        popup->m_menu->setValue(UndoObjectPopup::m_filter);
        popup->show();
    }
};

int UndoObjectPopup::m_filter = 0;

void UndoFilterPopup::onClose(cocos2d::CCObject* sender)
{
    UndoObjectPopup::m_filter = m_menu->getValue();
    geode::Popup::onClose(sender);
}

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