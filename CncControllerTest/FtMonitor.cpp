#include <thread>
#include "FtMonitor.h"

#include <Synchapi.h>

namespace CCS
{
	// -------------------------------------------------------------------
	const std::string CONTEXT_FEEDBACK = "Feedback";
	const std::string CONTEXT_PROMOTE  = "Promote";
	const std::string CONTEXT_DEMOTE   = "Demote";
	
	// -------------------------------------------------------------------
	FtMonitor::FtMonitor(const std::string& n, bool tm)
	: m_testMode			(tm)
	, m_active				(false)
	, m_name				(n)
	, m_onlineSever			()
	, m_state				(m_active ? Unknown : Online)
	, m_allServers			()
	, m_electionCounter		(0)
	, m_lastEvaluation		(0)
	, m_lastFeedback		(0)
	, m_maxServerInfoCount	(5)
	, m_maxServerBackTime	(int64_t(3) * 1000 * 1000 * 1000) // ns
	, m_maxFeedbackBackTime	(int64_t(3) * 1000 * 1000 * 1000) // ns
	{
	}
	
	// -------------------------------------------------------------------
	FtMonitor::~FtMonitor()
	{
	}
	
	// -------------------------------------------------------------------
	int64_t FtMonitor::determineServerRating()
	{
		Sleep(1); // ms
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}
	
	// -------------------------------------------------------------------
	void FtMonitor::activate(bool active)
	{ 
		m_active = active; 
		notifyActivation(m_active);
		switchStateTo(active ? Election : Online);
	}
	
	// -------------------------------------------------------------------
	void FtMonitor::reset()
	{
		m_allServers.clear();
		switchStateTo(m_active ? Unknown : Online);
		
		m_electionCounter	= 0;
	}
	
	// -------------------------------------------------------------------
	int64_t FtMonitor::getServerRating(const std::string& name) const
	{
		auto it = m_allServers.find(name);
		if ( it == m_allServers.end() )
			return -1;
			
		const ServerInfoQueue& siq = it->second;
		if ( siq.size() > 0 )
			return siq.back().m_rating;
		
		return -1;
	}
	
	// -------------------------------------------------------------------
	FtMonitor::State FtMonitor::getServerState(const std::string& name) const
	{
		auto it = m_allServers.find(name);
		if ( it == m_allServers.end() )
			return Unknown;
			
		const ServerInfoQueue& siq = it->second;
		if ( siq.size() > 0 )
			return siq.back().m_state;
		
		return Unknown;
	}
	
	// -------------------------------------------------------------------
	bool FtMonitor::serverExists(const std::string& name) const
	{
		return ( m_allServers.find(name) != m_allServers.end() );
	}
	
	// -------------------------------------------------------------------
	size_t FtMonitor::getActiveServerCount() const 
	{
		size_t ret= 0;
		for ( auto server : m_allServers )
		{
			ServerInfoQueue& siq = server.second;
			if ( siq.size() > 0 )
				ret++;
		}
		
		return ret;
	}
	
	// -------------------------------------------------------------------
	size_t FtMonitor::getCountOfOnlineServers() const
	{
		size_t ret = 0;
		
		for ( auto server : m_allServers )
		{
			ServerInfoQueue& siq = server.second;
			if ( siq.size() > 0 )
			{
				if ( siq.back().m_state == Online )
					ret++;
			}
		}
		
		return ret;
	}
	
	// -------------------------------------------------------------------
	int64_t FtMonitor::getOwnServerRating() const
	{
		auto it = m_allServers.find(m_name);
		if ( it == m_allServers.end() )
			return -1;
		
		const ServerInfoQueue& siq = it->second;
		if ( siq.empty() )
			return -1;
		
		return siq.back().m_rating;
	}
	
	// -------------------------------------------------------------------
	int FtMonitor::getOwnServerRatingLevel() const
	{
		const int64_t ownServerRating = getOwnServerRating();
		if ( ownServerRating < 0 )
			return INT_MAX;
		
		// initialize the level to the max level
		int ret = getActiveServerCount();
		
		for ( auto server : m_allServers )
		{
			// ignore the own rating
			const std::string& name = server.first;
			if ( m_name.compare(name) == 0 )
				continue;
			
			const ServerInfoQueue& siq = server.second;
			if ( siq.size() > 0 )
			{
				// if the own rating is better then the 
				// other move one level up/better
				if ( ownServerRating > siq.back().m_rating )
					ret--;
			}
			// otherwise the server isn't active
		}
		
		return ret;
	}
	
