/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_MULTIPLE_HANDS_H_
#define _XNV_MULTIPLE_HANDS_H_

#include "XnVHandPointContext.h"

class XnVIntHash;
class XnVIntList;

/**
* A XnVMultipleHands holds all the known hands in the system.
* It holds lists of which hands are new, which are active, and another list of the hands that aren't there anymore,
* but were still there last frame.
*/
class XNV_NITE_API XnVMultipleHands
{
public:
	/**
	 * Create an empty list of hands
	 */
	XnVMultipleHands();
	/**
	 * Copy constructor. Create a list of the same hands as the other.
	 *
	 * @param	[in]	other	The object from which to copy the list of hands
	 */
	XnVMultipleHands(const XnVMultipleHands& other);
	~XnVMultipleHands();

	/**
	* Add a hand
	*
	* @param	[in]	pContext	The Hand Point Context of the new hand
	*/
	XnStatus Add(const XnVHandPointContext* pContext);
	/**
	* Remove an existing hand
	*
	* @param	[in]	nID	The ID of the hand to be removed
	*
	* @return	The Hand Point Context that was removed, or NULL if it doesn't exist
	*/
	XnStatus Remove(XnUInt32 nID);

	/**
	* Get the context of a hand
	*
	* @param	[in]	nID	The ID of the hand
	*
	* @return	The Hand Point Context of that ID, or NULL if it doesn't exist
	*/
	XnVHandPointContext* GetContext(XnUInt32 nID); 
	/**
	* Get the context of a hand
	*
	* @param	[in]	nID	The ID of the hand
	*
	* @return	The Hand Point Context of that ID, or NULL if it doesn't exist
	*/
	const XnVHandPointContext* GetContext(XnUInt32 nID) const;
	/**
	* Get the context of the single selected hand
	*
	* @return	The Hand Point Context of the single selected hand, or NULL if no hands exist
	*/
	XnVHandPointContext* GetPrimaryContext();
	/**
	* Get the context of the single selected hand
	*
	* @return	The Hand Point Context of the single selected hand, or NULL if no hands exist
	*/
	const XnVHandPointContext* GetPrimaryContext() const;

	/**
	* Mark a hand as 'New'
	*
	* @param	[in]	nID	The ID that should be considered 'New'
	*/
	void MarkNew(XnUInt32 nID);
	/**
	* Mark a hand as 'Old'. This means a Hand Point Context may not exist for this ID
	*
	* @param	[in]	nID	The ID that should be considered 'Old'
	*/
	void MarkOld(XnUInt32 nID);
	/**
	* Mark a hand as 'Active'.
	*
	* @param	[in]	nID	The ID the should be considered 'Active'
	*/
	void MarkActive(XnUInt32 nID);

	/**
	* Remove marking as 'New'
	*
	* @param	[in]	nID	The ID that should no longer be considered 'New'
	*/
	void RemoveNew(XnUInt32 nID);
	/**
	* Remove marking as 'Old'
	*
	* @param	[in]	nID	The ID that should no longer be considered 'Old'
	*/
	void RemoveOld(XnUInt32 nID);
	/**
	* Remove marking as 'Active'
	*
	* @param	[in]	nID	The ID that should no longer be considered 'Active'
	*/
	void RemoveActive(XnUInt32 nID);

	/**
	* Get the number of hands that are considered 'New'
	*
	* @return	The number of 'New' entries
	*/
	XnUInt32 NewEntries() const;
	/**
	* Get the number of hands that are considered 'Old'
	*
	* @return	The number of 'Old' entries
	*/
	XnUInt32 OldEntries() const;
	/**
	* Get the number of hands that are considered 'Active'
	*
	* @return	The number of 'Active' entries
	*/
	XnUInt32 ActiveEntries() const;

	/**
	* Create a copy of the object, with same lists and copies of all Hand Point Contexts
	*
	* @param	[out]	other	The new copy of the object
	*/
	void Clone(XnVMultipleHands& other) const;

