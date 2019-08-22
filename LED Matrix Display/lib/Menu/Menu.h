#ifndef Menu_Menu_h
#define Menu_Menu_h

#include <Arduino.h>
#include <map>
#include <vector>

namespace Menu {

/**
 * Callback triggered when a menu item was activated.
 */
typedef std::function<void()> MenuActionCallback;

/**
 * A single item, can either be a leave or a node.
 */
struct MenuItem {
  MenuItem(String id, String displayName, MenuActionCallback actionCB) {
    this->id = id;
    this->displayName = displayName;
    this->next = this;
    this->previous = this;
    this->actionCB = actionCB;
  }

  MenuItem(String id, String displayName, MenuItem* parentItem, MenuActionCallback actionCB) {
    this->id = id;
    this->displayName = displayName;
    this->next = this;
    this->previous = this;
    this->parent = parentItem;
    this->actionCB = actionCB;
  }

  /**
   * The id of the item to find it within the menu structure.
   */
  String id;
  /**
   * A display name
   */
  String displayName;
  /**
   * The parent item if this is a child.
   */
  MenuItem* parent;
  /**
   * The the next menu item on the same layer as this item.
   */
  MenuItem* previous;
  /**
   * The the next menu item on the same layer as this item.
   */
  MenuItem* next;
  /**
   * The children of this item.
   */
  std::vector<MenuItem*> children;
  /**
   * Callback triggered when a menu item was selected.
   */
  MenuActionCallback actionCB;
};

/**
 * Callback triggered when a menu item was selected.
 */
typedef std::function<void(MenuItem* item)> MenuSelectCallback;

class Menu {
 public:
  Menu();

  /**
   * Returns a menu item by its id.
   *
   * @param id the id of the item to fetch.
   */
  MenuItem* getById(String id);

  /**
   * Add a single menu entry to the first layer of the menu.
   *
   * @param id the unique id of the menu item.
   * @param displayName the human readable display name.
   * @param cb callback triggered when the item is executed.
   */
  MenuItem* addMenuItem(String id, String displayName, MenuActionCallback cb = NULL);

  /**
   * Add a single menu entry to an existing menu item.
   *
   * @param id the unique id of the menu item.
   * @param displayName the human readable display name.
   * @param parentItemId the ID of the parent item.
   * @param cb callback triggered when the item is executed.
   */
  MenuItem* addMenuItem(String id, String displayName, String parentItemId, MenuActionCallback cb = NULL);

  /**
   * Selects the previos menu item on the current layer. If there is no previous item the last one is selected.
   */
  MenuItem* previous();

  /**
   * Selects the next menu item on the current layer. If there is no next item the first one is selected.
   */
  MenuItem* next();

  /**
   * Selects the first child menu of the active item on the current layer. If there is no child item the selection is
   * not changed.
   */
  MenuItem* child();

  /**
   * Selects the parent item of the currently selecte menu item. If there is no parent item the selection is
   * not changed.
   */
  MenuItem* back();

  /**
   * Executes the action callback of the selected menu item.
   */
  MenuItem* execute();

  /**
   * Returns the active item of the menu.
   */
  MenuItem* getActiveItem();

  /**
   * Changes the active item to the item with the given id.
   *
   * @param id he id of the menu item
   */
  MenuItem* setActiveItem(String id);

  /**
   * Registers a callback for menu selection
   */
  void onMenuSelected(MenuSelectCallback cb);

 private:
  /**
   * Find an menut item with the given it by recursive traversal of the given root item and its children.
   *
   * @param id the unique ID to find
   * @param item the item to start the search with
   */
  MenuItem* find(String id, MenuItem* item);

  /**
   * Add a single menu entry to an existing menu item.
   *
   * @param id the unique id of the menu item.
   * @param displayName the human readable display name.
   * @param parentItem the parent item.
   * @param cb callback triggered when the item is executed.
   */
  MenuItem* _addMenuItem(String id, String displayName, MenuItem* parentItem, MenuActionCallback cb = NULL);

  /**
   * The currently active item of this menu.
   */
  MenuItem* _activeItem;
  /**
   * List of items for the first layer of the menu.
   */
  MenuItem _rootItem = MenuItem("ROOT", "ROOT", NULL);

  MenuSelectCallback _menuSelectedCB;
};

}  // namespace Menu

#endif
