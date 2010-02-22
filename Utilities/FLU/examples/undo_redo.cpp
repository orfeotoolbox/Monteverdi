/*

  unlimited undo/redo

  Widget_History

  when a widget is created, create unique name, add to the Widget_History
  when a widget is deleted, remove from the Widget_History

  when a widget reaches its final (changed) value, push the old value, along with the
  uniqe ID of the widget (pointer?)

  on an undo, go back one widget in the history, reset the value, call the callback
  on a redo, advance one widget in the history, reset the value, call the callback
  on a new event, empty the stack above the current point, then push

  -- issues --

  should each widget keep a local copy of last value? local history? this may be better,
  since the history can then only keep pointers and unique names, and won't need to worry about
  data types

  interpolation for keyframing?

  initial keyframe: instance of Widget_History with all widgets? (yes-these are the initial values)
  at keyframe, all widgets have a value. create list of all widgets with values different
  than previous keyframe
  to interpolate: for each widget in next keyframe, retrieve previous value and blend with next
  value (may have to go back several keyframes to find most recent value). this should be handled
  on a widget-by-widget basis (e.g. Fl_Choice should "jump" not blend)

  save/load histories?

  need a unique way to identify each widget that is stable across transient program instances
  can still use pointers at runtime, but need to convert to unique strings or something

  networking?

  on change, send unique widget name + value
  on receive, look up widget pointer by name, set value, call callback

  how to uniquely identify widgets?

  programmer creates a Widget_History_Group with a unique name.
  programmer creates a widget, adding it to a group, and gives the widget a unique name w.r.t.
  the enclosing group. the final unique name is the concatenation of each group name and
  the widget.

 */

#include <stdlib.h>
#include <string.h>

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Slider.H>

#include "FLU/FluSimpleString.h"

class History_Group;
class History;

class History_Object
{
  friend class History_Group;
  friend class History;

public:

  History_Object( const char *name );
  virtual ~History_Object();

  inline const char* get_name() const
  { return _name.c_str(); }

  inline const char *get_full_name() const
  { return _fullName.c_str(); }

  virtual void set_keyframe( int k );

  inline int get_keyframe() const
  { return _keyframe; }

protected:

  class State
  {
  public:

    State( unsigned int s, const void *d, int k )
    {
      _keyframe = k;
      _size = s;
      _data = 0;
      if( d && _size )
	{
	  _data = malloc( _size );
	  memcpy( _data, d, _size );
	}
    }

    ~State()
    {
      if( _data )
	free( _data );
    }

    inline void* data() const { return _data; }
    inline unsigned int size() const { return _size; }
    inline int keyframe() const { return _keyframe; }

    bool operator ==( const History_Object::State &s )
    {
      if( s._size == _size )
	{
	  if( memcmp( s._data, _data, s._size ) == 0 )
	    return true;
	}
      return false;
    }

  protected:
    void *_data;
    unsigned int _size;
    int _keyframe;
  };

  class StateStack
  {
  public:

    StateStack();
    ~StateStack();

    void push( History_Object::State *e );
    History_Object::State* pop();
    void clear();

    inline unsigned int size() const
    { return _size; }

    inline History_Object::State* operator[]( unsigned int i )
    { if( i >= _size ) return 0; else return _stack[i]; }

    inline History_Object::State* top()
    { return (*this)[_size-1]; }

  protected:
    unsigned int _size;
    History_Object::State **_stack;
  };

  class StateList
  {
  public:

    StateList();
    ~StateList();

    void insert( int i, History_Object::State *e );
    void erase( int i );
    void clear();

    inline unsigned int size() const
    { return _size; }

    inline History_Object::State* operator[]( unsigned int i )
    { if( i >= _size ) return 0; else return _list[i]; }

  protected:
    unsigned int _size;
    History_Object::State **_list;
  };

  //! The object should call this when its state has changed
  void state_changed();

  void undo();
  void redo();
  bool insert_state( State *s );
  void set_initial_state();

  // for "reading" the current state
  virtual unsigned int get_state_size() = 0;
  virtual void *get_state_buffer() = 0;

  // for "writing" the current state
  virtual void interpolate_state( float f, 
				  unsigned int s1, const void *state1,
				  unsigned int s2, const void *state2 ) = 0;
  virtual void set_state( unsigned int s, const void *buf ) = 0;

  void find_full_name();
  void find_history();
  void set_new_state( unsigned int s, const void *buf );