	/**
	* Remove all Hand Point Contexts
	*/
	void Clear();
	/**
	* Remove all entries from 'New', 'Old' and 'Active' lists
	*/
	void ClearLists();
	/**
	* Remove all entries from the 'New' list.
	*/
	void ClearNewList();
	/**
	* Remove all entries from the 'Old' list.
	*/
	void ClearOldList();

	/**
	 * Enable iteration over hands. This is the const version of the iterator.
	 */
	class XNV_NITE_API ConstIterator
	{
	public:
		friend class XnVMultipleHands;

		/**
		* Copy constructor. Copies the internal state of the other iterator
		*
		* @param	[in]	other	The other iterator
		*/
		ConstIterator(const ConstIterator& other);

		/**
		* Advance the iterator to the next position.
		*
		* @return	iterator after advancing
		*/
		ConstIterator& operator++();
		/**
		* Advance the iterator to the next position.
		*
		* @return	iterator before advancing
		*/
		ConstIterator operator++(int);

		/**
		* Compare with another iterator.
		*
		* @return	TRUE if both iterators are valid and refer to the same position in the same MultipleHands, o/w FALSE
		*/
		XnBool operator==(const ConstIterator& other) const;
		/**
		* Compare with another iterator.
		*
		* @return	FALSE if both iterators are valid and refer to the same position in the same MultipleHands, o/w TRUE
		*/
		XnBool operator!=(const ConstIterator& other) const;

		/**
		* Get the HandPointContext the iterator currently refers to.
		*
		* @return	The current HandPointContext. NULL if invalid.
		*/
		const XnVHandPointContext* operator*() const;

		/**
		* Check if the current Point is a new point.
		*
		* @return	TRUE if point is new, FALSE o/w
		*/
		XnBool IsNew() const;
		/**
		* Check if the current Point is active
		*
		* @return	TRUE if point is active, FALSE o/w
		*/
		XnBool IsActive() const;
		/**
		* Check if the iterator is valid
		*
		* @return	TRUE if iterator is valid, FALSE o/w
		*/
		XnBool IsValid() const;
	protected:
		ConstIterator(const XnVMultipleHands* pHands);
		ConstIterator(const XnVMultipleHands* pHands, XnUInt32 nPosition);

		const XnVMultipleHands* m_pHands;
		XnUInt32 m_nPosition;
		XnBool m_bValid;
	};

	/**
	* Enable iteration over hands. This is the non-const version of the iterator.
	*/
	class XNV_NITE_API Iterator : public ConstIterator
	{
	public:
		friend class XnVMultipleHands;

		/**
		* Copy constructor. Copies the internal state of the other iterator
		*
		* @param	[in]	other	The other iterator
		*/
		Iterator(const Iterator& other);

		/**
		* Advance the iterator to the next position.
		*
		* @return	iterator after advancing
		*/
		Iterator& operator++();
		/**
		* Advance the iterator to the next position.
		*
		* @return	iterator before advancing
		*/
		Iterator operator++(int);

		/**
		* Get the HandPointContext the iterator currently refers to.
		*
		* @return	The current HandPointContext. NULL if invalid.
		*/
		XnVHandPointContext* operator*() const;
	protected:
		Iterator(XnVMultipleHands* pHands);
		Iterator(XnVMultipleHands* pHands, XnUInt32 nPosition);
	};

	class XNV_NITE_API ConstIteratorOld
	{
	public:
		friend class XnVMultipleHands;
		ConstIteratorOld(const ConstIteratorOld& other);
		ConstIteratorOld& operator++();
		ConstIteratorOld operator++(int);
		XnBool operator==(const ConstIteratorOld& other) const;
		XnBool operator!=(const ConstIteratorOld& other) const;
		XnUInt32 operator*() const;
	protected:
		ConstIteratorOld(const XnVIntList* pList);
		ConstIteratorOld(const XnVIntList* pList, XnUInt32 nPosition);

		const XnVIntList* m_pList;
		XnUInt32 m_nPosition;
		XnBool m_bValid;
		XnUInt32 m_nSize;
	};

