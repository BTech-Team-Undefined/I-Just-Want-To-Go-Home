#pragma once

#include <gl\glad.h>
#include <vector>

class OpenGLProfiler
{
// variables 
private:
	std::vector<GLuint> queries;		// stores all the opengl queries
	unsigned int queryBackBuffer = 0;	// determines which set of queries to use. 

// functions 
public:
	OpenGLProfiler();
	~OpenGLProfiler();
	
	// Creates count amount of timers to measure OpenGL commands. 
	// Calling this function clears any previous timers.
	void InitializeTimers(unsigned int count)
	{
		// cleanup any existing queries 
		if (queries.size() > 0)
			glDeleteQueries(queries.size(), &queries[0]);

		// for each timer we need to create 4 opengl queries 
		// (1 for start, 1 for end) x2 for back buffering 
		queries.resize(count * 4);
		if (count > 0) 
			glGenQueries(count * 4, &queries[0]);
	}

	// Start measuring duration for a specified timer. Timer's are 0 based. 
	void StartTimer(unsigned int timer)
	{
		auto index = GetIndex(timer);
		glQueryCounter(queries[index], GL_TIMESTAMP);
	}
	
	// Stop measuring duration for a specified timer. Timer's are 0 based.
	void StopTimer(unsigned int timer)
	{
		auto index = GetIndex(timer);
		glQueryCounter(queries[index + 1], GL_TIMESTAMP);
	}

	// Call this after starting/stopping all your timers for the frame.
	void FrameFinish()
	{
		if (queryBackBuffer)
			queryBackBuffer = 0;
		else
			queryBackBuffer = 1;
	}
	
	// Gets the duration for a timer in nanoseconds. Timer's are 0 based.
	GLuint64 GetDuration(unsigned int timer)
	{
		auto index = GetIndex(timer);
		GLuint64 t1, t2; 
		glGetQueryObjectui64v(queries[index], GL_QUERY_RESULT, &t1);
		glGetQueryObjectui64v(queries[index + 1], GL_QUERY_RESULT, &t2);
		return t2 - t1;
	}

private:
	// Helper function that determine how to index a timer.
	// Return represents start query index, return + 1 stop query index. 
	unsigned int GetIndex(unsigned int timer)
	{
		return (4 * timer) + (2 * queryBackBuffer);
	}
};

/* Notes:
Details on the functions here http://www.lighthouse3d.com/tutorials/opengl-timer-query/
*/