  FluSimpleString _name, _fullName;
  History_Group *_parent;
  bool _busy;
  History *_hist;
  int _keyframe, _keyframeStateIndex;

  StateStack _undoStack, _redoStack;
  StateList _keyframeList;
};

class History_Group : public History_Object
{
  friend class History_Object;
public:

  History_Group( const char *name );
  virtual ~History_Group();

  void begin();
  void end();

  void set_keyframe( int k );

  static History_Group *current();
  static void current( History_Group *g );

  inline int children() const { return _children; }
  History_Object* child( int n ) const;

  inline void add( History_Object &o ) { _add( o, true ); }
  inline void add( History_Object *o ) { add( *o ); }

  void remove( History_Object &o );
  inline void remove( History_Object *o ) { remove( *o ); }

protected:

  void _add( History_Object &o, bool init );

  unsigned int get_state_size() { return 0; }
  void *get_state_buffer() { return 0; }
  void interpolate_state( float, 
			  unsigned int, const void*,
			  unsigned int, const void* ) {}
  void set_state( unsigned int, const void* ) {}

  History_Object **_array;
  static History_Group *_current;
  int _children;

};

class History : public History_Group
{
  friend class History_Object;

public:

  History( const char *name );

  ~History();

  void undo();

  void redo();

  inline bool can_undo()
  { return( _undoStack.size() > 0 ); }

  inline bool can_redo()
  { return( _redoStack.size() > 0 ); }

  void remove_all_events( History_Object *o );

protected:

  class Event
  {
  public:

    Event( History_Object *o )
    {
      _object = o;
      _name = strdup( o->get_full_name() );
    }

    ~Event()
    {
      free( _name );
    }

    inline const char* name() const { return _name; }
    inline History_Object* object() const { return _object; }

  protected:
    char *_name;
    History_Object *_object;
  };

  void new_event( History_Object *o );
  void insert_event( Event *e );

  class EventStack
  {
  public:

    EventStack();
    ~EventStack();

    void push( History::Event *e );
    History::Event* pop();
    void erase( unsigned int i );
    void clear();

    inline unsigned int size() const
    { return _size; }

    inline History::Event* operator[]( unsigned int i )
    { if( i >= _size ) return 0; else return _stack[i]; }

    inline History::Event* top()
    { return (*this)[_size-1]; }

  protected:
    unsigned int _size;
    History::Event **_stack;
  };

  EventStack _undoStack, _redoStack;
};

/*
  2 kinds of event histories:

  undo-redo: each object in the tree is tracked for changes. each new Event (i.e. change)
  is added to a list. undoing goes back in the list, setting the object's older value.
  redoing goes forward in the list, setting the object's newer value. a new event ocurring
  while the "current" event is in the middle of the list causes all events to the end of the
  list to be discarded.

  animation: specifically, keyframing. a new history is constructed with the initial states
  of all objects in the tree. this is keyframe 0. changing the values of any objects in keyframe
  0 only changes the initial values. keyframe 1 differs from keyframe 0 in that some values have
  changed. a new "history" list records the differences between each keyframe.
 */

///////////////////////////////////////////

History_Object :: History_Object( const char *name )
{
  _name = _fullName = name;
  _hist = 0;
  _parent = 0;
  _keyframe = _keyframeStateIndex = -1;
  if( History_Group::current() )
    History_Group::current()->_add( *this, false );

  _busy = false;
}

History_Object :: ~History_Object()
{
  unsigned int i;
  for( i = 0; i < _undoStack.size(); i++ )
    delete _undoStack[i];
  for( i = 0; i < _redoStack.size(); i++ )
    delete _redoStack[i];
  for( i = 0; i < _keyframeList.size(); i++ )
    delete _keyframeList[i];
}

void History_Object :: set_keyframe( int k )
{
  unsigned int i;

  _keyframe = k;

  // see if there is an event in the keyframe list with keyframe==k
  _keyframeStateIndex = -1;
  for( i = 0; i < _keyframeList.size(); i++ )
    {
      if( _keyframeList[i]->keyframe() == k )
	{
	  _keyframeStateIndex = i;
	  break;
	}
    }

  if( _keyframeStateIndex != -1 )
    {
      // make the current state that state
      set_new_state( _keyframeList[_keyframeStateIndex]->size(), _keyframeList[_keyframeStateIndex]->data() );
    }
  else
    {
      // add a new keyframe state using the current state
      State *s = new State( get_state_size(), get_state_buffer(), k );
      if( _keyframeList.size() == 0 )
	_keyframeList.insert( 0, s );
      else
	{
	  for( i = 0; i < _keyframeList.size(); i++ )
	    {
	      if( k <_keyframeList[i]->keyframe() )
		{
		  _keyframeList.insert( i, s );
		  break;
		}
	    }
	}
    }
}

