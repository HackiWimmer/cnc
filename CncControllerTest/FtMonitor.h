#ifndef FT_MONITOR_H
#define FT_MONITOR_H

#include <iostream>
#include <map>
#include <queue>
#include <chrono>
#include <iomanip>

namespace CCS
{
	class FtMonitor
	{
		public:
			
			enum State { Unknown=1, Election=2, Standby=3, Online=4 };
			
			const std::string&	getName()		const { return m_name; }
			State				getState()		const { return m_state; }
			bool				isActive()		const { return m_active; }
			const char*			getStateAsStr()	const { return getStateAsStr(m_state); }
			
			// ---------------------------------------------------------------
			static const char* getStateAsStr(State s) 
			{
				switch ( s )
				{
					case Unknown:		return "Unknown";
					case Election:		return "Election";
					case Standby:		return "Standby";
					case Online:		return "Online";
				}
				
				return "???";
			}
			
			void activate(bool active = true);
			void deactivate()					{ activate(false); }
			
		private:
		
			struct ServerInfo
			{
				State		m_state;
				int64_t		m_rating;
				 
				ServerInfo(State s, int64_t r)
				: m_state	(s)
				, m_rating	(r)
				{}
			};
			
			typedef std::queue<ServerInfo>					ServerInfoQueue;
			typedef std::map<std::string, ServerInfoQueue>	Servers;
			
			bool				m_testMode;
			bool				m_active; 
			std::string			m_name;
			std::string			m_onlineSever;
			State				m_state;
			Servers				m_allServers;
			int					m_electionCounter;
			int64_t				m_lastEvaluation;
			int64_t				m_lastFeedback;
			size_t				m_maxServerInfoCount;
			int64_t				m_maxServerBackTime;
			int64_t				m_maxFeedbackBackTime;
			
			void housekeeping(int64_t rating);
			bool isFeedbackActive() const;
			State switchStateTo(State newState);
			
		protected:
		
			explicit FtMonitor(const std::string& n, bool tm = false);
			virtual ~FtMonitor();
			
			static int64_t determineServerRating();
			
			virtual void notifyActivation(bool active);
			virtual void notifyStateSwitch(State prvState, State newState);
			virtual void propagateServerInfo(const std::string& name, const std::string& context, State s, int64_t r);

			bool serverExists(const std::string& name) const;
			size_t getActiveServerCount() const;
			
			int64_t getOwnServerRating() const;
			size_t getCountOfOnlineServers() const;
			int getOwnServerRatingLevel() const;
			
			int64_t getServerRating(const std::string& name) const;
			State getServerState(const std::string& name) const;
			
			void reset();
			void promote();
			void demote();
			
			void evaluate();
			
			void processMessage(const std::string& name, const std::string& context, State s, int64_t r);
			void addServerFeedback(const std::string& name, State s, int64_t r);
			
			void trace(std::ostream& o) const;
			friend std::ostream &operator<< (std::ostream &o, const FtMonitor &m) 
			{
				m.trace(o);
				return o;
			}
			
		public:
			
			static bool test_All();
			static bool test_Inactive(std::ostream& trace);
			static bool test_Standalone(std::ostream& trace);
			static bool test_TwoServers(std::ostream& trace);
			static bool test_SeveralServers(std::ostream& trace);
			static bool test_DoubleOnline(std::ostream& trace);
			// ...
			static bool test_Housekeeping(std::ostream& trace);
	};
	
} // namespace
#endif