	/**
	* Enable olditeration over old hands. This is the non-const version of the iterator.
	*/
	class XNV_NITE_API IteratorOld : public ConstIteratorOld
	{
	public:
		friend class XnVMultipleHands;

		IteratorOld(const IteratorOld& other);
		IteratorOld& operator++();
		IteratorOld operator++(int);
	protected:
		IteratorOld(XnVIntList* pList);
		IteratorOld(XnVIntList* pList, XnUInt32 nPosition);
	};

	/**
	* Get an iterator to the first Hand Point Context
	*
	* @return	An iterator to the first Hand Point Context. Will be equal to end() if none exist
	*/
	Iterator begin();
	/**
	* Get a const iterator to the first Hand Point Context
	*
	* @return	A const iterator to the first Hand Point Context. Will be equal to end() if none exist
	*/
	ConstIterator begin() const;
	/**
	* Get an iterator after the last Hand Point Context
	*
	* @return	An iterator to after the last Hand Point Context
	*/
	Iterator end();
	/**
	* Get a const iterator after the last Hand Point Context
	*
	* @return	A const iterator to after the last Hand Point Context
	*/
	ConstIterator end() const;

	/**
	* Search for a Hand Point Context by ID
	*
	* @param	[in]	nID	The ID of the requested Hand Point Context
	*
	* @return An iterator to the Hand Point Context that matches the ID, or to end() if no match
	*/
	Iterator Find(XnUInt32 nID);
	/**
	* Search for a Hand Point Context by ID
	*
	* @param	[in]	nID	The ID of the requested Hand Point Context
	*
	* @return A const iterator to the Hand Point Context that matches the ID, or to end() if no match
	*/
	ConstIterator Find(XnUInt32 nID) const;
	/**
	* Check if a certain ID exists in the 'New' list
	*
	* @param	[in]	nID	The ID to check
	*
	* @return	TRUE of the ID is 'New', FALSE otherwise
	*/
	XnBool IsNew(XnUInt32 nID) const;
	/**
	* Check if a certain ID exists in the 'Old' list
	*
	* @param	[in]	nID	The ID to check
	*
	* @return	TRUE of the ID is 'Old', FALSE otherwise
	*/
	XnBool IsOld(XnUInt32 nID) const;
	/**
	* Check if a certain ID exists in the 'Active' list
	*
	* @param	[in]	nID	The ID to check
	*
	* @return	TRUE of the ID is 'Active', FALSE otherwise
	*/
	XnBool IsActive(XnUInt32 nID) const;

	/**
	* Get an iterator to the first item in the 'Old' list
	*
	* @return	An iterator to the first old ID. Will be equal to end() if list is empty
	*/
	IteratorOld beginOld();
	/**
	* Get a const iterator to the first item in the 'Old' list
	*
	* @return	A const iterator to the first old ID. Will be equal to end() if list is empty
	*/
	ConstIteratorOld beginOld() const;
	/**
	* Get an iterator after the last item in the 'Old list
	*
	* @return	An iterator to after the last old ID
	*/
	IteratorOld endOld();
	/**
	* Get a const iterator after the last item in the 'Old list
	*
	* @return	A const iterator to after the last old ID
	*/
	ConstIteratorOld endOld() const;

	/**
	* Pick a new Single Interesting Point
	*/
	void ReassignPrimary(XnUInt32 nHint = 0);

	/**
	 * Replace the focus point
	 *
	 * @param	[in]	ptFocusPoint	the new focus point
	 */
	void SetFocusPoint(const XnPoint3D& ptFocusPoint);
	/**
	 * Get the focus point
	 *
	 * @return	The focus point
	 */
	const XnPoint3D& GetFocusPoint() const;
private:
	XnVHandPointContext m_Hands[32];
	XnUInt32 m_nNextAvailable;
	XnUInt32 m_nAvailable;

	XnVIntList* m_pActiveIDs;
	XnVIntList* m_pNewIDs;
	XnVIntList* m_pOldIDs;

	XnUInt32 m_nPrimaryID;

	XnPoint3D m_ptFocusPoint;
};

#endif