void History_Object :: find_history()
{
  if( !_parent )
    _hist = 0;
  else
    {
      History_Group *g = _parent;
      while( g->_parent )
	g = g->_parent;
      _hist = (History*)g;
    }
}

void History_Object :: find_full_name()
{
  _fullName = _name;
  History_Group* h = _parent;
  while( h )
    {
      _fullName = h->get_name() + FluSimpleString(":") + _fullName;
      h = h->_parent;
    }
}

void History_Object :: set_new_state( unsigned int s, const void *buf )
{
  if( _busy ) // prevents infinite recursion
    return;
  _busy = true;
  set_state( s, buf );
  _busy = false;
}

void History_Object :: set_initial_state()
{
  State *s = new State( get_state_size(), get_state_buffer(), get_keyframe() );
  insert_state( s );
}

void History_Object :: state_changed()
{
  if( _busy )
    return; // prevent adding an event when we are setting the value FROM an event
  State *s = new State( get_state_size(), get_state_buffer(), get_keyframe() );
  if( insert_state( s ) )
    _hist->new_event( this );
  else
    delete s;
}

bool History_Object :: insert_state( State *s )
{
  // if the state is exactly the same as the top state on the undo stack, don't add it
  if( _undoStack.size() )
    {
      State *old = _undoStack.top();
      if( *s == *old )
	return false;
    }

  // clear the redo stack
  for( unsigned int i = 0; i < _redoStack.size(); i++ )
    delete _redoStack[i];
  _redoStack.clear();

  _undoStack.push( s );

  return true;
}

void History_Object :: undo()
{
  if( _undoStack.size() == 0 )
    return;

  if( _undoStack.size() > 1 )
    _redoStack.push( _undoStack.pop() );
  State *s = _undoStack.top();
  set_state( s->size(), s->data() );
}

void History_Object :: redo()
{
  if( _redoStack.size() == 0 )
    return;

  State *s = _redoStack.top();
  set_state( s->size(), s->data() );
  _undoStack.push( _redoStack.pop() );
}

History_Object :: StateStack :: StateStack()
{
  _stack = 0;
  _size = 0;
}

History_Object :: StateStack :: ~StateStack()
{
  clear();
}

void History_Object :: StateStack :: clear()
{
  delete[] _stack;
  _stack = 0;
  _size = 0;
}

void History_Object :: StateStack :: push( History_Object::State *e )
{
  if( !e ) return;

  if( _size == 0 )
    {
      _stack = new (History_Object::State*)[1];
    }
  else
    {
      if( (_size & (_size-1)) == 0 )
	{
	  History_Object::State **temp = new (History_Object::State*)[2*_size];
	  for( unsigned int i = 0; i < _size; i++ )
	    temp[i] = _stack[i];
	  delete[] _stack;
	  _stack = temp;
	}
    }
  _stack[_size] = e;
  _size++;
}

History_Object::State* History :: StateStack :: pop()
{
  if( _size == 0 )
    return 0;

  _size--;
  return _stack[_size];
}

History_Object :: StateList :: StateList()
{
  _list = 0;
  _size = 0;
}

History_Object :: StateList :: ~StateList()
{
  clear();
}

void History_Object :: StateList :: clear()
{
  delete[] _list;
  _list = 0;
  _size = 0;
}

void History_Object :: StateList :: insert( int i, History_Object::State *e )
{
  if( !e ) return;
  if( i < 0 ) return;
  if( i > _size )
    i = _size;

  if( _size == 0 )
    {
      _list = new (History_Object::State*)[1];
    }
  else
    {
      if( (_size & (_size-1)) == 0 )
	{
	  History_Object::State **temp = new (History_Object::State*)[2*_size];
	  for( unsigned int i = 0; i < _size; i++ )
	    temp[i] = _list[i];
	  delete[] _list;
	  _list = temp;
	}
    }
  for( int j = _size; j > i; j-- )
    _list[j] = _list[j-1];
  _list[i] = e;
  _size++;
}

