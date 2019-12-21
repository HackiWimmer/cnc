#ifndef SERIAL_ADMIN_CHANNEL_H
#define SERIAL_ADMIN_CHANNEL_H

#include <wx/any.h>
#include "CircularBuffer.h"

////////////////////////////////////////////////////////////////////
struct SerialAdminMessage {
	
	public:
		enum Mid 	   { 	DATA_NOTIFICATION, FORCE_DATA_NOTIFICATION, 
							NOTIFY_PIN_UPDATE, NOTIFY_CONFIG_UPDATE,
							SET_DIGITAL_PIN, SET_ANALOG_PIN,
							SET_I2C_STATES
					   };
		
		enum ValueName { VAL1=0, VAL2=1, VAL3=2, VAL4=3, VAL5=4, VAL6=5, VAL7=6, VAL8=7 };
		enum Value     { MAX=VAL8 + 1 };

		void setMid(Mid m)								{ mid = m; }
		Mid  getMid()									{ return mid; }
		bool hasValue(ValueName name)					{ return flags[name]; }
		
		template <class T>
		const T getValue(ValueName name)				{ return values[name].As<T>(); }
		
		template <class T>
		void setValue(ValueName name, T value)			{ values[name] = value; flags[name] = true; }

	private:
		Mid		mid;
		wxAny 	values[Value::MAX];
		bool 	flags[Value::MAX];
};

////////////////////////////////////////////////////////////////////
typedef CircularBuffer<SerialAdminMessage>   CircularAdminChannel;
class SerialAdminChannel : public CircularBuffer<CircularAdminChannel> {
	
	public:
		
		SerialAdminChannel();
		virtual ~SerialAdminChannel();
		
		void 	clear()		{ circularBuffer.clear(); }
		int 	capacity()	{ return circularBuffer.capacity(); }
		int 	size()		{ return circularBuffer.size(); }
		
		bool 	read(SerialAdminMessage& sam);
		bool	write(const SerialAdminMessage& sam);
		
	protected:
		
		CircularAdminChannel circularBuffer;
};

////////////////////////////////////////////////////////////////////
class SerialAdminChannelReceiver  {
	
	private:
		SerialAdminChannel* channel;
		
	public:
		SerialAdminChannelReceiver(SerialAdminChannel* chl)
		: channel(chl)
		{}
		
		~SerialAdminChannelReceiver()
		{}
		
		int capacity()	{ return channel->capacity(); }
		int size()		{ return channel->size(); }

		bool read(SerialAdminMessage& sam)
		{ return channel->read(sam); }
};

////////////////////////////////////////////////////////////////////
class SerialAdminChannelSender  {
	
	private:
		SerialAdminChannel* channel;
		
	public:
		SerialAdminChannelSender(SerialAdminChannel* chl)
		: channel(chl)
		{}
		
		~SerialAdminChannelSender()
		{}
		
		int capacity()	{ return channel->capacity(); }
		int size()		{ return channel->size(); }

		bool write(SerialAdminMessage& sam)
		{ return channel->write(sam); }
};

#endif
