#ifndef _WGD_ITERATOR_
#define _WGD_ITERATOR_

#include <cadence/agent.h>
#include <wgd/index.h>
#include <vector>

namespace wgd {

	/**
	 * An iterator to loop through all objects of a type.<br>
	 * There is no specific order in which the objects will be returned
	 * so you can only iterate forwards.<br>
	 * This example draws all IModel objects. This does not include child objects.<br>
	 * <code>for(Scene::Iterator<IModel> i = Scene::current()->begin(); i<i.size(); i++){<br>
	 * 	*i->draw();<br>
	 * }</code><br>
	 * You can iterate through all child objects of an object too.<br>
	 * This example will draw all child IModel objects of the "myModel" object <br>
	 * <code> IModel *mdl = object::get<IModel>("myModel");<br>
	 * for(Scene::Iterator<IModel> i = mdl->begin(); i<i.size(); i++){<br>
	 * 	*i->draw();<br>
	 * }</code><br>		
	 * Iterators can also be recrusive, ie they will also loop through all child items as well.<br>
	 * This example will draw all IModel objects in the scene<br> 
	 * <code>Scene::Iterator<IModel> i;<br>
	 * i.recursive(true);<br>
	 * for(i = Scene::current()->begin(); !i.end(); i++){<br>
	 * 	*i->draw();<br>
	 * }</code>
	 */
	template<class T> class Iterator {
		public:
		
		Iterator(bool recursive=false) : m_recurse(recursive) {};
		Iterator(const cadence::doste::OID::iterator &iter) : m_recurse(false) { build(iter.object()); }
		~Iterator(){};

		/** Return true if the iterator is at the end of the list */
		bool end(){ return m_position>=list.size() || m_position<0; };
		
		/** Return the number of instances of this type */
		int size(){ return list.size(); };
		
		/** Set if this iterator recurses to child instances */
		void recursive(bool r){ m_recurse = r; };
		
		Iterator& operator++()   { ++m_position; return *this; }; /// Move to the next item in the list
		Iterator& operator++(int){ ++m_position; return *this; }; /// Move to the next item in the list
		Iterator& operator--()   { --m_position; return *this; }; /// Move to the previous item in the list
		Iterator& operator--(int){ --m_position; return *this; }; /// Move to the previous item in the list
		
		/* Start the iterator for a given instance/scene */
		Iterator& operator=(const cadence::doste::OID::iterator &start){ build(start.object()); return *this; };
		
		/* set the iterator position */
		Iterator& operator=(int loc) { m_position = loc; return *this; }
		
		template <typename E>
		friend bool operator==(Iterator<E> &i, const cadence::doste::OID::iterator &e);
		template <typename E>
		friend bool operator!=(Iterator<E> &i, const cadence::doste::OID::iterator &e);
		
		/** Get a pointer to the current instance.
		 *  Will return NULL if invalid
		 */
		T* operator*(){ if(end()) return NULL; else return list[m_position]; };
		
		private:
		void build(const cadence::doste::OID &loc);
		std::vector<T*> list;
		unsigned int m_position;
		bool m_recurse;
	};
	

	template <typename T>
	bool operator==(Iterator<T> &i, const cadence::doste::OID::iterator &e) { return ( i.end()); }
	template <typename T>
	bool operator!=(Iterator<T> &i, const cadence::doste::OID::iterator &e) { return (!i.end()); };
	
	template<class T>
	void Iterator<T>::build(const cadence::doste::OID &loc){
		for(cadence::doste::OID::iterator i = loc.begin(); i!=loc.end(); i++) {
			if(loc[*i][cadence::doste::Type]==Null) continue;
			//get object
			cadence::Object *obj = loc[*i];
			if(obj!=NULL){
				//Add to list
				if(obj->isa(T::type())) list.push_back(loc[*i]);
				//recurse to children
				if(m_recurse && obj->get(ix::children)!=cadence::doste::Null) build(obj->get(ix::children));
			}
		}
		m_position = 0;	
	};

	
};

#endif
