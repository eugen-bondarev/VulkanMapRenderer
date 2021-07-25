// Stole this off the Cherno

//
// Basic instrumentation profiler by Cherno

// Usage: include this header file somewhere in your code (eg. precompiled header), and then use like:
//
// Instrumentor::Get().BeginSession("Session Name");        // Begin session
// {
//     InstrumentationTimer timer("Profiled Scope Name");   // Place code like this in scopes you'd like to include in profiling
//     // Code
// }
// Instrumentor::Get().EndSession();                        // End Session
//
// You will probably want to macro-fy this, to switch on/off easily and use things like __FUNCSIG__ for the profile name.
//

#pragma once

#define VT_ENABLE_PROFILING

#ifdef VT_ENABLE_PROFILING

#include <algorithm>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

#include <future>

namespace Engine
{
	namespace Util
	{
		namespace Profiler
		{
			struct ProfileResult
			{
				std::string Name;
				long long Start, End;
				uint32_t ThreadID;
			};

			struct InstrumentationSession
			{
				std::string Name;
			};

			class Instrumentor
			{
			private:
				InstrumentationSession *m_CurrentSession;
				std::ofstream m_OutputStream;
				int m_ProfileCount;

			public:
				Instrumentor() : m_CurrentSession(nullptr), m_ProfileCount(0)
				{
				}

				void BeginSession(const std::string &name, const std::string &filepath = "results.json")
				{
					m_OutputStream.open(filepath);
					WriteHeader();
					m_CurrentSession = new InstrumentationSession{name};
				}

				void EndSession()
				{
					WriteFooter();
					m_OutputStream.close();
					delete m_CurrentSession;
					m_CurrentSession = nullptr;
					m_ProfileCount = 0;
				}
				
				std::mutex mut;

				void WriteProfile(const ProfileResult &result)
				{
					std::lock_guard l(mut);
					
					if (m_ProfileCount++ > 0)
						m_OutputStream << ",";

					std::string name = result.Name;
					std::replace(name.begin(), name.end(), '"', '\'');

					m_OutputStream << "{";
					m_OutputStream << "\"cat\":\"function\",";
					m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
					m_OutputStream << "\"name\":\"" << name << "\",";
					m_OutputStream << "\"ph\":\"X\",";
					m_OutputStream << "\"pid\":0,";
					m_OutputStream << "\"tid\":" << result.ThreadID << ",";
					m_OutputStream << "\"ts\":" << result.Start;
					m_OutputStream << "}";

					m_OutputStream.flush();
				}

				void WriteHeader()
				{
					m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
					m_OutputStream.flush();
				}

				void WriteFooter()
				{
					m_OutputStream << "]}";
					m_OutputStream.flush();
				}

				static Instrumentor &Get()
				{
					static Instrumentor instance;
					return instance;
				}
			};

			class InstrumentationTimer
			{
			public:
				InstrumentationTimer(const char *name) : m_Name(name), m_Stopped(false)
				{
					m_StartTimepoint = std::chrono::high_resolution_clock::now();
				}

				~InstrumentationTimer()
				{
					if (!m_Stopped)
						Stop();
				}

				void Stop()
				{
					auto endTimepoint = std::chrono::high_resolution_clock::now();

					long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
					long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

					uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
					Instrumentor::Get().WriteProfile({m_Name, start, end, threadID});

					m_Stopped = true;
				}

			private:
				const char *m_Name;
				std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
				bool m_Stopped;
			};

		}
	}
}

#	define VT_PROFILER_BEGIN(NAME) 			::Engine::Util::Profiler::Instrumentor::Get().BeginSession(NAME)
#	define VT_PROFILER_END() 				::Engine::Util::Profiler::Instrumentor::Get().EndSession()
#	define VT_PROFILER_SCOPE() 				::Engine::Util::Profiler::InstrumentationTimer timer(__func__)
#	define VT_PROFILER_NAMED_SCOPE(NAME) 	::Engine::Util::Profiler::InstrumentationTimer timer(NAME)
#else
#	define VT_PROFILER_BEGIN(NAME) 			VT_VOID_ASSEMBLY
#	define VT_PROFILER_END() 				VT_VOID_ASSEMBLY
#	define VT_PROFILER_SCOPE() 				VT_VOID_ASSEMBLY
#	define VT_PROFILER_NAMED_SCOPE(NAME) 	VT_VOID_ASSEMBLY
#endif