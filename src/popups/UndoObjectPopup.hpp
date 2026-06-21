#pragma once
#include <Geode/Geode.hpp>
#include <Geode/utils/cocos.hpp>
using namespace geode::prelude;
using namespace cocos2d;
using namespace geode;

class UndoFilterPopup;
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

    void reload() {
        changePage(0);
    }

    void onClose(CCObject* sender)
    {
        geode::Popup::onClose(sender);
    }

    protected:
    void createCellFromOffset(int, UndoObject*, int);

    void changePage(int);

    void nextPage(CCObject*)
    {
        changePage(1);
    }

    void prevPage(CCObject*)
    {
        changePage(-1);
    }

    bool init(float, float, std::string const&); 

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

    void undo(int, int);


    /* void FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
    {
        if(btn2) 
        {
            auto offset = reinterpret_cast<uintptr_t>(layer->getUserData());
            undo((int)offset, layer->getTag());
        }
    } */

    void onPage(CCObject*);

    void setIDPopupClosed(SetIDPopup* popup, int val)
    {
        changePage(val - m_page);
    }

    void moreInfo(CCObject*);

    void onFilter(CCObject*);
};