	// -------------------------------------------------------------------
	bool FtMonitor::isFeedbackActive() const
	{
		// now - last
		return ( (determineServerRating() - m_lastFeedback) < m_maxFeedbackBackTime );
	}
	
	// -------------------------------------------------------------------
	void FtMonitor::housekeeping(int64_t rating)
	{
		for ( auto server = m_allServers.begin(); server != m_allServers.end(); ++server )
		{
			//const std::string& name = server->first;
			ServerInfoQueue& siq = server->second;
			if ( siq.size() > 0 )
			{
				ServerInfo& si = siq.back();
				if ( rating - si.m_rating > m_maxServerBackTime )
				{
					// the master does not sending feedback
					if ( si.m_state == Online ) 
						evaluate();
					
					// clear server info queue
					while ( siq.empty() == false )
						siq.pop();
				}
			}
		}
	}

	// -------------------------------------------------------------------
	void FtMonitor::promote()
	{
		if ( m_active == false )
			return;

		if ( m_state != Standby )
			return;
			
		switchStateTo(Election);
		propagateServerInfo(m_name, CONTEXT_PROMOTE, m_state, FtMonitor::determineServerRating());
		
		evaluate();
	}
	
	// -------------------------------------------------------------------
	void FtMonitor::demote()
	{
		if ( m_active == false )
			return;

		if ( m_state != Online )
			return;
			
		switchStateTo(Standby);
		propagateServerInfo(m_name, CONTEXT_DEMOTE, m_state, FtMonitor::determineServerRating());
		
		evaluate();
	}
	
	// -------------------------------------------------------------------
	void FtMonitor::processMessage(const std::string& name, const std::string& context, State s, int64_t r)
	{
		if ( context.compare(CONTEXT_FEEDBACK) == 0 )
		{
			addServerFeedback(name, s, r);
		}
		else if ( context.compare(CONTEXT_PROMOTE ) == 0 )
		{
			evaluate();
		}
		else if ( context.compare(CONTEXT_DEMOTE ) == 0 )
		{
			evaluate();
		}
		else
		{
			std::cout << __PRETTY_FUNCTION__ << ": Unknown context: " << context << std::endl;
		}
	}
	
	// -------------------------------------------------------------------
	void FtMonitor::addServerFeedback(const std::string& name, State s, int64_t r)
	{
		if ( m_active == false )
			return;
			
		m_lastFeedback = determineServerRating();
		
		if ( m_allServers.find(name) == m_allServers.end() )
		{
			// add the new server
			ServerInfoQueue siq;
			siq.push(std::move(ServerInfo(s, r)));
			
			m_allServers[name] = siq;
		}
		else
		{
			// update feedback queue
			ServerInfoQueue& siq = m_allServers[name]; 
			siq.push(std::move(ServerInfo(s, r)));
		
			// removed too old entries
			while ( siq.size() > m_maxServerInfoCount )
				siq.pop();
		}
	}
	
	// -------------------------------------------------------------------
	void FtMonitor::evaluate()
	{
		if ( m_active == false )
			return;
			
		m_lastEvaluation = determineServerRating();
		
		switch ( m_state )
		{
			case Unknown:	switchStateTo(Election);
							propagateServerInfo(m_name, CONTEXT_FEEDBACK, m_state, FtMonitor::determineServerRating());
							break;
			
			case Election:	m_electionCounter++;
							if ( m_electionCounter >= 2 )
							{
								if ( false )
								{
									std::cout << " getActiveServerCount()    : " << getActiveServerCount()		<< std::endl;
									std::cout << " getCountOfOnlineServers() : " << getCountOfOnlineServers()	<< std::endl;
									std::cout << " getOwnServerRatingLevel() : " << getOwnServerRatingLevel()	<< std::endl;
								}
								
								if ( getActiveServerCount() == 1 )
								{
									// nobody else there
									switchStateTo(Online);
								}
								else if ( getCountOfOnlineServers() > 0 )
								{
									// already a master exists
									switchStateTo(Standby);
								}
								else
								{
									// the server with the best rating goes online
									if ( getOwnServerRatingLevel() == 1 )	switchStateTo(Online);
									else 									switchStateTo(Standby);
								}
								
								m_electionCounter = 0;
							}
							propagateServerInfo(m_name, CONTEXT_FEEDBACK, m_state, FtMonitor::determineServerRating());
							break;
			
			case Standby:	propagateServerInfo(m_name, CONTEXT_FEEDBACK, m_state, FtMonitor::determineServerRating());
							break;
			
			case Online:	if ( getCountOfOnlineServers() > 1 )
							{
								demote();
								//break;
							}
							
							if ( isFeedbackActive() == false )
							{
								demote();
								//break;
							}
							
							propagateServerInfo(m_name, CONTEXT_FEEDBACK, m_state, FtMonitor::determineServerRating());
							break;
		}
		
		housekeeping(FtMonitor::determineServerRating());
	}
	
