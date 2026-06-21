#pragma once
#include "UndoObjectPopup.hpp"
#include "CCRadioMenu.h"

class UndoFilterPopup : public Popup {
    public:
    CCRadioMenu* m_menu;
    UndoObjectPopup* m_parent;

    static UndoFilterPopup* create(std::string const& text, UndoObjectPopup* parent)
    {
        auto ret = new UndoFilterPopup();

        if (ret && ret->init(240.f, 100.f, text, parent))
        {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }

    CCMenuItemToggler* toggler(char const*, const char*);

    bool init(float, float, std::string const&, UndoObjectPopup*);

    virtual void onClose(cocos2d::CCObject* sender) override;
};