void History :: StateList :: erase( int i )
{
  if( _size == 0 )
    return;
  if( i < 0 || i >= _size )
    return;

  _size--;
  for( ; i < _size; i++ )
    _list[i] = _list[i+1];
}

///////////////////////////////////////////

History_Group* History_Group::_current = 0;

History_Group :: History_Group( const char *name )
  : History_Object( name )
{
  _children = 0;
  _array = 0;
  begin();
}

History_Group :: ~History_Group()
{
  if( _array )
    free( _array );
}

void History_Group :: begin()
{
  _current = this;
}

void History_Group :: end()
{
  _current = _parent;
  for( unsigned int i = 0; i < _children; i++ )
    _array[i]->set_initial_state();
}

void History_Group :: set_keyframe( int k )
{
  _keyframe = k;
  for( unsigned int i = 0; i < _children; i++ )
    _array[i]->set_keyframe( k );
}

History_Group* History_Group :: current()
{
  return _current;
}

void History_Group :: current( History_Group *g )
{
  _current = g;
}

void History_Group :: _add( History_Object &o, bool init )
{
  if( o._parent )
    {
      History_Group *g = o._parent;
      if( g == this )
	return;
      else
	g->remove( o );
    }

  o._parent = this;
  o.find_history();
  o.find_full_name();
  if( init )
    o.set_initial_state();
  if( _children == 0 )
    {
      _array = (History_Object**)malloc( sizeof(History_Object*) );
    }
  else
    {
      if( (_children & (_children-1)) == 0 )
	_array = (History_Object**)realloc( (void*)_array, 2*_children*sizeof(History_Object*) );
    }
  _array[_children] = &o;
  _children++;
}

void History_Group :: remove( History_Object &o )
{
  for( unsigned int i = 0; i < _children; i++ )
    {
      if( _array[i] == &o )
	{
	  o._parent = 0;
	  _children--;
	  if( _children == 0 )
	    {
	      free( _array );
	      _array = 0;
	    }
	  else
	    {
	      for( ; i < _children; i++ )
		_array[i] = _array[i+1];
	    }
	  return;
	}
    }
}

///////////////////////////////////////////

History :: EventStack :: EventStack()
{
  _stack = 0;
  _size = 0;
}

History :: EventStack :: ~EventStack()
{
  clear();
}

void History :: EventStack :: clear()
{
  delete[] _stack;
  _stack = 0;
  _size = 0;
}

void History :: EventStack :: push( History::Event *e )
{
  if( !e ) return;

  if( _size == 0 )
    {
      _stack = new (History::Event*)[1];
    }
  else
    {
      if( (_size & (_size-1)) == 0 )
	{
	  History::Event **temp = new (History::Event*)[2*_size];
	  for( unsigned int i = 0; i < _size; i++ )
	    temp[i] = _stack[i];
	  delete[] _stack;
	  _stack = temp;
	}
    }
  _stack[_size] = e;
  _size++;
}

History::Event* History :: EventStack :: pop()
{
  if( _size == 0 )
    return 0;

  _size--;
  return _stack[_size];
}

void History :: EventStack :: erase( unsigned int i )
{
  if( _size == 0 || i >= _size )
    return;
  _size--;
  for( ; i < _size; i++ )
    _stack[i] = _stack[i+1];
}

History :: History( const char *name )
  : History_Group( name )
{
}

History :: ~History()
{
  unsigned int i;
  for( i = 0; i < _undoStack.size(); i++ )
    delete _undoStack[i];
  for( i = 0; i < _redoStack.size(); i++ )
    delete _redoStack[i];
}

void History :: new_event( History_Object *o )
{
  Event *e = new Event( o );
  insert_event( e );
}

void History :: insert_event( Event *e )
{
  // clear the redo stack
  for( unsigned int i = 0; i < _redoStack.size(); i++ )
    delete _redoStack[i];
  _redoStack.clear();

  // add the new event
  _undoStack.push( e );
}

void History :: undo()
{
  if( _undoStack.size() == 0 )
    return;

  //if( _undoStack.size() > 1 )
  Event *e = _undoStack.pop();
  _redoStack.push( e );
  e->object()->undo();
}

void History :: redo()
{
  if( _redoStack.size() == 0 )
    return;

  Event *e = _redoStack.pop();
  _undoStack.push( e );
  e->object()->redo();
}

