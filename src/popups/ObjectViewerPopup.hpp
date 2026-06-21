#include "UndoFilterPopup.hpp"

class ObjectViewerPopup : public Popup {
public:
    UndoObjectPopup* m_parent;
    UndoObject* m_undo;
    static ObjectViewerPopup* create(std::string const& text, UndoObjectPopup* parent, UndoObject* undo, std::string command)
    {
        auto ret = new ObjectViewerPopup();

        if (ret && ret->init(440.f, 280.f, text, parent, undo, command))
        {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }

    bool init(float, float, std::string const&, UndoObjectPopup*, UndoObject*, std::string);
};