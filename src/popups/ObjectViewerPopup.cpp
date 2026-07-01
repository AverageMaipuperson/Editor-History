#include "ObjectViewerPopup.hpp"

bool ObjectViewerPopup::init(float width, float height, std::string const& text, UndoObjectPopup* parent, UndoObject* undo, std::string command)
{   
    if (!geode::Popup::init(width, height, "GJ_square01.png")) return false;

    auto arr = CCArray::create();

    if(undo->m_objectCopy) arr->addObject(undo->m_objectCopy->m_object);
    else {
        for (int i = 0; i < undo->m_objects->count(); i++)
        {
            if(auto obj = dynamic_cast<GameObjectCopy*>(undo->m_objects->objectAtIndex(i)))
            {
                arr->addObject(obj->m_object);
                continue;
            }
            arr->addObject(static_cast<GameObject*>(undo->m_objects->objectAtIndex(i)));
        }
    }

    auto objects = CCArrayExt<GameObject*>(arr);

    m_parent = parent;
    m_undo = undo;

    std::string string;

    for (auto obj : objects)
    {
        // log::info("{}", std::string(obj->getSaveString(m_parent->m_ui->m_editorLayer)));

        string += std::string(obj->getSaveString(m_parent->m_ui->m_editorLayer)) + ";";
    }

    // log::info("str: {}", string);

    auto bg = CCScale9Sprite::create("square02_001.png");
    bg->setContentSize(ccp(220, 220));
    bg->setOpacity(127);
    bg->setPosition(ccp(120, 120));

    if(!string.empty() && objects.size() < 1001)
    {
        auto spr = m_parent->m_ui->spriteFromObjectString(string, false, false, 0, nullptr, nullptr, nullptr);
        auto bgSize = bg->getContentSize();
        auto sprSize = spr->getScaledContentSize(); 
    
        if(objects.size() == 1) spr->setScale(1.f);
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
        label->setSkewX(10);
        bg->addChild(label);
        label->setPosition(ccp(bg->getContentSize().width / 2, bg->getContentSize().height / 2));
    }
    m_mainLayer->addChild(bg);
    std::string str;

    if((undo->m_command == UndoCommand::Paste || undo->m_command == UndoCommand::Delete || undo->m_command == UndoCommand::DeleteMulti || undo->m_command == UndoCommand::Select) && undo->m_objects->count() > 0)
    str = fmt::format(R"(
        Command: {}
        Objects: {}
        ID: {}
        )", command, objects.size(), m_parent->m_lel->m_undoObjects->indexOfObject(undo));
    else
    str = fmt::format(R"(
        Command: {}
        ID: {}
        )", command, m_parent->m_lel->m_undoObjects->indexOfObject(undo));

    auto label = CCLabelBMFont::create(str.c_str(), "goldFont.fnt");
    
    label->setPosition(ccp(210, 210));
    label->setAnchorPoint(ccp(0, .5f));

    label->limitLabelWidth(200.f, 0.5f, 0.33f);

    m_mainLayer->addChild(label);

    auto menu = CCMenu::create();
    m_mainLayer->addChild(menu);
    menu->ignoreAnchorPointForPosition(false);

    auto btnSpr = ButtonSprite::create("Copy String");
    btnSpr->setScale(.75f);
    auto btn = CCMenuItemExt::createSpriteExtra(
        btnSpr, 
        [string](CCObject* sender)
        {
            if(utils::clipboard::write(string))
            {
                Notification::create(
                    "Copied!",
                    CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"),
                    1.5f
                )->show();
            } else {
                Notification::create(
                    "Copy failed.",
                    CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"),
                    1.5f
                )->show();
            }
        }
    );

    bool hasString = false;
    if(!string.empty()) 
    {
        menu->addChild(btn);
        hasString = true;
    }

    auto spr = CCSprite::createWithSpriteFrameName("GJ_undoBtn_001.png");
    spr->setScale(.75f);
    btn = CCMenuItemExt::createSpriteExtra(
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
    menu->addChild(btn);
    menu->setPosition(ccp(m_mainLayer->getContentSize().width - 10, 10));
    menu->setAnchorPoint(ccp(1,0));
    if(hasString)
    {
        auto layout = RowLayout::create();
        layout->setGap(30.0f);
        layout->setAxisAlignment(AxisAlignment::End);
        layout->setCrossAxisLineAlignment(AxisAlignment::Between);
    
        menu->setLayout(layout);
    } else {
        menu->setLayout(nullptr);
        menu->setPosition(ccp(m_mainLayer->getContentSize().width - 20, 20));
        menu->setAnchorPoint(ccp(0,0));
    }

    menu->updateLayout();

    spr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    spr->setScale(.75f);
    btn = CCMenuItemExt::createSpriteExtra(
        spr, 
        [undo](CCObject* sender)
        {
            auto t = undo->m_transformState;
            auto command = undo->m_command;

            FLAlertLayer::create(
                "Info",
                fmt::format("<cy>Command:</c> {} \n <cp>Transform State:</c> scaleX: {}, scaleY: {}, angleX: {}, angleY: {}, skewX: {}, skewY: {}, tRotation: {}, tPositionX: {}, tPositionY: {} \n <cr>Undo Transform</c>: {} \n <cg>Redo</c>: {}", static_cast<int>(undo->m_command), t.m_scaleX, t.m_scaleY, t.m_angleX, t.m_angleY, t.m_skewX, t.m_skewY, t.m_transformRotation, t.m_transformPosition.x, t.m_transformPosition.y, undo->m_undoTransform, undo->m_redo).c_str(),
                "OK"
            )->show();
        }
    );
    btn->setPosition(m_mainLayer->getContentSize());
    menu = CCMenu::create(btn, NULL);
    menu->setPosition(ccp(0,0));
    if (Mod::get()->getSettingValue<bool>("info-btn")) m_mainLayer->addChild(menu);

    this->setTitle(text.c_str());

    return true;
}