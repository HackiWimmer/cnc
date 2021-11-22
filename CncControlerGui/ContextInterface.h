#ifndef CONTEXT_INTERFACE_H
#define CONTEXT_INTERFACE_H

#include <iostream>
#include "CncCommon.h"
#include "ArduinoEnvWrapper.h"

//
class ContextInterface
{
	private:
		
		bool isEntryListValid() const;
		
	protected:
		
		static const int ERR_NO_ERROR						= 0;
		static const int ERR_GENERAL						= 1;
		static const int ERR_LIMIT							= 2;
		static const int ERR_MOVE_WITHOUT_SPINDLE			= 4;
		
		struct Entry
		{
			enum Type { UNKNOWN = -1, START, END, CID, LIMIT, MOVE, SPINDLE, SPEED_F, SPEED_S };
			
			Type						lastType		= Type::UNKNOWN;
			long						clientId		= CLIENT_ID.INVALID;
			
			CncInterface::ILS::States	limitStates		= CncInterface::ILS::States();
			
			ArdoObj::SpeedTupleValue	speedTupleVal	= 0;
			ArdoObj::SpindleTupleValue	spindleTupleVal	= 0;
			
			SpindleState				spindleState	= SPINDLE_STATE_OFF;
			
			unsigned char				moveCmd			= CMD_INVALID;
			int32_t						moveDx			= 0;
			int32_t						moveDy			= 0;
			int32_t						moveDz			= 0;
			
			void resetMovement()     { moveCmd = CMD_INVALID; moveDx = moveDy = moveDz = 0; }
			bool hasMovement() const { return ( moveDx != 0 || moveDy || moveDz ); }
			
			std::ostream& traceTo(std::ostream &ostr) const;
			friend std::ostream &operator<< (std::ostream &ostr, const Entry &e) 
			{
				return e.traceTo(ostr);
			}
			
			static const char* getTypeAsString(Type t);
		};
		
		typedef std::vector<Entry> ContextEntries;
		ContextEntries		contextInterfaceEntries;
		Entry				currentEntry;
		int					errorFlags;
		
	public:
	
		struct Result
		{
			static const ArdoObj::SpeedTupleValue	minD  = std::numeric_limits<int32_t>::max();
			static const ArdoObj::SpeedTupleValue	maxD  = 0;
			
			struct Summary
			{
				ArdoObj::SpeedTupleValue	minF = minD;
				ArdoObj::SpeedTupleValue	maxF = maxD;
				ArdoObj::SpindleTupleValue	minS = minD;
				ArdoObj::SpindleTupleValue	maxS = maxD;
				CncInterface::ILS::States	limitStates;
				uint64_t					lenX = 0;
				uint64_t					lenY = 0;
				uint64_t					lenZ = 0;
			} sum;
			
			struct Error
			{
				uint64_t					lenX = 0;
				uint64_t					lenY = 0;
				uint64_t					lenZ = 0;
			} err;
			
			std::ostream& traceTo(std::ostream &o) const;
			friend std::ostream &operator<< (std::ostream &o, const Result &r) 
			{
				return r.traceTo(o);
			}
		};

		ContextInterface();
		virtual ~ContextInterface();
		
		bool hasErrors() const { return errorFlags != ERR_NO_ERROR; }
		bool analized()  const { return contextInterfaceEntries.size() > 0; }
		
		virtual void notifyBeginRun();
		virtual void notifyEndRun();
		
		virtual void notifyClientId(long id);
		
		virtual void notifyLimit(const CncInterface::ILS::States& s);
		virtual void notifyMove(unsigned char cmd, int32_t dx, int32_t dy, int32_t dz);
		
		virtual void notifySpindleOn();
		virtual void notifySpindleOff();
		
		virtual void notifySpindleSpeed(unsigned char pid, ArdoObj::SpindleTupleValue s);
		virtual void notifyStepperSpeed(unsigned char pid, ArdoObj::SpeedTupleValue s);
		
		bool analizeContextEntries(Result& result);
		
		bool filterAllLimitEntries(std::ostream& o) const;
		bool filterAllMovesWithoutSpindle(std::ostream& o) const;

		std::ostream& traceErrorInfoTo(std::ostream &ostr) const;
		std::ostream& traceContextEntriesTo(std::ostream &ostr) const;
		friend std::ostream &operator<< (std::ostream &ostr, const ContextInterface &i) 
		{
			return i.traceContextEntriesTo(ostr);
		}
};

#endif