	// -------------------------------------------------------------------
	FtMonitor::State FtMonitor::switchStateTo(State newState)
	{
		if ( m_active == true )
		{
			State prvState	= m_state;
			m_state			= newState;
			
			notifyStateSwitch(prvState, newState);
		}
		
		return m_state;
	}
	
	// -------------------------------------------------------------------
	void FtMonitor::notifyStateSwitch(State prvState, State newState)
	{
		// override this method to implement a dedicated logging
		// as well as a all actions have to taken ...
		std::cout	<< ">SWS: "
					<< getStateAsStr(prvState)
					<< " -> "
					<< getStateAsStr(newState)
					<< std::endl;
		;
	}
	
	// -------------------------------------------------------------------
	void FtMonitor::notifyActivation(bool active)
	{
		// override this method to implement a dedicated logging
		// as well as a all actions have to taken ...
		std::cout	<< ">ACT: "
					<< active
					<< std::endl;
		;
	}
	
	// -------------------------------------------------------------------
	void FtMonitor::propagateServerInfo(const std::string& name, const std::string& context, State s, int64_t r)
	{
		// override this method to really send this information
		// end remove the feedback short circuit - see below
		std::cout	<< ">PSI: "
					<< name				<< ", "
					<< context			<< ", "
					<< getStateAsStr(s) << ", "
					<< r				<< std::endl
		;
		
		// short circuit for feedback - for test only 
		if ( m_testMode == true )
			processMessage(name, context, s, r);
	}
	
	// -------------------------------------------------------------------
	void FtMonitor::trace(std::ostream& o) const
	{
		o	<< "*TRC: " 
			<< m_name 
			<< ": "
			<< getStateAsStr()
			<< ", Svr Cnt Online/Active/Total = "	<< getCountOfOnlineServers()	<< "/"
													<< getActiveServerCount()		<< "/"
													<< m_allServers.size()
			<< ", OwnRatLvl = " 					<< getOwnServerRatingLevel();
		;
		
		o << std::endl;
		for ( auto server : m_allServers )
		{
			const std::string& name = server.first;
			const ServerInfoQueue& siq = server.second;
			o << " " << name << ": " << siq.size();
			
			if ( siq.size() > 0 )
			{
				o << " {";
				
				const ServerInfo& first = siq.back();
				const ServerInfo& last  = siq.front();
				
				if ( siq.size() > 1 )
				{
					o << std::setw(10) << std::left << getStateAsStr(first.m_state) << ", " << first.m_rating;
					o << ", ";
					o << std::setw(10) << std::left << getStateAsStr(last.m_state)  << ", " << last.m_rating;
				}
				else
				{
					o << std::setw(10) << std::left << getStateAsStr(last.m_state) << ", " << last.m_rating;
				}
				
				o << "}";
			}
			
			o << std::endl;
		}
	}
	
	
	
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	#define TEST_CASE_START \
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl; \
		std::cout << "Test: " << __PRETTY_FUNCTION__ << " started" << std::endl; \
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl; \
		typedef CCS::FtMonitor FtM; 
	
	#define TEST_CASE_END \
		std::cout << "****************************************************************************" << std::endl; \
		std::cout << "Test: " << __PRETTY_FUNCTION__ << " finalized " << (ret ? "successfully!" : "with error(s)!") << std::endl; \
		std::cout << "****************************************************************************" << std::endl; 
		
	#define TEST_CASE_TRACE \
		trace << monitor << std::endl << std::endl;
		
	#define TEST_CASE_EVALUATE \
		trace << "evaluate(...)" << std::endl; monitor.evaluate();
		
	#define TEST_CASE_EVALUATE_AND_TRACE \
		TEST_CASE_EVALUATE \
		TEST_CASE_TRACE
		
	#define TEST_CASE_SEND_FEEDBACK(node, state) \
		monitor.processMessage(node, CCS::CONTEXT_FEEDBACK, state, FtM::determineServerRating());
	
	#define TEST_CASE_SEND_MONITOR_FEEDBACK \
		TEST_CASE_SEND_FEEDBACK(monitor.getName(), monitor.getState())
		
