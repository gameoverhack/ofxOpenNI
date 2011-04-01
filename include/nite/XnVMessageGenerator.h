/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_MESSAGE_GENERATOR_H_
#define _XNV_MESSAGE_GENERATOR_H_

#include <XnStringsHash.h>

#include "XnVMessageListener.h"
#include "XnVMessage.h"
#include "XnVHandle.h"

/**
* A XnVMessageGenerator knows how to generate messages.
* Message Listeners register to it, and receive its Messages.
*/
class XNV_NITE_API XnVMessageGenerator
{
public:
	/**
	* Constructor. Create a new Message Generator
	*
	* @param	[in]	strName		Name of the control, for log purposes.
	*/
	XnVMessageGenerator(const XnChar* strName = "XnVMessageGenerator");
	virtual ~XnVMessageGenerator();

	/**
	* Add a Listener to the Generator
	*
	* @param	[in]	pListener	The listener to add
	*
	* @return	A handle to the listener, to allow removing it
	*/
	XnVHandle AddListener(XnVMessageListener* pListener);
	/**
	* Remove a listener from the generator
	*
	* @param	[in]	hListener	The handle of the listener to remove
	*
	* @return	The listener that was removed, or NULL if the handle was invalid
	*/
	XnVMessageListener* RemoveListener(XnVHandle hListener);
	/**
	* Remove a listener from the generator
	*
	* @param	[in]	pListener	The listener to remove
	*
	* @return	The listener that was removed, or NULL if the listener was never added
	*/
	XnVMessageListener* RemoveListener(XnVMessageListener* pListener);

	/**
	* Send the Message to all registered Listeners
	*
	* @param	[in]	pMessage	The Message to send
	*/
	void Generate(XnVMessage* pMessage);

	/**
	 * Clear the Multi-thread queue for all listeners
	 */
	void ClearAllQueues();

	/**
	 * Get the generator's name (for Log purposes)
	 *
	 * @return	The generator's name
	 */
	const XnChar* GetGeneratorName() const;
protected:
	XN_DECLARE_DEFAULT_HASH_DECL(XNV_NITE_API, XnUInt32, XnVMessageListener*, XnVIntMessageListenerHash);

	void OpenNewSession(XnVMessageListener* pListener);
	void CloseOldSession(XnVMessageListener* pListener);

	XnVIntMessageListenerHash m_hListeners;
	XnStringsHash m_Properties; // for last points
	XnVHandle m_hNextAvailable;

	XnChar* m_strGeneratorName;
};

#endif
