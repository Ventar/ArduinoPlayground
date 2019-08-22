#include "Menu.h"

#define DEBUG_MENU

#ifdef DEBUG_MENU

#define debug(x, ...) Serial.printf((String(x) + String("\n")).c_str(), __VA_ARGS__);
#define debugln(x) Serial.printf((String(x) + String("\n")).c_str());

#else
#define debugln(x)
#define debug(x, ...)
#endif

namespace Menu {

Menu::Menu() {}

MenuItem* Menu::getById(String id) { return find(id, &_rootItem); }

MenuItem* Menu::find(String id, MenuItem* item) {
  if (item->id == id) {
    return item;
  } else if (!item->children.empty()) {
    for (MenuItem* child : item->children) {
      MenuItem* found = find(id, child);
      if (found != NULL) {
        return found;
      }
    }
  }
  return NULL;
}

MenuItem* Menu::addMenuItem(String id, String displayName, MenuActionCallback cb) {
  if (_rootItem.children.empty()) {
    _activeItem = _addMenuItem(id, displayName, &_rootItem, cb);
    return _activeItem;
  } else {
    return _addMenuItem(id, displayName, &_rootItem, cb);
  }
}

MenuItem* Menu::addMenuItem(String id, String displayName, String parentItemId, MenuActionCallback cb) {
  MenuItem* parent = find(parentItemId, &_rootItem);
  return _addMenuItem(id, displayName, parent, cb);
}

MenuItem* Menu::_addMenuItem(String id, String displayName, MenuItem* parentItem, MenuActionCallback cb) {
  MenuItem* newItem = new MenuItem(id, displayName, parentItem, cb);

  if (!parentItem->children.empty()) {
    MenuItem* first = parentItem->children.front();
    MenuItem* last = parentItem->children.back();
    first->previous = newItem;
    last->next = newItem;
    newItem->previous = last;
    newItem->next = first;
  }

  parentItem->children.push_back(newItem);

  return newItem;
}

MenuItem* Menu::previous() {
  _activeItem = _activeItem->previous;
  _menuSelectedCB(_activeItem);
  return _activeItem;
}

MenuItem* Menu::next() {
  _activeItem = _activeItem->next;
  _menuSelectedCB(_activeItem);
  return _activeItem;
}

MenuItem* Menu::child() {
  if (!_activeItem->children.empty()) {
    _activeItem = _activeItem->children.front();
    _menuSelectedCB(_activeItem);
  }

  return _activeItem;
}

MenuItem* Menu::back() {
  if (_activeItem->parent != &_rootItem) {
    _activeItem = _activeItem->parent;
    _menuSelectedCB(_activeItem);
  }

  return _activeItem;
}

MenuItem* Menu::execute() {
  if (_activeItem->actionCB) {
    _activeItem->actionCB();
  }

  return _activeItem;
}

MenuItem* Menu::getActiveItem() { return _activeItem; }

MenuItem* Menu::setActiveItem(String id) {
  _activeItem = find(id, &_rootItem);
  return _activeItem;
}

void Menu::onMenuSelected(MenuSelectCallback cb) { _menuSelectedCB = cb; }

}  // namespace Menu