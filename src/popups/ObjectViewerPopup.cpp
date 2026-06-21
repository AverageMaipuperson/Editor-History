#include "ObjectViewerPopup.hpp"

bool ObjectViewerPopup::init(float width, float height, std::string const& text, UndoObjectPopup* parent, UndoObject* undo, std::string command)
{   
    if (!geode::Popup::init(width, height, "GJ_square01.png")) return false;

    m_parent = parent;
    m_undo = undo;

    std::string string;

    for (auto obj : CCArrayExt<GameObject*>(m_undo->m_objects))
    {
        string += std::string(obj->getSaveString(m_parent->m_ui->m_editorLayer)) + ";";
    }

    // log::info("str: {}", string);

    auto bg = CCScale9Sprite::create("square02_001.png");
    bg->setContentSize(ccp(220, 220));
    bg->setOpacity(127);
    bg->setPosition(ccp(120, 120));

    if(!string.empty() && undo->m_objects->count() < 1001)
    {
        auto spr = m_parent->m_ui->spriteFromObjectString(string, false, false, 0, nullptr, nullptr, nullptr);
        auto bgSize = bg->getContentSize();
        auto sprSize = spr->getScaledContentSize(); 
    
        if(undo->m_objects->count() == 1) spr->setScale(1.f);
        else if (sprSize.width > 0.f && sprSize.height > 0.f)
        {
            auto x = bgSize.width / sprSize.width;
            auto y = bgSize.height / sprSize.height;

            spr->setScale(spr->getScale() * std::min(x, y) / 1.5f);
        }

        bg->addChild(spr);
        spr->setPosition(ccp(bg->getContentSize().width / 2, bg->getContentSize().height / 2));
    } else {
        auto label = CCLabelBMFont::create("Nothing \n To \n Show...", "bigFont.fnt");
        label->setOpacity(127);
        bg->addChild(label);
        label->setPosition(ccp(bg->getContentSize().width / 2, bg->getContentSize().height / 2));
    }
    m_mainLayer->addChild(bg);
    std::string str;

    if(undo->m_command == UndoCommand::Paste || undo->m_command == UndoCommand::Delete || undo->m_command == UndoCommand::DeleteMulti || undo->m_command == UndoCommand::Select)
    str = fmt::format(R"(
        Command: {}
        Objects: {}
        )", command, undo->m_objects->count());
    else
    str = fmt::format(R"(
        Command: {}
        )", command);

    auto label = CCLabelBMFont::create(str.c_str(), "goldFont.fnt");
    
    label->setPosition(ccp(210, 210));
    label->setAnchorPoint(ccp(0, .5f));

    label->limitLabelWidth(200.f, 0.5f, 0.33f);

    m_mainLayer->addChild(label);

    auto spr = CCSprite::createWithSpriteFrameName("GJ_undoBtn_001.png");
    spr->setScale(.75f);
    auto btn = CCMenuItemExt::createSpriteExtra(
        spr, 
        [this, undo](CCObject* sender)
        {
            geode::createQuickPopup(
                "Undo Action",
                "Are you sure you want to <cr>undo</c> this action?",
                "Cancel",
                "Undo",
                [this, undo](FLAlertLayer* layer, bool btn2)
                {
                    if (btn2)
                    {
                        auto undoObjects = m_parent->m_lel->m_undoObjects;
                        int idx = static_cast<int>(undoObjects->count()) - 1;

                        undoObjects->exchangeObjectAtIndex(undoObjects->indexOfObject(undo), idx);
                        m_parent->m_ui->undoLastAction(m_parent->m_lel);

                        m_parent->onClose(nullptr);
                        onClose(nullptr);
                    }
                }
            ); 
        }
    );
    btn->setPosition(ccp(m_mainLayer->getContentSize().width - 20, 20));

    auto menu = CCMenu::create(btn, NULL);
    menu->ignoreAnchorPointForPosition(false);
    m_mainLayer->addChild(menu);

    this->setTitle(text.c_str());

    return true;
}