void History :: remove_all_events( History_Object *o )
{
  unsigned int i;
  for( i = 0; i < _undoStack.size(); )
    {
      if( _undoStack[i]->object() == o )
	{
	  delete _undoStack[i];
	  _undoStack.erase( i );
	}
      else
	i++;
    }
  for( i = 0; i < _redoStack.size(); )
    {
      if( _redoStack[i]->object() == o )
	{
	  delete _redoStack[i];
	  _redoStack.erase( i );
	}
      else
	i++;
    }
}

///////////////////////////////////////////
///////////////////////////////////////////

class Flu_History_Widget : public History_Object
{
public:

  Flu_History_Widget( const char *name, Fl_Widget *w );
  virtual ~Flu_History_Widget();

  inline Fl_Widget *widget() const
  { return _widget; }

  inline void hcallback( void (*cb)(Fl_Widget*,void*), void *cbd )
  { _flCallback = cb; _flCallbackData = cbd; }

  inline void do_widget_callback()
  { if( _flCallback ) _flCallback( _widget, _flCallbackData ); }

protected:

  Fl_Widget *_widget;

  void (*_flCallback)(Fl_Widget*,void*);
  void *_flCallbackData;

  inline static void _valueChangedCB( Fl_Widget*, void *arg )
  { ((Flu_History_Widget*)arg)->valueChangedCB(); }
  void valueChangedCB();

};

///////////////////////////////////////////

Flu_History_Widget :: Flu_History_Widget( const char *name, Fl_Widget *w )
  : History_Object( name )
{
  _widget = w;
  hcallback( 0, 0 );
  if( _widget )
    _widget->callback( _valueChangedCB, this );
}

Flu_History_Widget :: ~Flu_History_Widget()
{
  if( _widget )
    _widget->callback( (Fl_Callback*)0, (void*)0 );
}

void Flu_History_Widget :: valueChangedCB()
{
  state_changed();
  if( _flCallback )
    _flCallback( _widget, _flCallbackData );
}

///////////////////////////////////////////
///////////////////////////////////////////

class hFl_Slider : public Fl_Slider, public Flu_History_Widget
{
public:
  hFl_Slider( const char *name, int x, int y, int w, int h, const char *l = 0 )
    : Fl_Slider( x, y, w, h, l ), Flu_History_Widget( name, this ) {}

  ~hFl_Slider() {}

protected:

  unsigned int get_state_size()
  { return sizeof(float); }
  void *get_state_buffer()
  { val = value(); return &val; }
  void interpolate_state( float f, 
			  unsigned int s1, const void *state1,
			  unsigned int s2, const void *state2 )
  { }
  void set_state( unsigned int s, const void *buf )
  { val = *((float*)buf); value( val ); do_widget_callback(); }

  float val;

};

Fl_Button *undo, *redo;
History *history;

void cb( Fl_Widget *w, void* )
{
  hFl_Slider *s = (hFl_Slider*)w;

  if( history->can_undo() )
    undo->activate();
  else
    undo->deactivate();
  if( history->can_redo() )
    redo->activate();
  else
    redo->deactivate();
}

void undoCB( Fl_Widget *w, void *arg )
{
  History *h = (History*)arg;
  h->undo();
}

void redoCB( Fl_Widget *w, void *arg )
{
  History *h = (History*)arg;
  h->redo();
}

int main( int, const char** )
{
  history = new History( "hist" );

  Fl_Window *win = new Fl_Window( 300, 200, "Undo-Redo Try" );

  hFl_Slider *sliderA = new hFl_Slider( "sliderA", 10, 10, 250, 20, "Slider A" );
  sliderA->type( FL_HOR_NICE_SLIDER );
  sliderA->range( 0, 1 );
  sliderA->precision( 1 );
  sliderA->value( 0.5 );
  sliderA->hcallback( cb, 0 );

  hFl_Slider *sliderB = new hFl_Slider( "sliderB", 10, 60, 250, 20, "Slider B" );
  sliderB->type( FL_HOR_NICE_SLIDER );
  sliderB->range( 0, 1 );
  sliderB->precision( 1 );
  sliderB->value( 0.5 );
  sliderB->hcallback( cb, 0 );

  undo = new Fl_Button( 10, 110, 60, 25, "Undo" );
  undo->callback( undoCB, history );
  undo->deactivate();
  redo = new Fl_Button( 100, 110, 60, 25, "Redo" );
  redo->callback( redoCB, history );
  redo->deactivate();

  win->end();

  history->end();

  win->show();

  return Fl::run();
}