	#define TEST_CASE_CHECK_ACTIVE_SERVER_COUNT \
		const size_t asc = monitor.getActiveServerCount(); \
		if ( monitor.isActive() && ( asc > 0 ) == false ) \
		{ \
			std::cerr << "monitor.getActiveServerCount() > 0 failed! Value = " << asc << std::endl; \
			ret = false; \
		}
		
	#define TEST_CASE_CHECK_ONLINE_SERVER_COUNT \
		const size_t osc = monitor.getCountOfOnlineServers(); \
		if ( monitor.isActive() && ( osc == 1 ) == false ) \
		{ \
			std::cerr << "monitor.getCountOfOnlineServers() == 1 failed! Value = " << osc << std::endl; \
			ret = false; \
		}
		
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool FtMonitor::test_All()
	{
		bool silent = false;
		std::stringstream ss;
		
		bool ret = true;
		ret &= test_Inactive(silent ? ss : std::cout);
		ret &= test_Standalone(silent ? ss : std::cout);
		ret &= test_TwoServers(silent ? ss : std::cout);
		ret &= test_SeveralServers(silent ? ss : std::cout);
		ret &= test_DoubleOnline(silent ? ss : std::cout);
		// ...
		//ret &= test_Housekeeping(silent ? ss : std::cout);
		
		TEST_CASE_END
		return ret;
	}
	
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool FtMonitor::test_Inactive(std::ostream& trace)
	{
		TEST_CASE_START
		//---------------------------------------------
		const bool testMode = true;
		FtM monitor("Node1", testMode);
		TEST_CASE_TRACE
		
		//---------------------------------------------
		// send feedback and evaluate to complete the election steps
		TEST_CASE_EVALUATE_AND_TRACE
		TEST_CASE_EVALUATE_AND_TRACE
		TEST_CASE_EVALUATE_AND_TRACE
		TEST_CASE_EVALUATE_AND_TRACE
		
		//---------------------------------------------
		bool ret = true;
		TEST_CASE_CHECK_ACTIVE_SERVER_COUNT
		TEST_CASE_CHECK_ONLINE_SERVER_COUNT
		
		TEST_CASE_END
		return ret;

	}
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool FtMonitor::test_Standalone(std::ostream& trace)
	{
		TEST_CASE_START
		//---------------------------------------------
		const bool testMode = true;
		FtM monitor("Node1", testMode);
		monitor.activate();
		TEST_CASE_TRACE
		
		//---------------------------------------------
		// send feedback and evaluate to complete the election steps
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE
		
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE
		
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE
		
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE
		
		//---------------------------------------------
		bool ret = true;
		TEST_CASE_CHECK_ACTIVE_SERVER_COUNT
		TEST_CASE_CHECK_ONLINE_SERVER_COUNT
		
		TEST_CASE_END
		return ret;
	}
	
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool FtMonitor::test_TwoServers(std::ostream& trace)
	{
		TEST_CASE_START
		//---------------------------------------------
		const bool testMode = true;
		FtM monitor("Node2", testMode);
		monitor.activate();
		TEST_CASE_TRACE
		
		//---------------------------------------------
		TEST_CASE_SEND_FEEDBACK("Node1", FtM::Unknown)
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE
		
		//---------------------------------------------
		TEST_CASE_SEND_FEEDBACK("Node1", FtM::Election)
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE
		
		//---------------------------------------------
		TEST_CASE_SEND_FEEDBACK("Node1", FtM::Election)
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE
		
		//---------------------------------------------
		TEST_CASE_SEND_FEEDBACK("Node1", FtM::Election)
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE
		
		//---------------------------------------------
		TEST_CASE_SEND_FEEDBACK("Node1", FtM::Standby)
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE
		
		bool ret = true;
		TEST_CASE_CHECK_ACTIVE_SERVER_COUNT
		TEST_CASE_CHECK_ONLINE_SERVER_COUNT
		
		TEST_CASE_END
		return ret;
	}
	
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool FtMonitor::test_SeveralServers(std::ostream& trace)
	{
		TEST_CASE_START
		
		//---------------------------------------------
		const bool testMode = true;
		FtM monitor("Node1", testMode);
		monitor.activate();
		TEST_CASE_TRACE
		
		//---------------------------------------------
		TEST_CASE_SEND_FEEDBACK("Node2", FtM::Unknown)
		TEST_CASE_SEND_FEEDBACK("Node3", FtM::Unknown)
		TEST_CASE_SEND_FEEDBACK("Node4", FtM::Unknown)
		TEST_CASE_SEND_FEEDBACK("Node5", FtM::Unknown)
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE
		
		//---------------------------------------------
		TEST_CASE_SEND_FEEDBACK("Node2", FtM::Election)
		TEST_CASE_SEND_FEEDBACK("Node3", FtM::Election)
		TEST_CASE_SEND_FEEDBACK("Node4", FtM::Election)
		TEST_CASE_SEND_FEEDBACK("Node5", FtM::Election)
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE
		
		//---------------------------------------------
		TEST_CASE_SEND_FEEDBACK("Node2", FtM::Election)
		TEST_CASE_SEND_FEEDBACK("Node3", FtM::Election)
		TEST_CASE_SEND_FEEDBACK("Node4", FtM::Election)
		TEST_CASE_SEND_FEEDBACK("Node5", FtM::Election)
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE
		
		//---------------------------------------------
		TEST_CASE_SEND_FEEDBACK("Node2", FtM::Election)
		TEST_CASE_SEND_FEEDBACK("Node3", FtM::Election)
		TEST_CASE_SEND_FEEDBACK("Node4", FtM::Election)
		TEST_CASE_SEND_FEEDBACK("Node5", FtM::Election)
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE
		
		//---------------------------------------------
		TEST_CASE_SEND_FEEDBACK("Node2", FtM::Standby)
		TEST_CASE_SEND_FEEDBACK("Node3", FtM::Standby)
		TEST_CASE_SEND_FEEDBACK("Node4", FtM::Standby)
		TEST_CASE_SEND_FEEDBACK("Node5", FtM::Standby)
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE
		
		bool ret = true;
		TEST_CASE_CHECK_ACTIVE_SERVER_COUNT
		TEST_CASE_CHECK_ONLINE_SERVER_COUNT
		
		TEST_CASE_END
		return ret;
	}
	
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool FtMonitor::test_DoubleOnline(std::ostream& trace)
	{
		TEST_CASE_START
		//---------------------------------------------
		const bool testMode = true;
		FtM monitor("Node1", testMode);
		monitor.activate();
		TEST_CASE_TRACE
		
		//---------------------------------------------
		TEST_CASE_SEND_FEEDBACK("Node2", FtM::Unknown)
		TEST_CASE_SEND_FEEDBACK("Node3", FtM::Unknown)
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE
		
		//---------------------------------------------
		TEST_CASE_SEND_FEEDBACK("Node2", FtM::Election)
		TEST_CASE_SEND_FEEDBACK("Node3", FtM::Election)
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE
		
		//---------------------------------------------
		TEST_CASE_SEND_FEEDBACK("Node2", FtM::Election)
		TEST_CASE_SEND_FEEDBACK("Node3", FtM::Election)
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE
		
		//---------------------------------------------
		TEST_CASE_SEND_FEEDBACK("Node2", FtM::Election)
		TEST_CASE_SEND_FEEDBACK("Node3", FtM::Election)
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE
		
		//---------------------------------------------
		TEST_CASE_SEND_FEEDBACK("Node2", FtM::Standby)
		TEST_CASE_SEND_FEEDBACK("Node3", FtM::Online)
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE
		
		bool ret = true;
		TEST_CASE_CHECK_ACTIVE_SERVER_COUNT
		TEST_CASE_CHECK_ONLINE_SERVER_COUNT
		
		TEST_CASE_END
		return ret;
	}
	
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool FtMonitor::test_Housekeeping(std::ostream& trace)
	{
		TEST_CASE_START
		
		//---------------------------------------------
		const bool testMode = true;
		FtM monitor("Node3", testMode);
		monitor.activate();
		TEST_CASE_TRACE
		
		//---------------------------------------------
		// int several nodes
		TEST_CASE_SEND_FEEDBACK("Node1", FtM::Unknown)
		TEST_CASE_SEND_FEEDBACK("Node2", FtM::Unknown)
		TEST_CASE_SEND_FEEDBACK("Node3", FtM::Unknown)
		TEST_CASE_TRACE
		
		//---------------------------------------------
		// evaluate and send nothing for other nodes
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE
		
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE
		
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE
		
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE
		
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE

		//---------------------------------------------
		// sleep a while to simulate a server back time
		trace << "Sleep ....." << std::endl;
		//std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		Sleep(3000);
		
		//---------------------------------------------
		// send feedback again and trace monitors state
		TEST_CASE_SEND_MONITOR_FEEDBACK
		TEST_CASE_EVALUATE_AND_TRACE
		
		//---------------------------------------------
		bool ret = true;
		TEST_CASE_CHECK_ACTIVE_SERVER_COUNT
		TEST_CASE_CHECK_ONLINE_SERVER_COUNT
		
		TEST_CASE_END
		return ret;
	}
	
} //namespace